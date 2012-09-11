#include "sliderwindow.h"

using namespace std;

SliderWindow::SliderWindow() : d_changed(false) {
	create_controls();
}

//SliderWindow::~SliderWindow(){
//}

void SliderWindow::setValue(int value) {

	cout << "new value: " << value << endl;
}

void SliderWindow::create_controls() {
    controlsLayout = new QGridLayout;

    setLayout(controlsLayout);

//    controlsLayout->addWidget(horizontalSlider);
//    add_slider("test", &d_test, 0.0, 100.);
    setWindowTitle(tr("Sliders"));

//    connect(horizontalSlider, SIGNAL(valueChanged(int)), reinterpret_cast<SliderWindow*>(this), SLOT(setValue(int)));
}

void SliderWindow::add_slider(string name, double *value, double min_value, double max_value) {
	ValueSlider *slider_ptr = new ValueSlider(name, value, min_value, max_value, this);

	d_sliders.push_back(slider_ptr);

	controlsLayout->addWidget(new QLabel(tr(name.c_str())), d_sliders.size(), 0);
	controlsLayout->addWidget(slider_ptr->get_slider(), d_sliders.size(), 1);

	connect(slider_ptr, SIGNAL(something_changed()), this, SLOT(set_changed()));
}

void SliderWindow::set_changed() {
	d_changed = true;
	emit something_changed();
}
