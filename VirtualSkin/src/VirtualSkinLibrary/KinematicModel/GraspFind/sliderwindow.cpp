#include "sliderwindow.h"

SliderWindow::SliderWindow() {
	create_controls();
}

//SliderWindow::~SliderWindow(){
//}

void SliderWindow::setValue(int value) {
	std::cout << "new value: " << value << std::endl;
}

void SliderWindow::create_controls() {
    QGridLayout *controlsLayout = new QGridLayout;
    horizontalSlider = new QSlider(Qt::Vertical);
    horizontalSlider->setFocusPolicy(Qt::StrongFocus);
    horizontalSlider->setTickPosition(QSlider::TicksBothSides);
    horizontalSlider->setTickInterval(10);
    horizontalSlider->setSingleStep(1);
    controlsLayout->addWidget(horizontalSlider);

    setLayout(controlsLayout);

    setWindowTitle(tr("Sliders"));

    connect(horizontalSlider, SIGNAL(valueChanged(int)), reinterpret_cast<SliderWindow*>(this), SLOT(setValue(int)));
}
