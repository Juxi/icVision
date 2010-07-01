//#include <QPainter>
#include <QtGui>

#include "mainwindow.h"

//MainWindow::MainWindow(Camera* camera,CamThread *camThread, QWidget *parent)
//    : QMainWindow(parent),camThread(camThread),camera(camera)
MainWindow::MainWindow(Grabber* grabber, GrabberThread *grabberThread,
		QWidget *parent) :
	QMainWindow(parent), grabber(grabber), grabberThread(grabberThread) {
	QScrollArea* scrollArea = new QScrollArea();
	framePaintWidget = new FramePaintWidget();
	scrollArea->setWidget(framePaintWidget);
	scrollArea->setBackgroundRole(QPalette::Window);
	scrollArea->setWidgetResizable(true);
	setCentralWidget(scrollArea);
	// framePaintWidget->setPalette(QPalette(QColor(250, 250, 200)));
	framePaintWidget->setPalette(QPalette(Qt::white));
	// framePaintWidget->setBackgroundRole(QPalette::Window);
	framePaintWidget->setAutoFillBackground(true);
	//framePaintWidget->setMinimumSize ( 780, 582 );
	//setGeometry(100, 100, 780, 582);
	createActions();
	createMenus();
	createToolBars();
	readSettings();
	//framePaintWidget->show();
	setWindowTitle(tr("Camera View"));
	setUnifiedTitleAndToolBarOnMac(true);
	lastWidth = 0;
	lastHeight = 0;
	lastFramerate = -1.0;
	framePaintWidget->sequencenum = 0;
}

MainWindow::~MainWindow() {
	writeSettings();
}

void MainWindow::readSettings() {
	QSettings settings("Alexander", "Camera View");
	QPoint pos = settings.value("pos", QPoint(100, 100)).toPoint();
	QSize size = settings.value("size", QSize(780, 582)).toSize();
	resize(size);
	move(pos);
}

void MainWindow::writeSettings() {
	QSettings settings("Alexander", "Camera View");
	settings.setValue("pos", pos());
	settings.setValue("size", size());
}

void MainWindow::createActions() {
	//	newAct = new QAction(QIcon(":/images/new.png"),tr("&New"), this);
	//	newAct->setShortcut(tr("Ctrl+N"));
	//	newAct->setStatusTip(tr("Create a new file"));
	//	connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));
	//
	//	modeBayerFilterTypeGroup = new QActionGroup(this);
	//
	//	modeBayerFilterTypeGRBGAct = new QAction(tr("&GRBG"), this);
	//	modeBayerFilterTypeGRBGAct->setCheckable(true);
	//	modeBayerFilterTypeGRBGAct->setActionGroup(modeBayerFilterTypeGroup);
	//	modeBayerFilterTypeGRBGAct->setStatusTip(tr("Set Bayer Filter to GRBG"));
	//	connect(modeBayerFilterTypeGRBGAct, SIGNAL(triggered()), this, SLOT(modeBayerFilterTypeGRBG()));
	//
	//	modeBayerFilterTypeRGGBAct = new QAction(tr("&RGGB"), this);
	//	modeBayerFilterTypeRGGBAct->setCheckable(true);
	//	modeBayerFilterTypeRGGBAct->setActionGroup(modeBayerFilterTypeGroup);
	//	modeBayerFilterTypeRGGBAct->setStatusTip(tr("Set Bayer Filter to RGGB"));
	//	connect(modeBayerFilterTypeRGGBAct, SIGNAL(triggered()), this, SLOT(modeBayerFilterTypeRGGB()));
	//
	//	modeBayerFilterTypeBGGRAct = new QAction(tr("&BGGR"), this);
	//	modeBayerFilterTypeBGGRAct->setCheckable(true);
	//	modeBayerFilterTypeBGGRAct->setActionGroup(modeBayerFilterTypeGroup);
	//	modeBayerFilterTypeBGGRAct->setStatusTip(tr("Set Bayer Filter to BGGR"));
	//	connect(modeBayerFilterTypeBGGRAct, SIGNAL(triggered()), this, SLOT(modeBayerFilterTypeBGGR()));
	//
	//	modeBayerFilterTypeGBRGAct = new QAction(tr("&GBRG"), this);
	//	modeBayerFilterTypeGBRGAct->setCheckable(true);
	//	modeBayerFilterTypeGBRGAct->setActionGroup(modeBayerFilterTypeGroup);
	//	modeBayerFilterTypeGBRGAct->setStatusTip(tr("Set Bayer Filter to GBRG"));
	//	connect(modeBayerFilterTypeGBRGAct, SIGNAL(triggered()), this, SLOT(modeBayerFilterTypeGBRG()));

	//	switch(camThread->getBayerFilterType())
	//	{
	//		case CamThread::GRBG:
	//			modeBayerFilterTypeGRBGAct->setChecked(true);
	//			break;
	//		case CamThread::RGGB:
	//			modeBayerFilterTypeRGGBAct->setChecked(true);
	//			break;
	//		case CamThread::BGGR:
	//			modeBayerFilterTypeBGGRAct->setChecked(true);
	//			break;
	//		case CamThread::GBRG:
	//			modeBayerFilterTypeGBRGAct->setChecked(true);
	//			break;
	//	};

	//	modeImageTypeGroup = new QActionGroup(this);
	//
	//	modeImageTypeSTEREOAct = new QAction(tr("&Stereo"), this);
	//	modeImageTypeSTEREOAct->setCheckable(true);
	//	modeImageTypeSTEREOAct->setActionGroup(modeImageTypeGroup);
	//	modeImageTypeSTEREOAct->setStatusTip(tr("Set Stereo Camera decoding"));
	//	connect(modeImageTypeSTEREOAct, SIGNAL(triggered()), this, SLOT(modeImageTypeSTEREO()));
	//
	//	modeImageTypeMONOAct = new QAction(tr("&Mono"), this);
	//	modeImageTypeMONOAct->setCheckable(true);
	//	modeImageTypeMONOAct->setActionGroup(modeImageTypeGroup);
	//	modeImageTypeMONOAct->setStatusTip(tr("Set Monocular Camera decoding"));
	//	connect(modeImageTypeMONOAct, SIGNAL(triggered()), this, SLOT(modeImageTypeMONO()));

	//	switch(camThread->getImageType())
	//	{
	//		case CamThread::STEREO:
	//			modeImageTypeSTEREOAct->setChecked(true);
	//			break;
	//		case CamThread::MONO:
	//			modeImageTypeMONOAct->setChecked(true);
	//			break;
	//	};

	//	modeImageConverterGroup = new QActionGroup(this);
	//
	//	modeImageConverterYUV422Act = new QAction(tr("&YUV422"), this);
	//	modeImageConverterYUV422Act->setActionGroup(modeImageConverterGroup);
	//	modeImageConverterYUV422Act->setCheckable(true);
	//	modeImageConverterYUV422Act->setStatusTip(tr("Convert from yuv422 coding"));
	//	connect(modeImageConverterYUV422Act, SIGNAL(triggered()), this, SLOT(modeImageConverterYUV422()));
	//
	//	modeImageConverterYUV411Act = new QAction(tr("Y&UV411"), this);
	//	modeImageConverterYUV411Act->setActionGroup(modeImageConverterGroup);
	//	modeImageConverterYUV411Act->setCheckable(true);
	//	modeImageConverterYUV411Act->setStatusTip(tr("Convert from yuv411 coding"));
	//	connect(modeImageConverterYUV411Act, SIGNAL(triggered()), this, SLOT(modeImageConverterYUV411()));
	//
	//	modeImageConverterGRAYAct = new QAction(tr("&Gray"), this);
	//	modeImageConverterGRAYAct->setActionGroup(modeImageConverterGroup);
	//	modeImageConverterGRAYAct->setCheckable(true);
	//	modeImageConverterGRAYAct->setStatusTip(tr("Convert to Gray image"));
	//	connect(modeImageConverterGRAYAct, SIGNAL(triggered()), this, SLOT(modeImageConverterGRAY()));
	//
	//	modeImageConverterMASKAct = new QAction(tr("&Mask"), this);
	//	modeImageConverterMASKAct->setActionGroup(modeImageConverterGroup);
	//	modeImageConverterMASKAct->setCheckable(true);
	//	modeImageConverterMASKAct->setStatusTip(tr("Convert to Mask image"));
	//	connect(modeImageConverterMASKAct, SIGNAL(triggered()), this, SLOT(modeImageConverterMASK()));
	//
	//	modeImageConverterSIMPLEAct = new QAction(tr("&Simple"), this);
	//	modeImageConverterSIMPLEAct->setActionGroup(modeImageConverterGroup);
	//	modeImageConverterSIMPLEAct->setCheckable(true);
	//	modeImageConverterSIMPLEAct->setStatusTip(tr("Convert to Simple image"));
	//	connect(modeImageConverterSIMPLEAct, SIGNAL(triggered()), this, SLOT(modeImageConverterSIMPLE()));
	//
	//	modeImageConverterSIMPLE2Act = new QAction(tr("S&imple 2"), this);
	//	modeImageConverterSIMPLE2Act->setActionGroup(modeImageConverterGroup);
	//	modeImageConverterSIMPLE2Act->setCheckable(true);
	//	modeImageConverterSIMPLE2Act->setStatusTip(tr("Convert to Simple 2 image"));
	//	connect(modeImageConverterSIMPLE2Act, SIGNAL(triggered()), this, SLOT(modeImageConverterSIMPLE2()));
	//
	//	modeImageConverterLINEARAct = new QAction(tr("&Linear"), this);
	//	modeImageConverterLINEARAct->setActionGroup(modeImageConverterGroup);
	//	modeImageConverterLINEARAct->setCheckable(true);
	//	modeImageConverterLINEARAct->setStatusTip(tr("Convert to Linear image"));
	//	connect(modeImageConverterLINEARAct, SIGNAL(triggered()), this, SLOT(modeImageConverterLINEAR()));

	//	switch(camThread->getImageConverter())
	//	{
	//		case CamThread::YUV422:
	//			modeImageConverterYUV422Act->setChecked(true);
	//			break;
	//		case CamThread::YUV411:
	//			modeImageConverterYUV411Act->setChecked(true);
	//			break;
	//		case CamThread::GRAY:
	//			modeImageConverterGRAYAct->setChecked(true);
	//			break;
	//		case CamThread::MASK:
	//			modeImageConverterMASKAct->setChecked(true);
	//			break;
	//		case CamThread::SIMPLE:
	//			modeImageConverterSIMPLEAct->setChecked(true);
	//			break;
	//		case CamThread::SIMPLE2:
	//			modeImageConverterSIMPLE2Act->setChecked(true);
	//			break;
	//		case CamThread::LINEAR:
	//			modeImageConverterLINEARAct->setChecked(true);
	//			break;
	//	};

	modeViewGroup = new QActionGroup(this);

	modeViewPixelsAct = new QAction(tr("&Pixels"), this);
	modeViewPixelsAct->setActionGroup(modeViewGroup);
	modeViewPixelsAct->setCheckable(true);
	modeViewPixelsAct->setStatusTip(tr("View the detected Pixels"));
	connect(modeViewPixelsAct, SIGNAL(triggered()), this,
			SLOT(modeViewPixels()));

	modeViewImageAct = new QAction(tr("&Image"), this);
	modeViewImageAct->setActionGroup(modeViewGroup);
	modeViewImageAct->setCheckable(true);
	modeViewImageAct->setStatusTip(tr("View the Image"));
	connect(modeViewImageAct, SIGNAL(triggered()), this, SLOT(modeViewImage()));

	modeViewImageGroup = new QActionGroup(this);

	modeViewImageRGBAct = new QAction(tr("&RGB"), this);
	modeViewImageRGBAct->setActionGroup(modeViewImageGroup);
	modeViewImageRGBAct->setCheckable(true);
	modeViewImageRGBAct->setStatusTip(tr("View the real image"));
	connect(modeViewImageRGBAct, SIGNAL(triggered()), this, SLOT(
			modeViewImageRGB()));

	modeViewImageREDAct = new QAction(tr("&Red"), this);
	modeViewImageREDAct->setActionGroup(modeViewImageGroup);
	modeViewImageREDAct->setCheckable(true);
	modeViewImageREDAct->setStatusTip(tr("View the red channel of the Image"));
	connect(modeViewImageREDAct, SIGNAL(triggered()), this, SLOT(
			modeViewImageRED()));

	modeViewImageGREENAct = new QAction(tr("&Green"), this);
	modeViewImageGREENAct->setActionGroup(modeViewImageGroup);
	modeViewImageGREENAct->setCheckable(true);
	modeViewImageGREENAct->setStatusTip(tr(
			"View the green channel of the Image"));
	connect(modeViewImageGREENAct, SIGNAL(triggered()), this, SLOT(
			modeViewImageGREEN()));

	modeViewImageBLUEAct = new QAction(tr("&Blue"), this);
	modeViewImageBLUEAct->setActionGroup(modeViewImageGroup);
	modeViewImageBLUEAct->setCheckable(true);
	modeViewImageBLUEAct->setStatusTip(tr("View the blue channel of the Image"));
	connect(modeViewImageBLUEAct, SIGNAL(triggered()), this, SLOT(
			modeViewImageBLUE()));

	modeViewImageHUEAct = new QAction(tr("&Hue"), this);
	modeViewImageHUEAct->setActionGroup(modeViewImageGroup);
	modeViewImageHUEAct->setCheckable(true);
	modeViewImageHUEAct->setStatusTip(tr("View the hue channel of the Image"));
	connect(modeViewImageHUEAct, SIGNAL(triggered()), this, SLOT(
			modeViewImageHUE()));

	modeViewImageSATURATIONAct = new QAction(tr("&Saturation"), this);
	modeViewImageSATURATIONAct->setActionGroup(modeViewImageGroup);
	modeViewImageSATURATIONAct->setCheckable(true);
	modeViewImageSATURATIONAct->setStatusTip(tr(
			"View the saturatin channel of the Image"));
	connect(modeViewImageSATURATIONAct, SIGNAL(triggered()), this, SLOT(
			modeViewImageSATURATION()));

	modeViewImageVALUEAct = new QAction(tr("&Value"), this);
	modeViewImageVALUEAct->setActionGroup(modeViewImageGroup);
	modeViewImageVALUEAct->setCheckable(true);
	modeViewImageVALUEAct->setStatusTip(tr(
			"View the value channel of the Image"));
	connect(modeViewImageVALUEAct, SIGNAL(triggered()), this, SLOT(
			modeViewImageVALUE()));

	if (framePaintWidget->miro.viewPixels) {
		modeViewPixelsAct->setChecked(true);
		modeViewImageRGBAct->setEnabled(false);
		modeViewImageREDAct->setEnabled(false);
		modeViewImageGREENAct->setEnabled(false);
		modeViewImageBLUEAct->setEnabled(false);
		modeViewImageHUEAct->setEnabled(false);
		modeViewImageSATURATIONAct->setEnabled(false);
		modeViewImageVALUEAct->setEnabled(false);
		// TODO: imange Y, U and V, if theinput was yuv
	} else {
		modeViewImageAct->setChecked(true);
		modeViewImageRGBAct->setEnabled(true);
		modeViewImageREDAct->setEnabled(true);
		modeViewImageGREENAct->setEnabled(true);
		modeViewImageBLUEAct->setEnabled(true);
		modeViewImageHUEAct->setEnabled(true);
		modeViewImageSATURATIONAct->setEnabled(true);
		modeViewImageVALUEAct->setEnabled(true);
	};

	switch (framePaintWidget->miro.viewImage) {
	case MiroVision::RGB:
		modeViewImageRGBAct->setChecked(true);
		break;
	case MiroVision::RED:
		modeViewImageREDAct->setChecked(true);
		break;
	case MiroVision::GREEN:
		modeViewImageGREENAct->setChecked(true);
		break;
	case MiroVision::BLUE:
		modeViewImageBLUEAct->setChecked(true);
		break;
	case MiroVision::HUE:
		modeViewImageHUEAct->setChecked(true);
		break;
	case MiroVision::SATURATION:
		modeViewImageSATURATIONAct->setChecked(true);
		break;
	case MiroVision::VALUE:
		modeViewImageVALUEAct->setChecked(true);
		break;
	};

}

void MainWindow::createMenus() {
	//	fileMenu = menuBar()->addMenu(tr("&File"));
	//	fileMenu->addAction(newAct);
	//
	//	modeMenu = menuBar()->addMenu(tr("&Mode"));
	//	modeMenu->addAction(modeBayerFilterTypeGRBGAct);
	//	modeMenu->addAction(modeBayerFilterTypeRGGBAct);
	//	modeMenu->addAction(modeBayerFilterTypeBGGRAct);
	//	modeMenu->addAction(modeBayerFilterTypeGBRGAct);
	//modeBayerFilterTypeGroup = new QActionGroup(this);
	//modeBayerFilterTypeGroup->addAction(modeBayerFilterTypeGRBGAct);
	//modeBayerFilterTypeGroup->addAction(modeBayerFilterTypeRGGBAct);
	//modeBayerFilterTypeGroup->addAction(modeBayerFilterTypeBGGRAct);
	//modeBayerFilterTypeGroup->addAction(modeBayerFilterTypeGBRGAct);
	//	switch(camThread->getBayerFilterType())
	//	{
	//		case CamThread::GRBG:
	//			modeBayerFilterTypeGRBGAct->setChecked(true);
	//			break;
	//		case CamThread::RGGB:
	//			modeBayerFilterTypeRGGBAct->setChecked(true);
	//			break;
	//		case CamThread::BGGR:
	//			modeBayerFilterTypeBGGRAct->setChecked(true);
	//			break;
	//		case CamThread::GBRG:
	//			modeBayerFilterTypeGBRGAct->setChecked(true);
	//			break;
	//	};
	//	modeMenu->addSeparator();
	//	modeMenu->addAction(modeImageTypeSTEREOAct);
	//	modeMenu->addAction(modeImageTypeMONOAct);
	//	modeMenu->addSeparator();
	//	modeMenu->addAction(modeImageConverterYUV422Act);
	//	modeMenu->addAction(modeImageConverterYUV411Act);
	//	modeMenu->addSeparator();
	//	modeMenu->addAction(modeImageConverterGRAYAct);
	//	modeMenu->addAction(modeImageConverterMASKAct);
	//	modeMenu->addAction(modeImageConverterSIMPLEAct);
	//	modeMenu->addAction(modeImageConverterSIMPLE2Act);
	//	modeMenu->addAction(modeImageConverterLINEARAct);
	//
	viewMenu = menuBar()->addMenu(tr("&View"));
	viewMenu->addAction(modeViewPixelsAct);
	viewMenu->addAction(modeViewImageAct);
	viewMenu->addSeparator();
	viewMenu->addAction(modeViewImageRGBAct);
	viewMenu->addAction(modeViewImageREDAct);
	viewMenu->addAction(modeViewImageGREENAct);
	viewMenu->addAction(modeViewImageBLUEAct);
	viewMenu->addAction(modeViewImageHUEAct);
	viewMenu->addAction(modeViewImageSATURATIONAct);
	viewMenu->addAction(modeViewImageVALUEAct);
}

void MainWindow::createToolBars() {
	toolBar = addToolBar(tr("File"));
	toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	//	toolBar->addAction(newAct);
}

//void MainWindow::newFile()
//{
//	QMessageBox::warning(this, tr("New File"),
//								   tr("I cannot create a new File.\n"
//									  "Do you want to improve the program?"),
//								   QMessageBox::Yes | QMessageBox::No
//								   | QMessageBox::Cancel,
//								   QMessageBox::Yes);
//}

//void MainWindow::modeBayerFilterTypeGRBG(){camThread->setBayerFilterType(CamThread::GRBG);};
//void MainWindow::modeBayerFilterTypeRGGB(){camThread->setBayerFilterType(CamThread::RGGB);};
//void MainWindow::modeBayerFilterTypeBGGR(){camThread->setBayerFilterType(CamThread::BGGR);};
//void MainWindow::modeBayerFilterTypeGBRG(){camThread->setBayerFilterType(CamThread::GBRG);};
//
//void MainWindow::modeImageTypeSTEREO(){camThread->setImageType(CamThread::STEREO);};
//void MainWindow::modeImageTypeMONO(){camThread->setImageType(CamThread::MONO);};
//void MainWindow::modeImageConverterYUV422(){camThread->setImageConverter(CamThread::YUV422);};
//void MainWindow::modeImageConverterYUV411(){camThread->setImageConverter(CamThread::YUV411);};
//void MainWindow::modeImageConverterGRAY(){camThread->setImageConverter(CamThread::GRAY);};
//void MainWindow::modeImageConverterMASK(){camThread->setImageConverter(CamThread::MASK);};
//void MainWindow::modeImageConverterSIMPLE(){camThread->setImageConverter(CamThread::SIMPLE);};
//void MainWindow::modeImageConverterSIMPLE2(){camThread->setImageConverter(CamThread::SIMPLE2);};
//void MainWindow::modeImageConverterLINEAR(){camThread->setImageConverter(CamThread::LINEAR);};

void MainWindow::modeViewPixels() {
	framePaintWidget->miro.viewPixels = true;

	modeViewImageRGBAct->setEnabled(false);
	modeViewImageREDAct->setEnabled(false);
	modeViewImageGREENAct->setEnabled(false);
	modeViewImageBLUEAct->setEnabled(false);
	modeViewImageHUEAct->setEnabled(false);
	modeViewImageSATURATIONAct->setEnabled(false);
	modeViewImageVALUEAct->setEnabled(false);
}
;

void MainWindow::modeViewImage() {
	framePaintWidget->miro.viewPixels = false;

	modeViewImageRGBAct->setEnabled(true);
	modeViewImageREDAct->setEnabled(true);
	modeViewImageGREENAct->setEnabled(true);
	modeViewImageBLUEAct->setEnabled(true);
	modeViewImageHUEAct->setEnabled(true);
	modeViewImageSATURATIONAct->setEnabled(true);
	modeViewImageVALUEAct->setEnabled(true);
}
;

void MainWindow::modeViewImageRGB() {
	framePaintWidget->miro.viewImage = MiroVision::RGB;
}
;

void MainWindow::modeViewImageRED() {
	framePaintWidget->miro.viewImage = MiroVision::RED;
}
;

void MainWindow::modeViewImageGREEN() {
	framePaintWidget->miro.viewImage = MiroVision::GREEN;
}
;

void MainWindow::modeViewImageBLUE() {
	framePaintWidget->miro.viewImage = MiroVision::BLUE;
}
;

void MainWindow::modeViewImageHUE() {
	framePaintWidget->miro.viewImage = MiroVision::HUE;
}
;

void MainWindow::modeViewImageSATURATION() {
	framePaintWidget->miro.viewImage = MiroVision::SATURATION;
}
;

void MainWindow::modeViewImageVALUE() {
	framePaintWidget->miro.viewImage = MiroVision::VALUE;
}
;

void MainWindow::paintEvent(QPaintEvent * /* event */) {
	//	if (framePaintWidget->image.width()!=lastWidth || framePaintWidget->image.height()!=lastHeight || camThread->framerate!=lastFramerate)
	if (framePaintWidget->image.width() != lastWidth
			|| framePaintWidget->image.height() != lastHeight
			|| grabberThread->framerate != lastFramerate) {
		lastFramerate = grabberThread->framerate;
		//		lastFramerate = camThread->framerate;
		//		QString encodingname;
		//		switch (camera->getImageEncoding())
		//		{
		//			case DC1394_COLOR_CODING_MONO8:
		//				encodingname = "MONO 8";
		//				break;
		//			case DC1394_COLOR_CODING_YUV411:
		//				encodingname = "Y'UV 411";
		//				break;
		//			case DC1394_COLOR_CODING_YUV422:
		//				encodingname = "Y'UV 422";
		//				break;
		//			case DC1394_COLOR_CODING_YUV444:
		//				encodingname = "Y'UV 444";
		//				break;
		//			case DC1394_COLOR_CODING_RGB8:
		//				encodingname = "RGB 8";
		//				break;
		//			case DC1394_COLOR_CODING_MONO16:
		//				encodingname = "MONO 16";
		//				break;
		//			case DC1394_COLOR_CODING_RGB16:
		//				encodingname = "RGB 16";
		//				break;
		//			case DC1394_COLOR_CODING_MONO16S:
		//				encodingname = "MONO 16S";
		//				break;
		//			case DC1394_COLOR_CODING_RGB16S:
		//				encodingname = "RGB 16S";
		//				break;
		//			case DC1394_COLOR_CODING_RAW8:
		//				encodingname = "RAW 8";
		//				break;
		//			case DC1394_COLOR_CODING_RAW16:
		//				encodingname = "RAW 16";
		//				break;
		//			default:
		//				encodingname = "unknown";
		//				break;
		//		};

		statusBar()->showMessage(tr("width = ") + QString::number(
				framePaintWidget->image.width()) + tr(" height = ")
				+ QString::number(framePaintWidget->image.height())
		//								 + tr(" depth = ") + QString::number(camera->getImageDepth())
				//								 + tr(" encoding = ") + encodingname
				+ tr(" framerate = ") + QString::number(
				grabberThread->framerate));
		//								 + tr(" framerate = ") + QString::number(camThread->framerate));
		lastWidth = framePaintWidget->image.width();
		lastHeight = framePaintWidget->image.height();
		framePaintWidget->setMinimumSize(framePaintWidget->image.width(),
				framePaintWidget->image.height());
	};
	framePaintWidget->repaint();
}

void MainWindow::newFrame(const QImage& image) {
	framePaintWidget->image = image;
	framePaintWidget->repaint(); // better update() ?	... see documentation in qwidget
}

void FramePaintWidget::paintEvent(QPaintEvent * /* event */) {
	QPainter painter(this);
#if 0
	miro.findRobotsInImage(image);
	if (miro.viewPixels)
	{
		painter.drawImage((width()-miro.detectedImage.width())/2,(height()-miro.detectedImage.height())/2,miro.detectedImage);
	}
	else
	{
		painter.drawImage((width()-image.width())/2,(height()-image.height())/2,image);
	};
	painter.setPen(QPen(Qt::black));
	//painter.drawText(QRectF(0,0,width(),height()),"Hi!",QTextOption(Qt::AlignHCenter | Qt::AlignVCenter));
	//painter.drawText(QRectF(0,0,width(),height()),
	//				 tr("width = ") + QString::number(image.width()) + tr(" height = ") + QString::number(image.height()),
	//				 QTextOption(Qt::AlignHCenter | Qt::AlignVCenter));
#else
	char buffer[1024];
	sprintf(buffer,recordimagefilename,sequencenum++);
	image.save(buffer);
	painter.drawImage((width() - image.width()) / 2,
			(height() - image.height()) / 2, image);
	painter.setPen(QPen(Qt::black));
	//	painter.drawText(QRectF(0,0,width(),height()),tr("Mirosot Vision is disabled! Enable it in ")+tr(__FILE__)+tr(" line ")+
	//					 QString::number(__LINE__-19),
	//					 QTextOption(Qt::AlignHCenter | Qt::AlignVCenter));
	//painter.drawText(QRectF(0,0,width(),height()),
	//				 tr("width = ") + QString::number(image.width()) + tr(" height = ") + QString::number(image.height()),
	//				 QTextOption(Qt::AlignHCenter | Qt::AlignVCenter));
#endif
}
