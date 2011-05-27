#include <QApplication>
#include <QTime>

int global_numPrimitives = 0;

#include "controlThread.h"

/*int experiment()
{
	RobotModel::Model model(false);
	RobotModel::CompositeObject* box = NULL;
	
	if ( !model.robot->open("../../../../../../xml/icub.xml") ) return 1;
	
	QTime	qTime;
	int		poses;
	qreal	seconds;
	int		numEpochs     = 100;
	int		geomsPerEpoch = 10;
	qreal	epochDuration = 2; //seconds
	qreal	msEpochDuration = epochDuration*1000;
	
	srand( time(NULL) );
	printf("Begin Test!\n");
	
	for ( int j = 0; j <= numEpochs ; j++ )
	{
		poses = 0;
		qTime.restart();
		
		while ( qTime.elapsed() < msEpochDuration )
		{
			for ( double i=0; i<6.282; i+=.01 )
			{
				model.robot->setNormalPosition( (sin(i)+1)/2 );  // sets all motor positions on the robot
				model.computePose();
				poses++;
				if ( qTime.elapsed() >= msEpochDuration ) break;
			}
		}
		seconds = (qreal)qTime.elapsed()/1000;
		printf( "%i, %f, ( %i / %f )\n", model.world->size(), (qreal)poses/seconds, poses, seconds );
		
		//add geometries to the world
		for ( int k = 0; k < geomsPerEpoch ; k++ )
		{
			box = model.world->newBox(   QVector3D(.1,.1,.1),
									  QVector3D(	-.3 + (double)rand()/RAND_MAX * .6,
													-.3 + (double)rand()/RAND_MAX * .6,
													-.3 + (double)rand()/RAND_MAX * .6
												)
									  );
			box->setCartesianOrientation(QVector3D(	(double)rand()/RAND_MAX * 6.282,
												    (double)rand()/RAND_MAX * 6.282,
												    (double)rand()/RAND_MAX * 6.282
												   )
										 );
		}
		
	}
	return 0;
}*/

int qAppVersion(int argc, char *argv[])
{
	bool visualize = true;
	
	QApplication app( argc, argv, visualize );	// create the QT application
	
	ControlThread myControlThread(visualize);
	myControlThread.model.robot->open("../../../../../../xml/icub.xml");
	myControlThread.start();
	
	return app.exec();						// run the Qt application
}

int main(int argc, char *argv[])
{
	//return experiment();
	return qAppVersion( argc, argv );
}
