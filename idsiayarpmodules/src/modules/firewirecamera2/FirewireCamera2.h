// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick, Giorgio Metta
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 */

//
// $Id: FirewireCamera.h,v 1.1 2008-03-19 15:04:52 eshuy Exp $
//
//

#ifndef __FirewireCamerah__
#define __FirewireCamerah__

//=============================================================================
// YARP Includes
//=============================================================================

// May 06, readapted for YARP2 by nat
// May 07, renamed FirewireCamera by nat

#include <yarp/os/Semaphore.h>
#include <yarp/os/Bottle.h>
#include <yarp/dev/FrameGrabberInterfaces.h>

namespace yarp {
	namespace dev {
		class FirewireCameraOpenParameters;
		class FirewireCamera2;
	}
}

/**
 * \file FirewireCamera.h device driver for managing the 
 * IEEE-1394 Camera
 */

/**
 * Structure for defining the open() parameters of the camera.
 */
class yarp::dev::FirewireCameraOpenParameters {
public:
	// Parameters
	unsigned int _unit_number;
	unsigned int _size_x;
	unsigned int _size_y;
	unsigned int _video_type;

	double _whiteR;
	double _whiteB;

	double _brightness;
	double _shutter;
	double _gain;

	bool _fleacr; //FLEA color reconstruction flag

	/**
	 * Constructor. Add here the parameters for the open().
	 */
	FirewireCameraOpenParameters() {
		// parameters initialization
		_unit_number = 0;
		_size_x = 640;
		_size_y = 480;
		_video_type = 0;

		//uninitialized - inherit registry stored values
		_brightness = -1;
		_shutter = -1;
		_gain = -1;
		_whiteR = -1;
		_whiteB = -1;

		// FLEA cameras are compatible with DRAGONFLY's but ...
		// the color reconstruction method is different 
		// (GBRG instead of RGGB)
		// The default is to use Dragonsfly's method
		_fleacr = false;

	}

};

/**
 * @ingroup dev_impl_media
 *
 * A generic firewire digital camera (or, on Linux, any digital camera).
 */
class yarp::dev::FirewireCamera2: public IFrameGrabber,
		public IFrameGrabberRgb,
		public IFrameGrabberImage,
		public IFrameGrabberControls,
		// public IFrameGrabberControlsDC1394,
		public DeviceDriver {
private:
	FirewireCamera2(const FirewireCamera2&);
	void operator=(const FirewireCamera2&);

public:
	/**
	 * Constructor.
	 */
	FirewireCamera2();

	/**
	 * Destructor.
	 */
	virtual ~FirewireCamera2();

	// temp: here for debug purposes only
	void recColorFSBilinear(const unsigned char *src, unsigned char *out);
	void recColorFSNN(const unsigned char *src, unsigned char *out);
	void recColorHSBilinear(const unsigned char *src, unsigned char *out);

	/**
	 * Open the device driver.
	 * @param par parameters for the device driver
	 * @return returns true on success, false on failure.
	 */
	bool open(const FirewireCameraOpenParameters& par);

	virtual bool open(yarp::os::Searchable& config) {
		FirewireCameraOpenParameters params;
		yarp::os::Value *value;
		if (config.check("unit_number", value) || config.check("d", value)) {
			params._unit_number = value->asInt();
		}
		if (config.check("size_x", value) || config.check("width", value)) {
			params._size_x = value->asInt();
		}
		if (config.check("size_y", value) || config.check("height", value)) {
			params._size_y = value->asInt();
		}
		params._video_type = config.find("video_type").asInt();

		//params._offset_y = config.find("offset_y").asInt();
		//params._offset_x = config.find("offset_x").asInt();
		//params._alfa = (float)config.find("alfa").asInt();
		yarp::os::Bottle& whites = config.findGroup("white_balance");
		if (!whites.isNull()) {
			params._whiteR = whites.get(1).asDouble();
			params._whiteB = whites.get(2).asDouble();
		}

		if (config.check("brightness", value)) {
			params._brightness = value->asDouble();
		}

		if (config.check("shutter", value)) {
			params._shutter = value->asDouble();
		}
		if (config.check("gain", value)) {
			params._gain = value->asDouble();
		}

		params._fleacr = config.check("flea",
				"If present indicates to use Flea color reconstruction ");

		return open(params);
	}

	/**
	 * Closes the device driver.
	 * @return returns true/false on success/failure.
	 */
	virtual bool close(void);

	/**
	 * Implements FrameGrabber basic interface.
	 * @param buffer the pointer to the array to store the last frame.
	 * @return returns true/false on success/failure.
	 */
	virtual bool getRawBuffer(unsigned char *buffer);

	/**
	 * Implements the Frame grabber basic interface.
	 * @return the size of the raw buffer (for the Dragonfly
	 * camera this is 1x640x480).
	 */
	virtual int getRawBufferSize();

	/**
	 * Implements FrameGrabber basic interface.
	 */
	virtual int height() const;

	/**
	 * Implements FrameGrabber basic interface.
	 */
	virtual int width() const;

	/**
	 * FrameGrabber rgb interface, returns the last acquired frame as
	 * a buffer of rgb triplets. A demosaicking method is applied to
	 * reconstuct the color from the Bayer pattern of the sensor.
	 * @param buffer pointer to the array that will contain the last frame.
	 * @return true/false upon success/failure
	 */
	virtual bool getRgbBuffer(unsigned char *buffer);

	/**
	 * FrameGrabber image interface, returns the last acquired frame as
	 * an rgb image. A demosaicking method is applied to
	 * reconstuct the color from the Bayer pattern of the sensor.
	 * @param image that will store the last frame.
	 * @return true/false upon success/failure
	 */
	virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image);

	// &&&&&&&&&&&&&&&&&&&&&&&&

	// checks whether the camera exposes the given feature
	// dc1394_is_feature_present -> dc1394_feature_is_present
	virtual bool hasFeatureDC1394(int feature);

	// dc1394_set_feature_value -> dc1394_feature_set_value
	virtual bool setFeatureDC1394(int feature, double value);

	// dc1394_get_feature_value -> dc1394_feature_get_value
	virtual double getFeatureDC1394(int feature);

//	// dc1394_can_turn_on_off -> dc1394_feature_is_switchable
//	virtual bool hasOnOffDC1394(int feature)=0;
//
//	// dc1394_feature_on_off -> dc1394_feature_set_power
//	virtual bool setActiveDC1394(int feature, bool onoff)=0;
//
//	// dc1394_is_feature_on -> dc1394_feature_get_power
//	virtual bool getActiveDC1394(int feature)=0;
//
//	// dc1394_has_auto_mode -> dc1394_feature_get_modes
//	virtual bool hasAutoDC1394(int feature)=0;
//
//	// dc1394_has_manual_mode -> dc1394_feature_get_modes
//	virtual bool hasManualDC1394(int feature)=0;
//
//	// dc1394_has_one_push_auto -> dc1394_feature_get_modes
//	virtual bool hasOnePushDC1394(int feature)=0;
//
//	// dc1394_auto_on_off -> dc1394_feature_set_mode
//	virtual bool setModeDC1394(int feature, bool auto_onoff)=0;
//
//	// dc1394_is_feature_auto -> dc1394_feature_get_mode
//	virtual bool getModeDC1394(int feature)=0;
//
//	// dc1394_start_one_push_operation -> dc1394_feature_set_mode
//	virtual bool setOnePushDC1394(int feature)=0;
//
//	// dc1394_query_supported_modes -> dc1394_video_get_supported_modes
//	virtual unsigned int getVideoModeMaskDC1394()=0;
//
//	// dc1394_get_video_mode -> dc1394_video_get_mode
//	virtual unsigned int getVideoModeDC1394()=0;
//
//	// dc1394_set_video_mode -> dc1394_video_set_mode
//	virtual bool setVideoModeDC1394(int video_mode)=0;
//
//	// dc1394_query_supported_framerates -> dc1394_video_get_supported_framerates
//	virtual unsigned int getFPSMaskDC1394()=0;
//
//	// dc1394_get_video_framerate -> dc1394_video_get_framerate
//	virtual unsigned int getFPSDC1394()=0;
//
//	// dc1394_set_video_framerate -> dc1394_video_set_framerate
//	virtual bool setFPSDC1394(int fps)=0;
//
//	// dc1394_get_iso_channel_and_speed -> dc1394_video_get_iso_speed
//	virtual unsigned int getISOSpeedDC1394()=0;
//
//	// dc1394_set_iso_channel_and_speed -> dc1394_video_set_iso_speed
//	virtual bool setISOSpeedDC1394(int speed)=0;
//
//	// ??
//	virtual unsigned int getColorCodingMaskDC1394(unsigned int video_mode)=0;
//
//	// ??
//	virtual unsigned int getColorCodingDC1394()=0;
//
//	// ??
//	virtual bool setColorCodingDC1394(int coding)=0;
//
//	// dc1394_set_white_balance -> dc1394_feature_whitebalance_set_value
//	virtual bool setWhiteBalanceDC1394(double b, double r)=0;
//
//	// dc1394_get_white_balance -> dc1394_feature_whitebalance_get_value
//	virtual bool getWhiteBalanceDC1394(double &b, double &r)=0;
//
//	// dc1394_query_format7_max_image_size -> dc1394_format7_get_max_image_size ???
//	virtual bool getFormat7MaxWindowDC1394(unsigned int &xdim,
//			unsigned int &ydim, unsigned int &xstep, unsigned int &ystep)=0;
//
//	// dc1394_query_format7_image_size -> dc1394_format7_get_image_size
//	virtual bool
//			getFormat7WindowDC1394(unsigned int &xdim, unsigned int &ydim)=0;
//
//	// dc1394_set_format7_image_size -> dc1394_format7_set_image_size
//	virtual bool setFormat7WindowDC1394(unsigned int xdim, unsigned int ydim)=0;
//
//	// dc1394_set_operation_mode -> dc1394_video_set_operation_mode
//	virtual bool setOperationModeDC1394(bool b1394b)=0;
//
//	// dc1394_get_operation_mode -> dc1394_video_get_operation_mode
//	virtual bool getOperationModeDC1394()=0;
//
//	// dc1394_start_iso_transmission -> dc1394_video_set_transmission ???
//	// dc1394_stop_iso_transmission -> dc1394_video_set_transmission ???
//	virtual bool setTransmissionDC1394(bool bTxON)=0;
//
//	// dc1394_get_iso_status -> dc1394_video_get_transmission ???
//	virtual bool getTransmissionDC1394()=0;
//
//	// ??? -> dc1394_camera_set_broadcast
//	virtual bool setBroadcastDC1394(bool onoff)=0;
//
//	// dc1394_init_camera -> dc1394_camera_reset ???
//	virtual bool setDefaultsDC1394()=0;
//
//	// dc1394_init_camera -> dc1394_camera_reset ???
//	virtual bool setResetDC1394()=0;
//
//	// dc1394_camera_on -> dc1394_camera_set_power
//	// dc1394_camera_off -> dc1394_camera_set_power
//	virtual bool setPowerDC1394(bool onoff)=0;
//
//	// dc1394_setup_capture -> *OBSOLETE*
//	virtual bool setCaptureDC1394(bool bON)=0;
//
//	// ???
//	virtual unsigned int getBytesPerPacketDC1394()=0;
//
//	// ???
//	virtual bool setBytesPerPacketDC1394(unsigned int bpp)=0;
//
//	// &&&&&&&&&&&&&&&&&&&&&&&&

	/**
	 * Set normalized brightness [0.0 : 1.0].
	 */
	virtual bool setBrightness(double v);

	/**
	 * Set normalized exposure [0.0 : 1.0].
	 */
	virtual bool setExposure(double v);

	/**
	 * Set normalized sharpness [0.0 : 1.0].
	 */
	virtual bool setSharpness(double v);

	/**
	 * Set normalized white balance [0.0 : 1.0].
	 */
	virtual bool setWhiteBalance(double blue, double red);

	/**
	 * Set normalized hue [0.0 : 1.0].
	 */
	virtual bool setHue(double v);

	/**
	 * Set normalized saturation [0.0 : 1.0].
	 */
	virtual bool setSaturation(double v);

	/**
	 * Set normalized gamma [0.0 : 1.0].
	 */
	virtual bool setGamma(double v);

	/**
	 * Set normalized shutter time [0.0 : 1.0].
	 */
	virtual bool setShutter(double v);

	/**
	 * Set normalized gain [0.0 : 1.0].
	 */
	virtual bool setGain(double v);

	/**
	 * Set normalized iris [0.0 : 1.0].
	 */
	virtual bool setIris(double v);

	/**
	 * Get normalized brightness [0.0 : 1.0].
	 */
	virtual double getBrightness();

	/**
	 * Get normalized exposure [0.0 : 1.0].
	 */
	virtual double getExposure();

	/**
	 * Get normalized sharpness [0.0 : 1.0].
	 */
	virtual double getSharpness();

	/**
	 * Get normalized white balance [0.0 : 1.0].
	 */
	virtual bool getWhiteBalance(double &blue, double &red);

	/**
	 * Get normalized hue [0.0 : 1.0].
	 */
	virtual double getHue();

	/**
	 * Get normalized saturation [0.0 : 1.0].
	 */
	virtual double getSaturation();

	/**
	 * Get normalized gamma [0.0 : 1.0].
	 */
	virtual double getGamma();

	/**
	 * Get normalized shutter [0.0 : 1.0].
	 */
	virtual double getShutter();

	/**
	 * Get normalized gain [0.0 : 1.0].
	 */
	virtual double getGain();

	/**
	 * Get normalized iris [0.0 : 1.0].
	 */
	virtual double getIris();

	virtual bool setAutoBrightness(bool bAuto = true);

	virtual bool setAutoGain(bool bAuto = true);

	virtual bool setAutoShutter(bool bAuto = true);

	virtual bool setAutoWhiteBalance(bool bAuto = true);

	virtual bool setAuto(bool bAuto = true);

	virtual void PrintSettings();

protected:
	void *system_resources;
};

/**
 * @ingroup dev_runtime
 * \defgroup cmd_device_firewirecamera firewirecamera

 A streaming digital camera source, see yarp::dev::FirewireCamera.

 */

#endif
