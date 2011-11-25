#include "iCubController.h"
#include "roadmap.h"

int main()
{	
	yarp::os::Network yarp;
	yarp::os::Port vSkinStatus;
	vSkinStatus.open("/statusOut");
	if ( !yarp.connect("/filterStatus","/statusOut") )
	{ 
		printf("failed to connect to robot filter status port\n");
		return 1;
	}

	try {
		Roadmap roadmap( "icubSimF" );
		roadmap.buildRandomMap( 50,	// number of vertices
								3 );	// number of neighbors per node
		
		if ( !roadmap.gotoNearest() )  { printf("Failed to move iCub onto the roadmap\n"); }
		else
		{
			std::cout << "iCub is on the roadmap" << std::endl;
			
			yarp::os::Bottle b;
			//int filterStatus;
			while (true)
			{
				vSkinStatus.read(b);
				printf("Filter status: %s\n",b.toString().c_str());
				//filterStatus = b.get(0).asInt();
				//printf("filter status: %d\n", filterStatus );
				
				if (  b.get(0).asInt() == 1 )
				{
					//printf("filter open\n");
					if ( !roadmap.isOnMap() ) 
					{
						if ( !roadmap.gotoNearest() )  { printf("Failed to move iCub onto the roadmap\n"); } 
					}
					if ( roadmap.randomMove() ) std::cout << "OK" << std::endl;			
					else std::cout << "OH NO!!!" << std::endl;
				}
				else { printf("waiting for filter to open\n"); }
				usleep(100000);
			}
		}
	} catch ( char const* e ) { std::cout << e << std::endl; }
	
	return 0;
}
