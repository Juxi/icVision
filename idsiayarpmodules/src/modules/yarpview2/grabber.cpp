#include "grabber.h"

Grabber::Grabber(yarp::dev::PolyDriver& driver) :
	driver(driver) {
	if (driver.view(fgCtrls) == false)
		exit(1);

	listFeatures();
}

Grabber::~Grabber() {
}

#define print_case(A) case A: printf(#A ""); break;

/*-----------------------------------------------------------------------
 *  Prints the type of format to standard out
 *-----------------------------------------------------------------------*/
//void Grabber::PrintFormat( uint32_t format )
//{
//    switch( format ) {
//        print_case(DC1394_VIDEO_MODE_160x120_YUV444);
//        print_case(DC1394_VIDEO_MODE_320x240_YUV422);
//        print_case(DC1394_VIDEO_MODE_640x480_YUV411);
//        print_case(DC1394_VIDEO_MODE_640x480_YUV422);
//        print_case(DC1394_VIDEO_MODE_640x480_RGB8);
//        print_case(DC1394_VIDEO_MODE_640x480_MONO8);
//        print_case(DC1394_VIDEO_MODE_640x480_MONO16);
//        print_case(DC1394_VIDEO_MODE_800x600_YUV422);
//        print_case(DC1394_VIDEO_MODE_800x600_RGB8);
//        print_case(DC1394_VIDEO_MODE_800x600_MONO8);
//        print_case(DC1394_VIDEO_MODE_1024x768_YUV422);
//        print_case(DC1394_VIDEO_MODE_1024x768_RGB8);
//        print_case(DC1394_VIDEO_MODE_1024x768_MONO8);
//        print_case(DC1394_VIDEO_MODE_800x600_MONO16);
//        print_case(DC1394_VIDEO_MODE_1024x768_MONO16);
//        print_case(DC1394_VIDEO_MODE_1280x960_YUV422);
//        print_case(DC1394_VIDEO_MODE_1280x960_RGB8);
//        print_case(DC1394_VIDEO_MODE_1280x960_MONO8);
//        print_case(DC1394_VIDEO_MODE_1600x1200_YUV422);
//        print_case(DC1394_VIDEO_MODE_1600x1200_RGB8);
//        print_case(DC1394_VIDEO_MODE_1600x1200_MONO8);
//        print_case(DC1394_VIDEO_MODE_1280x960_MONO16);
//        print_case(DC1394_VIDEO_MODE_1600x1200_MONO16);
//		print_case(DC1394_VIDEO_MODE_EXIF);
//		print_case(DC1394_VIDEO_MODE_FORMAT7_0);
//		print_case(DC1394_VIDEO_MODE_FORMAT7_1);
//		print_case(DC1394_VIDEO_MODE_FORMAT7_2);
//		print_case(DC1394_VIDEO_MODE_FORMAT7_3);
//		print_case(DC1394_VIDEO_MODE_FORMAT7_4);
//		print_case(DC1394_VIDEO_MODE_FORMAT7_5);
//		print_case(DC1394_VIDEO_MODE_FORMAT7_6);
//		print_case(DC1394_VIDEO_MODE_FORMAT7_7);
//
//    default:
//        dc1394_log_error("Unknown format\n");
//        exit(1);
//    }
//
//}

//void Grabber::GetMode7Info(uint64_t guid, uint16_t unit, bool use_unit,
//		dc1394video_mode_t mode, vector<dc1394color_coding_t> & codings,
//		uint32_t *h_size, uint32_t *v_size) {
//
//	if (mode < DC1394_VIDEO_MODE_FORMAT7_0) {
//		// TODO return GetModeName stuff??
//		codings.clear();
//		*h_size = 0;
//		*v_size = 0;
//		return;
//	};
//
//	dc1394error_t err;
//	dc1394camera_t *camera = OpenCamera(guid, unit, use_unit);
//
//	dc1394color_codings_t c;
//	err = dc1394_format7_get_color_codings(camera, mode, &c);
//	if (err != DC1394_SUCCESS) {
//		CloseCamera(camera);
//		throw CameraError("Could not get color codings");
//	};
//
//	codings.clear();
//	for (unsigned int i = 0; i < c.num; i++) {
//		codings.push_back(c.codings[i]);
//	};
//	err = dc1394_format7_get_max_image_size(camera, mode, h_size, v_size);
//	if (err != DC1394_SUCCESS) {
//		CloseCamera(camera);
//		throw CameraError("Could not get max image size");
//	};
//	CloseCamera(camera);
//}

//std::string Grabber::GetModeName(dc1394video_mode_t mode,
//		dc1394color_coding_t *coding, uint32_t *h_size, uint32_t *v_size) {
//	dc1394color_coding_t c = (dc1394color_coding_t) 0;
//	uint32_t h = 0;
//	uint32_t v = 0;
//	std::string str;
//	switch (mode) {
//	case (DC1394_VIDEO_MODE_160x120_YUV444):
//		h = 160;
//		v = 120;
//		c = DC1394_COLOR_CODING_YUV444;
//		str = "160x120 YUV444";
//		break;
//	case (DC1394_VIDEO_MODE_320x240_YUV422):
//		h = 320;
//		v = 240;
//		c = DC1394_COLOR_CODING_YUV422;
//		str = "320x240 YUV422";
//		break;
//	case (DC1394_VIDEO_MODE_640x480_YUV411):
//		h = 640;
//		v = 480;
//		c = DC1394_COLOR_CODING_YUV411;
//		str = "640x480 YUV411";
//		break;
//	case (DC1394_VIDEO_MODE_640x480_YUV422):
//		h = 640;
//		v = 480;
//		c = DC1394_COLOR_CODING_YUV422;
//		str = "640x480 YUV422";
//		break;
//	case (DC1394_VIDEO_MODE_640x480_RGB8):
//		h = 640;
//		v = 480;
//		c = DC1394_COLOR_CODING_RGB8;
//		str = "640x480 RGB8";
//		break;
//	case (DC1394_VIDEO_MODE_640x480_MONO8):
//		h = 640;
//		v = 480;
//		c = DC1394_COLOR_CODING_MONO8;
//		str = "640x480 MONO8";
//		break;
//	case (DC1394_VIDEO_MODE_640x480_MONO16):
//		h = 640;
//		v = 480;
//		c = DC1394_COLOR_CODING_MONO16;
//		str = "640x480 MONO16";
//		break;
//	case (DC1394_VIDEO_MODE_800x600_YUV422):
//		h = 800;
//		v = 600;
//		c = DC1394_COLOR_CODING_YUV422;
//		str = "800x600 YUV422";
//		break;
//	case (DC1394_VIDEO_MODE_800x600_RGB8):
//		h = 800;
//		v = 600;
//		c = DC1394_COLOR_CODING_RGB8;
//		str = "800x600 RGB8";
//		break;
//	case (DC1394_VIDEO_MODE_800x600_MONO8):
//		h = 800;
//		v = 600;
//		c = DC1394_COLOR_CODING_MONO8;
//		str = "800x600 MONO8";
//		break;
//	case (DC1394_VIDEO_MODE_1024x768_YUV422):
//		h = 1024;
//		v = 768;
//		c = DC1394_COLOR_CODING_YUV422;
//		str = "1024x768 YUV422";
//		break;
//	case (DC1394_VIDEO_MODE_1024x768_RGB8):
//		h = 1024;
//		v = 768;
//		c = DC1394_COLOR_CODING_RGB8;
//		str = "1024x768 RGB8";
//		break;
//	case (DC1394_VIDEO_MODE_1024x768_MONO8):
//		h = 1024;
//		v = 768;
//		c = DC1394_COLOR_CODING_MONO8;
//		str = "1024x768 MONO8";
//		break;
//	case (DC1394_VIDEO_MODE_800x600_MONO16):
//		h = 800;
//		v = 600;
//		c = DC1394_COLOR_CODING_MONO16;
//		str = "800x600 MONO16";
//		break;
//	case (DC1394_VIDEO_MODE_1024x768_MONO16):
//		h = 1024;
//		v = 768;
//		c = DC1394_COLOR_CODING_MONO16;
//		str = "1024x768 MONO16";
//		break;
//	case (DC1394_VIDEO_MODE_1280x960_YUV422):
//		h = 1280;
//		v = 960;
//		c = DC1394_COLOR_CODING_YUV422;
//		str = "1280x960 YUV422";
//		break;
//	case (DC1394_VIDEO_MODE_1280x960_RGB8):
//		h = 1280;
//		v = 960;
//		c = DC1394_COLOR_CODING_RGB8;
//		str = "1280x960 RGB8";
//		break;
//	case (DC1394_VIDEO_MODE_1280x960_MONO8):
//		h = 1280;
//		v = 960;
//		c = DC1394_COLOR_CODING_MONO8;
//		str = "1280x960 MONO8";
//		break;
//	case (DC1394_VIDEO_MODE_1600x1200_YUV422):
//		h = 1600;
//		v = 1200;
//		c = DC1394_COLOR_CODING_YUV422;
//		str = "1600x1200 YUV422";
//		break;
//	case (DC1394_VIDEO_MODE_1600x1200_RGB8):
//		h = 1600;
//		v = 1200;
//		c = DC1394_COLOR_CODING_RGB8;
//		str = "1600x1200 RGB8";
//		break;
//	case (DC1394_VIDEO_MODE_1600x1200_MONO8):
//		h = 1600;
//		v = 1200;
//		c = DC1394_COLOR_CODING_MONO8;
//		str = "1600x1200 MONO8";
//		break;
//	case (DC1394_VIDEO_MODE_1280x960_MONO16):
//		h = 1280;
//		v = 960;
//		c = DC1394_COLOR_CODING_MONO16;
//		str = "1280x960 MONO16";
//		break;
//	case (DC1394_VIDEO_MODE_1600x1200_MONO16):
//		h = 1600;
//		v = 1200;
//		c = DC1394_COLOR_CODING_MONO16;
//		str = "1600x1200 MONO16";
//		break;
//	case (DC1394_VIDEO_MODE_EXIF):
//		str = "EXIF";
//		break;
//	case (DC1394_VIDEO_MODE_FORMAT7_0):
//		str = "FORMAT7 0";
//		break;
//	case (DC1394_VIDEO_MODE_FORMAT7_1):
//		str = "FORMAT7 1";
//		break;
//	case (DC1394_VIDEO_MODE_FORMAT7_2):
//		str = "FORMAT7 2";
//		break;
//	case (DC1394_VIDEO_MODE_FORMAT7_3):
//		str = "FORMAT7 3";
//		break;
//	case (DC1394_VIDEO_MODE_FORMAT7_4):
//		str = "FORMAT7 4";
//		break;
//	case (DC1394_VIDEO_MODE_FORMAT7_5):
//		str = "FORMAT7 5";
//		break;
//	case (DC1394_VIDEO_MODE_FORMAT7_6):
//		str = "FORMAT7 6";
//		break;
//	case (DC1394_VIDEO_MODE_FORMAT7_7):
//		str = "FORMAT7 7";
//		break;
//	default:
//		dc1394_log_error("Unknown mode\n");
//		throw CameraError("Unknown mode");
//	};
//	if (h_size != 0)
//		*h_size = h;
//	if (v_size != 0)
//		*v_size = v;
//	if (coding != 0)
//		*coding = c;
//	return str;
//}

//std::string Grabber::GetFramerateName(dc1394framerate_t framerate) {
//	float f_framerate;
//	dc1394_framerate_as_float(framerate, &f_framerate);
//	stringstream sstream;
//	sstream << f_framerate;
//	return sstream.str();
//}

//char* Grabber::GetFeatureName(dc1394feature_t feature) {
//	if (feature >= DC1394_FEATURE_MIN && feature <= DC1394_FEATURE_MAX) {
//		return feature_name[feature - DC1394_FEATURE_MIN];
//	} else {
//		return feature_name[feature - DC1394_FEATURE_MAX - DC1394_FEATURE_MIN];
//	};
//}

//void Grabber::PrintFeature(dc1394feature_t feature) {
//
//	switch (feature) {
//	print_case(DC1394_FEATURE_BRIGHTNESS)
//		;
//	print_case(DC1394_FEATURE_EXPOSURE)
//		;
//	print_case(DC1394_FEATURE_SHARPNESS)
//		;
//	print_case(DC1394_FEATURE_WHITE_BALANCE)
//		;
//	print_case(DC1394_FEATURE_HUE)
//		;
//	print_case(DC1394_FEATURE_SATURATION)
//		;
//	print_case(DC1394_FEATURE_GAMMA)
//		;
//	print_case(DC1394_FEATURE_SHUTTER)
//		;
//	print_case(DC1394_FEATURE_GAIN)
//		;
//	print_case(DC1394_FEATURE_IRIS)
//		;
//	print_case( DC1394_FEATURE_FOCUS)
//		;
//	print_case( DC1394_FEATURE_TEMPERATURE)
//		;
//	print_case(DC1394_FEATURE_TRIGGER)
//		;
//	print_case(DC1394_FEATURE_TRIGGER_DELAY)
//		;
//	print_case(DC1394_FEATURE_WHITE_SHADING)
//		;
//	print_case(DC1394_FEATURE_FRAME_RATE)
//		;
//	print_case(DC1394_FEATURE_ZOOM)
//		;
//	print_case(DC1394_FEATURE_PAN)
//		;
//	print_case(DC1394_FEATURE_TILT)
//		;
//	print_case(DC1394_FEATURE_OPTICAL_FILTER)
//		;
//	print_case(DC1394_FEATURE_CAPTURE_SIZE)
//		;
//	print_case(DC1394_FEATURE_CAPTURE_QUALITY)
//		;
//	default:
//		dc1394_log_error("Unknown feature\n");
//		throw CameraError("Unknown feature");
//	}
//}

/*-----------------------------------------------------------------------
 *  Returns the number of pixels in the image based upon the format
 *-----------------------------------------------------------------------*/
//uint32_t Grabber::get_num_pixels(dc1394camera_t *camera, uint32_t format) {
//	uint32_t w, h;
//
//	dc1394_get_image_size_from_video_mode(camera, format, &w, &h);
//
//	return w * h;
//}

/*-----------------------------------------------------------------------
 *  Prints the type of color encoding
 *-----------------------------------------------------------------------*/
//std::string Grabber::GetColorCodingName(uint32_t color_id) {
//	std::string str;
//	switch (color_id) {
//	case DC1394_COLOR_CODING_MONO8:
//		str = "MONO8";
//		break;
//	case DC1394_COLOR_CODING_YUV411:
//		str = ("YUV411");
//		break;
//	case DC1394_COLOR_CODING_YUV422:
//		str = ("YUV422");
//		break;
//	case DC1394_COLOR_CODING_YUV444:
//		str = ("YUV444");
//		break;
//	case DC1394_COLOR_CODING_RGB8:
//		str = ("RGB8");
//		break;
//	case DC1394_COLOR_CODING_MONO16:
//		str = ("MONO16");
//		break;
//	case DC1394_COLOR_CODING_RGB16:
//		str = ("RGB16");
//		break;
//	case DC1394_COLOR_CODING_MONO16S:
//		str = ("MONO16S");
//		break;
//	case DC1394_COLOR_CODING_RGB16S:
//		str = ("RGB16S");
//		break;
//	case DC1394_COLOR_CODING_RAW8:
//		str = ("RAW8");
//		break;
//	case DC1394_COLOR_CODING_RAW16:
//		str = ("RAW16");
//		break;
//	default:
//		dc1394_log_error("Unknown color coding = %d\n", color_id);
//		throw CameraError("Unknown color coding");
//	}
//	return str;
//}

//void Grabber::PrintColorCoding(uint32_t color_id) {
//	printf("%s", GetColorCodingName(color_id).c_str());
//}

/*-----------------------------------------------------------------------
 *  Prints various information about the mode the camera is in
 *-----------------------------------------------------------------------*/
//void Grabber::PrintModeInfo(dc1394video_mode_t mode) {
//	printf("Mode: ");
//	PrintFormat(mode);
//	printf("\n");
//
//}

//void Grabber::ListCameraFramerates(uint64_t guid, uint16_t unit, bool use_unit,
//		dc1394video_mode_t mode, std::vector<dc1394framerate_t> &vlist) {
//	if (mode > DC1394_VIDEO_MODE_1600x1200_MONO16) {
//		// printf("Framerates not supported for this mode.\n");
//		vlist.clear();
//		return;
//	};
//
//	dc1394error_t err;
//	dc1394camera_t *camera = OpenCamera(guid, unit, use_unit);
//
//	dc1394framerates_t framerates;
//
//	err = dc1394_video_get_supported_framerates(camera, mode, &framerates);
//	DC1394_WRN(err, "Could not get frame rates");
//	if (err != DC1394_SUCCESS) {
//		dc1394_camera_free(camera);
//		throw CameraError("Could not get list of modes");
//	};
//
//	vlist.clear();
//
//	printf("%u frame rates for this mode:\n", (unsigned int) framerates.num);
//	for (unsigned int j = 0; j < framerates.num; j++) {
//		vlist.push_back(framerates.framerates[j]);
//	}
//	CloseCamera(camera);
//}

//const vector<CamFeature> & Grabber::getFeatures() {
//	return m_features;
//}

//void Grabber::PrintCameraFramerates(uint64_t guid, uint16_t unit,
//		bool use_unit, dc1394video_mode_t mode) {
//	std::vector<dc1394framerate_t> vlist;
//	ListCameraFramerates(guid, unit, use_unit, mode, vlist);
//	printf("%u frame rates for this mode:\n", (unsigned int) vlist.size());
//	for (size_t j = 0; j < vlist.size(); j++) {
//		dc1394framerate_t rate = vlist[j];
//		float f_rate;
//		dc1394_framerate_as_float(rate, &f_rate);
//		printf("  [%u] rate = %f\n", (unsigned int) j, f_rate);
//	}
//
//}

//void Grabber::PrintExtModeInfo(dc1394camera_t *camera, dc1394video_mode_t mode) {
//
//	printf("Mode: ");
//	PrintFormat(mode);
//	printf("\n");
//
//	if (mode >= DC1394_VIDEO_MODE_FORMAT7_0) {
//		unsigned int c;
//		dc1394color_codings_t codings;
//		dc1394error_t err;
//		uint32_t h_size, v_size;
//		err = dc1394_format7_get_color_codings(camera, mode, &codings);
//		DC1394_ERR(err, "Could not get color codings");
//		for (c = 0; c < codings.num; c++) {
//			printf("  [%u] coding = ", c);
//			PrintColorCoding(codings.codings[c]);
//			printf("\n");
//
//		};
//		err = dc1394_format7_get_max_image_size(camera, mode, &h_size, &v_size);
//		DC1394_ERR(err, "Could not get max image size");
//		printf("max x res = %u\n", (unsigned int) h_size);
//		printf("max y res = %u\n", (unsigned int) v_size);
//
//	};
//}

//void Grabber::cleanup() {
//	dc1394_video_set_transmission(m_camera, DC1394_OFF);
//	dc1394_capture_stop( m_camera);
//	dc1394_camera_free(m_camera);
//}

//void Grabber::Cleanup(dc1394camera_t *camera) {
//	dc1394_video_set_transmission(camera, DC1394_OFF);
//	dc1394_capture_stop(camera);
//	dc1394_camera_free(camera);
//}

//dc1394error_t Grabber::FeatureHasAutoMode(dc1394camera_t *camera,
//		dc1394feature_t feature, dc1394bool_t* b) {
//	dc1394feature_modes_t modes;
//	dc1394error_t err;
//	err = dc1394_feature_get_modes(camera, feature, &modes);
//	DC1394_WRN(err, "Could not ask if feature is present");
//	if (err != DC1394_SUCCESS) {
//		Cleanup(camera);
//		throw CameraError("Could not ask if feature is present");
//	};
//	if (modes.num > DC1394_FEATURE_MODE_AUTO - DC1394_FEATURE_MODE_MIN
//			&& modes.modes[DC1394_FEATURE_MODE_AUTO - DC1394_FEATURE_MODE_MIN]) {
//		*b = DC1394_TRUE;
//	} else {
//		*b = DC1394_FALSE;
//	};
//	return err;
//}

//dc1394error_t Grabber::FeatureHasManualMode(dc1394camera_t *camera,
//		dc1394feature_t feature, dc1394bool_t* b) {
//	dc1394feature_modes_t modes;
//	dc1394error_t err;
//	err = dc1394_feature_get_modes(camera, feature, &modes);
//	DC1394_WRN(err, "Could not ask if feature is present");
//	if (err != DC1394_SUCCESS) {
//		Cleanup(camera);
//		throw CameraError("Could not ask if feature is present");
//	};
//	if (modes.num > DC1394_FEATURE_MODE_MANUAL - DC1394_FEATURE_MODE_MIN
//			&& modes.modes[DC1394_FEATURE_MODE_MANUAL - DC1394_FEATURE_MODE_MIN]) {
//		*b = DC1394_TRUE;
//	} else {
//		*b = DC1394_FALSE;
//	};
//	return err;
//}

//dc1394error_t Grabber::FeatureHasOnePushAuto(dc1394camera_t *camera,
//		dc1394feature_t feature, dc1394bool_t* b) {
//	dc1394feature_modes_t modes;
//	dc1394error_t err;
//	err = dc1394_feature_get_modes(camera, feature, &modes);
//	DC1394_WRN(err, "Could not ask if feature is present");
//	if (err != DC1394_SUCCESS) {
//		Cleanup(camera);
//		throw CameraError("Could not ask if feature is present");
//	};
//	if (modes.num > DC1394_FEATURE_MODE_ONE_PUSH_AUTO - DC1394_FEATURE_MODE_MIN
//			&& modes.modes[DC1394_FEATURE_MODE_ONE_PUSH_AUTO
//					- DC1394_FEATURE_MODE_MIN]) {
//		*b = DC1394_TRUE;
//	} else {
//		*b = DC1394_FALSE;
//	};
//	return err;
//}

//dc1394_t *Grabber::dc1394 = 0; // TODO: at the end of everything: if (dc1394==0) {dc1394_free (dc1394);dc1394=0;};

//void Grabber::ListCameras(std::vector<std::pair<uint64_t, uint16_t> > &vplist) {
//	if (dc1394 == 0)
//		dc1394 = dc1394_new();//dc1394 = dc1394_new();
//	dc1394camera_list_t *list;
//	dc1394error_t err;
//
//	err = dc1394_camera_enumerate(dc1394, &list);
//	if (dc1394 == 0) {
//		dc1394_log_error("Failed to initialize firewire");
//		throw CameraError("Failed to initialize firewire");
//	};
//
//	DC1394_WRN(err, "Failed to enumerate cameras");
//	if (err != DC1394_SUCCESS) {
//		dc1394_free( dc1394);
//		dc1394 = 0;
//		throw CameraError("Failed to enumerate cameras");
//	};
//
//	printf("%u cameras found.\n", list->num);
//	vplist.clear();
//	for (uint32_t i = 0; i < list->num; i++) {
//		assert(sizeof(uint16_t) == sizeof(unsigned short)); //	uint16_t             unit;
//		assert(sizeof(uint64_t) == sizeof(unsigned long long)); //	uint64_t             guid;
//		vplist.push_back(std::pair<uint64_t, uint16_t>(list->ids[i].guid,
//				list->ids[i].unit));
//		printf("%u: GUID %llx UNIT %hu.\n", i, list->ids[i].guid,
//				list->ids[i].unit);
//	};
//
//	dc1394_camera_free_list(list);
//}

//void Grabber::PrintCameraFeatures(const vector<CamFeature> &features) {
//	/*-----------------------------------------------------------------------
//	 *  list other camera features
//	 *-----------------------------------------------------------------------*/
//	printf("------ Camera feature information ------\n");
//	for (size_t f = 0; f <= features.size(); ++f) {
//		printf("\nFeature: ");
//		PrintFeature((dc1394feature_t) features[f].id);
//		printf("\n  ");
//		printf("is%spresent\n  ", features[f].present ? " " : " not ");
//		if (features[f].present) {
//			printf("is%sreadable\n  ", features[f].readable ? " " : " not ");
//			if (features[f].readable) {
//				if (features[f].id == DC1394_FEATURE_WHITE_BALANCE) {
//					printf("values are u_b %u v_r %u\n  ", features[f].value0,
//							features[f].value1);
//				} else if (features[f].id == DC1394_FEATURE_TEMPERATURE) {
//					printf("value are target temp %u temp %u\n  ",
//							features[f].value0, features[f].value1);
//				} else if (features[f].id == DC1394_FEATURE_WHITE_SHADING) {
//					printf("value are %u %u %u\n  ", features[f].value0,
//							features[f].value1, features[f].value2);
//				} else {
//					printf("value is %u\n  ", features[f].value0);
//				};
//			} else /* is not readable */
//			{
//				printf("Feature not readable\n");
//			}; /* is not readable */
//			if (features[f].id == DC1394_FEATURE_TRIGGER) {
//				printf("boundaries are nor readable for the trigger\n  ");
//			} else {
//				printf("boundaries are %u %u\n  ", features[f].min_value,
//						features[f].max_value);
//			};
//			printf("is%sswitchable\n  ", features[f].switchable ? " " : " not ");
//			if (features[f].switchable) {
//				printf("is%s\n  ", features[f].on ? " on" : " off");
//			};
//			printf("has%sauto mode\n  ", features[f].possible_modes
//					& CamFeature::Auto ? " " : " not ");
//			printf("has%smanual mode\n  ", features[f].possible_modes
//					& CamFeature::Manual ? " " : " not ");
//			printf("has%sone push auto mode\n  ", features[f].possible_modes
//					& CamFeature::One ? " " : " not ");
//			if (features[f].possible_modes) {
//				switch (features[f].mode) {
//				case CamFeature::Manual:
//					printf("mode manual is active\n  ");
//					break;
//				case CamFeature::Auto:
//					printf("mode auto is active\n  ");
//					break;
//				case CamFeature::One:
//					printf("mode one push is active\n  ");
//					break;
//				default:
//					printf("mode unknown mode\n  ");
//				};
//			};
//		};
//	}; /* for */
//	printf("------------\n");
//}

//dc1394camera_t * Grabber::OpenCamera(uint64_t guid, uint16_t unit,
//		bool use_unit) {
//	dc1394camera_t *camera;
//
//	if (use_unit) {
//		camera = dc1394_camera_new_unit(dc1394, guid, unit);
//		if (!camera) {
//			dc1394_log_error(
//					"Failed to initialize camera with guid %llx and unit %hu",
//					guid, unit);
//			throw CameraError("Failed to initialize camera");
//		}
//	} else {
//		camera = dc1394_camera_new(dc1394, guid);
//		if (!camera) {
//			dc1394_log_error("Failed to initialize camera with guid %llx", guid);
//			throw CameraError("Failed to initialize camera");
//		}
//	};
//	return camera;
//}

//void Grabber::CloseCamera(dc1394camera_t *camera) {
//	dc1394_camera_free(camera);
//}

//void Grabber::ListCameraFeatures(uint64_t guid, uint16_t unit, bool use_unit,
//		vector<CamFeature> &features) {
//	if (dc1394 == 0)
//		dc1394 = dc1394_new();
//	dc1394camera_t *camera = OpenCamera(guid, unit, use_unit);
//	ListCameraFeatures(camera, features);
//	CloseCamera(camera);
//}

//void Grabber::ListCameraFeatures(dc1394camera_t *camera,
//		vector<CamFeature> &features) {
//
//	/*-----------------------------------------------------------------------
//	 *  list other camera features
//	 *-----------------------------------------------------------------------*/
//	dc1394error_t err;
//
//	for (unsigned int i = DC1394_FEATURE_MIN; i <= DC1394_FEATURE_MAX; ++i) {
//		dc1394feature_t f = (dc1394feature_t) i;
//		CamFeature feature;
//		feature.id = f;
//		dc1394bool_t is_present;
//		dc1394bool_t is_readable;
//		dc1394bool_t is_switchable;
//		dc1394bool_t has_one_push_mode;
//		dc1394bool_t has_auto_mode;
//		dc1394bool_t has_manual_mode;
//		dc1394switch_t pwr;
//		dc1394feature_mode_t mode;
//		uint32_t value;
//		uint32_t min;
//		uint32_t max;
//		feature.name = GetFeatureName(f);
//		err = dc1394_feature_is_present(camera, f, &is_present);
//		DC1394_WRN(err, "Could not ask if feature is present");
//		if (err != DC1394_SUCCESS) {
//			dc1394_camera_free(camera);
//			throw CameraError("Could not ask if feature is present");
//		};
//		feature.present = is_present;
//		if (is_present) {
//			err = dc1394_feature_is_readable(camera, (dc1394feature_t) f,
//					&is_readable);
//			DC1394_WRN(err, "Could not ask if feature is readable");
//			if (err != DC1394_SUCCESS) {
//				dc1394_camera_free(camera);
//				throw CameraError("Could not ask if feature is readable");
//			};
//			feature.readable = is_readable;
//			if (is_readable) {
//				if ((dc1394feature_t) f == DC1394_FEATURE_WHITE_BALANCE) {
//					unsigned int u_b_value;
//					unsigned int v_r_value;
//					err = dc1394_feature_whitebalance_get_value(camera,
//							&u_b_value, &v_r_value);
//					DC1394_WRN(err, "Could not get whitebalance feature value");
//					if (err != DC1394_SUCCESS) {
//						dc1394_camera_free(camera);
//						throw CameraError(
//								"Could not get whitebalance feature value");
//					};
//					feature.value0 = u_b_value;
//					feature.value1 = v_r_value;
//					feature.values = 2;
//					feature.setable = 2;
//				} else if ((dc1394feature_t) f == DC1394_FEATURE_TEMPERATURE) {
//					unsigned int target_temp;
//					unsigned int temp;
//					err = dc1394_feature_temperature_get_value(camera,
//							&target_temp, &temp);
//					DC1394_WRN(err, "Could not get temperature feature value");
//					if (err != DC1394_SUCCESS) {
//						dc1394_camera_free(camera);
//						throw CameraError(
//								"Could not get temperature feature value");
//					};
//					feature.value0 = target_temp;
//					feature.value1 = temp;
//					feature.values = 2;
//					feature.setable = 1;
//				} else if ((dc1394feature_t) f == DC1394_FEATURE_WHITE_SHADING) {
//					unsigned int r_value;
//					unsigned int g_value;
//					unsigned int b_value;
//					err = dc1394_feature_whiteshading_get_value(camera,
//							&r_value, &g_value, &b_value);
//					DC1394_WRN(err, "Could not get whiteshading feature value");
//					if (err != DC1394_SUCCESS) {
//						dc1394_camera_free(camera);
//						throw CameraError(
//								"Could not get whiteshading feature value");
//					};
//					feature.value0 = r_value;
//					feature.value1 = g_value;
//					feature.value2 = b_value;
//					feature.values = 3;
//					feature.setable = 3;
//				} else {
//					err = dc1394_feature_get_value(camera, (dc1394feature_t) f,
//							&value);
//					DC1394_WRN(err, "Could not get feature value");
//					if (err != DC1394_SUCCESS) {
//						dc1394_camera_free(camera);
//						throw CameraError("Could not get feature value");
//					};
//					feature.value0 = value;
//					feature.values = 1;
//					feature.setable = 1;
//				};
//			} else /* is not readable */
//			{
//				/* set only the number of parameters */
//				if ((dc1394feature_t) f == DC1394_FEATURE_WHITE_BALANCE) {
//					feature.value0 = 0;
//					feature.value1 = 0;
//					feature.values = 2;
//					feature.setable = 2;
//				} else if ((dc1394feature_t) f == DC1394_FEATURE_TEMPERATURE) {
//					feature.value0 = 0;
//					feature.value1 = 0;
//					feature.values = 2;
//					feature.setable = 1;
//				} else if ((dc1394feature_t) f == DC1394_FEATURE_WHITE_SHADING) {
//					feature.value0 = 0;
//					feature.value1 = 0;
//					feature.value2 = 0;
//					feature.values = 3;
//					feature.setable = 3;
//				} else {
//					feature.value0 = 0;
//					feature.values = 1;
//					feature.setable = 1;
//				};
//			}; /* is not readable */
//			if (f == DC1394_FEATURE_TRIGGER) {
//				feature.min_value = 0;
//				feature.max_value = 0;
//			} else {
//				err = dc1394_feature_get_boundaries(camera,
//						(dc1394feature_t) f, &min, &max);
//				DC1394_WRN(err, "Could not ask for boundaries");
//				if (err != DC1394_SUCCESS) {
//					dc1394_camera_free(camera);
//					throw CameraError("Could not ask for boundaries");
//				};
//				feature.min_value = min;
//				feature.max_value = max;
//			};
//			err = dc1394_feature_is_switchable(camera, (dc1394feature_t) f,
//					&is_switchable);
//			DC1394_WRN(err, "Could not ask if feature is switchable");
//			if (err != DC1394_SUCCESS) {
//				dc1394_camera_free(camera);
//				throw CameraError("Could not ask if feature is switchable");
//			};
//			feature.switchable = is_switchable;
//			if (is_switchable) {
//				err = dc1394_feature_get_power(camera, (dc1394feature_t) f,
//						&pwr);
//				DC1394_WRN(err, "Could not ask if feature is on or off");
//				if (err != DC1394_SUCCESS) {
//					dc1394_camera_free(camera);
//					throw CameraError("Could not ask if feature is on or off");
//				};
//				feature.on = pwr;
//			} else {
//				feature.on = true;
//			}; /* switchable */
//			feature.possible_modes = 0;
//			err = FeatureHasAutoMode(camera, f, &has_auto_mode);
//			DC1394_WRN(err, "Could not ask if feature has auto mode");
//			if (err != DC1394_SUCCESS) {
//				dc1394_camera_free(camera);
//				throw CameraError("Could not ask if feature has auto mode");
//			};
//			feature.possible_modes |= has_auto_mode ? CamFeature::Auto : 0;
//			err = FeatureHasManualMode(camera, f, &has_manual_mode);
//			DC1394_WRN(err, "Could not ask if feature has manual mode");
//			if (err != DC1394_SUCCESS) {
//				dc1394_camera_free(camera);
//				throw CameraError("Could not ask if feature has manual mode");
//			};
//			feature.possible_modes |= has_manual_mode ? CamFeature::Manual : 0;
//			err = FeatureHasOnePushAuto(camera, f, &has_one_push_mode);
//			DC1394_WRN(err, "Could not ask if feature has one push auto mode");
//			if (err != DC1394_SUCCESS) {
//				dc1394_camera_free(camera);
//				throw CameraError(
//						"Could not ask if feature has one push auto mode");
//			};
//			feature.possible_modes |= has_one_push_mode ? CamFeature::One : 0;
//			if (feature.possible_modes) {
//				err = dc1394_feature_get_mode(camera, (dc1394feature_t) f,
//						&mode);
//				DC1394_WRN(err, "Could not ask feature's mode");
//				if (err != DC1394_SUCCESS) {
//					dc1394_camera_free(camera);
//					throw CameraError("Could not ask feature's mode");
//				};
//				switch (mode) {
//				case DC1394_FEATURE_MODE_MANUAL:
//					feature.mode = CamFeature::Manual;
//					break;
//				case DC1394_FEATURE_MODE_AUTO:
//					feature.mode = CamFeature::Auto;
//					break;
//				case DC1394_FEATURE_MODE_ONE_PUSH_AUTO:
//					feature.mode = CamFeature::One;
//					break;
//				default:
//					dc1394_log_error("Unknown mode\n");
//					feature.mode = CamFeature::Unknown;
//					dc1394_camera_free(camera);
//					throw CameraError("Could not get feature value");
//				};
//			} else {
//				feature.mode = CamFeature::Unknown;
//			};
//			//TODO: Same for the absolute features
//		} else /* is not present */
//		{
//			/* set only the number of parameters */
//			if ((dc1394feature_t) f == DC1394_FEATURE_WHITE_BALANCE) {
//				feature.value0 = 0;
//				feature.value1 = 0;
//				feature.values = 2;
//				feature.setable = 2;
//			} else if ((dc1394feature_t) f == DC1394_FEATURE_TEMPERATURE) {
//				feature.value0 = 0;
//				feature.value1 = 0;
//				feature.values = 2;
//				feature.setable = 1;
//			} else if ((dc1394feature_t) f == DC1394_FEATURE_WHITE_SHADING) {
//				feature.value0 = 0;
//				feature.value1 = 0;
//				feature.value2 = 0;
//				feature.values = 3;
//				feature.setable = 3;
//			} else {
//				feature.value0 = 0;
//				feature.values = 1;
//				feature.setable = 1;
//			};
//		}; /* is not present */
//		features.push_back(feature);
//	}; /* for */
//}

//std::string Grabber::GetUniqueName(uint64_t guid, uint16_t unit, bool use_unit) {
//	dc1394camera_t *camera = OpenCamera(guid, unit, use_unit);
//
//	char * vendor = camera->vendor;
//	char * model = camera->model;
//	assert(guid == camera->guid);
//	assert(unit == camera->unit);
//	uint32_t unit_spec_ID = camera->unit_spec_ID;
//
//	stringstream sstream;
//
//	sstream << vendor << " " << model << " (" << guid << "/" << unit << "/"
//			<< unit_spec_ID << ")";
//
//	CloseCamera(camera);
//	return sstream.str();
//}

//void Grabber::PrintOtherInfo(uint64_t guid, uint16_t unit, bool use_unit) {
//	dc1394error_t err;
//	dc1394camera_t *camera = OpenCamera(guid, unit, use_unit);
//
//	/*-----------------------------------------------------------------------
//	 *  print some other info
//	 *-----------------------------------------------------------------------*/
//	err = dc1394_camera_print_info(camera, stdout);
//	DC1394_WRN(err, "Could not get camera info");
//	if (err != DC1394_SUCCESS) {
//		dc1394_camera_free(camera);
//		throw CameraError("Could not get camera info");
//	};
//
//	CloseCamera(camera);
//
//}

//void Grabber::PrintCameraModes(uint64_t guid, uint16_t unit, bool use_unit) {
//	std::vector<dc1394video_mode_t> vlist;
//	ListCameraModes(guid, unit, use_unit, vlist);
//
//	printf("Number of supported modes: %u\n", (unsigned int) vlist.size());
//	for (size_t m = 0; m < vlist.size(); m++) {
//		printf("Mode id: %i\n", (int) vlist[m]);
//		PrintModeInfo(vlist[m]);
//	};
//}

//void Grabber::ListCameraModes(uint64_t guid, uint16_t unit, bool use_unit,
//		std::vector<dc1394video_mode_t> &vlist) {
//	dc1394error_t err;
//	dc1394camera_t *camera = OpenCamera(guid, unit, use_unit);
//
//	dc1394video_modes_t modes;
//
//	/*-----------------------------------------------------------------------
//	 *  list Capture Modes
//	 *-----------------------------------------------------------------------*/
//	err = dc1394_video_get_supported_modes(camera, &modes);
//	DC1394_WRN(err, "Could not get list of modes");
//	if (err != DC1394_SUCCESS) {
//		dc1394_camera_free(camera);
//		throw CameraError("Could not get list of modes");
//	};
//
//	vlist.clear();
//
//	printf("Number of supported modes: %u\n", (unsigned int) modes.num);
//	for (unsigned int m = 0; m < modes.num; m++) {
//		vlist.push_back(modes.modes[m]);
//	};
//	CloseCamera(camera);
//}

void Grabber::setMode(GrabberFeature::Feature feature,
		GrabberFeature::FeatureMode mode) {
	// THIS FUNCTIONALITY IS NOT YET SUPPORTED -- NEEDS DC1394 SUPPORT IN GRABBER
	throw GrabberError("setMode is not yet supported");
}

void Grabber::setPower(GrabberFeature::Feature feature,
		GrabberFeature::FeatureSwitch pwr) {
	// THIS FUNCTIONALITY IS NOT YET SUPPORTED -- NEEDS DC1394 SUPPORT IN GRABBER
	throw GrabberError("setPower is not yet supported");
}

void Grabber::setRelativeValue(GrabberFeature::Feature feature, double value) {
	if (value < 0.0 || value > 1.0) {
		throw GrabberError(
				"setRelativeValue: value is outside of valid range [0, 1]");
	}

	printf("Feature: %d, value: %f\n", feature, value);

	switch (feature) {
	case GrabberFeature::Brightness:
		fgCtrls->setBrightness(value);
		printf("  new brightness: %f\n", fgCtrls->getBrightness());
		break;
	case GrabberFeature::Exposure:
		fgCtrls->setExposure(value);
		printf("  new exposure: %f\n", fgCtrls->getExposure());
		break;
	case GrabberFeature::Sharpness:
		fgCtrls->setSharpness(value);
		break;
	case GrabberFeature::Hue:
		fgCtrls->setHue(value);
		break;
	case GrabberFeature::Saturation:
		fgCtrls->setSaturation(value);
		break;
	case GrabberFeature::Gamma:
		fgCtrls->setGamma(value);
		break;
	case GrabberFeature::Shutter:
		fgCtrls->setShutter(value);
		break;
	case GrabberFeature::Gain:
		fgCtrls->setGain(value);
		break;
	case GrabberFeature::Iris:
		fgCtrls->setIris(value);
		break;
	default:
		throw GrabberError("setRelativeValue: this feature is not supported");
	}
}

void Grabber::setRelativeValues(GrabberFeature::Feature feature, double value0, double value1, double value2) {
	if (value0 < 0.0 || value0 > 1.0) {
		throw GrabberError(
				"setRelativeValues: value0 is outside of valid range [0, 1]");
	}

// TODO: Fix this sanity check
//	if (value1 < 0.0 || value1 > 1.0) {
//		throw GrabberError(
//				"setRelativeValues: value1 is outside of valid range [0, 1]");
//	}
//	if (value2 < 0.0 || value2 > 1.0) {
//		throw GrabberError(
//				"setRelativeValues: value2 is outside of valid range [0, 1]");
//	}

	printf("Feature: %d, values: (%f, %f, %f)\n", feature, value0, value1, value2);

	switch (feature) {
	case GrabberFeature::Brightness:
		fgCtrls->setBrightness(value0);
		printf("  new brightness: %f\n", fgCtrls->getBrightness());
		break;
	case GrabberFeature::Exposure:
		fgCtrls->setExposure(value0);
		printf("  new exposure: %f\n", fgCtrls->getExposure());
		break;
	case GrabberFeature::Sharpness:
		fgCtrls->setSharpness(value0);
		printf("  new sharpness: %f\n", fgCtrls->getSharpness());
		break;
	case GrabberFeature::WhiteBalance:
		fgCtrls->setWhiteBalance(value0, value1);
		double bValue, rValue;
		fgCtrls->getWhiteBalance(bValue, rValue);
		printf("  new whitebalance: (%f, %f)\n", bValue, rValue);
		break;
	case GrabberFeature::Hue:
		fgCtrls->setHue(value0);
		printf("  new hue: %f\n", fgCtrls->getHue());
		break;
	case GrabberFeature::Saturation:
		fgCtrls->setSaturation(value0);
		printf("  new saturation: %f\n", fgCtrls->getSaturation());
		break;
	case GrabberFeature::Gamma:
		fgCtrls->setGamma(value0);
		printf("  new gamma: %f\n", fgCtrls->getGamma());
		break;
	case GrabberFeature::Shutter:
		fgCtrls->setShutter(value0);
		printf("  new shutter: %f\n", fgCtrls->getShutter());
		break;
	case GrabberFeature::Gain:
		fgCtrls->setGain(value0);
		printf("  new gain: %f\n", fgCtrls->getGain());
		break;
	case GrabberFeature::Iris:
		fgCtrls->setIris(value0);
		printf("  new iris: %f\n", fgCtrls->getIris());
		break;
	default:
		throw GrabberError("setRelativeValues: this feature is not supported");
	}
}

void Grabber::getRelativeValues(GrabberFeature::Feature feature,
		double *value0, double *value1, double *value2) {
	*value0 = *value1 = *value2 = -1.0;

	switch (feature) {
	case GrabberFeature::Brightness:
		*value0 = fgCtrls->getBrightness();
		break;
	case GrabberFeature::Exposure:
		*value0 = fgCtrls->getExposure();
		break;
	case GrabberFeature::Sharpness:
		*value0 = fgCtrls->getSharpness();
		break;
	case GrabberFeature::WhiteBalance:
		fgCtrls->getWhiteBalance(*value0, *value1);
		break;
	case GrabberFeature::Hue:
		*value0 = fgCtrls->getHue();
		break;
	case GrabberFeature::Saturation:
		*value0 = fgCtrls->getSaturation();
		break;
	case GrabberFeature::Gamma:
		*value0 = fgCtrls->getGamma();
		break;
	case GrabberFeature::Shutter:
		*value0 = fgCtrls->getShutter();
		break;
	case GrabberFeature::Gain:
		*value0 = fgCtrls->getGain();
		break;
	case GrabberFeature::Iris:
		*value0 = fgCtrls->getIris();
		break;
	default:
		throw GrabberError("getRelativeValues: this feature is not supported");
	}
}

void Grabber::listFeatures() {
	double value;
	double bValue;
	double rValue;

	features.clear();

	value = fgCtrls->getBrightness();
	if (value >= 0.0) {
		printf("POINT x001, value a: %f", value);
		features.push_back(GrabberFeature(GrabberFeature::Brightness, true,
				value, 0.0, 1));
		GrabberFeature feature = features.back();
		printf(", value b: %f\n", feature.value0);
	} else {
		printf("POINT x002\n");
		features.push_back(GrabberFeature(GrabberFeature::Brightness, false,
				0.0, 0.0, 1));
	}

	value = fgCtrls->getExposure();
	if (value >= 0.0) {
		features.push_back(GrabberFeature(GrabberFeature::Exposure, true,
				value, 0.0, 1));
	} else {
		features.push_back(GrabberFeature(GrabberFeature::Exposure, false,
				0.0, 0.0, 1));
	}

	value = fgCtrls->getSharpness();
	if (value >= 0.0) {
		features.push_back(GrabberFeature(GrabberFeature::Sharpness, true,
				value, 0.0, 1));
	} else {
		features.push_back(GrabberFeature(GrabberFeature::Sharpness, false,
				0.0, 0.0, 1));
	}

	fgCtrls->getWhiteBalance(bValue, rValue);
	if (bValue >= 0.0 && rValue >= 0.0) {
		features.push_back(GrabberFeature(GrabberFeature::WhiteBalance, true,
				bValue, rValue, 2));
	} else {
		features.push_back(GrabberFeature(GrabberFeature::WhiteBalance, false,
				0.0, 0.0, 2));
	}

	value = fgCtrls->getHue();
	if (value >= 0.0) {
		features.push_back(GrabberFeature(GrabberFeature::Hue, true,
				value, 0.0, 1));
	} else {
		features.push_back(GrabberFeature(GrabberFeature::Hue, false,
				0.0, 0.0, 1));
	}

	value = fgCtrls->getSaturation();
	if (value >= 0.0) {
		features.push_back(GrabberFeature(GrabberFeature::Saturation, true,
				value, 0.0, 1));
	} else {
		features.push_back(GrabberFeature(GrabberFeature::Saturation, false,
				0.0, 0.0, 1));
	}

	value = fgCtrls->getGamma();
	if (value >= 0.0) {
		features.push_back(GrabberFeature(GrabberFeature::Gamma, true,
				value, 0.0, 1));
	} else {
		features.push_back(GrabberFeature(GrabberFeature::Gamma, false,
				0.0, 0.0, 1));
	}

	value = fgCtrls->getShutter();
	if (value >= 0.0) {
		features.push_back(GrabberFeature(GrabberFeature::Shutter, true,
				value, 0.0, 1));
	} else {
		features.push_back(GrabberFeature(GrabberFeature::Shutter, false,
				0.0, 0.0, 1));
	}

	value = fgCtrls->getGain();
	if (value >= 0.0) {
		features.push_back(GrabberFeature(GrabberFeature::Gain, true,
				value, 0.0, 1));
	} else {
		features.push_back(GrabberFeature(GrabberFeature::Gain, false,
				0.0, 0.0, 1));
	}

	value = fgCtrls->getIris();
	if (value >= 0.0) {
		features.push_back(GrabberFeature(GrabberFeature::Iris, true,
				value, 0.0, 1));
	} else {
		features.push_back(GrabberFeature(GrabberFeature::Iris, false,
				0.0, 0.0, 1));
	}
}

const vector<GrabberFeature> & Grabber::getFeatures() {
	listFeatures();
	return features;
}

//dc1394speed_t Grabber::GetISOSpeed(uint64_t guid, uint16_t unit, bool use_unit) {
//	dc1394camera_t * camera = OpenCamera(guid, unit, use_unit);
//	dc1394speed_t speed = GetISOSpeed(camera);
//	CloseCamera(camera);
//	return speed;
//}

//dc1394speed_t Grabber::GetISOSpeed(dc1394camera_t * camera) {
//	dc1394speed_t speed;
//	dc1394error_t err = dc1394_video_get_iso_speed(camera, &speed);
//	DC1394_WRN(err, "Could not get ISO speed");
//	if (err != DC1394_SUCCESS) {
//		dc1394_camera_free(camera);
//		throw CameraError("Could not get ISO speed");
//	};
//	return speed;
//}

//dc1394speed_t Grabber::getISOSpeed() {
//	dc1394speed_t speed;
//	dc1394error_t err = dc1394_video_get_iso_speed(m_camera, &speed);
//	DC1394_WRN(err, "Could not set power");
//	if (err != DC1394_SUCCESS) {
//		cleanup();
//		throw CameraError("Could not set power");
//	};
//	return speed;
//}

//void Grabber::SetISOSpeed(uint64_t guid, uint16_t unit, bool use_unit,
//		dc1394speed_t speed) {
//	dc1394camera_t * camera = OpenCamera(guid, unit, use_unit);
//	SetISOSpeed(camera, speed);
//	CloseCamera(camera);
//}

//void Grabber::SetISOSpeed(dc1394camera_t * camera, dc1394speed_t speed) {
//	dc1394error_t err = dc1394_video_set_iso_speed(camera, speed);
//	DC1394_WRN(err, "Could not set ISO speed");
//	if (err != DC1394_SUCCESS) {
//		dc1394_camera_free(camera);
//		throw CameraError("Could not set ISO speed");
//	};
//}

//void Grabber::setISOSpeed(dc1394speed_t speed) {
//	dc1394error_t err = dc1394_video_set_iso_speed(m_camera, speed);
//	DC1394_WRN(err, "Could not set ISO speed");
//	if (err != DC1394_SUCCESS) {
//		cleanup();
//		throw CameraError("Could not set ISO speed");
//	};
//}

//void Grabber::getValues(dc1394feature_t feature, uint32_t *value0,
//		uint32_t *value1, uint32_t *value2) {
//	dc1394error_t err;
//	if (feature == DC1394_FEATURE_WHITE_BALANCE) {
//		err = dc1394_feature_whitebalance_get_value(m_camera, value0, value1); // &u_b_value,&v_r_value
//	} else if (feature == DC1394_FEATURE_TEMPERATURE) {
//		err = dc1394_feature_temperature_get_value(m_camera, value0, value1); // &target_temp,&temp
//	} else if (feature == DC1394_FEATURE_WHITE_SHADING) {
//		err = dc1394_feature_whiteshading_get_value(m_camera, value0, value1,
//				value2); //camera,&r_value,&g_value,&b_value
//	} else {
//		err = dc1394_feature_get_value(m_camera, feature, value0); //&value
//	};
//
//	DC1394_WRN(err, "Could not get whitebalance feature value");
//	if (err != DC1394_SUCCESS) {
//		cleanup();
//		throw CameraError("Could not get whitebalance feature value");
//	};
//}

//void Grabber::getValues(dc1394feature_t feature, uint32_t values[3]) {
//	getValues(feature, &values[0], &values[1], &values[2]);
//}

//void Grabber::setValues(dc1394feature_t feature, uint32_t value[3]) {
//	setValues(feature, value[0], value[1], value[2]);
//}

//void Grabber::setValues(dc1394feature_t feature, uint32_t value0,
//		uint32_t value1, uint32_t value2) {
//	dc1394error_t err;
//
//	if (feature == DC1394_FEATURE_WHITE_BALANCE) {
//		err = dc1394_feature_whitebalance_set_value(m_camera, value0, value1);
//	} else if (feature == DC1394_FEATURE_TEMPERATURE) {
//		err = dc1394_feature_temperature_set_value(m_camera, value0); // sets only the target temp, not the current temp!
//	} else if (feature == DC1394_FEATURE_WHITE_SHADING) {
//		err = dc1394_feature_whiteshading_set_value(m_camera, value0, value1,
//				value2);
//	} else {
//		err = dc1394_feature_set_value(m_camera, feature, value0);
//	};
//
//	DC1394_WRN(err, "Could not set value");
//	if (err != DC1394_SUCCESS) {
//		cleanup();
//		throw CameraError("Could not set value");
//	};
//}

//void Grabber::startTransmission() {
//	dc1394error_t err;
//	/*-----------------------------------------------------------------------
//	 *  have the camera start sending us data
//	 *-----------------------------------------------------------------------*/
//	err = dc1394_video_set_transmission(m_camera, DC1394_ON);
//	DC1394_WRN(err, "Could not start camera iso transmission\n");
//	if (err != DC1394_SUCCESS) {
//		cleanup();
//		throw CameraError("Could not start camera iso transmission");
//	};
//}

//void Grabber::stopTransmission() {
//	dc1394error_t err;
//	/*-----------------------------------------------------------------------
//	 *  stop data transmission
//	 *-----------------------------------------------------------------------*/
//	err = dc1394_video_set_transmission(m_camera, DC1394_OFF);
//	DC1394_WRN(err, "Could not stop the camera?\n");
//	if (err != DC1394_SUCCESS) {
//		cleanup();
//		throw CameraError("Could not stop the camera");
//	};
//}

//dc1394video_frame_t* Grabber::captureFrame() {
//	dc1394error_t err;
//	dc1394video_frame_t *frame = NULL;
//	/*-----------------------------------------------------------------------
//	 *  capture one frame
//	 *-----------------------------------------------------------------------*/
//	err = dc1394_capture_dequeue(m_camera, DC1394_CAPTURE_POLICY_WAIT, &frame);
//	DC1394_WRN(err, "Could not capture a frame\n");
//	if (err != DC1394_SUCCESS) {
//		cleanup();
//		throw CameraError("Could not capture a frame");
//	};
//	return frame;
//}

//void Grabber::releaseFrame(dc1394video_frame_t* frame) {
//	dc1394_capture_enqueue(m_camera, frame);
//}

//unsigned int Grabber::getImageWidth() {
//	unsigned int width, height;
//	dc1394error_t err = dc1394_get_image_size_from_video_mode(m_camera,
//			m_selected_mode, &width, &height);
//	if (err != DC1394_SUCCESS) {
//		cleanup();
//		throw CameraError("Could not get image size from vodeo mode");
//	};
//	return width;
//}

//unsigned int Grabber::getImageHeight() {
//	unsigned int width, height;
//	dc1394error_t err = dc1394_get_image_size_from_video_mode(m_camera,
//			m_selected_mode, &width, &height);
//	if (err != DC1394_SUCCESS) {
//		cleanup();
//		throw CameraError("Could not get image size from vodeo mode");
//	};
//	return height;
//}

//dc1394color_coding_t Grabber::getImageEncoding() {
//	dc1394color_coding_t color_coding;
//	dc1394error_t err = dc1394_get_color_coding_from_video_mode(m_camera,
//			m_selected_mode, &color_coding);
//	if (err != DC1394_SUCCESS) {
//		cleanup();
//		throw CameraError("Could not get color coding from camera mode");
//	};
//	return color_coding;
//}

//unsigned int Grabber::GetDepthFromColorCoding(dc1394color_coding_t color_coding) {
//	unsigned int depth;
//	switch (color_coding) {
//	case DC1394_COLOR_CODING_MONO8: // mono: 1 bit per pixel
//		depth = 1 * 8;
//		break;
//	case DC1394_COLOR_CODING_YUV411: // color: 6 bytes for 4 pixels with sharing u v chanel
//		depth = 12; // 1.5*8
//		break;
//	case DC1394_COLOR_CODING_YUV422: // color: 4 bytes for 2 pixels with sharing u v shannel
//		depth = 2 * 8;
//		break;
//	case DC1394_COLOR_CODING_YUV444: // color: 3 bytes for 1 pixel
//		depth = 3 * 8;
//		break;
//	case DC1394_COLOR_CODING_RGB8: // color: 3 bytes per pixel
//		depth = 3 * 8;
//		break;
//	case DC1394_COLOR_CODING_MONO16: // mono: 2 bytes per pixel (non color pixel)
//		depth = 2 * 8;
//		break;
//	case DC1394_COLOR_CODING_RGB16: //color:  6 bytes per pixel
//		depth = 6 * 8;
//		break;
//	case DC1394_COLOR_CODING_MONO16S: // mono: 2 bytes per pixel
//		depth = 2 * 8;
//		break;
//	case DC1394_COLOR_CODING_RGB16S: // color: 6 bytes per pixel
//		depth = 6 * 8;
//		break;
//	case DC1394_COLOR_CODING_RAW8: // mono/color: 1 byte per pixel info, mostly shared color information
//		depth = 1 * 8;
//		break;
//	case DC1394_COLOR_CODING_RAW16: // mono/color: 2 byte per pixel info, mostly shared color information
//		depth = 2 * 8;
//		break;
//	default:
//		depth = 0;
//		break;
//	};
//	return depth;
//}

//unsigned int Grabber::getImageDepth() {
//	dc1394color_coding_t color_coding;
//	dc1394error_t err = dc1394_get_color_coding_from_video_mode(m_camera,
//			m_selected_mode, &color_coding);
//	if (err != DC1394_SUCCESS) {
//		cleanup();
//		throw CameraError("Could not get color coding from camera mode");
//	};
//	return GetDepthFromColorCoding(color_coding);
//}

//void Grabber::saveAs(dc1394video_frame_t *frame, const char* filename) {
//
//	FILE* imagefile;
//
//	/*-----------------------------------------------------------------------
//	 *  convert the image from what ever format it is to its RGB8
//	 *-----------------------------------------------------------------------*/
//
//	unsigned int width, height;
//	dc1394_get_image_size_from_video_mode(m_camera, m_selected_mode, &width,
//			&height);
//	uint64_t numPixels = height * width;
//
//	dc1394video_frame_t *new_frame = (dc1394video_frame_t*) calloc(1,
//			sizeof(dc1394video_frame_t));
//	new_frame->color_coding = DC1394_COLOR_CODING_RGB8;
//	dc1394_convert_frames(frame, new_frame);
//
//	/*-----------------------------------------------------------------------
//	 *  save image as 'Image.pgm'
//	 *-----------------------------------------------------------------------*/
//	imagefile = fopen(filename, "wb");
//
//	if (imagefile == NULL) {
//		perror("Can't create file");
//		dc1394_video_set_transmission(m_camera, DC1394_OFF);
//		dc1394_capture_stop( m_camera);
//		dc1394_camera_free(m_camera);
//		exit(1);
//	}
//
//	fprintf(imagefile, "P6\n%u %u\n255\n", width, height);
//	fwrite((const char *) new_frame->image, 1, numPixels * 3, imagefile);
//	fclose(imagefile);
//	printf("wrote: %s \n", filename);
//
//	free(new_frame->image);
//	free(new_frame);
//}
