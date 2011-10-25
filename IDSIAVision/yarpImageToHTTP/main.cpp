// Copyright: (C) 2011 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <cv.h>
#include <highgui.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>


typedef unsigned short int u_short;

using namespace yarp::os;
using namespace yarp::sig;

// Functions
static void catch_sigint( int );
CvMat* getImage(BufferedPort<ImageOf<PixelRgb> > *port) ;
int startup(u_short *port);
void* accept_request(void *cl);
int get_line(int sock, char *buf, int size);
void not_found(int client);
void serve_image(int client, const char *filename);
void serve_txt(int client, const char *filename);

// global vars (argh)
BufferedPort<ImageOf<PixelBgr> > leftInPort;	//!< The port to handle incoming left eye images
BufferedPort<ImageOf<PixelBgr> > rightInPort; 	//!< The port to handle incoming right eye images
Port iCubHeadPort;
Port iCubTorsoPort;
Port iCubLeftArmPort;
Port iCubRightArmPort;

int server_sock = -1;
#define SERVER_STRING "Server: yarp2http/0.0.1\r\n"

///
/// MAIN FUNCTION
///
int main(int argc, char * argv[]) {
	
	(void) signal(SIGINT, catch_sigint);	
	
	printf("Launching iCub YARP Image HTTP Server \n");


	std::string inputPortName, serverPortName;
	
	/********************************************
	 ** Init YARP
	 ********************************************/
	yarp::os::Network yarp;			//!< Identifies the yarp network
		
	yarp::os::ResourceFinder config;
	config.configure("MYYARP2HTTP", argc, argv, true);

	/* Process all parameters from both command-line and .ini file */
	std::string robotName = (config.find("robot")).toString().c_str();
	if( robotName.empty() ) {
		robotName = "icubSim";
		std::cout << "WARNING! No robot name found using " << robotName << std::endl;
	}
		
	if(! config.check("port")) {
		std::cout << "WARNING! No port defined, using dynamic allocation " << std::endl;	
	}
   int portNumber = config.find("port").asInt();
	
	// trying to connect to the left camera
	inputPortName = "/";		  serverPortName = "/";
	inputPortName += "HTTPSERVER";serverPortName += robotName;
	inputPortName += "/left";	  serverPortName += "/cam/left";
	if(! leftInPort.open( inputPortName.c_str() )){
		std::cout << "HTTPSERVER" << ": Unable to open port " << inputPortName << std::endl;
		return false;
	}

	printf("Trying to connect to %s\n", inputPortName.c_str());
	if(! yarp.connect(serverPortName.c_str(), inputPortName.c_str()) ) {
		std::cout << "HTTPSERVER" << ": Unable to connect to port " << serverPortName.c_str() << " with " << inputPortName.c_str() << std::endl;
		return false;
	}
	
	// trying to connect to the right camera
	inputPortName = "/";		  serverPortName = "/";
	inputPortName += "HTTPSERVER";serverPortName += robotName;
	inputPortName += "/right";	  serverPortName += "/cam/right";
	if(! rightInPort.open( inputPortName.c_str() )){
		std::cout << "HTTPSERVER" << ": Unable to open port " << inputPortName << std::endl;		
		return false;
	}
		
	printf("Trying to connect to %s\n", inputPortName.c_str());
	if(! yarp.connect(serverPortName.c_str(), inputPortName.c_str()) ) {
		std::cout << "HTTPSERVER" << ": Unable to connect to port " << serverPortName.c_str() << " with " << inputPortName.c_str() << std::endl;
		return false;
	}
	
	// trying to connect to Head
	inputPortName = "/";		  serverPortName = "/";
	inputPortName += "HTTPSERVER";serverPortName += robotName;
	inputPortName += "/head";	  serverPortName += "/head/state:o";
	if(! iCubHeadPort.open( inputPortName.c_str() )){
		std::cout << "HTTPSERVER" << ": Unable to open port " << inputPortName << std::endl;
		return false;
	}
	
	printf("Trying to connect to %s\n", inputPortName.c_str());
	if(! yarp.connect(serverPortName.c_str(), inputPortName.c_str()) ) {
		std::cout << "HTTPSERVER" << ": Unable to connect to port " << serverPortName.c_str() << " with " << inputPortName.c_str() << std::endl;
		return false;
	}

	// trying to connect to torso
	inputPortName = "/";		  serverPortName = "/";
	inputPortName += "HTTPSERVER";serverPortName += robotName;
	inputPortName += "/torso";	  serverPortName += "/torso/state:o";
	if(! iCubTorsoPort.open( inputPortName.c_str() )){
		std::cout << "HTTPSERVER" << ": Unable to open port " << inputPortName << std::endl;
		return false;
	}
	
	printf("Trying to connect to %s\n", inputPortName.c_str());
	if(! yarp.connect(serverPortName.c_str(), inputPortName.c_str()) ) {
		std::cout << "HTTPSERVER" << ": Unable to connect to port " << serverPortName.c_str() << " with " << inputPortName.c_str() << std::endl;
		return false;
	}

	
	
	// trying to connect to left arm
	inputPortName = "/";		  serverPortName = "/";
	inputPortName += "HTTPSERVER";serverPortName += robotName;
	inputPortName += "/left_arm"; serverPortName += "/left_arm/state:o";
	if(! iCubLeftArmPort.open( inputPortName.c_str() )){
		std::cout << "HTTPSERVER" << ": Unable to open port " << inputPortName << std::endl;
		return false;
	}
	
	printf("Trying to connect to %s\n", inputPortName.c_str());
	if(! yarp.connect(serverPortName.c_str(), inputPortName.c_str()) ) {
		std::cout << "HTTPSERVER" << ": Unable to connect to port " << serverPortName.c_str() << " with " << inputPortName.c_str() << std::endl;
		return false;
	}
	// trying to connect to right arm
	inputPortName = "/";		  serverPortName = "/";
	inputPortName += "HTTPSERVER";serverPortName += robotName;
	inputPortName += "/right_arm"; serverPortName += "/right_arm/state:o";
	if(! iCubRightArmPort.open( inputPortName.c_str() )){
		std::cout << "HTTPSERVER" << ": Unable to open port " << inputPortName << std::endl;
		return false;
	}
	
	printf("Trying to connect to %s\n", inputPortName.c_str());
	if(! yarp.connect(serverPortName.c_str(), inputPortName.c_str()) ) {
		std::cout << "HTTPSERVER" << ": Unable to connect to port " << serverPortName.c_str() << " with " << inputPortName.c_str() << std::endl;
		return false;
	}
	
	
	
	/********************************************
	 ** Init HTTP Server Sockets
	 ********************************************/
	
	int client_sock = -1;
	u_short port = portNumber;
	struct sockaddr_in client_name;
	int client_name_len = sizeof(client_name);
	pthread_t newthread;
	
	server_sock = startup(&port);
	printf("httpd running on port %d\n", port);
	
	/********************************************
	 ** main loop
	 ********************************************/
	
	while (1) {
		client_sock = accept(server_sock,
							 (struct sockaddr *)&client_name,
							 (socklen_t*)&client_name_len);
		if (client_sock == -1)
			perror("accept");
		/* accept_request(client_sock); */
		if (pthread_create(&newthread , NULL, accept_request, &client_sock) != 0)
			perror("pthread_create");
		
		pthread_detach(newthread);
	}
	
	close(server_sock);
	
	return 0;
}

CvMat* getImage(BufferedPort<ImageOf<PixelRgb> > *port)
{
	ImageOf<PixelRgb> *img = port->read();  // read an image
	if (img != NULL) { // check we actually got something
		return cvEncodeImage("png", img);
	}
	return NULL;
}


/********************************************
 ** cleanup
 ********************************************/

void catch_sigint( int ) {	
	std::cout << "Caught SIGING! Closing and cleaning up the module!" << std::endl;
	
	leftInPort.close();
	rightInPort.close();
	close(server_sock);
	
	exit(1);
}


/**********************************************************************/
/* This function starts the process of listening for web connections
 * on a specified port.  If the port is 0, then dynamically allocate a
 * port and modify the original port variable to reflect the actual
 * port.
 * Parameters: pointer to variable containing the port to connect on
 * Returns: the socket */
/**********************************************************************/
int startup(u_short *port)
{
	int httpd = 0;
	struct sockaddr_in name;
	
	httpd = socket(PF_INET, SOCK_STREAM, 0);
	if (httpd == -1) {
		perror("socket");
		exit(1);
	}
	memset(&name, 0, sizeof(name));
	name.sin_family = AF_INET;
	name.sin_port = htons(*port);
	name.sin_addr.s_addr = INADDR_ANY; //htonl(INADDR_ANY);
	if (bind(httpd, (struct sockaddr *)&name, sizeof(name)) < 0) {
		perror("bind");
		exit(1);
	}
	if (*port == 0)  /* if dynamically allocating a port */
	{
		int namelen = sizeof(name);
		if (getsockname(httpd, (struct sockaddr *)&name, (socklen_t*) &namelen) == -1) {
			perror("getsockname");
			exit(1);
		}
		
		*port = ntohs(name.sin_port);
	}
	if (listen(httpd, 5) < 0) {
		perror("listen");
		exit(1);
	}
	return(httpd);
}

/**********************************************************************/
/* A request has caused a call to accept() on the server port to
 * return.  Process the request appropriately.
 * Parameters: the socket connected to the client */
/**********************************************************************/
void* accept_request(void* cl)
{
	char buf[1024];
	int numchars;
	char url[255];
	char path[512];
	size_t i, j;

	int client = *(int*)cl;

	numchars = get_line(client, buf, sizeof(buf));
	i = 0; j = 0;
	while (!isspace(buf[j])) j++;	// read method (ignore, assume GET)
	while (isspace(buf[j]) && (j < sizeof(buf))) j++;	// read space
	while (!isspace(buf[j]) && (i < sizeof(url) - 1) && (j < sizeof(buf)))	// read request URL
	{
		url[i] = buf[j];
		i++; j++;
	}
	url[i] = '\0';
	//
//	if (strcasecmp(method, "GET") == 0)
//	{
//		query_string = url;
//		while ((*query_string != '?') && (*query_string != '\0'))
//			query_string++;
//		if (*query_string == '?')
//		{
//			cgi = 1;
//			*query_string = '\0';
//			query_string++;
//		}
//	}
//	
			
	sprintf(path, ".%s", url);
//	printf("Request received for: %s (%s)\n", url, path);
	
	buf[0] = 'A'; buf[1] = '\0';
	while ((numchars > 0) && strcmp("\n", buf))  /* read & discard headers */
		numchars = get_line(client, buf, sizeof(buf));
	
	
	if(strlen(url) <= 1) not_found(client);
	else {
		if(strcmp(url + (strlen(url) - 4), ".csv") == 0)
			serve_txt(client, path);
		else if( strcmp(url + (strlen(url) - 4), ".png") == 0 || 
		 		 strcmp(url + (strlen(url) - 4), ".jpg") == 0 ||
				 strcmp(url + (strlen(url) - 5), ".jpeg") == 0 )
			serve_image(client, path);
	}
	
	close(client);
	return NULL;
}

/**********************************************************************/
/* Get a line from a socket, whether the line ends in a newline,
 * carriage return, or a CRLF combination.  Terminates the string read
 * with a null character.  If no newline indicator is found before the
 * end of the buffer, the string is terminated with a null.  If any of
 * the above three line terminators is read, the last character of the
 * string will be a linefeed and the string will be terminated with a
 * null character.
 * Parameters: the socket descriptor
 *             the buffer to save the data in
 *             the size of the buffer
 * Returns: the number of bytes stored (excluding null) */
/**********************************************************************/
int get_line(int sock, char *buf, int size)
{
	int i = 0;
	char c = '\0';
	int n;
	
	while ((i < size - 1) && (c != '\n'))
	{
		n = recv(sock, &c, 1, 0);
		/* DEBUG printf("%02X\n", c); */
		if (n > 0)
		{
			if (c == '\r')
			{
				n = recv(sock, &c, 1, MSG_PEEK);
				/* DEBUG printf("%02X\n", c); */
				if ((n > 0) && (c == '\n'))
					recv(sock, &c, 1, 0);
				else
					c = '\n';
			}
			buf[i] = c;
			i++;
		}
		else
			c = '\n';
	}
	buf[i] = '\0';
	
	return(i);
}



/**********************************************************************/
/* Give a client a 404 not found status message. */
/**********************************************************************/
void not_found(int client)
{
	char buf[1024];
	
	sprintf(buf, "HTTP/1.0 404 NOT FOUND\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, SERVER_STRING);
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "Content-Type: text/html\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "<HTML><TITLE>Not Found</TITLE>\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "<BODY><P>The server could not fulfill\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "your request because the resource specified\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "is unavailable or nonexistent.</P>\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "<p>Try these:<br/>\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "<a href=\"left.png\">Left Eye</a><br/>\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "<a href=\"right.png\">Right Eye</a><br/>\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "</p>\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "</BODY></HTML>\r\n");
	send(client, buf, strlen(buf), 0);
}

/**********************************************************************/
/* Send a regular file to the client.  Use headers, and report
 * errors to client if they occur.
 * Parameters: a pointer to a file structure produced from the socket
 *              file descriptor
 *             the name of the file to serve */
/**********************************************************************/
void serve_image(int client, const char *filename)
{
	FILE *resource = NULL;
	char buf[1024];
	bool error = false;
	
	int stackOfFrames = 2;
		
	IplImage *in = NULL;
	IplImage *out = NULL;
	IplImage *help = NULL;
	
	if( strncmp(filename + 2, "left.", 5) == 0 ) 
	{	
		remove(filename);
//		printf("reading from yarp file: %s\n", filename);
		for(int i = 0; i < stackOfFrames; i++) {
			// read image from the port
			ImageOf<PixelBgr> *image = leftInPort.read();  // read an image
			if (image == NULL) { 
				std::cout << "ERROR: Could not read from port '" << leftInPort.getName() << "'!" << std::endl;
				error = true;
			}else {
				help = (IplImage*) image->getIplImage();
				if(out == NULL)
					out = help;
//				else
//					cvAdd(out, help, out);
				
//				std::cout << "INFO: Stack #" << i << " !" << std::endl;
			}
		}
		// write to disk
	   if(! error) {
//		   IplImage* unity  = (IplImage*) cvClone(out);
		   
		   // multiply with 1/frames
//		   cvSet(unity, cvRealScalar(1.0));
//		   cvMul(out, unity, out, 1.0/stackOfFrames);
		   
//		   cvReleaseImage(&unity);
		   
		   cvSaveImage(filename, out);
		   //cvReleaseImage(&out);
		   out = NULL;
	   }
	}else if( strncmp(filename + 2, "right.", 6) == 0 ) {
		remove(filename);
//		printf("reading from yarp file: %s\n", filename);
		
		// read image from the port
		ImageOf<PixelBgr> *image = rightInPort.read();  // read an image
		if (image == NULL) { 
			std::cout << "ERROR: Could not read from port '" << rightInPort.getName() << "'!" << std::endl;
		}else {
			in = (IplImage*) image->getIplImage();
			// write to disk
			cvSaveImage(filename, in);						
		}
		
	}
	   

	resource = fopen(filename, "r");
	if( resource == NULL) {
	   not_found(client);
	}else {  
	
		strcpy(buf, "HTTP/1.0 200 OK\r\n");
		send(client, buf, strlen(buf), 0);
		
		strcpy(buf, SERVER_STRING);
		send(client, buf, strlen(buf), 0);

		if(strcmp(filename + (strlen(filename) - 4), ".png") == 0)
			sprintf(buf, "Content-Type: image/png\r\n");
		if(strcmp(filename + (strlen(filename) - 4), ".jpg") == 0 ||
		   strcmp(filename + (strlen(filename) - 5), ".jpeg") == 0 )			
			sprintf(buf, "Content-Type: image/jpeg\r\n");

		send(client, buf, strlen(buf), 0);

		strcpy(buf, "\r\n");
		send(client, buf, strlen(buf), 0);

		int n;
		FILE *fclient = fdopen(client, "r+");
		while ((n = fread(buf, 1, sizeof(buf), resource)) > 0)
			fwrite(buf, 1, n, fclient);
		
		fclose(fclient);
		
	}
	fclose(resource);
}


/**********************************************************************/
/* Send a regular txt content to the client.  Use headers, and report
 * errors to client if they occur.
 * Parameters: a pointer to a file structure produced from the socket
 *              file descriptor
 *             the name of the file to serve */
/**********************************************************************/
void serve_txt(int client, const char *filename)
{
	char buf[1024];
	
	if( strcmp(filename + 2, "joints.csv") != 0 &&
	    strcmp(filename + 2, "alljoints.csv") != 0 ) {
		not_found(client);
	}else {  

		remove(filename);
		printf("reading from yarp file: %s\n", filename);
		
		// YARP READING

		Bottle input;
		iCubHeadPort.read(input);
		
		char sHead[512] = "n/a,n/a,n/a,n/a,n/a,n/a";
		if (input != NULL) {
			strcpy(sHead, input.toString());
			for(uint i = 0; i < strlen(sHead); i++)
				if(sHead[i] == ' ') sHead[i] = ',';
		}
		
		input.clear();
		iCubTorsoPort.read(input);
		
		char sTorso[512] = "n/a,n/a,n/a";
		if (input != NULL) {
			strcpy(sTorso, input.toString());
			for(uint i = 0; i < strlen(sTorso); i++)
				if(sTorso[i] == ' ') sTorso[i] = ',';
		}
		
		
		char sLeftArm[1024];
		char sRightArm[1024];
		
		if(strcmp(filename + 2, "alljoints.csv") == 0 ) {
			// add also left arm and right arm
			
			input.clear();
			iCubLeftArmPort.read(input);
			
			if (input != NULL) {
				strcpy(sLeftArm, input.toString());
				for(uint i = 0; i < strlen(sLeftArm); i++)
					if(sLeftArm[i] == ' ') sLeftArm[i] = ',';
			}

			input.clear();
			iCubRightArmPort.read(input);
			
			if (input != NULL) {
				strcpy(sRightArm, input.toString());
				for(uint i = 0; i < strlen(sRightArm); i++)
					if(sRightArm[i] == ' ') sRightArm[i] = ',';
			}
			
		}
		
		
		// output HTTP
		strcpy(buf, "HTTP/1.0 200 OK\r\n");
		send(client, buf, strlen(buf), 0);
		
		strcpy(buf, SERVER_STRING);
		send(client, buf, strlen(buf), 0);
		
		sprintf(buf, "Content-Type: text/plain\r\n");
		send(client, buf, strlen(buf), 0);
		
		strcpy(buf, "\r\n");
		send(client, buf, strlen(buf), 0);
	
		sprintf(buf, "%s,%s\r\n", sHead, sTorso);
		send(client, buf, strlen(buf), 0);
		
		if(strcmp(filename + 2, "alljoints.csv") == 0 ) {		
			sprintf(buf, "%s\r\n", sLeftArm);
			send(client, buf, strlen(buf), 0);
			sprintf(buf, "%s\r\n", sRightArm);
			send(client, buf, strlen(buf), 0);
		}
			
			
		strcpy(buf, "\r\n");
		send(client, buf, strlen(buf), 0);
			
		
	}
}

