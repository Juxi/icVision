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

#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QtGui/QGraphicsView>
#include <boost/graph/adjacency_list.hpp>
#include <QMenuBar>
#include <QTimer>

#include "roadmap.h"

#include "widgetEdge.h"
#include "widgetNode.h"

class GraphWidget : public QGraphicsView
{
    Q_OBJECT
	
	typedef Roadmap::vertex_t vertex_t;
	typedef Roadmap::edge_t edge_t;

public:
	
    GraphWidget( Roadmap *roadmap );

public slots:
	
	void addNode( vertex_t );
	void addEdge( edge_t, QtGraphNode* a, QtGraphNode* b );
	void resize( QResizeEvent* event );
	
	void setNodePosition( QtGraphNode*, QPointF );
	void setNodeColor( QtGraphNode*, QColor, QColor );
	void setEdgeColor( QtGraphEdge*, QColor );
	void setEdgeWeight( QtGraphEdge*, int );
	//void removeNode( QtGraphNode* n ) { if (n) n->kill(); }
	//void removeEdge( QtGraphEdge* e ) { if (e) e->kill(); }
	void removeNode( QtGraphNode* n );
	void removeEdge( QtGraphEdge* e );

signals:
	
	void newQtGraphNode(vertex_t,QtGraphNode*);
	void newQtGraphEdge(edge_t,QtGraphEdge*);
	
protected:

    void drawBackground(QPainter *painter, const QRectF &rect);	
    //void keyPressEvent(QKeyEvent *event);
    //void wheelEvent(QWheelEvent *event);
	void mousePressEvent(QGraphicsSceneMouseEvent *event) { timer.stop(); }
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) { timer.start(); }
	
	QTimer timer;
	
private:

	Roadmap *roadmap;
	// this is because the cast in the elastic nodes example fails...
	//QList<QtGraphNode *> nodes;
	//QList<QtGraphEdge *> edges;

};

#endif
