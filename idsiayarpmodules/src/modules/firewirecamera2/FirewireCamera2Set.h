// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Alessandro Scalzo
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef __YARP_FIREWIRE_CAMERA2_SET_H__
#define __YARP_FIREWIRE_CAMERA2_SET_H__

//extern "C"{
#include <dc1394/dc1394.h>
//}

#include "FirewireCamera2DC1394.h"

class CFWCamera2Set {
public:
	CFWCamera2Set() :
		m_pDc1394Context(NULL), m_pCameraList(NULL), m_apCamera(NULL),
				m_ActiveCams(0) {
	}

	virtual ~CFWCamera2Set() {
		Shutdown();
	}

	///////////////////////////////////////////////////////
	// Init/Close functions
	///////////////////////////////////////////////////////

	bool Init();
	void Shutdown();
	void Restart();
	bool StartCamera(int idCamera, int xdim = XDIM, int ydim = YDIM);
	void ShutdownCamera(int idCamera);
	void GetCameraInfo(int idCamera);

	///////////////////////////////////////////////////////
	// Capture functions
	///////////////////////////////////////////////////////

	bool Capture(int idCamera, unsigned char* image);
	bool CaptureRaw(int idCamera, unsigned char* image);

	///////////////////////////////////////////////////////
	// camera controls
	///////////////////////////////////////////////////////
	bool hasFeatureDC1394(int camera, int iFeature);
	bool setFeatureDC1394(int camera, int iFeature, double dValue);
	double getFeatureDC1394(int camera, int iFeature);

	bool SetBrightness(int camera, double dBrightness);
	bool SetExposure(int camera, double dExposure);
	bool SetSharpness(int camera, double dSharpness);
	bool SetWhiteBalance(int camera, double dUB, double dVR);
	bool SetHue(int camera, double dHue);
	bool SetSaturation(int camera, double dSaturation);
	bool SetGamma(int camera, double dGamma);
	bool SetShutter(int camera, double dShutter);
	bool SetGain(int camera, double dGain);
	bool SetIris(int camera, double dGain);


	double GetBrightness(int camera);
	double GetExposure(int camera);
	double GetSharpness(int camera);
	bool GetWhiteBalance(int camera, double& dUB, double& dVR);
	double GetHue(int camera);
	double GetSaturation(int camera);
	double GetGamma(int camera);
	double GetShutter(int camera);
	double GetGain(int camera);
	double GetIris(int camera);

	bool SetAutoShutter(int camera, bool bAuto = true);
	bool SetAutoGain(int camera, bool bAuto = true);
	bool SetAutoBrightness(int camera, bool bAuto = true);
	bool SetAutoWhiteBalance(int camera, bool bAuto = true);

	int GetCameraNum() {
		return m_ActiveCams;
	}

	void PrintSettings(int idCamera) {
		if (!IsCameraReady(idCamera))
			return;

		m_apCamera[idCamera]->settings.PrintSettings();
	}

protected:
	dc1394_t *m_pDc1394Context;
	dc1394camera_list_t *m_pCameraList;
	CFWCamera2** m_apCamera;

	int m_ActiveCams;

	bool IsCameraReady(int idCamera) {
		if ((m_pDc1394Context != NULL) && (idCamera >= 0) && (idCamera
				< m_pCameraList->num) && (m_apCamera != NULL)
				&& (m_apCamera[idCamera] != NULL)) {
			return true;
		} else {
			return false;
		}
	}

	unsigned char* CaptureFrame(int camera);

	void cleanup();
};

#endif
