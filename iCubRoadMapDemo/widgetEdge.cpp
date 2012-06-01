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

#include <QPainter>

#include "widgetEdge.h"
#include "widgetNode.h"

#include <math.h>

static const double Pi = 3.14159265358979323846264338327950288419717;
static double TwoPi = 2.0 * Pi;

QtGraphEdge::QtGraphEdge(QtGraphNode *sourceNode, QtGraphNode *destNode) :	color(Qt::darkGray)
																			//newColor(Qt::black)
																			//deleteMe(false),
																			//changed(false)
{
    setAcceptedMouseButtons(0);
    source = sourceNode;
    dest = destNode;
    source->addEdge(this);
    dest->addEdge(this);
    adjust();
}

QtGraphEdge::~QtGraphEdge()
{
}

QtGraphNode *QtGraphEdge::sourceNode() const
{
    return source;
}

void QtGraphEdge::setSourceNode(QtGraphNode *node)
{
    source = node;
    adjust();
}

QtGraphNode *QtGraphEdge::destNode() const
{
    return dest;
}

void QtGraphEdge::setDestNode(QtGraphNode *node)
{
    dest = node;
    adjust();
}

void QtGraphEdge::setColor( QColor c )
{
	//mutex.lock();
		color = c;
		update();
	//mutex.unlock();
}

void QtGraphEdge::adjust()
{
    if (!source || !dest)
        return;

    QLineF line(mapFromItem(source, 0, 0), mapFromItem(dest, 0, 0));
	//printf("adjusting edge...  p1(%f,%f) p2(%f,%f)\n", line.p1().x(), line.p1().y(), line.p2().x(), line.p2().y());
	
	
    qreal length = line.length();

    prepareGeometryChange();

    if (length > qreal(20.)) {
        QPointF edgeOffset((line.dx() * 10) / length, (line.dy() * 10) / length);
        sourcePoint = line.p1() + edgeOffset;
        destPoint = line.p2() - edgeOffset;
    } else {
        sourcePoint = destPoint = line.p1();
    }
	
	
	//changed = true;
}

QRectF QtGraphEdge::boundingRect() const
{
    if (!source || !dest)
        return QRectF();

    qreal penWidth = 1;
    qreal extra = (penWidth /*+ arrowSize*/) / 2.0;

    return QRectF(sourcePoint, QSizeF(destPoint.x() - sourcePoint.x(),
                                      destPoint.y() - sourcePoint.y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}

void QtGraphEdge::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (!source || !dest)
        return;

	//mutex.lock();
	
		QLineF line(sourcePoint, destPoint);
	
		QLineF offset = line.normalVector();
		offset.setLength(1);
	
		QPointF a(sourcePoint.x()-offset.dx(),sourcePoint.y()-offset.dy());
		QPointF b(destPoint.x()-offset.dx(),destPoint.y()-offset.dy());
	
		QLineF offLine(a,b);
		
	int lineWeight = 1;
	if ( color == Qt::red || color == Qt::black )
		lineWeight = 3;

	painter->setPen(QPen(color, lineWeight, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
	painter->drawLine(offLine);
	
		//double angle = ::acos(line.dx() / line.length());
		//if (line.dy() >= 0)
		//	angle = TwoPi - angle;
	
	
	
		//QRectF rectangle(sourcePoint,QSizeF(line.length(),5));
		//int startAngle,spanAngle;
		//if		( sourcePoint.x() <  destPoint.x() && sourcePoint.y() <  destPoint.y() ) rectangle = QRectF(sourcePoint,destPoint);
		//else if ( sourcePoint.x() >= destPoint.x() && sourcePoint.y() >= destPoint.y() )	rectangle = QRectF(destPoint,sourcePoint);
		//else if ( sourcePoint.x() <  destPoint.x() && sourcePoint.y() >= destPoint.y() )	rectangle = QRectF(QPointF(sourcePoint.x(),destPoint.y()),
		//																									   QPointF(sourcePoint.y(),destPoint.x()));
		//else if ( sourcePoint.x() >= destPoint.x() && sourcePoint.y() <  destPoint.y() )	rectangle = QRectF(QPointF(sourcePoint.y(),destPoint.x()),
		//																									   QPointF(sourcePoint.x(),destPoint.y()));
		//else { return; }
	
		//int startAngle = 0 * 16;
	//	int spanAngle = 180 * 16;
		
	//QPainterPath myPath;
	//myPath.moveTo(center);
	//myPath.arcTo(rectangle, startAngle, spanAngle);
	//painter->drawPath(myPath);	
		//QPainter painter(this);
		//painter->drawArc(rectangle, startAngle, spanAngle);
		//setRotation(angle);
	
		// Draw the line itself
		
	
	//mutex.unlock();
	
	//changed = false;
}
