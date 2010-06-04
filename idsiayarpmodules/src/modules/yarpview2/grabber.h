#ifndef GRABBER_ISDEF
#define GRABBER_ISDEF

#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/FrameGrabberInterfaces.h>

#include "grabberfeature.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <vector>
#include <string>

using namespace std;

class GrabberError {
public:
	GrabberError(std::string s) {
		message = s;
	}

	const std::string& getMessage() {
		return message;
	}

private:
	std::string message;
};

class Grabber {
public:
	Grabber(yarp::dev::PolyDriver& driver);
	~Grabber();

	//	static void ListCameras(std::vector<std::pair<uint64_t,uint16_t> > &list);
	//	static std::string GetUniqueName(uint64_t guid,uint16_t unit,bool use_unit);
	//	static void GetMode7Info(uint64_t guid,uint16_t unit,bool use_unit,dc1394video_mode_t mode,vector<dc1394color_coding_t> &coding, uint32_t *h_size,uint32_t *v_size);
	//	static unsigned int GetDepthFromColorCoding(dc1394color_coding_t color_coding);
	//	static std::string GetModeName(dc1394video_mode_t mode,dc1394color_coding_t *coding=0, uint32_t *h_size=0,uint32_t *v_size=0);
	//	static std::string GetFramerateName(dc1394framerate_t framerate);
	//	static std::string GetColorCodingName(uint32_t color_id);
	//	static void ListCameraModes(uint64_t guid,uint16_t unit,bool use_unit,std::vector<dc1394video_mode_t> &vlist);
	//	static void ListCameraFeatures(uint64_t guid,uint16_t unit,bool use_unit,vector<CamFeature> &features);
	//	static void ListCameraFeatures(dc1394camera_t *camera,vector<CamFeature> &features);
	//	static void ListCameraFramerates(uint64_t guid,uint16_t unit,bool use_unit,dc1394video_mode_t mode, std::vector<dc1394framerate_t> &vlist);
	//	static void PrintCameraFramerates(uint64_t guid,uint16_t unit,bool use_unit,dc1394video_mode_t mode);
	//	static void PrintCameraFeatures(uint64_t guid,uint16_t unit,bool use_unit);
	//	static void PrintCameraFeatures(const vector<CamFeature> &features);
	//	static void PrintColorCoding( uint32_t color_id );
	//	static void PrintOtherInfo(uint64_t guid,uint16_t unit,bool use_unit);
	//	static void PrintCameraModes(uint64_t guid,uint16_t unit,bool use_unit);
	//	static void PrintModeInfo(dc1394video_mode_t mode );
	//	static void PrintExtModeInfo( dc1394camera_t *camera , dc1394video_mode_t mode );
	//	static dc1394camera_t *OpenCamera(uint64_t guid,uint16_t unit,bool use_unit);
	//	static void CloseCamera(dc1394camera_t *);
	//	void startTransmission();
	//	void stopTransmission();
	//	dc1394video_frame_t* captureFrame();
	//	void releaseFrame( dc1394video_frame_t* frame);
	//	static char* GetFeatureName( dc1394feature_t feature );
	//	static void PrintFormat( uint32_t format );
	//	static void PrintFeature( dc1394feature_t feature );
	//	uint32_t get_num_pixels(dc1394camera_t *camera, uint32_t format );
	//	void saveAs(dc1394video_frame_t* frame,const char* filename);
	//	unsigned int getImageHeight() ;
	//	unsigned int getImageWidth() ;
	//	unsigned int getImageDepth() ;
	//	dc1394color_coding_t getImageEncoding() ;

	void setMode(GrabberFeature::Feature feature, GrabberFeature::FeatureMode mode);
	void setPower(GrabberFeature::Feature feature, GrabberFeature::FeatureSwitch pwr);
	void setRelativeValue(GrabberFeature::Feature feature, double value);
	void setRelativeValues(GrabberFeature::Feature feature, double value0, double value1, double value2);

	void getRelativeValues(GrabberFeature::Feature, double *value0,
			double *value1, double *value2);

	const vector<GrabberFeature> & getFeatures();
	void listFeatures();

	//	void setValues(dc1394feature_t feature, uint32_t value[3]);
	//	void setValues(dc1394feature_t feature, uint32_t value0, uint32_t value1, uint32_t value2);
	//	void getValues(dc1394feature_t feature, uint32_t value[3]);
	//	void getValues(dc1394feature_t feature, uint32_t *value0, uint32_t *value1, uint32_t *value2);
	//
	//  static dc1394_t *dc1394;
	//	const vector<CamFeature> & getFeatures();
	//	static dc1394speed_t GetISOSpeed(uint64_t guid,uint16_t unit,bool use_unit);
	//	static dc1394speed_t GetISOSpeed(dc1394camera_t * camera);
	//	dc1394speed_t getISOSpeed();
	//	static void SetISOSpeed(uint64_t guid,uint16_t unit,bool use_unit,dc1394speed_t speed);
	//	static void SetISOSpeed(dc1394camera_t * camera,dc1394speed_t speed);
	//	void setISOSpeed(dc1394speed_t speed);

private:
	yarp::dev::PolyDriver& driver;
	yarp::dev::IFrameGrabberControls *fgCtrls;
	vector<GrabberFeature> features;

	//	void cleanup();
	//	static void Cleanup(dc1394camera_t *camera);
	//	dc1394camera_t *m_camera;
	//	dc1394video_mode_t m_selected_mode;
	//	static dc1394error_t FeatureHasAutoMode(dc1394camera_t *camera, dc1394feature_t feature,dc1394bool_t* b);
	//	static dc1394error_t FeatureHasManualMode(dc1394camera_t *camera, dc1394feature_t feature,dc1394bool_t* b);
	//	static dc1394error_t FeatureHasOnePushAuto(dc1394camera_t *camera, dc1394feature_t feature,dc1394bool_t* b);
	//	vector<CamFeature> m_features;
};
#endif
