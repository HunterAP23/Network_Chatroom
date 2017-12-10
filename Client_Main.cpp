#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <Winsock2.h>
#include <WS2tcpip.h>
#include <string>
#include <ctime>
#include <ratio>
#include <chrono>

using namespace std;
using namespace std::chrono;

#define LINESZ  2560


static SOCKET connectTCP(char *host, char *service);
void sendToServer(string mess, string time, SOCKET server); 
string recFromServer(SOCKET server);

int main(int argc, char *argv[])
{
	// Make sure that we can connect:
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	string message;	//holds message to be posted
	string mestime;	//holds time the message was sent
	time_t rightnow; //used to get current time

	// A macro to create a word with low order and high order bytes as specified.
	wVersionRequested = MAKEWORD(1, 1);

	// Test that version 1.1 is available.
	err = WSAStartup(wVersionRequested, &wsaData);

	if (err != 0) {

		// Tell the user that we couldn't find a useable winsock.dll. */
		cerr << "Could not find useable DLL" << endl;
		return 1;
	}

	// Confirm that the Windows Sockets DLL supports version 1.1.
	// Note that if the DLL supports versions greater than 1.1 in addition 
	//  to 1.1, it will still return1.1 in wVersion since that is the version we
	// requested.  Note the use of macros.  Further not that this check is not 
	// really needed.

	if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1) {

		// Tell the user that we couldn't find a useable winsock DLL. 
		WSACleanup();  // Terminates the use of the 
		cerr << "Could not find useable DLL" << endl;
		return 1;
	}

	char *host = "cs.ramapo.edu";		/* The name of the remote host. */
	char *service = "echo";				/* The service to be accessed. */
	SOCKET s;							/* The socket for the connection to the remote host. */
	string chat;						//hold the most current line from chat


										/* Fill in the remote host name and service name from the run-time
										* parameters. */
	switch (argc) {

		/* If there are no run-time parameters the default values will be
		* used. */
	case 1:
		break;

		/* Record the host name. */
	case 2:
		host = argv[1];
		break;

		/* If there are more than 1 run-time parameters, the user is in
		* error. */
	default:
		fprintf(stderr, "Usage: client [host [service]]\n");
		return 1;
	}
	
	//connect to chat server
	s = connectTCP(host, service);

	while (true) {
		cout << "Message: ";
		cin >> message;

		rightnow = system_clock::to_time_t(system_clock::now());
		mestime = ctime(&rightnow);

		sendToServer(message, mestime, s);
		chat = recFromServer(s);
		
	}

	return 0;
}

//sends message and time message was created to server
void sendToServer(string mess, string time, SOCKET server) {
	string output = mess + "\nMessage sent at:" + time;

	send(server, output.c_str(), output.length(), 0);
}

//recieve messages from server
string recFromServer(SOCKET server) {
	int nb;								/* The number of bytes read from a single read. */
	int tnb;							/* The total number of bytes read. */
	char buff[LINESZ + 1];				/* Response data from the server. */

	/* Read the response from the server.  Recall that the response is
	* stream oriented and may take more than one read.  (not likely)
	* A zero length read implies that the server has broken the
	* connection. */
	tnb = 0;
	while ((nb = recv(server, &buff[tnb], LINESZ - tnb, 0)) > 0) {
		tnb += nb;
	}
	/* If there was an error on the read, report it. */
	if (nb < 0) {
		perror("read");
		return "";
	}

	string response = "";	//response from server
	//add buff to response
	for (int i = 0; i < nb; i++) {
		response += buff[i];
	}

	return response;
}

static SOCKET connectTCP(char *host, char *service)
{
	struct hostent *he; /* Information about the remote host. */
	struct servent *se; /* Information about the requested service. */
	struct sockaddr_in sin; /* Internet end point address.  How do I find the format of this struct? */
	SOCKET s;              /* File descriptor for the connected socket. */


						   /* Create an endpoint address for the server.   Structure for address
						   * found in: netinet/in.h */
	memset((char *)&sin, 0, sizeof(sin)); /* sets the address to zero. */
	sin.sin_family = AF_INET;  /* Record the fact that we are using the
							   * internet family of addresses. */

							   /* Get the portnumber.  Note: services found in /etc/services. */
	if ((se = getservbyname(service, "tcp")) != NULL) {
		sin.sin_port = se->s_port;
		printf("port No. %ld\n", (long int)ntohs(se->s_port));

	}
	else if ((sin.sin_port = htons((short int)atoi(service)))
		== 0) {
		cerr << "Can't get the port number of the service." << endl;
		exit(1);
	}

	/* Get the address of the remote host.  The host's name is looked up
	* on a name server. */

	if ((he = gethostbyname(host)) != NULL) {
		memcpy((char *)&sin.sin_addr, he->h_addr, he->h_length);

	}
	else if ((sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE) {
		cerr << "Invalid host address" << endl;
		exit(1);

	}
	// Display the IP address in hexadecimal.  Let's convert it to dotted 
	// notation in class.
	printf("IP address = 0x%x\n", sin.sin_addr.s_addr);

	/* Allocate a socket from which the connection to the server will be
	* be made.  Note: by setting the third argument to 0, means that we
	* are using the default protocol (since there is usually one choice,
	* 0 seems like a good choice for the argument. */
	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		cerr << "Could not allocate socket." << endl;
		exit(1);
	}
	/* Connect to server. */
	if (connect(s, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
		cerr << "Attempt to connect to server failed." << endl;
		exit(1);
	}
	/* Return the file descriptor for the connected socket. */
	return(s);
}