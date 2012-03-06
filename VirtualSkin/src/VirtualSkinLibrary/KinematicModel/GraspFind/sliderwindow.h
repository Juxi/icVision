#ifndef __SLIDER2_WINDOW__
#define __SLIDER2_WINDOW__

#include <QSlider>
#include <QThread>
#include <QGridLayout>

#include <iostream>

class SliderWindow : public QWidget {
	Q_OBJECT
//	Q_INTERFACES(QWidget)

public:
	SliderWindow();

	virtual ~SliderWindow(){}

private:
    QSlider *horizontalSlider;
    QGridLayout *controlsLayout;

    void create_controls();

public slots:

	void setValue(int value);

signals:
};

#endif
