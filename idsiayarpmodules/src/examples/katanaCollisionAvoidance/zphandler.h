#ifndef KINTREEHANDLER_H
#define KINTREEHANDLER_H

#include <QtGui>
#include <QXmlDefaultHandler>

class Robot;
class Branch;
class Motor;
class KinTreeNode;
class Object;
class Joint;

/*! \brief A custom handler for the Qt XML parser (see Robot)
 *
 * This code is mostly borrowed from the QtXML example. The functions startElement(...), and endElement(...)
 * have been reimplemented to handle our XML specification, and the function createChildItem() in the QtXML example
 * has become createChildBranch(), createChildMotor(), and createChildJoint().
 *
 * The XML specification is as follows: ***ADD XML SPEC HERE***
 */

class ZPHandler : public QXmlDefaultHandler
{
public:
    ZPHandler(Robot *tree);
    bool startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &attributes);
    bool endElement(const QString &namespaceURI, const QString &localName, const QString &qName);
    bool characters(const QString &str);
    bool fatalError(const QXmlParseException &exception);
    //bool error(const QXmlParseException &exception);
    //bool warning(const QXmlParseException &exception);
    QString errorString() const;

private:
    Robot  *robot;
    Branch *branch;
    Motor  *motor;
    KinTreeNode *node;
    Object *object;

    Branch *createChildBranch();
    Motor  *createChildMotor();
    KinTreeNode *createChildLink();
	Joint *createChildJoint( const QString& type );

    QString currentText;
    QString errorStr;
    bool metKinTreeTag;
};

#endif
