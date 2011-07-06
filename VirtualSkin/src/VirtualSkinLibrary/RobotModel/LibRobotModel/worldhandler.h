/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RobotModel
 *	@{
 */

#ifndef WORLDHANDLER_H
#define WORLDHANDLER_H

#include <QXmlDefaultHandler>

namespace RobotModel
{
	class World;
	class WorldHandler;
	class CompositeObject;
	class PrimitiveObject;
}

/*! \brief A custom handler for the Qt XML parser
 *
 * This code is mostly borrowed from the QtXML example. The functions startElement(...), and endElement(...)
 * have been reimplemented to handle our XML specification, and the function createChildItem() in the QtXML example
 * has become createChildBodyPart(), createChildMotor(), and createChildJoint().
 */
//!TODO: Document XML Spec
class RobotModel::WorldHandler : public QXmlDefaultHandler
{
public:
	WorldHandler(World *world);
	bool startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &attributes);
	bool endElement(const QString &namespaceURI, const QString &localName, const QString &qName);
	bool characters(const QString &str);
	bool fatalError(const QXmlParseException &exception);
	QString errorString() const;

private:
	
	World		*world;
	CompositeObject *obj;
	PrimitiveObject *prim;

	QString currentText;
	QString errorStr;
	bool	metWorldTag;

	unsigned int markerCounter;
	static const Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive;	
	
};

#endif

/** @} */
