/*
 * WorldObject.h
 *
 *  Created on: Feb 3, 2011
 *      Author: migliore
 */

#ifndef WORLDOBJECT_H_
#define WORLDOBJECT_H_

#include <opencv2/opencv.hpp>
#include "ColoredRect.h"

using namespace cv;
using namespace std;

enum Shape{CUBE, CYLINDER, SPHERE, BOX};
enum CommandType{POSE, ORIENTATION};

class WorldObject
{
  private:
    int id;
    ColoredRect bb_left, bb_right;
    Scalar color;
    bool newobj;
    bool positioned, oriented;
    Shape shapeis;
    bool isVisible;
    bool sendData2Sim;

    //Object information
    double bank, heading, attitude;
    Point3f position;
    double radius;
    double boxSize1, boxSize2, boxSize3;

    //Maybe not here
    Mat w2SimRot;


  public:
    WorldObject(ColoredRect & rect_left, ColoredRect & rect_right, int idx, Point3f pos);
    void changeData(ColoredRect & rect_left, ColoredRect & rect_right, Point3f pos);
    double matchColoredRects(ColoredRect &rect_left, ColoredRect &rect_right, Point3f &estimatedPose);
    ColoredRect getColoredRect(char camera){ if(camera=='l') return bb_left; else return bb_right;}
    void setShape(Shape shape);
    string getShape();
    int getId(){return id;}
    string getSimCommand(CommandType type, bool isICubSimulator);
    Point3f getObjPosition(){return position;}
    void setVisibility(bool visibility){isVisible = visibility;}
    bool getVisibility(){return isVisible;}


    virtual
    ~WorldObject();


};

#endif /* WORLDOBJECT_H_ */
