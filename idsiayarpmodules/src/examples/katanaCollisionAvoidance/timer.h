#ifndef TIMER_H
#define TIMER_H

#include <QThread>
#include <iostream>
#include "robotinterface.h"

using namespace std;

/*! \brief A thread that periodically updates the current pose of the Robot
 *
 * The transformation matrix (from the local CS to the world CS) for each joint is calculated and then the tock()
 * signal is emitted, signifying that the new pose is ready, at which time GLWidget will redraw the scene. This is
 * done periodically, according to a delay specified by tick. Additionally, for testing purposes this class can set
 * all motor positions on the Robot using the RobotInterface in conjunction with oscillate().
 */

class Timer : public QThread
{
    Q_OBJECT

public:
    //! The standard constructor
    /*! The object constructed updates the pose and emits a signal. */
    Timer( RobotInterface* robot, double tick ) : robot(robot),
                                                  tick(tick),
                                                  poses(0),
                                                  count(0),
                                                  col_count(0),
                                                  oscillate(false),
                                                  runThread(true) {}
    //! A constructor for testing new Robot models
    /*! The object constructed sets all joint positions, updates the pose, and emits a signal. */
    Timer( RobotInterface* robot, double tick, int poses ) : robot(robot),
                                                             tick(tick),
                                                             poses(poses),
                                                             count(0),
                                                             col_count(0),
                                                             oscillate(true),
                                                             runThread(true) {}

    /* ARGSUSED */
    static void collide1(void * client_data, DtObjectRef obj1, DtObjectRef obj2, const DtCollData *coll_data)
    {
        ((Timer*)client_data)->col_count++;
    }
    static void collide2(void * client_data, DtObjectRef obj1, DtObjectRef obj2, const DtCollData *coll_data)
    {
    }

    //! Runs the thread
    /*!  */
    virtual void run()
    {
        cout << "Starting timer thread..." << endl;
        dtSetDefaultResponse( collide1, DT_SIMPLE_RESPONSE, (void*) this);
        robot->filterAdjacentCollisions( collide2 ); // ignore collisions between component objects of the same link and adjacent links


        while (runThread) {

            col_count = 0;
            msleep(tick);

            if (oscillate) {
                robot->setPos(oscillator());
            }

            robot->updateTxfrMatrices();


            dtTest();
            cout << "Number of collisions: " << col_count << endl;

            emit tock();
            count++;
        }

    }

    //! Stops the thread gracefully
    /*!  */
    void stopThread()
    {
        runThread = false;
        while ( this->isRunning() ){ /* wait */ }
    }

signals:
    //! Signifies that the new pose has been calculated
    /*!  */
    void tock();

private:
    RobotInterface* robot;      //! The Robot whose pose we are updating
    double          tick;       //! The time to wait in between subsequent pose calculations (frame rate)
    int             poses,      //! The number of points oscillator() should interpolate on its sine wave
                    count,      //! The number of pose uptates computed
                    col_count;  //! The number of collisions in the current robot configuration
    bool            oscillate,  //! Whether or not to update joint positions using oscillator()
                    runThread;  //! Whether or not to continue running the thread

    //! Interpolates a sine wave
    /*! The number of samples per period is stored in poses. User should be aware that joints will not move through
        their entire range of motion if their motor limits are outside the interval [-1,1] */
    double oscillator() { return sin( (double)count/poses *2*M_PI ); }
};

#endif // TIMER_H
