/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RobotModel
 *	@{
 */

#ifndef KINTREEHANDLER_H
#define KINTREEHANDLER_H

#include <QXmlDefaultHandler>

namespace RobotModel
{
	class Robot;
	class BodyPart;
	class Motor;
	class KinTreeNode;
	class ZPHandler;
}

/*! \brief A custom handler for the Qt XML parser
 *
 * This code is mostly borrowed from the QtXML example. The functions startElement(...), and endElement(...)
 * have been reimplemented to handle our XML specification, and the function createChildItem() in the QtXML example
 * has become createChildBodyPart(), createChildMotor(), and createChildJoint().
 */
//!TODO: Document XML Spec
class RobotModel::ZPHandler : public QXmlDefaultHandler
{
public:
	ZPHandler(Robot *tree);
	bool startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &attributes);
	bool endElement(const QString &namespaceURI, const QString &localName, const QString &qName);
	bool characters(const QString &str);
	bool fatalError(const QXmlParseException &exception);
	QString errorString() const;

private:
	
	Robot		*robot;
	BodyPart	*bodyPart;
	Motor		*motor;
	KinTreeNode *node;

	BodyPart*		createChildPart();
	Motor*			createChildMotor( const int motorIdx );
	KinTreeNode*	createChildLink();
	KinTreeNode*	createChildJoint( const QString& type );

	QString currentText;
	QString errorStr;
	bool	metKinTreeTag;
	
	static const Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive;	
	
};

#endif

/** @} */
