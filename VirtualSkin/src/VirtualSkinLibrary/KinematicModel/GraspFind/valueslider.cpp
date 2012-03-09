#include "valueslider.h"
#include <iostream>

using namespace std;

ValueSlider::ValueSlider(std::string name, double *value, double min_value, double max_value, QWidget *parent) :
	d_slider( new QSlider(Qt::Horizontal, parent)),
	d_value(value),
	d_min_value(min_value),
	d_max_value(max_value) {

	d_slider->setFocusPolicy(Qt::StrongFocus);
	d_slider->setTickPosition(QSlider::TicksBothSides);
	d_slider->setTickInterval(10);
	d_slider->setSingleStep(1);

	set_slider_to_value(*value);

    connect(d_slider, SIGNAL(valueChanged(int)), this, SLOT(set_value(int)));
}

QSlider *ValueSlider::get_slider() {
	return d_slider;
}

double ValueSlider::slider_to_value(double in) {
	int slider_max(100);
	double new_val = (double) in / slider_max;
	new_val = new_val * (d_max_value - d_min_value) + d_min_value;
	return new_val;
}

double ValueSlider::value_to_slider(double in) {
	double slider_max(100);
	double new_val = (in - d_min_value) / (d_max_value - d_min_value);
	new_val *= slider_max;
	return new_val;
}

void ValueSlider::set_slider_to_value(double val) {
	int slider_val = value_to_slider(val);
	cout << "slider val: " << slider_val << endl;
	cout << "in value: " << val << " " << value_to_slider(val) << endl;
	d_slider->setValue(slider_val);
}

void ValueSlider::set_value(int val) {
	double new_val = slider_to_value((double) val);


	*d_value = new_val;

	cout << "new value: " << *d_value << endl;
	emit something_changed();
}
