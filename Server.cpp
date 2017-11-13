#include <iostream>
using namespace std;

#include <time.h>
#include <Winsock2.h>
#include <vector>

#define LINESZ 2048

/* Function to prepare to receive connections for a service. */
static SOCKET passiveTCP(char *service, int qlen);

bool sendAll(vector<SOCKET> c, const char * str, int nb);

/**/
/*
NAME

server - to receive connections from clients and report back the time.

SYNOPSIS

server [service]

DESCRIPTION

The server process with receive connections from client processes and
send back the date and time.
*/
/**/
int main(int argc, char *argv[])
{
	// Make sure that we can connect:
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(1, 1);  // A macro to create a word with low order and high order bytes as specified.

	err = WSAStartup(wVersionRequested, &wsaData);

	if (err != 0) {

		// Tell the user that we couldn't find a useable winsock.dll. */
		cerr << "Could not find useable DLL" << endl;
		return 1;
	}

	// Confirm that the Windows Sockets DLL supports version 1.1.
	// Note that if the DLL supports versions greater than 1.1 in addition 
	//  to 1.1, it will still return1.1 in wVersion since that is the version we/
	// requested.  Note the use of macros.

	if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1) {

		// Tell the user that we couldn't find a useable winsock DLL. 
		WSACleanup();  // Terminates the use of the 
		cerr << "Could not find useable DLL" << endl;
		return 1;
	}
	char *service = "7777"; // The service to be provided.

							/* Fill in the remote host name and service name from the run-time
							* parameters. */

							/* Connect to the server process. */
	SOCKET ls = passiveTCP(service, 10);

	timeval tv;
	tv.tv_sec = 3;
	tv.tv_usec = 0;

	char buff[LINESZ];
	char acpt[100] = "Connection accepted.";


	// Set of clients we're connected to.
	vector<SOCKET> clients;

	clients.push_back(ls);

	/* Wait for connections from clients. */
	while (true) {
		fd_set navi;
		FD_ZERO(&navi);
		for (unsigned int i = 0; i < clients.size(); i++) {
			if (clients[i] != 0) {
				FD_SET(clients[i], &navi);
			}
		}

		// Gets how many sockets are ready to read.
		int waiting = select(clients.size(), &navi, NULL, NULL, &tv);
		// Checks if there are any sockets ready, skips the loop if none.
		if (waiting == 0) continue;
		// Checks if there was an error, terminates if so.
		if (waiting == SOCKET_ERROR) {
			cerr << "There was an error in checking the sockets.  Err no. " << WSAGetLastError() << endl;
			system("PAUSE");
			exit(1);
		}

		// Check if someone is trying to connect, accept and add them to clients if so.
		if (FD_ISSET(clients[0], &navi) != 0) {
			/* Note: the casing of the address sockaddr_in to sockaddr is
			* neccessary to support other types of connections which use
			* different address families. */
			struct sockaddr_in fsin;// Address of the client.
			int alen = sizeof(sockaddr_in);  // Length of client address.  UNIX does not 
											 // require that you specify a size.
			SOCKET s = accept(clients[0], (struct sockaddr *) &fsin, &alen/*NULL*/);

			if (s == INVALID_SOCKET) {

				int errorcode = WSAGetLastError();
			}
			else {
				clients.push_back(s);
			}
		}

		// Iterate through the sockets, starting after the listening socket.
		for (unsigned int i = 1; i < clients.size(); i++) {

			// Check if the socket is ready to read.
			if (FD_ISSET(clients[i], &navi) == 0) {

				// To know if we read anything.
				int nb;

				// Read the socket.
				if ((nb = recv(clients[i], &buff[0], LINESZ, 0))) {

					// Read nothing.
					if (nb == 0) {

						continue;
					}

					// Message everyone.
					else if (sendAll(clients, &buff[0], nb)) {

						// Print what was sent.
						for (int j = 0; j < nb; j++) {
							cout << buff[j];
						}
						cout << endl;

					}

					// Error in messaging.
					else {

						cerr << "There was an error sending.  Err no. " << WSAGetLastError() << endl;
						system("PAUSE");
						exit(1);
					}
				}

				// Error in reading.
				else {

					cerr << "There was an error reading.  Err no. " << WSAGetLastError() << endl;
					system("PAUSE");
					exit(1);
				}
			}
		}
	} // End of while loop
}
/*
NAME

passiveTCP - allocates and binds a server socket using TCP

SYNOPSYS
*/
static SOCKET passiveTCP(char *service, int qlen)
/*
DESCRIPTION

This function will create a socket, bind the server "service" to
it and perpare to listen for connections with a queue length of
"qlen".

RETURNS

This function returns the listening socket if it is successful and
does not return if it fails.
*/
{
	struct servent *pse; /* Points to service information. */
	struct sockaddr_in sin; /* Internet endpoint address. */

							/* Create an end point address for this computer. We use INADDR_ANY
							* for the address of the computer so that we may receive connection
							* requests through any of the computer's IP addresses.  */
	memset((char *)&sin, 0, sizeof(sockaddr_in));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;

	/* Get the port number for the service. */
	if ((pse = getservbyname(service, "tcp")) != NULL) {
		sin.sin_port = (u_short)pse->s_port;

	}
	else if ((sin.sin_port = htons((u_short)atoi(service))) == 0) {
		cerr << "There was an error in establishing the port number.  Err no. " << WSAGetLastError() << endl;
		system("PAUSE");
		exit(1);
	}
	/* Allocate a socket. */
	SOCKET ls;
	if ((ls = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		cerr << "There was an error in creating the socket.  Err no. " << WSAGetLastError() << endl;
		system("PAUSE");
		exit(1);
	}
	/* Bind the address to the socket. */
	if (bind(ls, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
		cerr << "There was an error in binding the socket address.  Err no. " << WSAGetLastError() << endl;
		system("PAUSE");
		exit(1);
	}
	// Indicate that we are ready to wait for connects.
	if (listen(ls, qlen) < 0) {
		cerr << "There was an error listening.  Err no. " << WSAGetLastError() << endl;
		system("PAUSE");
		exit(1);
	}
	// Return the listening socket. 
	return ls;
}

// Sends a message, str, of length nb, to all connections contained in c.
bool sendAll(vector<SOCKET> c, const char * str, int nb) {

	for (int i = 1; i < c.size(); i++) {

		if (!send(c[i], str, nb, 0)) {

			return false;
		}
	}

	return true;
}
