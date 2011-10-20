/*
 * WorldObject.cpp
 *
 *  Created on: Feb 3, 2011
 *      Author: migliore
 */

#include "WorldObject.h"


WorldObject::WorldObject(ColoredRect & rect_left, ColoredRect & rect_right, int idx, Point3f pos)
{
  bb_left = rect_left;
  bb_right = rect_right;
  color = bb_left.getColor();
  id = idx;
  newobj = true;
  positioned = false;
  oriented = false;
  position = pos;

  disappearCounter = 0;

  //only for demonstration
/*  shapeis = SPHERE;
  radius = 0.01;
  bank = 0;
  heading = 0;
  attitude = 0;*/
  shapeis = CYLINDER;
  radius = 0.04;
  lenght = 0.08;
  bank = CV_PI/2;
  heading = 0;
  attitude = 0;



  //Select the simulator
  //Configure W2SimRot T = [ 0 -1 0 0; 0 0 1 0.5484; -1 0 0 -0.04; 0 0 0 1 ]
//  w2SimRot =  (Mat_<float>(4,4) << 0, -1, 0, 0, 0, 0, 1, 0.5484, -1, 0, 0, -0.04, 0, 0, 0, 1 ); //iCub SIMULATOR
  w2SimRot =  (Mat_<float>(4,4) << 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 ); //Kail SIMULATOR

}

void WorldObject::changeData(ColoredRect & rect_left, ColoredRect & rect_right, Point3f pos){
   bb_left = rect_left;
   bb_right = rect_right;
   color = bb_left.getColor();
   newobj = false;
   position = pos;
   disappearCounter = 0;
}

double WorldObject::matchColoredRects(ColoredRect &rect_left, ColoredRect &rect_right, Point3f &estimatedPose){

  //TODO to change
  if( rect_left.getColor()[0] == color[0] &&
      rect_left.getColor()[1] == color[1] &&
      rect_left.getColor()[2] == color[2] ){

      double distance = sqrt((position.x-estimatedPose.x)*(position.x-estimatedPose.x) + (position.y-estimatedPose.y)*(position.y-estimatedPose.y) + (position.z-estimatedPose.z)*(position.z-estimatedPose.z));

      return distance;
  }
  else{
      return DBL_MAX;
  }

}



string WorldObject::getSimCommand(CommandType type, bool isICubSimulator){
  ostringstream buffer2send;

  Point3f sendedPosition = position;

  if(isICubSimulator){

      Mat tmpposition = (Mat_<float>(4,1) << position.x, position.y, position.z, 1 );
      tmpposition = w2SimRot*tmpposition;

     // cout<<tmpposition<<endl;
      sendedPosition.x = tmpposition.at<float>(0,0);
      sendedPosition.y = tmpposition.at<float>(1,0);
      sendedPosition.z = tmpposition.at<float>(2,0);

  }


  switch (type) {
    case POSE:
      if(!positioned){
          if(shapeis == BOX){
              //buffer2send << "world mk "<<getShape()<<" "<<boxSize1<<" "<<boxSize2<<" "<<boxSize3<<" "<<sendedPosition.x<<" "<<sendedPosition.y<<" "<<sendedPosition.z<<" 1 0 0";
              buffer2send << "world mk "<<getShape()<<" "<<boxSize1<<" "<<boxSize2<<" "<<boxSize3<<" "<<sendedPosition.x<<" "<<sendedPosition.y<<" "<<sendedPosition.z<<" "<<color[2]/255<<" "<<color[1]/255<<" "<<color[0]/255;
          }
          else if(shapeis == CUBE){
              //TODO
          }
          else if(shapeis == CYLINDER){
              buffer2send << "world mk "<<getShape()<<" "<<radius<<" "<<lenght<<" "<<sendedPosition.x<<" "<<sendedPosition.y<<" "<<sendedPosition.z<<" "<<color[2]/255<<" "<<color[1]/255<<" "<<color[0]/255;
          }
          else if(shapeis == SPHERE){
              buffer2send << "world mk "<<getShape()<<" "<<radius<<" "<<sendedPosition.x<<" "<<sendedPosition.y<<" "<<sendedPosition.z<<" "<<color[2]/255<<" "<<color[1]/255<<" "<<color[0]/255;
          }

          positioned = true;
      }
      else
        buffer2send << "world set "<<getShape()<<" "<<id+1<<" "<<sendedPosition.x<<" "<<sendedPosition.y<<" "<<sendedPosition.z<<" "<<color[2]/255<<" "<<color[1]/255<<" "<<color[0]/255;

      break;

    case ORIENTATION:

      if(!oriented){
          buffer2send << "world rot "<<getShape()<<" "<<id+1<<" "<<bank<<" "<<heading<<" "<<attitude;
          oriented = true;
      }
      else
          buffer2send << "Nocommand"; //TODO to write
      break;

    default:
      return NULL;
      break;
  }

  return buffer2send.str();
}

void WorldObject::setShape(Shape shape){
  shapeis = shape;
}

string WorldObject::getShape(){

  switch (shapeis) {
    case CUBE:
      return string("cube"); //???
      break;
    case CYLINDER:
      return string("scyl");
          break;
    case SPHERE:
      return string("ssph");
          break;
    case BOX:
      return string("sbox");
          break;
    default:
      return NULL;
      break;
  }
}

void WorldObject::updateNotchangedCounter(){

  disappearCounter++;
}

int WorldObject::getNotchangedCounter(){

  return disappearCounter;
}

WorldObject::~WorldObject()
{
  // TODO Auto-generated destructor stub
}
