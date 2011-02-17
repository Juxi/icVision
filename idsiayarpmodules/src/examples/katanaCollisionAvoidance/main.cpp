#include <QApplication>
#include <iostream>
#include "window.h"
#include "robot.h"
#include "collisiondetector.h"
#include "box.h"
#include "cylinder.h"
#include "sphere.h"
#include "world.h"
#include "object.h"
#include "RobotFilter.h"

using namespace std;

int main(int argc, char *argv[])
{
    bool visualize = true;
    QString fileName = "xml/katana.xml";
    if ( argc > 1 ) { fileName = QString(argv[1]); }

    QApplication app( argc, argv, visualize );
    cout << "Launching... " << endl;

    Window* window;
    Robot  robot;
	RobotFilter robotFilter;

    CollisionDetector* detector;
    detector = new CollisionDetector( &robot, 20 );

    // VISUALIZATION
    if ( visualize ) {
        cout << "   -with visualization" << endl;
        window = new Window;
        QObject::connect( &robot.world, SIGNAL(addedObject(DisplayList*)), window->getGlWidget(), SLOT(createGLObject(DisplayList*)) );
        QObject::connect(       &robot, SIGNAL(finishLink(DisplayList*)), window->getGlWidget(), SLOT(createGLObject(DisplayList*)) );
        QObject::connect( detector, SIGNAL(tock()), window->getGlWidget(), SLOT(callUpdateGL()) );
		QObject::connect( detector, SIGNAL(collision()), &robotFilter, SLOT(stop()) );
        window->show();
    }
    else { cout << "   -without visualization" << endl; }

    // CONFIGURE THE ROBOT
    bool result = 0;
    if ( robot.configure( fileName ) && robotFilter.open(robot) ) {

        //populate(world);

        //robot->setEncoderPosition(.1);
        //robot->updateTxfrMatrices();

        //robot->printBranches();
        //robot.printLinks();

        detector->start();

        result = app.exec();

        detector->stop();
		robotFilter.close();

    }

    // clean up
    //delete robot;
    delete window;
    delete detector;

    return result;
}
