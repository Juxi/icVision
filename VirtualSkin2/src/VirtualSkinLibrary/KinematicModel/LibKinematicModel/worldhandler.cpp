#include "worldhandler.h"
#include "model.h"
#include "compositeObject.h"
#include "primitiveobject.h"
#include "cylinder.h"
#include "sphere.h"
#include "box.h"
#include "modelexception.h"

using namespace std;
using namespace KinematicModel;

WorldHandler::WorldHandler( Model* m ) : model(m)
{
	obj = 0;
	prim = 0;
	metWorldTag = false;
}

bool WorldHandler::startElement( const QString & /* namespaceURI */,
                                   const QString & /* localName */,
                                   const QString &qName,
                                   const QXmlAttributes &attributes )
{
	/*****************************************************************************************
    *** READ THE DOCUMENT HEADER AND MAKE SURE IT IS ZERO-POSITION KINEMATIC TREE XML FILE ***
	*****************************************************************************************/
    if (!metWorldTag) {
		if ( QString::compare(qName,"WorldList",caseSensitivity) != 0 )
		{
			//printf("%s\n",qName.toStdString().c_str());
			errorStr = "The file is not a 'WorldList' file.";
			return false;
		}
		else
		{
			QString version = attributes.value("version");
			if (!version.isEmpty() && version != "1.0") {
				errorStr = "The file is not  a 'WorldList' version 1.0 file.";
				return false;
			}
			metWorldTag = true;
		}
    }
	
	/**********************
	 *** HANDLE OBJECTS ***
	 **********************/
	else if ( QString::compare(qName,"object",caseSensitivity) == 0 )
	{
		if (obj)
		{
            errorStr = "Encountered nested <object> tags.";
            return 0;
        }
		/*else if ( attributes.value("name").isEmpty() )
		{ 
			errorStr = "<object> tag must have a 'name' property. Uniqueness is not checked.";
			return 0;
		}*/
		else
		{
			if ( attributes.value("type").isEmpty() || attributes.value("type").toStdString() == "obstacle" )
			{
				obj = new CompositeObject( model->OBSTACLE() );
			}
			else if ( attributes.value("type").toStdString() == "target" )
			{
				obj = new CompositeObject( model->TARGET() ) ;
			}
			else
			{
				errorStr = "the 'type' attribute of an <object> tag must be 'obstacle' or 'target'.";
				return 0;
			}
			
			QVector3D heightAxis = QVector3D( attributes.value("hx").toDouble(), attributes.value("hy").toDouble(), attributes.value("hz").toDouble() );
			QVector3D position = QVector3D( attributes.value("px").toDouble(),  attributes.value("py").toDouble(), attributes.value("pz").toDouble() );
			
			qreal rotAngle = attributes.value("angle").toDouble()*M_PI/180;
			if ( qFuzzyIsNull( heightAxis.length() ) ) { heightAxis.setY(1); }
			
			
			obj->setName( attributes.value("name") );
			obj->setSpecialEulerOrientation( heightAxis, rotAngle );
			obj->translate(position);
		}
	}

	/*******************************************************************************
	*** HANDLE PRIMITIVES														 ***
	*******************************************************************************/
    else if ( QString::compare(qName,"sphere",caseSensitivity) == 0 || 
			  QString::compare(qName,"cylinder",caseSensitivity) == 0 || 
			  QString::compare(qName,"box",caseSensitivity) == 0)
	{
		if (!obj)
		{
            errorStr = "Encountered <sphere/>, <cylinder/>, or <box/> outside of <object>.";
            return 0;
        }
		
		/*
		 *	MAKE A GEOMETRY
		 */
		//PrimitiveObject* primitive = 0;
		try
		{
			if ( QString::compare(qName,"sphere",caseSensitivity) == 0 ) {
				qreal radius = attributes.value("radius").toDouble();
				prim = new Sphere(radius);
				//
			}
			else if ( QString::compare(qName,"cylinder",caseSensitivity) == 0 ) {
				qreal radius = attributes.value("radius").toDouble();
				qreal height = attributes.value("height").toDouble();
				prim = new Cylinder(radius,height);
			}
			else if ( QString::compare(qName,"box",caseSensitivity) == 0 ) {
				QVector3D size = QVector3D(attributes.value("width").toDouble(),
										   attributes.value("height").toDouble(),
										   attributes.value("depth").toDouble());
				prim = new Box(size,true);
			}
			obj->append(prim);
        }
		catch (std::exception& e)
		{ 
			errorStr = e.what();
			return 0;
		}
		
		QVector3D heightAxis = QVector3D( attributes.value("hx").toDouble(), attributes.value("hy").toDouble(), attributes.value("hz").toDouble() );
		QVector3D position = QVector3D( attributes.value("px").toDouble(),  attributes.value("py").toDouble(), attributes.value("pz").toDouble() );
		
		qreal rotAngle = attributes.value("angle").toDouble()*M_PI/180;
		if ( qFuzzyIsNull( heightAxis.length() ) ) { heightAxis.setY(1); }
		
		prim->setSpecialEulerOrientation( heightAxis, rotAngle );
		prim->translate(position);
    }

    else { printf("WARNING: Encountered unknown tag: %s ...skipping it.\n",qName.toStdString().c_str()); }

	//prim = 0;
    currentText.clear();
    return true;
}


bool WorldHandler::endElement(const QString & /* namespaceURI */, const QString & /* localName */, const QString &qName)
{
    if ( qName == "object") {
		model->appendObject(obj);
		obj = NULL;
    }
    else if ( qName == "sphere" || qName == "cylinder" || qName == "box" ) {
        prim = 0;
    }

    return true;
}

bool WorldHandler::characters(const QString &str)
{
    currentText += str;
    return true;
}
bool WorldHandler::fatalError(const QXmlParseException &exception)
{
	printf("XML PARSE FATAL ERROR: line %d column %d\n", exception.lineNumber(), exception.columnNumber());
	printf("%s\n", exception.message().toStdString().c_str());
    return false;
}

QString WorldHandler::errorString() const
{
    return errorStr;
}
