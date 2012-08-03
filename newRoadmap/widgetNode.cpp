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

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>

#include "widgetEdge.h"
#include "widgetNode.h"
#include "graphwidget.h"

QtGraphNode::QtGraphNode(GraphWidget *graphWidget) :	deathWish(false),
														graph(graphWidget),
														//x(0), y(0),
														primaryColor(Qt::lightGray),
														secondaryColor(Qt::gray)
														//newPrimaryColor(Qt::yellow),
														//newSecondaryColor(Qt::darkYellow),
														//changed(false)
{
	normPos = QPointF(0,0);
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setZValue(-1);
}

void QtGraphNode::addEdge(QtGraphEdge *edge)
{
    edgeList << edge;
    edge->adjust();
}

QList<QtGraphEdge *> QtGraphNode::edges() const
{
    return edgeList;
}

void QtGraphNode::setNormPos( const QPointF q )
{
	//mutex.lock();
		if ( q.x() < 0.0 || q.x() > 1.0 || q.y() < 0.0 || q.y() > 1.0 )
		{
			printf("invalid call to QtGraphNode::setNormPos()...  values must be [0...1]\n");
			return;
		}
		
		normPos = q;
		updatePosition();
	//mutex.unlock();
}

void QtGraphNode::updatePosition()
{
	QRectF sceneRect = scene()->sceneRect();
	QPointF p( sceneRect.left() + normPos.x()*(sceneRect.width()),
			  sceneRect.top() + normPos.y()*(sceneRect.height()));
	if ( p.x() - sceneRect.left() < boundingRect().width()/2 )
		p.setX( sceneRect.left() + boundingRect().width()/2 );
	else if ( sceneRect.right() - p.x() < boundingRect().width()/2 )
		p.setX( sceneRect.right() - boundingRect().width()/2 );
	if ( p.y() - sceneRect.top() < boundingRect().height()/2 ) 
		p.setY( sceneRect.top() + boundingRect().height()/2 );
	else if ( sceneRect.bottom() - p.y() < boundingRect().height()/2 )
		p.setY( sceneRect.bottom() - boundingRect().height()/2 );
	
	QGraphicsItem::setPos(p);
}

void QtGraphNode::setColor( QColor c1, QColor c2 )
{ 
	//printf("changing vertex color\n");
	//mutex.lock();
		primaryColor = c1; 
		secondaryColor = c2;
		update();
	//mutex.unlock();
}

QRectF QtGraphNode::boundingRect() const
{
    qreal adjust = 2;
    return QRectF(-10 - adjust, -10 - adjust,
                  23 + adjust, 23 + adjust);
}

QPainterPath QtGraphNode::shape() const
{
    QPainterPath path;
    path.addEllipse(-10, -10, 20, 20);
    return path;
}

void QtGraphNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
	int size = 10;
	
	//mutex.lock();
	
	// drop shadow
    //painter->setPen(Qt::NoPen);
    //painter->setBrush(Qt::darkGray);
    //painter->drawEllipse(0, 0, size, size);

    QRadialGradient gradient(-3, -3, 10);
    if (option->state & QStyle::State_Sunken) {
        gradient.setCenter(3, 3);
        gradient.setFocalPoint(3, 3);
        gradient.setColorAt(1, primaryColor.light(120));
        gradient.setColorAt(0, secondaryColor.light(120));
    } else {
        gradient.setColorAt(0, primaryColor);
        gradient.setColorAt(1, secondaryColor);
    }
	
    painter->setBrush(gradient);
    painter->setPen(QPen(Qt::black, 0));
    painter->drawEllipse(-size/2, -size/2, size, size);
	
	//mutex.unlock();
}

QVariant QtGraphNode::itemChange(GraphicsItemChange change, const QVariant &value)
{
	//printf("called itemChange()\n");
    switch (change) {
    case ItemPositionHasChanged:
        foreach (QtGraphEdge *edge, edgeList)
            edge->adjust();
        break;
    default:
        break;
    };

    return QGraphicsItem::itemChange(change, value);
}

void QtGraphNode::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	updatePosition();
    QGraphicsItem::mousePressEvent(event);
}

void QtGraphNode::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	QRectF sceneRect = scene()->sceneRect();
	QPointF p = event->scenePos();
	
	// handle the item being dropped outside the window
	if ( p.x() > sceneRect.right() ) p.setX( sceneRect.right() );
	else if ( p.x() < sceneRect.left() ) p.setX( sceneRect.left() );
	if ( p.y() < sceneRect.top() ) p.setY( sceneRect.top() );
	else if ( p.y() > sceneRect.bottom() ) p.setY( sceneRect.bottom() );
	p.setX( (p.x()-sceneRect.left())/sceneRect.width() );
	p.setY( (p.y()-sceneRect.top())/sceneRect.height() );
	
	setNormPos(p);
    //updatePosition();
    QGraphicsItem::mouseReleaseEvent(event);
}
