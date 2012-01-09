/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>

#include "newMapDialog.h"

//! [0]
NewMapDialog::NewMapDialog()
{

	bigEditor = new QTextEdit;
	
	createMenu();
	createButtons();
    //bigEditor->setPlainText(tr("This widget takes up all the remaining space "
    //                           "in the top-level layout."));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMenuBar(menuBar);
    mainLayout->addWidget(bigEditor);
    mainLayout->addWidget(horizontalGroupBox);

    setLayout(mainLayout);

    setWindowTitle(tr("Create Roadmap"));
}
//! [5]

void NewMapDialog::insertSample( std::vector<double> p )
{
	QString thisline;
	thisline.append( QString("%1 ").arg( (double)rand()/(double)RAND_MAX ) );
	thisline.append( QString("%1 ").arg( (double)rand()/(double)RAND_MAX ) );
	
	std::vector<double>::iterator i;
	for ( i = p.begin(); i!=p.end(); ++i )
	{
		thisline.append( QString("%1 ").arg(*i) );
	}
	
	bigEditor->append(thisline);
}

void NewMapDialog::accept()
{
	QString fileName = QFileDialog::getSaveFileName(this,
													tr("Save Roadmap Vertices"), "",
													tr("All Files (*)"));
	if (fileName.isEmpty())
		return;
	else {
		QFile file(fileName);
		if (!file.open(QIODevice::WriteOnly)) {
			QMessageBox::information(this, tr("Unable to open file"),
									 file.errorString());
			return;
		}
		QTextStream out(&file);
		out << bigEditor->toPlainText();
		file.close();
	}
	
	QDialog::accept();
}

void NewMapDialog::createButtons()
{	
	randomSampleButton = new QPushButton(tr("&Random Sample"));
	currentPoseButton = new QPushButton(tr("&Current Pose"));
	buttonBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel);
	
	QVBoxLayout *subLayout = new QVBoxLayout;
	subLayout->addWidget(randomSampleButton);
	subLayout->addWidget(currentPoseButton);
	
	QHBoxLayout *layout = new QHBoxLayout;
	layout->addLayout(subLayout);
	//addWidget(sampleButton, 40, Qt::AlignLeft);
	layout->addWidget(buttonBox, 60, Qt::AlignRight);
	
	horizontalGroupBox = new QGroupBox;
	horizontalGroupBox->setFlat(true);	
	horizontalGroupBox->setLayout(layout);
	
	connect(randomSampleButton, SIGNAL(clicked()), this, SIGNAL(requestRandomSample()));
	connect(currentPoseButton, SIGNAL(clicked()), this, SIGNAL(requestCurrentPose()));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

//! [6]
void NewMapDialog::createMenu()
{
    menuBar = new QMenuBar;

    mapMenu = new QMenu(tr("&File"), this);
    exitAction = mapMenu->addAction(tr("E&xit"));
    menuBar->addMenu(mapMenu);

    connect(exitAction, SIGNAL(triggered()), this, SLOT(accept()));
}
