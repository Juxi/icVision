#include <QtGui>
#include "skinWindow.h"
#include "glwidget.h"

using namespace RobotModel;

SkinWindow::SkinWindow()
{
    glWidget = new GLWidget;
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(glWidget);
    setLayout(mainLayout);
    setWindowTitle(tr("Virtual Skin"));
}

