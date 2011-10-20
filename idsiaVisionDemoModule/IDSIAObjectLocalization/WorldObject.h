/*
 * WorldObject.h
 *
 *  Created on: Feb 3, 2011
 *      Author: migliore
 *      Description: This class represent a world object
 */

#ifndef WORLDOBJECT_H_
#define WORLDOBJECT_H_

#include <opencv2/opencv.hpp>
#include "ColoredRect.h"

using namespace cv;
using namespace std;

//Type of shapes
enum Shape{CUBE, CYLINDER, SPHERE, BOX};

//Type of commands
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
    int  disappearCounter;
    bool sendData2Sim;

    //Object information
    double bank, heading, attitude;
    Point3f position;
    double radius;
    double lenght;
    double boxSize1, boxSize2, boxSize3;

    //Maybe not here
    Mat w2SimRot;


  public:
    WorldObject(ColoredRect & rect_left, ColoredRect & rect_right, int idx, Point3f pos);
    //Change data of world object
    void changeData(ColoredRect & rect_left, ColoredRect & rect_right, Point3f pos);
    //Match objects using colors
    double matchColoredRects(ColoredRect &rect_left, ColoredRect &rect_right, Point3f &estimatedPose);
    //Get color rectangle in correspondance of the object
    ColoredRect getColoredRect(char camera){ if(camera=='l') return bb_left; else return bb_right;}
    //Set shape of the object
    void setShape(Shape shape);
    //Get shape
    string getShape();
    //Get the id
    int getId(){return id;}
    //Get the command for the simulator
    string getSimCommand(CommandType type, bool isICubSimulator);
    //Get object position
    Point3f getObjPosition(){return position;}
    //Update hte counter that measures the non-changes of the object
    void updateNotchangedCounter();
    //Get the counter
    int getNotchangedCounter();


    virtual
    ~WorldObject();


};

#endif /* WORLDOBJECT_H_ */
