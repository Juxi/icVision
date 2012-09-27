// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 Gregor Kaufmann
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

/** @file RpcFilterImpl.cpp Implementation file for the \c RpcFilterImpl class.
 *
 * Version: $Rev$
 *
 * $Date$
 *
 */

#include "./yarp/os/impl/RpcFilterImpl.h"

using namespace yarp::os;
using yarp::os::impl::RpcFilterImpl;

// This is an internal helper class. The unnamed namespace ensures that it
// can only be accessed from within this file.
namespace {
	class TextReader: public yarp::os::PortReader {
	public:
		yarp::os::ConstString str;
		virtual bool read(yarp::os::ConnectionReader& reader) {
			str = reader.expectText();
			return reader.isValid();
		}
	};
}

RpcFilterImpl::RpcFilterImpl() :
	PortReader(), targetPortName(), m_out_prot(NULL), m_out_route(),
			callObserver(NULL), responseObserver(NULL), replier(NULL) {
}

RpcFilterImpl::~RpcFilterImpl() {
	close();
}

bool RpcFilterImpl::open(ConstString targetPortName) {
	RpcFilterImpl::targetPortName = targetPortName;
	return setupOutputConnection();
}

void RpcFilterImpl::close() {
	if (NULL != m_out_prot) {
		m_out_prot->interrupt();
		m_out_prot->close();
		delete m_out_prot;
		m_out_prot = NULL;
	}
}

void RpcFilterImpl::cutConnection(bool cut) {
	isCut = cut;
}

void RpcFilterImpl::setCallObserver(IObserver *o) {
	callObserver = o;
}

void RpcFilterImpl::setResponseObserver(IObserver *o) {
	responseObserver = o;
}

void RpcFilterImpl::setReplier(IReplier *r) {
	replier = r;
}

bool RpcFilterImpl::injectCall(const Bottle &b) {
	Bottle call = b;
	Bottle response;

	// TODO: Complete
	forwardCall(call, NULL);
	
//	if (responseObserver != NULL) {
//		responseObserver->onDataObserved(response);
//	}

	return true;
}

bool RpcFilterImpl::read(ConnectionReader& connection) {
	Bottle call;
	Bottle response;
	bool ok = call.read(connection);

	if (!ok) {
		return false;
	}

	//printf("Checking that call observer exists ... ");
	if (NULL != callObserver) {
		//printf(" ... OK. Calling onDataObserved(RPC_Call)\n");
		callObserver->onDataObserved(call);
	}

	ConnectionWriter *returnToSender = connection.getWriter();
	if (returnToSender == NULL) {
		return false;
	}

	if (true != isCut) {
		//printf("  ... Forwarding RPC_Call to robot\n");
		return forwardCall(call, returnToSender);
	} else {
		//printf("  ... Filter cut. RPC_Call intercepted\n");
		// ask a possibly existing replier for it's response(s)
		bool responded = false;
		if (replier != NULL) {
			Bottle *pb = replier->getNextReponse();
			while (NULL != pb) {
				if (returnResponse(*pb, returnToSender) == false) {
					printf("###\n");
					return false;
				}
				responded = true;

				delete pb;
				pb = replier->getNextReponse();
			}
		}

		if (false == responded) {
			response.clear();
			response.addVocab(Vocab::encode("fail"));
			response.write(*returnToSender);
		}
		return true;
	}
}

bool RpcFilterImpl::setupOutputConnection() {
	yarp::os::impl::NameClient& nic = yarp::os::impl::NameClient::getNameClient();
	yarp::os::impl::Address address = nic.queryName(targetPortName.c_str());
	if (!address.isValid()) {
		YARP_ERROR(yarp::os::impl::Logger::get(), "could not find port");
		return false;
	}

	m_out_prot = yarp::os::impl::Carriers::connect(address);
	if (m_out_prot == NULL) {
		ACE_OS::fprintf(stderr, "Cannot make connection\n");
		return false;
	}
	printf("RPC connection to %s at %s (connection name %s)\n",
			targetPortName.c_str(), address.toString().c_str(),
			connectionName.c_str());

	m_out_route = yarp::os::impl::Route(connectionName.c_str(),
			targetPortName.c_str(), "text_ack");
	return m_out_prot->open(m_out_route);
}

bool RpcFilterImpl::forwardCall(Bottle call, ConnectionWriter *responseWriter) {
	bool adminMode = false;

	if (m_out_prot == NULL) {
		if (setupOutputConnection() == false) {
			return false;
		}
	}

	yarp::os::impl::OutputStream&				os = m_out_prot->getOutputStream();
	yarp::os::impl::InputStream&				is = m_out_prot->getInputStream();
	yarp::os::impl::StreamConnectionReader		reader;
	yarp::os::impl::PortCommand					pc(0, adminMode ? "a" : "d");
	yarp::os::impl::BufferedConnectionWriter	bw(m_out_prot->isTextMode());

	bool ok = pc.write(bw);
	if (!ok) {
		ACE_OS::fprintf(stderr, "Cannot write on connection\n");
		if (m_out_prot != NULL)
			delete m_out_prot;
		return false;
	}
	ok = call.write(bw);
	if (!ok) {
		ACE_OS::fprintf(stderr, "Cannot write on connection\n");
		if (m_out_prot != NULL)
			delete m_out_prot;
		return false;
	}

	bw.write(os);

	Bottle resp;
	TextReader formattedResp;
	reader.reset(is, NULL, m_out_route, 0, true);

	bool done = false;
	bool err = false;

	while (!done) {
		if (reader.isError()) {
			err = true;
			done = true;
			break;
		}
		if (reader.isTextMode()) {
			formattedResp.read(reader);
			resp.fromString(formattedResp.str.c_str());
		} else {
			resp.read(reader);
		}

		if (yarp::os::impl::String(resp.get(0).asString()) == "<ACK>") {
			// exit loop !!!!!
			done = true;
		} else {
			if (NULL != responseObserver) {
				responseObserver->onDataObserved(resp);
			}

			if (responseWriter != NULL) {
				if (returnResponse(resp, responseWriter) == false) {
					return false;
				}
			}
		}
	}

	return true;
}

bool RpcFilterImpl::returnResponse(Bottle response,
		ConnectionWriter *responseWriter) {
	if (responseWriter == NULL) {
		return false;
	}

	Bottle resp;
	if (response.size() > 1) {
		Bottle v = resp.addList();
		for (int i = 0; i < response.size(); i++) {
			v.add(response.get(i));
		}
		v.write(*responseWriter);
	} else {
		resp.add(response.get(0));
		resp.write(*responseWriter);
	}

	return true;
}

yarp::os::ConstString RpcFilterImpl::connectionName = ConstString(
		"anon_rpcfilter");

