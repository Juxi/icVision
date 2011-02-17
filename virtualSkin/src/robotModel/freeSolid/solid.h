/*
  SOLID - Software Library for Interference Detection
  Copyright (C) 1997-1998  Gino van den Bergen

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public
  License along with this library; if not, write to the Free
  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

  Please send remarks, questions and bug reports to gino@win.tue.nl,
  or write to:
                  Gino van den Bergen
		  Department of Mathematics and Computing Science
		  Eindhoven University of Technology
		  P.O. Box 513, 5600 MB Eindhoven, The Netherlands
*/

#ifndef SOLID_SOLID_H
#define SOLID_SOLID_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"

typedef void *DtObjectRef;
DT_DECLARE_HANDLE(DtShapeRef);

typedef enum DtPolyType {
  DT_SIMPLEX,
  DT_POLYGON,
  DT_POLYHEDRON
} DtPolyType;

typedef enum DtResponseType { 
  DT_NO_RESPONSE,
  DT_SIMPLE_RESPONSE,
  DT_SMART_RESPONSE,
  DT_WITNESSED_RESPONSE
} DtResponseType;

typedef struct DtCollData {
  DT_Vector3 point1;
  DT_Vector3 point2;
  DT_Vector3 normal;
} DtCollData;

typedef void (*DtResponse)(
  void *client_data,
  DtObjectRef object1,
  DtObjectRef object2,
  const DtCollData *coll_data);



/* Shape definition */

extern DECLSPEC DtShapeRef dtBox(DT_Scalar x, DT_Scalar y, DT_Scalar z);
extern DECLSPEC DtShapeRef dtCone(DT_Scalar radius, DT_Scalar height);
extern DECLSPEC DtShapeRef dtCylinder(DT_Scalar radius, DT_Scalar height);
extern DECLSPEC DtShapeRef dtSphere(DT_Scalar radius);

extern DECLSPEC DtShapeRef dtNewComplexShape();
extern DECLSPEC void dtEndComplexShape();

extern DECLSPEC void dtBegin(DtPolyType type);
extern DECLSPEC void dtEnd();

extern DECLSPEC void dtVertex(DT_Scalar x, DT_Scalar y, DT_Scalar z);
extern DECLSPEC void dtVertexBase(const void *base);
extern DECLSPEC void dtVertexIndex(DT_Index index);
extern DECLSPEC void dtVertexIndices(DtPolyType type, DT_Count count, 
			    const DT_Index *indices);
extern DECLSPEC void dtVertexRange(DtPolyType type, DT_Index first, DT_Count count); 

extern DECLSPEC void dtChangeVertexBase(DtShapeRef shape, const void *base);

extern DECLSPEC void dtDeleteShape(DtShapeRef shape);


/* Object  */

extern DECLSPEC void dtCreateObject(DtObjectRef object, DtShapeRef shape); 
extern DECLSPEC void dtDeleteObject(DtObjectRef object);
extern DECLSPEC void dtSelectObject(DtObjectRef object);

extern DECLSPEC void dtLoadIdentity();

extern DECLSPEC void dtLoadMatrixf(const float *m);
extern DECLSPEC void dtLoadMatrixd(const double *m);

extern DECLSPEC void dtMultMatrixf(const float *m);
extern DECLSPEC void dtMultMatrixd(const double *m);

extern DECLSPEC void dtTranslate(DT_Scalar x, DT_Scalar y, DT_Scalar z);
extern DECLSPEC void dtRotate(DT_Scalar x, DT_Scalar y, DT_Scalar z, DT_Scalar w);
extern DECLSPEC void dtScale(DT_Scalar x, DT_Scalar y, DT_Scalar z);


/* Response */

extern DECLSPEC void dtSetDefaultResponse(DtResponse response, DtResponseType type, 
				 void *client_data);

extern DECLSPEC void dtClearDefaultResponse();

extern DECLSPEC void dtSetObjectResponse(DtObjectRef object, DtResponse response, 
				DtResponseType type, void *client_data);
extern DECLSPEC void dtClearObjectResponse(DtObjectRef object);
extern DECLSPEC void dtResetObjectResponse(DtObjectRef object);

extern DECLSPEC void dtSetPairResponse(DtObjectRef object1, DtObjectRef object2, 
			      DtResponse response, DtResponseType type, 
			      void *client_data);
extern DECLSPEC void dtClearPairResponse(DtObjectRef object1, DtObjectRef object2);
extern DECLSPEC void dtResetPairResponse(DtObjectRef object1, DtObjectRef object2);


/* Global */

extern DECLSPEC DT_Count dtTest();
extern DECLSPEC void dtTestObjects(DtObjectRef object1, DtObjectRef object2);
extern DECLSPEC void dtProceed();

extern DECLSPEC void dtEnableCaching();
extern DECLSPEC void dtDisableCaching();

extern DECLSPEC void dtSetTolerance(DT_Scalar tol);

#ifdef __cplusplus
}
#endif

#endif
