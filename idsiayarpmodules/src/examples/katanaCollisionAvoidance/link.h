#ifndef LINK_H
#define LINK_H

#include <iostream>
#include<QtGui>
#include "zphandler.h"
#include "physobject.h"

class Robot;
class Joint;

/*! \brief Represents a link in the robot
 *
 */

class Link
{
public:
    Link( Robot* robot );
    Link( Link* parentLink );
    ~Link();            // how does this work???

    // set stuff
    void setJoint( Joint* phatJoint ) { joint = phatJoint; }
    Joint* getJoint() { return joint; }

    // get stuff
    Link* parent() const;
    const Robot* robot() const;
    const int idx() const;
    const QString& name() const;
    //const QVector3D& bodyVector() const;
    const QMatrix4x4& txfrToWorldCS() const;
    const QVector<PhysObject*>& getPhysObjectList() const { return physObjectList; }

    void print() const;
    void printAll();



protected:
    Link*    parentLink;
    Robot*   parentRobot;
    int      index;

    QString        linkName;        // a user-readable identifier for the joint (no uniqueness check)
    QVector<Link*> children;

    Joint*         joint;
    QVector<PhysObject*> physObjectList;    // for SOLID and OpenGL...  is there a better way?
    QVector3D      bodyVector;
    QMatrix4x4     M,       // transformation (displacement and rotation) matrix from current joint CS to the CS of this link
                   T;        // transformation (displacement and rotation) matrix from current joint CS to WCS



    void initialize();
    void emitFinished();
    void updateTxfrMatrix( const QMatrix4x4& TransformationMatrix );

    void filterCollisionPairs(DtResponse response);
    void filterAdjacentChildren( Link* link, PhysObject* obj, DtResponse response );

    friend class Robot;
    friend bool ZPHandler::startElement( const QString &namespaceURI,
                                              const QString &localName,
                                              const QString &qName,
                                              const QXmlAttributes &attributes );
    friend bool ZPHandler::endElement(const QString & /* namespaceURI */,
                                           const QString & /* localName */,
                                           const QString &qName);
};

#endif // LINK_H
