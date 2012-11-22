#include <QtGui>
#include "modelwindow.h"
#include "glwidget.h"

using namespace KinematicModel;

ModelWindow::ModelWindow()
{
    glWidget = new GLWidget;
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(glWidget);
    setLayout(mainLayout);
    setWindowTitle(tr("Kinematic Robot Model"));
}

