#include <QtDebug>
#include <QtGui>

#include <yarp/os/all.h>
#include <yarp/os/ControlBoardFilter.h>
#include <iostream>

#include "SimpleQtObserver.h"
#include "SimpleQtSimulator.h"

using namespace std;
using namespace yarp::os;

static bool isCut = false;

int main(int argc, char **argv) {
	qDebug() << "enter main";
	
	Network yarp; 

	QApplication app(argc, argv);

	QWidget widget;

	// state:i
	QLabel *statPrompt = new QLabel("State:");
	statPrompt->setFixedWidth(75);
	statPrompt->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	QLabel *statText = new QLabel();
	statText->setFixedWidth(600);
	QHBoxLayout *statLayout = new QHBoxLayout();
	statLayout->addWidget(statPrompt);
	statLayout->addWidget(statText);

	// command:o
	QLabel *cmdPrompt = new QLabel("Command:");
	cmdPrompt->setFixedWidth(75);
	cmdPrompt->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	QLabel *cmdText = new QLabel();
	cmdText->setFixedWidth(600);
	QHBoxLayout *cmdLayout = new QHBoxLayout();
	cmdLayout->addWidget(cmdPrompt);
	cmdLayout->addWidget(cmdText);

	// rpc:call
	QLabel *callPrompt = new QLabel("Proc Call:");
	callPrompt->setFixedWidth(75);
	callPrompt->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	QLabel *callText = new QLabel();
	callText->setFixedWidth(600);
	QHBoxLayout *callLayout = new QHBoxLayout();
	callLayout->addWidget(callPrompt);
	callLayout->addWidget(callText);

	// rpc:response
	QLabel *respPrompt = new QLabel("Response:");
	respPrompt->setFixedWidth(75);
	respPrompt->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	QLabel *respText = new QLabel();
	respText->setFixedWidth(600);
	QHBoxLayout *respLayout = new QHBoxLayout();
	respLayout->addWidget(respPrompt);
	respLayout->addWidget(respText);

	// cut state
	QLabel *cutLabel = new QLabel(isCut == true ? "connection CUT" : "");
	cutLabel->setFixedWidth(100);
	QHBoxLayout *cutLayout = new QHBoxLayout();
	cutLayout->addStretch();
	cutLayout->addWidget(cutLabel);

	// main layout
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(statLayout);
	mainLayout->addLayout(cmdLayout);
	mainLayout->addLayout(callLayout);
	mainLayout->addLayout(respLayout);
	mainLayout->addLayout(cutLayout);

	widget.setLayout(mainLayout);

	// create the observers
	SimpleQtObserver *statObserver = new SimpleQtObserver(&app);
	SimpleQtObserver *cmdObserver = new SimpleQtObserver(&app);
//	SimpleQtObserver *callObserver = new SimpleQtObserver(&app);
	SimpleQtObserver *respObserver = new SimpleQtObserver(&app);

	// create the simulator
	SimpleQtSimulator *simulator = new SimpleQtSimulator(&app);

	// the filter
	yarp::os::ControlBoardFilter rightArmFilter;
	rightArmFilter.cutConnection(isCut);

	// set the initial observers
	rightArmFilter.setStateObserver(statObserver);
	rightArmFilter.setCommandObserver(cmdObserver);
//	rightArmFilter.setCallObserver(callObserver);
	rightArmFilter.setCallObserver(simulator);
	rightArmFilter.setResponseObserver(respObserver);
	rightArmFilter.setReplier(simulator);

	// enable the simulator to cut / connect the control board filter
	if (false == isCut) {
		simulator->setControlBoardFilter(&rightArmFilter);
	}

	rightArmFilter.open("/icubSimF/right_arm", "/icubSim/right_arm");
	//rightArmFilter.open("/katana400F/arm", "/katana400/arm");

	// connect the observers to the Qt labels
	QObject::connect(statObserver, SIGNAL(dataObserved(const QString&)),
			statText, SLOT(setText(const QString&)) );
	QObject::connect(cmdObserver, SIGNAL(dataObserved(const QString&)),
			cmdText, SLOT(setText(const QString&)) );
//	QObject::connect(callObserver, SIGNAL(dataObserved(const QString&)),
//			callText, SLOT(setText(const QString&)) );
	QObject::connect(simulator, SIGNAL(dataObserved(const QString&)),
			callText, SLOT(setText(const QString&)) );
	QObject::connect(respObserver, SIGNAL(dataObserved(const QString&)),
			respText, SLOT(setText(const QString&)) );
	QObject::connect(simulator, SIGNAL(cutStatusChanged(const QString&)),
			cutLabel, SLOT(setText(const QString&)) );

	widget.show();

	int result = app.exec();

	rightArmFilter.close();

	qDebug() << "leave main";
	return result;
}

