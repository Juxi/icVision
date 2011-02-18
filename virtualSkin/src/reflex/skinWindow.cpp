#include <QtGui>
#include "skinWindow.h"
#include "glwidget.h"

SkinWindow::SkinWindow()
{
    glWidget = new GLWidget;
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(glWidget);
    setLayout(mainLayout);
    setWindowTitle(tr("Virtual Skin"));
}

