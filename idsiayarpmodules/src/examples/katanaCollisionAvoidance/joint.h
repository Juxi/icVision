#ifndef JOINT_H
#define JOINT_H

#include <QtCore>
#include "interval.h"
#include "zphandler.h"
//class Link;

class Joint
{
public:
    Joint( Motor* motor );
    virtual ~Joint();

    const QMatrix4x4& getM() const { return M; }

protected:
    //Link*    parentLink;
    //Robot*   parentRobot;
    Motor*   motor;

    //int      index;
    qreal    position;      // joint position
    Interval limits;

    QVector3D  axis;     // unit vector parallel to the joint axis
    QMatrix4x4 M;        // transformation (displacement and rotation) matrix from current joint CS to WCS

    void setPos();
    virtual void setM() = 0;

    friend class Motor;
    friend class Robot;
    friend bool ZPHandler::startElement( const QString &namespaceURI,
                                              const QString &localName,
                                              const QString &qName,
                                              const QXmlAttributes &attributes );
    friend bool ZPHandler::endElement(const QString & /* namespaceURI */,
                                           const QString & /* localName */,
                                           const QString &qName);
};

#endif // JOINT_H
