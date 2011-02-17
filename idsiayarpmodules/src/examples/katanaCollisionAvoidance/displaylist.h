#ifndef DISPLAYLIST_H
#define DISPLAYLIST_H

#include <QMatrix4x4>
#include <qgl.h>

class DisplayList
{
public:
    DisplayList();
    virtual ~DisplayList();

    virtual void makeDisplayList() = 0;

    void setDisplayListIdx( int idx ) { index = idx; }
    int displayListIdx() { return index; }
    virtual void render();

    virtual void setT() { setT(QMatrix4x4()); }
    virtual void setT( const QMatrix4x4& txfr ) { T = txfr; }
    const QMatrix4x4& getT() { return T; }

    void setColliding() { colliding = true; }
    void unSetColliding() { colliding = false; }
    bool isColliding() const { return colliding; }

private:
    int index;
    bool colliding;
    QMatrix4x4 T;

    GLfloat red[4];
    GLfloat gray[4];
};

#endif // DISPLAYLIST_H
