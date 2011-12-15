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

QtGraphNode::QtGraphNode(GraphWidget *graphWidget)
    : graph(graphWidget), primaryColor(Qt::yellow), secondaryColor(Qt::darkYellow)//, colorChanged(false)
{
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

void QtGraphNode::setPos( qreal x, qreal y)
{
	QRectF sceneRect = scene()->sceneRect();
	QPointF p( sceneRect.left() + x*(sceneRect.right()-sceneRect.left()),
			   sceneRect.bottom() + y*(sceneRect.top()-sceneRect.bottom()));
	QGraphicsItem::setPos(p);
}

void QtGraphNode::setColor( QColor c1, QColor c2 )
{ 
	mutex.lock();
	primaryColor = c1; 
	secondaryColor = c2; 
	//colorChanged = true;
	printf("***SET NODE COLOR***\n");
	
	update();
	mutex.unlock();
}

void QtGraphNode::calculateForces()
{
	//printf("CALLED CALCULATE_FORCES()\n");
	//printf("             pos: %f, %f\n", pos().x(), pos().y() );
	//printf("      target pos: %f, %f\n", targetPos.x(), targetPos.y());
	
    if (!scene() || scene()->mouseGrabberItem() == this) {
        newPos = pos();
        return;
    }

	//QGraphicsItem::setPos(targetPos);
	
	//printf("  new target pos: %f, %f\n\n", targetPos.x(), targetPos.y());
	
	qreal xvel = 0;
    qreal yvel = 0;
	
	QLineF line(targetPos, pos());
	//qreal dx = line.dx();
	//qreal dy = line.dy();
	//printf("target vector: %f, %f\n", dx, dy);
	
	//double l = 2.0 * (dx * dx + dy * dy);
	//if (l > 0) {
	//	xvel += (dx * 150.0) / l;
	//	yvel += (dy * 150.0) / l;
	//}
	/*xvel = dx*dx;
	yvel = dy*dy;
	printf("velocity command: %f, %f\n", xvel, yvel);

    // Sum up all forces pushing this item away
    
    foreach (QGraphicsItem *item, scene()->items()) {
        QtGraphNode *node = qgraphicsitem_cast<QtGraphNode *>(item);
        if (!node)
            continue;

            }

    // Now subtract all forces pulling items together
    double weight = (edgeList.size() + 1) * 10;
    foreach (QtGraphEdge *edge, edgeList) {
        QPointF pos;
        if (edge->sourceNode() == this)
            pos = mapFromItem(edge->destNode(), 0, 0);
        else
            pos = mapFromItem(edge->sourceNode(), 0, 0);
        xvel += pos.x() / weight;
        yvel += pos.y() / weight;
    } 
	
    
   //if (qAbs(xvel) < 0.1 && qAbs(yvel) < 0.1)
   //     xvel = yvel = 0;
*/
    QRectF sceneRect = scene()->sceneRect();
    newPos = pos() + QPointF(xvel, yvel);
	 
	//printf("newPos: %f, %f\n", newPos.x(), newPos.y() );
	
    newPos.setX(qMin(qMax(newPos.x(), sceneRect.left() + 10), sceneRect.right() - 10));
    newPos.setY(qMin(qMax(newPos.y(), sceneRect.top() + 10), sceneRect.bottom() - 10));
	
}

bool QtGraphNode::advance()
{
	/*printf("computing qtGraphNode.advance()\n");
	if ( colorChanged )
	{
		printf("COLOR CHANGED!!!!\n");
		colorChanged = false;
		return true;
	}*/
	
    if (newPos == pos())
        return false;

	QGraphicsItem::setPos(newPos);
    return true;
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
	
	mutex.lock();
	
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
    painter->drawEllipse(-5, -5, size, size);
	mutex.unlock();
}

QVariant QtGraphNode::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change) {
    case ItemPositionHasChanged:
        foreach (QtGraphEdge *edge, edgeList)
            edge->adjust();
        graph->itemMoved();
        break;
    default:
        break;
    };

    return QGraphicsItem::itemChange(change, value);
}

void QtGraphNode::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mousePressEvent(event);
}

void QtGraphNode::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}
