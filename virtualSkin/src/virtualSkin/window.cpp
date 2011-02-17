#include <QtGui>
#include "window.h"
#include "glwidget.h"

Window::Window()
{
    glWidget = new GLWidget;
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(glWidget);
    setLayout(mainLayout);
    setWindowTitle(tr("Virtual Skin"));
}

