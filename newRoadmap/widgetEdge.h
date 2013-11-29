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

#ifndef EDGE_H
#define EDGE_H

#include <QGraphicsItem>
#include <QMutex>

class QtGraphNode;

class QtGraphEdge : public QGraphicsItem
{
public:
    QtGraphEdge(QtGraphNode *sourceNode, QtGraphNode *destNode);
    ~QtGraphEdge();

    QtGraphNode *sourceNode() const;
    void setSourceNode(QtGraphNode *node);

    QtGraphNode *destNode() const;
    void setDestNode(QtGraphNode *node);
	
	void adjust();
	//void update() { mutex.lock(); color = newColor; mutex.unlock(); }
	
	// these are thread safe
	void setColor( QColor c );
	void setWeight( int w );
	//void setRender( bool r ) { renderMe = r; }
	//void flagRemoval() { deleteMe = true; }
	//bool removeMe() { return deleteMe; }
	//bool reRenderMe() { return changed; }
	void kill() { deathWish = true; }
    
protected:
	
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    
private:
	
	//bool renderMe;
	bool deathWish;
    QtGraphNode *source, *dest;

    QPointF sourcePoint;
    QPointF destPoint;
	
	QColor color;
	int weight;
	
	//bool changed;
	
	//QMutex mutex;
	
	//bool deleteMe;
};

#endif