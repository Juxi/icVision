#ifndef __SLIDER2_WINDOW__
#define __SLIDER2_WINDOW__

#include <QSlider>
#include <QLabel>
#include <QThread>
#include <QGridLayout>

#include <iostream>
#include <string>

#include "valueslider.h"

class SliderWindow : public QWidget {
	Q_OBJECT
//	Q_INTERFACES(QWidget)

	std::vector<ValueSlider*> d_sliders;
	bool d_changed;
	double d_test;
public:
	SliderWindow();

	virtual ~SliderWindow(){}
	void add_slider(std::string name, double *value, double min_value, double max_value);

	bool changed() {return d_changed;}
	void changed(bool val) {d_changed = val;}


private:
    QSlider *horizontalSlider;
    QGridLayout *controlsLayout;

    void create_controls();

public slots:

	void setValue(int value);
	void set_changed();

signals:
	void something_changed();
};

#endif
