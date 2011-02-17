#include <QApplication>
#include <QtGui>

#include <vector>

#include <yarp/os/Property.h>
#include <yarp/dev/PolyDriver.h>

#include "grabber.h"
#include "grabberthread.h"
#include "grabberfeaturewindow.h"
#include "mainwindow.h"
//#include "camera.h"
//#include "camthread.h"
//#include "camprop.h"
//#include "camfeature.h"
//#include "featurewindow.h"
//#include "selectcamera.h"

int main(int argc, char *argv[]) {
	Q_INIT_RESOURCE( camview);
	QApplication app(argc, argv);
#ifdef __APPLE__
	app.setAttribute(Qt::AA_DontShowIconsInMenus);
#endif
	//	uint64_t guid;
	//	uint16_t unit;
	//	dc1394video_mode_t mode;
	//	dc1394speed_t speed;
	//	dc1394framerate_t framerate;
	//#if 1
	//	FILE *fh = fopen("default.cam","r");
	//	unsigned long long ullguid;
	//	unsigned uiunit;
	//	unsigned uimode;
	//	unsigned uispeed;
	//	unsigned uiframerate;
	//	fscanf(fh,"GUID: %llu UNIT: %u MODE: %u SPEED: %u FRAMERATE: %u",&ullguid,&uiunit,&uimode,&uispeed,&uiframerate);
	//	guid = (uint64_t)ullguid;
	//	unit = (uint16_t)uiunit;
	//	mode = (dc1394video_mode_t)uimode;
	//	speed = (dc1394speed_t)uispeed;
	//	framerate = (dc1394framerate_t)uiframerate;
	//	fclose(fh);
	//#else
	//	try
	//	{
	//		SelectCameraDialog *selectCameraDialog = new SelectCameraDialog();
	//		int retval = selectCameraDialog->exec();
	//		printf("retval = %d\n",retval);
	//		if (retval==0) return 1;
	//		guid = selectCameraDialog->getGuid();
	//		unit = selectCameraDialog->getUnit();
	//		mode = selectCameraDialog->getMode();
	//		speed=DC1394_ISO_SPEED_400;//selectCameraDialog->getSpeed();
	//		framerate = selectCameraDialog->getFramerate();
	//	}
	//	catch(CameraError &ce)
	//	{
	//		QMessageBox::critical(0,"Cam View",
	//							  ce.getMessage().c_str(),
	//							  QMessageBox::Abort,
	//							  QMessageBox::Abort);
	//		return -2;
	//	};
	//	FILE *fh = fopen("default.cam","w");
	//	unsigned long long ullguid =guid;
	//	unsigned uiunit = unit;
	//	unsigned uimode = mode;
	//	unsigned uispeed = speed;
	//	unsigned uiframerate = framerate;
	//	fprintf(fh,"GUID: %llu UNIT: %u MODE: %u SPEED: %u FRAMERATE: %u",ullguid,uiunit,uimode,uispeed,uiframerate);
	//	fclose(fh);
	//#endif
	//#if 0
	//	std::vector<std::pair<uint64_t,uint16_t> >list;
	//	Camera::ListCameras(list);
	//	while (list.size() == 0)
	//	{
	//		if(QMessageBox::Abort==QMessageBox::critical(0,"Cam View",
	//			 "No cameras found.",
	//			 QMessageBox::Retry | QMessageBox::Abort,
	//			 QMessageBox::Abort))return -1;
	//		Camera::ListCameras(list);
	//    };
	//#endif
	//		try
	//{
	// Camera camera(list[0].first);//,list[0].second); // take the first camera!
	//		Camera camera(guid,unit,true,mode,speed,framerate);
	//		CamThread camThread(camera);
	//		FeatureWindow *featureWindow = new FeatureWindow(&camera);
	//		featureWindow->show();

	yarp::os::Property remoteClientConfig;
	remoteClientConfig.put("device", "remote_grabber");
	remoteClientConfig.put("local", "/remote_grabber");
	remoteClientConfig.put("remote", "/guppy");

	yarp::dev::PolyDriver driver(remoteClientConfig);

	if (driver.isValid()) {
		try {
			Grabber grabber(driver);
			GrabberThread grabberThread(driver);
			grabberThread.setImageType(GrabberThread::MONO);

			//		MainWindow mainWindow(&camera, &camThread);
			MainWindow mainWindow(&grabber, &grabberThread);
			mainWindow.show();
			// connect...
			QObject::connect(&grabberThread, SIGNAL(newFrame(const QImage&)),
					&mainWindow, SLOT(newFrame(const QImage&)));

			//		FeatureWindow *featureWindow = new FeatureWindow(&camera);
			//		featureWindow->show();
			GrabberFeatureWindow *featureWindow = new GrabberFeatureWindow(
					&grabber);
			featureWindow->show();

			//		camThread.start();
			grabberThread.start();

			return app.exec();
		} catch (GrabberError &ge) {
			QMessageBox::critical(0, "Yarp View", ge.getMessage().c_str(),
					QMessageBox::Abort, QMessageBox::Abort);
			return -2;
		};

	} else {
		printf("Instantiation of YARP grabber driver failed\n");
	}

	//	}
	//	catch(CameraError &ce)
	//	{
	//		  QMessageBox::critical(0,"Cam View",
	//				ce.getMessage().c_str(),
	//				QMessageBox::Abort,
	//								QMessageBox::Abort);
	//		  return -2;
	//	};

}
