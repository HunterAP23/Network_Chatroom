#include <iostream>
#include <Winsock2.h>
#include <WS2tcpip.h>

using namespace std;

#define LINESZ  2560

static SOCKET connectTCP(char *host, char *service);

int main()
{

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