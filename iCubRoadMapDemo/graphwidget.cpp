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

#include "graphwidget.h"
#include "widgetEdge.h"
#include "widgetNode.h"

#include <QDebug>
#include <QGraphicsScene>
#include <QWheelEvent>

#include <math.h>

GraphWidget::GraphWidget()
{
	//createMenu();
	
	QGraphicsScene *scene;
    scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
	
    setScene(scene);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setWindowTitle(tr("Kail's Roadmap Environment"));
	
	//connect( &timer, SIGNAL(timeout()), this, SLOT(update()));
	timer.start(100);
}

void GraphWidget::addNode( Roadmap::vertex_t n, qreal _x, qreal _y )
{
	QtGraphNode* node = new QtGraphNode(this);
	nodes << node;
	scene()->addItem(node);
	node->setNormPos(QPointF(_x,_y));
	emit newQtGraphNode(n,node);
	//printf("added node to graph\n");
}

void GraphWidget::addEdge( Roadmap::edge_t n, QtGraphNode* a, QtGraphNode* b )
{
	QtGraphEdge* edge = new QtGraphEdge( a, b );
	edges << edge;
	scene()->addItem(edge);
	emit newQtGraphEdge(n,edge);
	//printf("added edge to graph\n");
}

void GraphWidget::setNodePosition( QtGraphNode* node, QPointF p )
{
	if ( node ) node->setNormPos(p);
}

void GraphWidget::setNodeColor( QtGraphNode* node, QColor a, QColor b )
{
	if ( node ) node->setColor(a,b);
}

void GraphWidget::setEdgeColor( QtGraphEdge* edge, QColor c )
{
	if ( edge ) edge->setColor(c);
}

void GraphWidget::setEdgeWeight( QtGraphEdge* edge, int w)
{
	if ( edge ) edge->setWeight(w);
}

/*void GraphWidget::update()
{
    foreach (QGraphicsItem *item, scene()->items())
	{
		if (QtGraphNode *node = qgraphicsitem_cast<QtGraphNode *>(item))
		{
			//if (node->reRenderMe())
				node->updatePosition();
		}
		else if (QtGraphEdge *edge = qgraphicsitem_cast<QtGraphEdge *>(item))
		{

			//if (edge->reRenderMe())
				//edge->update();
			
            //if ( edge->removeMe() )
				//scene()->removeItem(edge);
				//delete edge;
		}
	}
}*/

/*void GraphWidget::wheelEvent(QWheelEvent *event)
{
    //scaleView(pow((double)2, -event->delta() / 240.0));
}*/

void GraphWidget::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);

	/*QPixmap bg = QPixmap(":/images/brain.jpg");
	qreal bgRatio = (qreal)bg.width()/(qreal)bg.height();
	qreal pRatio = (qreal)parentWidget()->width()/(qreal)parentWidget()->height();
	
	if ( bgRatio > pRatio )
		bg = bg.scaledToHeight(parentWidget()->height());
	else
		bg = bg.scaledToWidth(parentWidget()->width());

	painter->drawPixmap( -bg.width()/2, -bg.height()/2, bg );*/
}

/*void GraphWidget::scaleView(qreal scaleFactor)
{
    qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);
}*/

void GraphWidget::resize( QResizeEvent* event )
{
	//printf("\n\n*** resizing graph ***\n");
	qreal vScale = 0.85;
	qreal hScale = 0.95;

	scene()->setSceneRect( -hScale*event->size().width()/2,
						   -vScale*event->size().height()/2,
						   hScale*event->size().width(),
						   vScale*event->size().height());
	
    foreach (QtGraphNode *node, nodes)
		node->updatePosition();
}