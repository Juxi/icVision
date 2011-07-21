/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup KinematicModel
 *	@{
 */

#ifndef DISPLAYLIST_H
#define DISPLAYLIST_H

#include "displmatrix.h"
#include <qgl.h>
#include <QTime>

namespace KinematicModel { class DisplayList; }

/** \brief A container for an OpenGL display list and its associated 4x4 displacement matrix
 *
 * This class contains wraps an OpenGL display list. It provides methods to write the display list, to call/render it, and to change its position/orientation.
 */
class KinematicModel::DisplayList
{
	
public:
	
    DisplayList();			//!< Just initializes index to 0
    virtual ~DisplayList();	//!< Nothing to do here

    virtual void makeDisplayList() = 0;	//!< Implemented by the sub-classes Sphere, Cylinder and Box
	virtual void render();				//!< Renders the display list with glCallList(int index)
										/**< Renders in red if isColliding() returns true and in gray otherwise */
	
	const DisplMatrix& getT() { return T; }	//!< Returns the 4x4 displacement matrix that specifies the position/orientation of the display list
    int displayListIdx() { return index; }	//!< Returns the index of the display list
											/**< Zero means there is no list yet because makeDisplayList() has not been called */
	
	//void setSemiTransparent() { semiTransparent = true; }	//!< Render the display list semi-transparent
	//void setOpaque() { semiTransparent = false; }			//!< Render the display list opaque
	
    void setColliding() { timeSinceLastCollision.restart(); }	//!< Sets the time of last collision to now
																/**< This is called by Model.collisionHandler( void*, DtObjectRef, DtObjectRef, const DtCollData* ) */ 
    bool isColliding() const;									//!< Determines whether or not the collision timestamp is fresh enough
	
	virtual void setCartesianOrientation( const QVector3D& rot ) { T.setCartesianOrientation(rot); }									//!< Set the orientation of the display list absolutely with respect to the world coordinate system, rotating about z, then y, then x
																																		/**< See DisplMatrix.setCartesianRotation( const QVector3D& ) */
	virtual void cartesianRotate( const QVector3D& rot ) { T.cartesianRotate(rot); }													//!< Rotate the display list from its current orientation about z, then y, then x 
																																		/**< See DisplMatrix.setCartesianRotation( const QVector3D& ) */
	virtual void setAxisAngleOrientation( const QVector3D& axis, qreal angle ) { T.setAxisAngleOrientation(axis, angle); }				//!< Set the orientation of the display list absolutely with respect to the world coordinate system, using axis-angle rotation
																																		/**< See DisplMatrix.setAxisAngleOrientation( const QVector3D&, qreal ) */ 
	virtual void axisAngleRotate( const QVector3D& axis, qreal angle ) { T.axisAngleRotate(axis, angle); }								//!< Rotate the display list from its current orientation using axis-angle rotation
																																		/**< See DisplMatrix.axisAngleRotate( const QVector3D&, qreal ) */ 
	virtual void setSpecialEulerOrientation( const QVector3D& axis, qreal angle = 0 ) { T.setSpecialEulerOrientation(axis, angle); }	//!< Rotate the display list from its current orientation using euler angles
																																		/**< See DisplMatrix.setSpecialEulerOrientation( const QVector3D&, qreal ) */ 
	
	virtual void setPosition( const QVector3D& trans ) { T.setPosition(trans); }											//!< Set the position of the display list absolutely with respect to the world coordinate system
																															/**< See DisplMatrix.setPosition( const QVector3D& ) */ 
	virtual void translate( const QVector3D& trans ) { T.translate(trans); }												//!< Translate the display list form its current position
																															/**< See DisplMatrix.translate( const QVector3D& ) */ 
	
protected:
	
	int  index;						//!< The index is passed to glCallLists(int) to render the display list
	//void setDisplayListIdx( int idx ) { index = idx; }		//!< Set the display list index
															/**< The safe thing to do is: setDisplayListIdx( glGenLists(1) )  */
	DisplMatrix T;											//!< The position/orientation of the display list as a 4x4 rigid-body transformation matrix
	
	// some colors for rendering purposes
	static GLfloat gray[4];
	static GLfloat transpGray[4];
    static GLfloat red[4];
	static GLfloat transpRed[4];
	static GLfloat green[4];
	static GLfloat transpGreen[4];
	static GLfloat blue[4];
	static GLfloat transpBlue[4];
	
private:
	
    
	QTime timeSinceLastCollision;	//!< The time stamp of the last collision event
	//GLfloat* currentColor;
	
	static const int collisionTimeout = 400;	//!< Number of milliseconds after collision that objects are still considered "colliding"

};

#endif

/** @} */
