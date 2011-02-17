// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Alessandro Scalzo
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <unistd.h>
#include <stdio.h>
#include "FirewireCamera2DC1394.h"
#include "FirewireCamera2Set.h"

///////////////////////////////////////////////////////
// Init/Close functions
///////////////////////////////////////////////////////

bool CFWCamera2Set::Init() {
	dc1394error_t err;

	if (m_pDc1394Context != NULL) {
		printf("ERROR: Firewire already open, shutdown first or restart\n");
		fflush(stdout);
		return false;
	}

	m_pDc1394Context = dc1394_new();

	if (m_pDc1394Context == NULL) {
		printf("Unable to aquire a dc1394 context\n");
		fflush(stdout);
		return false;
	}

	err = dc1394_camera_enumerate(m_pDc1394Context, &m_pCameraList);
	DC1394_ERR_CLN_RTN(err, cleanup(), "Failed to enumerate cameras");

	printf("Init Firewire OK\n");
	fflush(stdout);

	printf("%d cameras detected\n", m_pCameraList->num);

	m_apCamera = new CFWCamera2*[m_pCameraList->num];
	for (int cam = 0; cam < m_pCameraList->num; ++cam)
		m_apCamera[cam] = NULL;

	return true;
}

void CFWCamera2Set::Shutdown() {
	if (m_pDc1394Context == NULL)
		return;

	if (m_apCamera != NULL) {
		for (int cam = 0; cam < m_pCameraList->num; ++cam)
			ShutdownCamera(cam);

		delete[] m_apCamera;
		m_apCamera = NULL;
	}

	cleanup();

	m_ActiveCams = 0;
}

void CFWCamera2Set::Restart() {
	Shutdown();
	Init();
}

bool CFWCamera2Set::StartCamera(int idCamera, int xdim, int ydim) {
	if (idCamera < 0 || idCamera >= m_pCameraList->num) {
		printf("Invalid camera number\n");
		fflush(stdout);
		return false;
	}

	if (m_apCamera[idCamera] != NULL) {
		printf("ERROR: camera already started!\n");
		fflush(stdout);
		return false;
	}

	m_apCamera[idCamera] = new CFWCamera2();

	dc1394camera_t *pCamera;
	pCamera = dc1394_camera_new(m_pDc1394Context, m_pCameraList->ids[0].guid);
	if (pCamera == NULL) {
		printf("ERROR: can't retrieve camera %d from list\n", idCamera);
		fflush(stdout);
		return false;
	}

	bool bOk = m_apCamera[idCamera]->Create(pCamera, idCamera, xdim, ydim);

	if (bOk == false) {
		delete m_apCamera[idCamera];
		m_apCamera[idCamera] = NULL;
		printf("ERROR: can't start camera %d\n", idCamera);
		fflush(stdout);
		return false;
	}

	++m_ActiveCams;

	return true;
}

void CFWCamera2Set::ShutdownCamera(int idCamera) {
	if (IsCameraReady(idCamera)) {
		m_apCamera[idCamera]->Close();
		delete m_apCamera[idCamera];
		m_apCamera[idCamera] = NULL;
		--m_ActiveCams;
	}
}

void CFWCamera2Set::GetCameraInfo(int idCamera) {
	if (IsCameraReady(idCamera)) {
		m_apCamera[idCamera]->GetCameraInfo();
	}
}

///////////////////////////////////////////////////////
// Capture functions
///////////////////////////////////////////////////////

bool CFWCamera2Set::Capture(int idCamera, unsigned char* image) {
	if (!IsCameraReady(idCamera))
		return false;

	return m_apCamera[idCamera]->Capture(image);
}

bool CFWCamera2Set::CaptureRaw(int idCamera, unsigned char* image) {
	if (!IsCameraReady(idCamera))
		return false;

	return m_apCamera[idCamera]->CaptureRaw(image);
}

////////////////////////////////////////////////////////////////////////
// camera controls
////////////////////////////////////////////////////////////////////////

bool CFWCamera2Set::hasFeatureDC1394(int camera, int iFeature) {
	if (!IsCameraReady(camera))
		return false;

	return m_apCamera[camera]->settings.hasFeatureDC1394(iFeature);
}

bool CFWCamera2Set::setFeatureDC1394(int camera, int iFeature, double dValue) {
	if (!IsCameraReady(camera))
		return false;

	return m_apCamera[camera]->settings.setFeatureDC1394(iFeature, dValue);
}

double CFWCamera2Set::getFeatureDC1394(int camera, int iFeature) {
	if (!IsCameraReady(camera))
		return -1.0;

	return m_apCamera[camera]->settings.getFeatureDC1394(iFeature);
}


bool CFWCamera2Set::SetBrightness(int camera, double dBrightness) {
	if (!IsCameraReady(camera))
		return false;

	m_apCamera[camera]->settings.SetBrightness(dBrightness);

	return true;
}

bool CFWCamera2Set::SetExposure(int camera, double dExposure) {
	if (!IsCameraReady(camera))
		return false;

	m_apCamera[camera]->settings.SetExposure(dExposure);

	return true;
}

bool CFWCamera2Set::SetSharpness(int camera, double dSharpness) {
	if (!IsCameraReady(camera))
		return false;

	m_apCamera[camera]->settings.SetSharpness(dSharpness);

	return true;
}

bool CFWCamera2Set::SetWhiteBalance(int camera, double dUB, double dVR) {
	if (!IsCameraReady(camera))
		return false;

	m_apCamera[camera]->settings.SetWhiteBalance(dUB, dVR);

	return true;
}

bool CFWCamera2Set::SetHue(int camera, double dHue) {
	if (!IsCameraReady(camera))
		return false;

	m_apCamera[camera]->settings.SetHue(dHue);

	return true;
}

bool CFWCamera2Set::SetSaturation(int camera, double dSaturation) {
	if (!IsCameraReady(camera))
		return false;

	m_apCamera[camera]->settings.SetSaturation(dSaturation);

	return true;
}

bool CFWCamera2Set::SetGamma(int camera, double dGamma) {
	if (!IsCameraReady(camera))
		return false;

	m_apCamera[camera]->settings.SetGamma(dGamma);

	return true;
}

bool CFWCamera2Set::SetShutter(int camera, double dShutter) {
	if (!IsCameraReady(camera))
		return false;

	m_apCamera[camera]->settings.SetShutter(dShutter);

	return true;
}

bool CFWCamera2Set::SetGain(int camera, double dGain) {
	if (!IsCameraReady(camera))
		return false;

	m_apCamera[camera]->settings.SetGain(dGain);

	return true;
}

bool CFWCamera2Set::SetIris(int camera, double dIris) {
	if (!IsCameraReady(camera))
		return false;

	m_apCamera[camera]->settings.SetIris(dIris);

	return true;
}

double CFWCamera2Set::GetBrightness(int camera) {
	if (!IsCameraReady(camera))
		return -1.0;

	return m_apCamera[camera]->settings.GetBrightness();
}

double CFWCamera2Set::GetExposure(int camera) {
	if (!IsCameraReady(camera))
		return -1.0;

	return m_apCamera[camera]->settings.GetExposure();
}

double CFWCamera2Set::GetSharpness(int camera) {
	if (!IsCameraReady(camera))
		return -1.0;

	return m_apCamera[camera]->settings.GetSharpness();
}

bool CFWCamera2Set::GetWhiteBalance(int camera, double& dUB, double& dVR) {
	if (!IsCameraReady(camera))
		return false;

	m_apCamera[camera]->settings.GetWhiteBalance(dUB, dVR);

	return true;
}

double CFWCamera2Set::GetHue(int camera) {
	if (!IsCameraReady(camera))
		return -1.0;

	return m_apCamera[camera]->settings.GetHue();
}

double CFWCamera2Set::GetSaturation(int camera) {
	if (!IsCameraReady(camera))
		return -1.0;

	return m_apCamera[camera]->settings.GetSaturation();
}

double CFWCamera2Set::GetGamma(int camera) {
	if (!IsCameraReady(camera))
		return -1.0;

	return m_apCamera[camera]->settings.GetGamma();
}

double CFWCamera2Set::GetShutter(int camera) {
	if (!IsCameraReady(camera))
		return -1.0;

	return m_apCamera[camera]->settings.GetShutter();
}

double CFWCamera2Set::GetGain(int camera) {
	if (!IsCameraReady(camera))
		return -1.0;

	return m_apCamera[camera]->settings.GetGain();
}

double CFWCamera2Set::GetIris(int camera) {
	if (!IsCameraReady(camera))
		return -1.0;

	return m_apCamera[camera]->settings.GetIris();
}


bool CFWCamera2Set::SetAutoShutter(int camera, bool bAuto) {
	if (!IsCameraReady(camera))
		return false;

	m_apCamera[camera]->settings.SetAutoShutter(bAuto);

	return true;
}

bool CFWCamera2Set::SetAutoGain(int camera, bool bAuto) {
	if (!IsCameraReady(camera))
		return false;

	m_apCamera[camera]->settings.SetAutoGain(bAuto);

	return true;
}

bool CFWCamera2Set::SetAutoBrightness(int camera, bool bAuto) {
	if (!IsCameraReady(camera))
		return false;

	m_apCamera[camera]->settings.SetAutoBrightness(bAuto);

	return true;
}

bool CFWCamera2Set::SetAutoWhiteBalance(int camera, bool bAuto) {
	if (!IsCameraReady(camera))
		return false;

	m_apCamera[camera]->settings.SetAutoWhiteBalance(bAuto);

	return true;
}

void CFWCamera2Set::cleanup() {
	dc1394_camera_free_list(m_pCameraList);
	dc1394_free(m_pDc1394Context);
}
