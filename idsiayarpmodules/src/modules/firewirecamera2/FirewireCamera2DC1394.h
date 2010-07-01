// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Alessandro Scalzo
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef __FIREWIRE_CAMERA2_H__
#define __FIREWIRE_CAMERA2_H__

#include <stdio.h>
#include <memory.h>

extern "C" {
#include <dc1394/dc1394.h>
}

#include "dc1394settings.h"

#define USED_ACQ_MODE DC1394_VIDEO_MODE_640x480_MONO8;
// the half, because of bayer mosaic structure
#define XDIM 320
#define YDIM 240

#define FW_UNINIT 0
#define FW_NORMAL 1
#define FW_HALF   2
#define FW_CUSTOM 3

class CFWCamera2 {
public:
	CDC1394Settings settings;

	CFWCamera2() :
		settings(), m_aXinc(NULL), m_aYinc(NULL), m_Xdim(0), m_Ydim(0),
				m_PlaneSize(0), m_RowLen(0), m_AcqMode(FW_UNINIT), m_pCamera(
						NULL) {
	}

	bool Create(dc1394camera_t *pCamera, int CameraId,
			int xdim = XDIM, int ydim = YDIM) {
		if (m_AcqMode != FW_UNINIT) {
			printf("Camera %d already open\n", CameraId);
			fflush(stdout);
			return false;
		}

		settings.Create(pCamera);

		m_pCamera = pCamera;
		m_Xdim = xdim;
		m_Ydim = ydim;
		m_PlaneSize = m_Xdim * m_Ydim;
		m_RowLen = XDIM * 2;

		dc1394error_t err;
		err = dc1394_video_set_iso_speed(m_pCamera, DC1394_ISO_SPEED_400);
		DC1394_ERR_CLN_RTN(err, Close(), "Could not set iso speed\n");

		dc1394video_mode_t mode = USED_ACQ_MODE;
		err = dc1394_video_set_mode(m_pCamera, mode);
		DC1394_ERR_CLN_RTN(err, Close(), "Could not set video mode\n");

		err = dc1394_video_set_framerate(m_pCamera, DC1394_FRAMERATE_30);
		DC1394_ERR_CLN_RTN(err, Close(), "Could not set framerate\n");

		uint_t num_dma_buffers = 4;
		err = dc1394_capture_setup(m_pCamera, num_dma_buffers,
				DC1394_CAPTURE_FLAGS_DEFAULT);
		DC1394_ERR_CLN_RTN(err, Close(), "Could not setup capture\n");

		// Start isochronous data transmission
		err = dc1394_video_set_transmission(m_pCamera, DC1394_ON);
		DC1394_ERR_CLN_RTN(err, Close(), "Could not start camera iso transmission\n");

		if (xdim == XDIM && ydim == YDIM) {
			m_aXinc = m_aYinc = NULL;
			m_AcqMode = FW_NORMAL;
		} else if (xdim << 1 == XDIM && ydim << 1 == YDIM) {
			m_aXinc = m_aYinc = NULL;
			m_AcqMode = FW_HALF;
		} else {
			m_aXinc = new int[xdim];
			m_aYinc = new int[ydim];
			m_AcqMode = FW_CUSTOM;

			double dmx = double(XDIM << 1) / double(xdim);

			for (int x = 0; x < xdim; ++x)
				if ((m_aXinc[x] = int(double(x) * dmx)) & 0x01)
					--m_aXinc[x];

			for (int x = 0; x < xdim - 1; ++x)
				m_aXinc[x] = m_aXinc[x + 1] - m_aXinc[x];

			double dmy = double(YDIM << 1) / double(ydim);

			for (int y = 0; y < ydim; ++y)
				if ((m_aYinc[y] = int(double(y) * dmy)) & 0x01)
					--m_aYinc[y];

			for (int y = 0; y < ydim - 1; ++y)
				m_aYinc[y] = m_aYinc[y + 1] - m_aYinc[y];
		}

		return true;
	}

	bool Close() {
		if (m_AcqMode == FW_UNINIT)
			return false;

		m_AcqMode = FW_UNINIT;

		dc1394_video_set_transmission(m_pCamera, DC1394_OFF);
		dc1394_capture_stop(m_pCamera);
		dc1394_camera_free(m_pCamera);

		if (m_aXinc)
			delete[] m_aXinc;
		if (m_aYinc)
			delete[] m_aYinc;

		m_aXinc = m_aYinc = NULL;

		return true;
	}

	~CFWCamera2() {
		Close();
	}

	void GetCameraInfo() {
		if (m_AcqMode == FW_UNINIT)
			return;

		dc1394featureset_t feature_set;
		dc1394_feature_get_all(m_pCamera, &feature_set);

		printf("\n\n");
		dc1394_feature_print_all(&feature_set, stdout);
		printf("\n\n");

		dc1394speed_t speed;
		if (dc1394_video_get_iso_speed(m_pCamera, &speed) != DC1394_SUCCESS)
			printf("Can't get speed\n");

		if (dc1394_video_set_iso_speed(m_pCamera, speed) != DC1394_SUCCESS)
			printf("Can't set speed\n");

		fflush(stdout);
	}

	bool Capture(unsigned char* image) {
		switch (m_AcqMode) {
		case FW_UNINIT:
			return false;

		case FW_NORMAL:
			return CaptureZoom100(image);

		case FW_HALF:
			return CaptureZoom50(image);

		case FW_CUSTOM:
			return CaptureCustomZoom(image);
		}

		return false;
	}

	bool CaptureRaw(unsigned char* image) {
		if (CaptureFrame() != DC1394_SUCCESS)
			return false;

		memcpy(image, m_pFrame->image, m_PlaneSize << 2);

		dc1394_capture_enqueue(m_pCamera, m_pFrame);

		return true;
	}

protected:
	dc1394camera_t *m_pCamera;
	dc1394video_frame_t *m_pFrame;

	int *m_aXinc, *m_aYinc;
	int m_Xdim, m_Ydim;
	int m_RowLen, m_PlaneSize;
	int m_AcqMode;

	bool CaptureZoom100(unsigned char* image) {
		if (m_AcqMode != FW_NORMAL)
			return false;

		if (CaptureFrame() != DC1394_SUCCESS)
			return false;

		unsigned char *pR = m_pFrame->image;
		unsigned char *pB = pR + m_RowLen;

		int x;

		for (int y = 0; y < m_Ydim; ++y) {
			for (x = 0; x < m_Xdim; ++x) {
				*image++ = *pR++;
				*image++ = (int(*pR++) + int(*pB++)) >> 1;
				*image++ = *pB++;
			}

			pR += m_RowLen;
			pB += m_RowLen;
		}

		dc1394_capture_enqueue(m_pCamera, m_pFrame);

		return true;
	}

	bool CaptureZoom50(unsigned char* image) {
		if (m_AcqMode != FW_HALF)
			return false;

		if (CaptureFrame() != DC1394_SUCCESS)
			return false;

		int _3_row_len = 3 * m_RowLen;

		unsigned char *pRin1 = m_pFrame->image, *pRin2 = pRin1 + 2, *pRin3 = pRin1 + 2
				* m_RowLen, *pRin4 = pRin2 + 2 * m_RowLen;
		unsigned char *pBin1 = pRin1 + m_RowLen, *pBin2 = pBin1 + 2, *pBin3 =
				pBin1 + 2 * m_RowLen, *pBin4 = pBin2 + 2 * m_RowLen;

		for (int y = 0; y < m_Ydim; ++y) {
			for (int x = 0; x < m_Xdim; ++x) {
				*image++ = (int(*pRin1++) + int(*pRin2++) + int(*pRin3++)
						+ int(*pRin4++)) >> 2; //red
				*image++ = (int(*pRin1++) + int(*pRin2++) + int(*pRin3++)
						+ int(*pRin4++) + int(*pBin1++) + int(*pBin2++)
						+ int(*pBin3++) + int(*pBin4++)) >> 3; //green
				*image++ = (int(*pBin1++) + int(*pBin2++) + int(*pBin3++)
						+ int(*pBin4++)) >> 2; //blue

				pRin1 += 2;
				pRin2 += 2;
				pRin3 += 2;
				pRin4 += 2;
				pBin1 += 2;
				pBin2 += 2;
				pBin3 += 2;
				pBin4 += 2;
			}

			pRin1 += _3_row_len;
			pRin2 += _3_row_len;
			pRin3 += _3_row_len;
			pRin4 += _3_row_len;
			pBin1 += _3_row_len;
			pBin2 += _3_row_len;
			pBin3 += _3_row_len;
			pBin4 += _3_row_len;
		}

		dc1394_capture_enqueue(m_pCamera, m_pFrame);

		return true;
	}

	bool CaptureCustomZoom(unsigned char *image) {
		if (m_AcqMode != FW_CUSTOM)
			return false;

		if (CaptureFrame() != DC1394_SUCCESS)
			return false;

		int row_len = XDIM << 1;
		int col_len = YDIM << 1;

		double dmy = double(col_len) / double(m_Ydim);

		int inc, yin;

		unsigned char *pRin, *pGin1, *pGin2, *pBin;

		for (int y = 0; y < m_Ydim; ++y) {
			if ((yin = int(double(y) * dmy)) & 0x01)
				--yin;

			pRin = m_pFrame->image + yin * row_len;

			pGin1 = pRin + 1;
			pGin2 = pRin + row_len;
			pBin = pGin2 + 1;

			for (int x = 0; x < m_Xdim; ++x) {
				*image++ = *pRin; //red
				*image++ = (int(*pGin1) + int(*pGin2)) >> 1; //green
				*image++ = *pBin; //blue

				pRin += (inc = m_aXinc[x]);
				pBin += inc;
				pGin1 += inc;
				pGin2 += inc;
			}
		}

		dc1394_capture_enqueue(m_pCamera, m_pFrame);

		return true;
	}

	int CaptureFrame() {
		dc1394capture_policy_t policy = DC1394_CAPTURE_POLICY_WAIT;		// TODO: is this correct?
		dc1394error_t err;
		err = dc1394_capture_dequeue(m_pCamera, policy, &m_pFrame);
		DC1394_ERR_RTN(err, "Could not dequeue\n");

		return DC1394_SUCCESS;
	}
};

#endif
