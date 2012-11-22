#ifndef __VALUE_SLIDER__
#define __VALUE_SLIDER__

#include <QSlider>

class ValueSlider : public QObject {
	Q_OBJECT

	QSlider *d_slider;
	double *d_value;
	double d_min_value, d_max_value;

public:
	ValueSlider(std::string name, double *value, double min_value, double max_value, QWidget *parent);

	QSlider *get_slider();


	double slider_to_value(double in);
	double value_to_slider(double in);

	void set_slider_to_value(double val);

public slots:
	void set_value(int val);

signals:

	void something_changed(void);
};

#endif
