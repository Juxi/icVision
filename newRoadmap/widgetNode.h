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

#ifndef NODE_H
#define NODE_H

#include <QGraphicsItem>
#include <QList>
#include <QMutex>

class QtGraphEdge;
class GraphWidget;
QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE

class QtGraphNode : public QGraphicsItem
{
public:
    QtGraphNode(GraphWidget *graphWidget);

	void setIdx( int i ) { idx=i; }
	int getIdx() { return idx; }
	
    void addEdge(QtGraphEdge *edge);
    QList<QtGraphEdge *> edges() const;

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	void updatePosition();
	
	// these need to be thread safe
	void setNormPos(const QPointF);
	qreal getNormX() { return normPos.x(); }
	qreal getNormY() { return normPos.y(); }
	
	void setColor( QColor c1, QColor c2 );
	//bool reRenderMe() { return changed; }
	
protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	
private:
	int idx;
	GraphWidget *graph;
    QList<QtGraphEdge *> edgeList;
	
	//qreal x,y;
	QPointF normPos;
	QColor	primaryColor,
			secondaryColor;
	//		newPrimaryColor,
	//		newSecondaryColor;
	
	//bool changed;
	
	//QMutex mutex;
};

#endif