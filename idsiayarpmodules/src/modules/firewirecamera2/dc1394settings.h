// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Alessandro Scalzo
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef __YARP_DC1394_SETTINGS_H__
#define __YARP_DC1394_SETTINGS_H__

extern "C" {
#include <dc1394/dc1394.h>
}

#include <cstdio>

#define uint_t   unsigned int
#define uint16_t unsigned short int
#define uint64_t unsigned long long int
#define uchar_t  unsigned char

class CDC1394Settings {
public:
	CDC1394Settings() :
		m_pCamera(NULL) {
	}

	void Create(dc1394camera_t *pCamera) {
		m_pCamera = pCamera;

		dc1394error_t error;
		error
				= dc1394_feature_get_boundaries(m_pCamera,
						DC1394_FEATURE_BRIGHTNESS, &m_iMinBrightness,
						&m_iMaxBrightness);
		error = dc1394_feature_get_boundaries(m_pCamera,
				DC1394_FEATURE_EXPOSURE, &m_iMinExposure, &m_iMaxExposure);
		error = dc1394_feature_get_boundaries(m_pCamera,
				DC1394_FEATURE_SHARPNESS, &m_iMinSharpness, &m_iMaxSharpness);
		error = dc1394_feature_get_boundaries(m_pCamera,
				DC1394_FEATURE_WHITE_BALANCE, &m_iMinWB, &m_iMaxWB);
		error = dc1394_feature_get_boundaries(m_pCamera, DC1394_FEATURE_HUE,
				&m_iMinHue, &m_iMaxHue);
		error
				= dc1394_feature_get_boundaries(m_pCamera,
						DC1394_FEATURE_SATURATION, &m_iMinSaturation,
						&m_iMaxSaturation);
		error = dc1394_feature_get_boundaries(m_pCamera, DC1394_FEATURE_GAMMA,
				&m_iMinGamma, &m_iMaxGamma);
		error = dc1394_feature_get_boundaries(m_pCamera,
				DC1394_FEATURE_SHUTTER, &m_iMinShutter, &m_iMaxShutter);
		error = dc1394_feature_get_boundaries(m_pCamera, DC1394_FEATURE_GAIN,
				&m_iMinGain, &m_iMaxGain);
		error = dc1394_feature_get_boundaries(m_pCamera, DC1394_FEATURE_IRIS,
				&m_iMinIris, &m_iMaxIris);

		SetAutoBrightness(false);
		SetAutoGain(false);
		SetAutoShutter(false);
		SetAutoWhiteBalance(false);

		// set to default values
		SetBrightness(0.5);
		SetExposure(0.5);
		SetSharpness(0.5);
		SetWhiteBalance(0.5, 0.5);
		SetHue(0.5);
		SetSaturation(0.5);
		SetGamma(0.5);
		SetShutter(0.5);
		SetGain(0.5);
		SetIris(0.5);
	}

	bool hasFeatureDC1394(int iFeature) {
		dc1394bool_t value;
		dc1394error_t error;
		error = dc1394_feature_is_present(m_pCamera, (dc1394feature_t) iFeature, &value);
		DC1394_WRN(error, "Could not determine presence of feature\n");
		return value;
	}

	bool setFeatureDC1394(int iFeature, double dValue) {
		dc1394error_t error;
		error = dc1394_feature_set_value(m_pCamera, (dc1394feature_t) iFeature, (uint_t) dValue);
		DC1394_WRN(error, "Could not set feature value\n");
		return error;
	}

	double getFeatureDC1394(int iFeature) {
		uint_t value;
		dc1394error_t error;
		error = dc1394_feature_get_value(m_pCamera, (dc1394feature_t) iFeature, &value);
		DC1394_WRN(error, "Could not get feature value\n");

		if (error != DC1394_SUCCESS) {
			value = -1;
		}

//		printf("#### The value of value is %d\n", value);

		return value;
	}



	void SetBrightness(double dBrightness) {
		uint_t value = NormToValue(dBrightness, m_iMinBrightness,
				m_iMaxBrightness);
		dc1394error_t error;
		error = dc1394_feature_set_value(m_pCamera, DC1394_FEATURE_BRIGHTNESS,
				value);
	}

	void SetExposure(double dExposure) {
		uint_t value = NormToValue(dExposure, m_iMinExposure, m_iMaxExposure);
		dc1394error_t error;
		error = dc1394_feature_set_value(m_pCamera, DC1394_FEATURE_EXPOSURE,
				value);
	}

	void SetSharpness(double dSharpness) {
		uint_t value =
				NormToValue(dSharpness, m_iMinSharpness, m_iMaxSharpness);
		dc1394error_t error;
		error = dc1394_feature_set_value(m_pCamera, DC1394_FEATURE_SHARPNESS,
				value);
	}

	void SetWhiteBalance(double dUB, double dVR) {
		uint_t u_b_value = NormToValue(dUB, m_iMinWB, m_iMaxWB);
		uint_t v_r_value = NormToValue(dVR, m_iMinWB, m_iMaxWB);
		dc1394error_t error;
		error = dc1394_feature_whitebalance_set_value(m_pCamera, u_b_value,
				v_r_value);
	}

	void SetHue(double dHue) {
		uint_t value = NormToValue(dHue, m_iMinHue, m_iMaxHue);
		dc1394error_t error;
		error = dc1394_feature_set_value(m_pCamera, DC1394_FEATURE_HUE, value);
	}

	void SetSaturation(double dSaturation) {
		uint_t value = NormToValue(dSaturation, m_iMinSaturation,
				m_iMaxSaturation);
		dc1394error_t error;
		error = dc1394_feature_set_value(m_pCamera, DC1394_FEATURE_SATURATION,
				value);
	}

	void SetGamma(double dGamma) {
		uint_t value = NormToValue(dGamma, m_iMinGamma, m_iMaxGamma);
		dc1394error_t error;
		error
				= dc1394_feature_set_value(m_pCamera, DC1394_FEATURE_GAMMA,
						value);
	}

	void SetShutter(double dShutter) {
		uint_t value = NormToValue(dShutter, m_iMinShutter, m_iMaxShutter);
		dc1394error_t error;
		error = dc1394_feature_set_value(m_pCamera, DC1394_FEATURE_SHUTTER,
				value);
	}

	void SetGain(double dGain) {
		uint_t value = NormToValue(dGain, m_iMinGain, m_iMaxGain);
		dc1394error_t error;
		error = dc1394_feature_set_value(m_pCamera, DC1394_FEATURE_GAIN, value);
	}

	void SetIris(double dIris) {
		uint_t value = NormToValue(dIris, m_iMinIris, m_iMaxIris);
		dc1394error_t error;
		error = dc1394_feature_set_value(m_pCamera, DC1394_FEATURE_IRIS, value);
	}

	void SetAutoBrightness(bool bAuto = true) {
		dc1394feature_mode_t mode;
		if (bAuto == true) {
			mode = DC1394_FEATURE_MODE_AUTO;
		} else {
			mode = DC1394_FEATURE_MODE_MANUAL;
		}
		dc1394error_t error;
		error = dc1394_feature_set_mode(m_pCamera, DC1394_FEATURE_BRIGHTNESS,
				mode);
	}

	void SetAutoGain(bool bAuto = true) {
		dc1394feature_mode_t mode;
		if (bAuto == true) {
			mode = DC1394_FEATURE_MODE_AUTO;
		} else {
			mode = DC1394_FEATURE_MODE_MANUAL;
		}
		dc1394error_t error;
		error = dc1394_feature_set_mode(m_pCamera, DC1394_FEATURE_GAIN, mode);
	}

	void SetAutoShutter(bool bAuto = true) {
		dc1394feature_mode_t mode;
		if (bAuto == true) {
			mode = DC1394_FEATURE_MODE_AUTO;
		} else {
			mode = DC1394_FEATURE_MODE_MANUAL;
		}
		dc1394error_t error;
		error
				= dc1394_feature_set_mode(m_pCamera, DC1394_FEATURE_SHUTTER,
						mode);
	}

	//	void SetAutoWhiteBalance(bool bAuto = true) {
	//		if (!m_pHandle)
	//			return;
	//
	//		dc1394_auto_on_off(m_pHandle, m_CameraNode, FEATURE_WHITE_BALANCE,
	//				bAuto);
	//	}
	void SetAutoWhiteBalance(bool bAuto = true) {
		dc1394feature_mode_t mode;
		if (bAuto == true) {
			mode = DC1394_FEATURE_MODE_AUTO;
		} else {
			mode = DC1394_FEATURE_MODE_MANUAL;
		}
		dc1394error_t error;
		error = dc1394_feature_set_mode(m_pCamera,
				DC1394_FEATURE_WHITE_BALANCE, mode);
	}

	//	double GetBrightness() {
	//		unsigned int iVal;
	//		dc1394_get_brightness(m_pHandle, m_CameraNode, &iVal);
	//		return m_dBrightness = ValueToNorm(iVal, m_iMinBrightness,
	//				m_iMaxBrightness);
	//	}
	double GetBrightness() {
		uint_t value;
		dc1394error_t error;
		error = dc1394_feature_get_value(m_pCamera, DC1394_FEATURE_BRIGHTNESS,
				&value);
		return m_dBrightness = ValueToNorm(value, m_iMinBrightness,
				m_iMaxBrightness);
	}

	double GetExposure() {
		uint_t value;
		dc1394error_t error;
		error = dc1394_feature_get_value(m_pCamera, DC1394_FEATURE_EXPOSURE,
				&value);
		return m_dExposure = ValueToNorm(value, m_iMinExposure, m_iMaxExposure);
	}

	double GetSharpness() {
		uint_t value;
		dc1394error_t error;
		error = dc1394_feature_get_value(m_pCamera, DC1394_FEATURE_SHARPNESS,
				&value);
		return m_dSharpness = ValueToNorm(value, m_iMinSharpness,
				m_iMaxSharpness);
	}

	//    void GetWhiteBalance(double &dUB,double& dVR)
	//    {
	//        unsigned int iUB,iVR;
	//        dc1394_get_white_balance(m_pHandle,m_CameraNode,&iUB,&iVR);
	//        m_dUB=dUB=ValueToNorm(iUB,m_iMinWB,m_iMaxWB);
	//        m_dVR=dVR=ValueToNorm(iVR,m_iMinWB,m_iMaxWB);
	//    }
	void GetWhiteBalance(double &dUB, double& dVR) {
		uint_t u_b_value;
		uint_t v_r_value;
		dc1394error_t error;
		error = dc1394_feature_whitebalance_get_value(m_pCamera, &u_b_value,
				&v_r_value);
		m_dUB = dUB = ValueToNorm(u_b_value, m_iMinWB, m_iMaxWB);
		m_dVR = dVR = ValueToNorm(v_r_value, m_iMinWB, m_iMaxWB);
	}

	double GetHue() {
		uint_t value;
		dc1394error_t error;
		error = dc1394_feature_get_value(m_pCamera, DC1394_FEATURE_HUE, &value);
		return m_dHue = ValueToNorm(value, m_iMinHue, m_iMaxHue);
	}

	double GetSaturation() {
		uint_t value;
		dc1394error_t error;
		error = dc1394_feature_get_value(m_pCamera, DC1394_FEATURE_SATURATION,
				&value);
		return m_dSaturation = ValueToNorm(value, m_iMinSaturation,
				m_iMaxSaturation);
	}

	double GetGamma() {
		uint_t value;
		dc1394error_t error;
		error = dc1394_feature_get_value(m_pCamera, DC1394_FEATURE_GAMMA,
				&value);
		return m_dGamma = ValueToNorm(value, m_iMinGamma, m_iMaxGamma);
	}

	//	double GetShutter() {
	//		unsigned int iVal;
	//		dc1394_get_shutter(m_pHandle, m_CameraNode, &iVal);
	//		return m_dShutter = ValueToNorm(iVal, m_iMinShutter, m_iMaxShutter);
	//	}
	double GetShutter() {
		uint_t value;
		dc1394error_t error;
		error = dc1394_feature_get_value(m_pCamera, DC1394_FEATURE_SHUTTER,
				&value);
		return m_dShutter = ValueToNorm(value, m_iMinShutter, m_iMaxShutter);
	}

	//	double GetGain() {
	//		unsigned int iVal;
	//		dc1394_get_gain(m_pHandle, m_CameraNode, &iVal);
	//		return m_dGain = ValueToNorm(iVal, m_iMinGain, m_iMaxGain);
	//	}
	double GetGain() {
		uint_t value;
		dc1394error_t error;
		error
				= dc1394_feature_get_value(m_pCamera, DC1394_FEATURE_GAIN,
						&value);
		return m_dGain = ValueToNorm(value, m_iMinGain, m_iMaxGain);
	}

	double GetIris() {
		uint_t value;
		dc1394error_t error;
		error
				= dc1394_feature_get_value(m_pCamera, DC1394_FEATURE_IRIS,
						&value);
		return m_dIris = ValueToNorm(value, m_iMinIris, m_iMaxIris);
	}

	void PrintSettings() {
		double dUB, dVR;
		GetBrightness();
		GetExposure();
		GetSharpness();
		GetWhiteBalance(dUB, dVR);
		GetHue();
		GetSaturation();
		GetGamma();
		GetShutter();
		GetGain();
		GetIris();

		printf("Brightness = %lf\n", m_dBrightness);
		printf("Exposure   = %lf\n", m_dExposure);
		printf("Sharpness  = %lf\n", m_dSharpness);
		printf("White Bal  = %lf %lf\n", m_dUB, m_dVR);
		printf("Hue        = %lf\n", m_dHue);
		printf("Saturation = %lf\n", m_dSaturation);
		printf("Gamma      = %lf\n", m_dGamma);
		printf("Shutter    = %lf\n", m_dShutter);
		printf("Gain       = %lf\n", m_dGain);
		printf("Iris       = %lf\n", m_dIris);
		fflush(stdout);
	}

protected:
	//	raw1394handle_t m_pHandle;
	//	nodeid_t m_CameraNode;
	dc1394camera_t *m_pCamera;

	double m_dBrightness, m_dExposure, m_dSharpness, m_dUB, m_dVR, m_dHue,
			m_dSaturation, m_dGamma, m_dShutter, m_dGain, m_dIris;
	unsigned int m_iMinBrightness, m_iMinExposure, m_iMinSharpness, m_iMinWB,
			m_iMinHue, m_iMinSaturation, m_iMinGamma, m_iMinShutter, m_iMinGain, m_iMinIris;
	unsigned int m_iMaxBrightness, m_iMaxExposure, m_iMaxSharpness, m_iMaxWB,
			m_iMaxHue, m_iMaxSaturation, m_iMaxGamma, m_iMaxShutter, m_iMaxGain, m_iMaxIris;

	unsigned int NormToValue(double& dVal, unsigned int iMin, unsigned int iMax) {
		if (dVal < 0.0)
			dVal = 0.0;
		if (dVal > 1.0)
			dVal = 1.0;

		unsigned int iVal = iMin + (unsigned int) (dVal * double(iMax - iMin));

		if (iVal < iMin)
			iVal = iMin;
		if (iVal > iMax)
			iVal = iMax;

		return iVal;
	}

	double ValueToNorm(unsigned int iVal, unsigned int iMin, unsigned int iMax) {
		double dVal = double(iVal - iMin) / double(iMax - iMin);

		if (dVal < 0.0)
			return 0.0;
		if (dVal > 1.0)
			return 1.0;

		return dVal;
	}
};

#endif
