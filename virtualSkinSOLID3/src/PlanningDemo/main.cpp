/*******************************************************************
 ***             Copyright (C) 2011 Tobias Glasmachers           ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

#include <QApplication>
#include "fwdkin.h"
#include "controlThread.h"
#include "fitness.h"
#include "kernelfunction.h"
#include "elitistxnes.h"


#define ROBOT_FILE             "iCubSimWithHandMarker.xml"
#define MAX_GENERATIONS        2000


void help()
{
	printf(
		"\n"
		"usage: PlanningDemo <mode> [seed]\n"
		"with the modes:\n"
		"   plan        no graphics, just plan the trajectory\n"
		"   show        graphics output of the trajectory\n"
		"and seed is an integer seeding the random number generator\n"
		"\n");
	exit(0);
}

void plan()
{
	FwdKin kin;
	kin.robot.open(ROBOT_FILE, false);

	// initialize the planning environment
	QObject::connect(&kin.robot, SIGNAL(observation(RobotObservation const&)),
							&kin, SLOT(obtainObservation(RobotObservation const&)));

	// coefficients of the indirect representation
	unsigned int i, j;
	unsigned int N = 2;
	Vector mu(N * 7);
	double sigma = 1.0;
	Matrix B = Matrix::identity(mu.size());

	// incremental evolutionary search
	unsigned int levels = 1;
	Matrix K(N, N);
	for (i=0; i<N; i++) for (j=0; j<N; j++) K(i, j) = inner_product(i, j);
	ICubFitness* fit = new ICubFitness(kin, levels);
	ElitistxNES* opt = new ElitistxNES(*fit);
	opt->init(mu, sigma, B);

	unsigned int iter, start_iter = 0;
	printf("iter=  1   fitness=%20.15g   sigma=%g\n", opt->bestFitness(), opt->standardDeviation());

	// (1+1)-xNES optimization with adaptive level
	for (iter=0; ; iter++)
	{
		opt->iterate();

		// decide whether we should switch to the next level
		if (opt->bestFitness() < 0.001
				|| opt->standardDeviation() < 1e-10
				|| iter + 1 >= start_iter + MAX_GENERATIONS)
		{
			printf("iter=%d   fitness=%20.15g   sigma=%g\n", iter+1, opt->bestFitness(), opt->standardDeviation());
			break;
		}
		else if ((iter % 100) == (99))
		{
			printf("iter=%d   fitness=%20.15g   sigma=%g\n", iter+1, opt->bestFitness(), opt->standardDeviation());
		}
	}
	printf("done.\n");

	// save coefficients
	const double* coeff = &opt->bestPoint()[0];
	FILE* file = fopen("coeff.bin", "wb+");
	if (file == NULL) throw "[show] failed to write 'coeff.bin'";
	fwrite(coeff, sizeof(double), 14, file);
	fclose(file);

	exit(0);		// avoids segmentation fault?
}

void show(int argc, char** argv)
{
	try
	{
		QApplication app(argc, argv, true);

		FwdKin kin(true);		// visualization ON
		kin.robot.open(ROBOT_FILE, false);
		QObject::connect(&kin.robot, SIGNAL(observation(RobotObservation const&)),
							&kin, SLOT(obtainObservation(RobotObservation const&)));

		// load coefficients
		double coeff[14];
		FILE* file = fopen("coeff.bin", "rb");
		if (file == NULL) throw "[show] failed to read 'coeff.bin'";
		fread(coeff, sizeof(coeff), 1, file);
		fclose(file);

		printf("starting control thread\n");
		ControlThread myControlThread(kin, coeff);
		myControlThread.start();
		app.exec();
	}
	catch (const char* exception)
	{
		printf("exception: %s\n", exception);
	}
}

int main(int argc, char *argv[])
{
	unsigned int seed = time(NULL);
	if (argc > 2) seed = atoi(argv[2]);
	rng.seed(seed);
// 	printf("seed=%d\n", seed);

	if (argc < 2) help();
	if (strcmp(argv[1], "plan") == 0) plan();
	else if (strcmp(argv[1], "show") == 0) show(argc, argv);
	else help();
}
