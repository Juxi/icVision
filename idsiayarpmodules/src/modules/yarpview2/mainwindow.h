#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QImage>
#include <QtGui>

//#include "camthread.h"
#include "grabber.h"
#include "grabberthread.h"
#include "mirovision.h"

#define recordimagefilename "image%05u.jpg"

class FramePaintWidget: public QWidget
{
    Q_OBJECT
	friend class MainWindow;
protected:
    void paintEvent(QPaintEvent *event);
	QImage image;
	MiroVision miro;
public:
	unsigned int sequencenum;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
//    MainWindow(Camera* camera, CamThread *camThread, QWidget *parent = 0);
    MainWindow(Grabber* grabber, GrabberThread *grabberThread, QWidget *parent = 0);
    ~MainWindow();
	
public slots:
    void newFrame(const QImage& image);
	
protected:
    void paintEvent(QPaintEvent *event);
    Grabber *grabber;
    GrabberThread *grabberThread;
//	CamThread *camThread;
//	Camera *camera;
	int lastWidth;
	int lastHeight;
	float lastFramerate;
private:
	void readSettings();
	void writeSettings();
	void createActions();
	void createMenus();
	void createToolBars();
	FramePaintWidget* framePaintWidget;
private:
//	QAction *newAct;
//	QActionGroup *modeBayerFilterTypeGroup;
//	QAction *modeBayerFilterTypeGRBGAct;
//	QAction *modeBayerFilterTypeRGGBAct;
//	QAction *modeBayerFilterTypeBGGRAct;
//	QAction *modeBayerFilterTypeGBRGAct;
//	QActionGroup *modeImageTypeGroup;
//	QAction *modeImageTypeSTEREOAct;
//	QAction *modeImageTypeMONOAct;
//	QActionGroup *modeImageConverterGroup;
//	QAction *modeImageConverterYUV422Act;
//	QAction *modeImageConverterYUV411Act;
//	QAction *modeImageConverterGRAYAct;
//	QAction *modeImageConverterMASKAct;
//	QAction *modeImageConverterSIMPLEAct;
//	QAction *modeImageConverterSIMPLE2Act;
//	QAction *modeImageConverterLINEARAct;
	QActionGroup *modeViewGroup;
	QAction *modeViewImageAct;
	QAction *modeViewPixelsAct;
	QActionGroup *modeViewImageGroup;
	QAction *modeViewImageRGBAct;
	QAction *modeViewImageREDAct;
	QAction *modeViewImageGREENAct;
	QAction *modeViewImageBLUEAct;
	QAction *modeViewImageHUEAct;
	QAction *modeViewImageSATURATIONAct;
	QAction *modeViewImageVALUEAct;

	QMenu *fileMenu;
	QMenu *modeMenu;
	QMenu *viewMenu;

	QToolBar* toolBar;

private slots:
//	void newFile();
//	void modeBayerFilterTypeGRBG();
//	void modeBayerFilterTypeRGGB();
//	void modeBayerFilterTypeBGGR();
//	void modeBayerFilterTypeGBRG();
//	void modeImageTypeSTEREO();
//	void modeImageTypeMONO();
//	void modeImageConverterYUV422();
//	void modeImageConverterYUV411();
//	void modeImageConverterGRAY();
//	void modeImageConverterMASK();
//	void modeImageConverterSIMPLE();
//	void modeImageConverterSIMPLE2();
//	void modeImageConverterLINEAR();
	void modeViewPixels();
	void modeViewImage();
	void modeViewImageRGB();
	void modeViewImageRED();
	void modeViewImageGREEN();
	void modeViewImageBLUE();
	void modeViewImageHUE();
	void modeViewImageSATURATION();
	void modeViewImageVALUE();
};

#endif
