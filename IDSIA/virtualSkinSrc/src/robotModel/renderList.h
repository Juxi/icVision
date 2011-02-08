#ifndef RENDERLIST_H
#define RENDERLIST_H

#include <QObject>

class DisplayList;

class RenderList : public QObject
{
	Q_OBJECT
	
public:
    RenderList();
    virtual ~RenderList();
	
	virtual void render() = 0;
	virtual void notColliding() = 0;

public slots:
	void callLists() { render(); }
	
signals:
	void appendedObject( DisplayList* list );
	void outdatedDisplayList( int idx );
	void changedState();
};

#endif // RENDERLIST_H
