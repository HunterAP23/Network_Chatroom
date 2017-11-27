
//class server functions
#include "ServerClass.h"

void ServerClass::InitServer()
{
	// Make sure that we can connect:
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	// A macro to create a word with low order and high order bytes as specified.
	wVersionRequested = MAKEWORD(1, 1);

	err = WSAStartup(wVersionRequested, &wsaData);

	if (err != 0) {
		// Tell the user that we couldn't find a useable winsock.dll.
		std::cerr << "Could not find useable DLL" << std::endl;
		exit(1);
	}

	// Confirm that the Windows Sockets DLL supports version 1.1.
	// Note that if the DLL supports versions greater than 1.1 in addition 
	//  to 1.1, it will still return1.1 in wVersion since that is the version we/
	// requested.  Note the use of macros.
	if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1) {
		// Tell the user that we couldn't find a useable winsock DLL. 
		WSACleanup();
		std::cerr << "Could not find useable DLL" << std::endl;
		exit(1);
	}

	// The service to be provided.
	char *service = "7777";

	// Set up a listening socket.
	SOCKET ls = passiveTCP(service, QUEUESZ);

	if (!receiveMsg(ls)) {
		exit(1);
	}
}

bool ServerClass::receiveMsg(SOCKET ls)
{
	// Struct dicating the timeout interval.
	timeval tv;
	tv.tv_sec = 3;
	tv.tv_usec = 0;

	// Char buff for accepting messages.
	char buff[LINESZ];

	// Set of clients we're connected to.
	std::vector<SOCKET> clients;
	clients.push_back(ls);



	while (true) {
		// Construct the fd_set from scratch each loop.
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
			std::cerr << "There was an error in checking the sockets.  Err no. " << WSAGetLastError() << std::endl;
			serverFailure(clients);
			system("PAUSE");
			return false;
		}

		/**************************************************************************************************************
		* This is the block where login attempts will be processed.                                                   *
		**************************************************************************************************************/
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
		/**************************************************************************************************************/

		// Iterate through the sockets, starting after the listening socket.
		for (unsigned int i = 1; i < clients.size(); i++) {

			// Check if the socket is ready to read.
			if (FD_ISSET(clients[i], &navi) != 0) {

				// Number of Bytes read.
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
							std::cout << buff[j];
						}
						std::cout << std::endl;
					}

					// Error in messaging.
					else {
						std::cerr << "There was an error sending.  Err no. " << WSAGetLastError() << std::endl;
						serverFailure(clients);
						system("PAUSE");
						return false;
					}

				}

				// Error in reading.
				else {
					std::cerr << "There was an error reading.  Err no. " << WSAGetLastError() << std::endl;
					serverFailure(clients);
					system("PAUSE");
					return false;
				}
			}
		}
	} // End of while loop

	return true;
}


bool ServerClass::sendAll(std::vector<SOCKET> c, const char * str, int nb)
{
	for (int i = 1; i < c.size(); i++) {

		if (!send(c[i], str, nb, 0)) {

			return false;
		}
	}

	return true;
}

/*Whitelist format
Username1 :: Password1
UserName2 :: Password2
UserName3 :: Password3
...
...
*/

//build map using text file of whitelist

/* Something along these lines has to go in server recieve message loop
if (SOC[i].loggedin ==false)
{
if (Server.LoginAttempt(SOC[i])==false)
{
//disconnect this socket
}
}
*/

bool ServerClass::LoginAttempt(client& x)
{
	char buff[50]; //login message will not be longer than this
	std::string username, password;
	int returnval;
	returnval = recv(x.s, buff, 50, NULL);

	std::string buffer(buff);

	//parse their message for username and password
	buffer.erase(std::remove_if(buffer.begin(), buffer.end(), isspace), buffer.end());
	username = buffer.substr(0, buffer.find(':'));
	password = buffer.substr(buffer.find(':') + 1);
	//check readmap
	if (CheckWhiteList(username, password) == true)
	{
		x.LoggedIn = true; //log them in
		return true;
	}
	else
	{
		//this will tell us to disconnect
		return false;
	}
}

void ServerClass::LoadWhiteList()
{
	std::ifstream WhiteList;
	WhiteList.open(m_WhiteList);
	std::string buffer;
	std::string UserName;
	std::string PassWord;
	while (std::getline(WhiteList, buffer))
	{
		buffer.erase(std::remove_if(buffer.begin(), buffer.end(), isspace), buffer.end());
		if (buffer.find('//') != std::string::npos)
		{
			buffer.erase(buffer.find('//'));

			if (buffer.empty() == true) //was a comment line
			{
				continue;
			}
		}
		UserName = buffer.substr(0, buffer.find('::'));
		PassWord = buffer.substr(buffer.find('::') + 2);
		WhiteListMap[UserName] = PassWord;
	}

	for (auto elem : WhiteListMap) //print map for testing purposes
	{
		std::cout << elem.first << " " << elem.second << std::endl;
	}
}

//map version of checkwhitelist, probably faster, checks if UserName and Password exist on server
bool ServerClass::CheckWhiteList(std::string UserName, std::string Password)
{
	//find unique key (username) and match it to Password, if no such combination exists, return false, if combo exists, return true
	std::cout << "Searching for " << UserName << " with password " << Password << std::endl;
	if (WhiteListMap.find(UserName) == WhiteListMap.end()) //check to see if username exists
	{
		std::cout << "no such username exists" << std::endl;
		return false;
	}
	else if (WhiteListMap[UserName] == Password)
	{
		std::cout << "Yes this is a correct combination" << std::endl;
		return true;
	}

	return false;
}

//sets the path for whitelist, if we need to update while running
void ServerClass::SetWhiteList(std::string NewPath)
{
	m_WhiteList = NewPath;
}

//retrieves the white list path name
std::string ServerClass::GetWhiteList()
{
	return m_WhiteList;
}

/*NewConnect
for after the socket has initally connected, but needs to do the 'handshake' and login process
first check for security code, then check login process
*/
bool ServerClass::NewConnect(SOCKET s)
{
	std::cout << "New connect at socket " << s << std::endl;
	//ask for security code, then ask for password
	//honestly will probably delete this
	return true;
	return false;
}

/* CheckSecurityCode
for checking security code to make sure client has the correct code
*/
bool ServerClass::CheckSecurityCode(int code)
{
	if (code == keycode) {
		std::cout << "It's an older code, but it checks out " << std::endl;
		return true;
	}
	else
	{
		std::cout << "Incorrect keycode, access denied" << std::endl;
		return false;
	}
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
		std::cerr << "There was an error in establishing the port number.  Err no. " << WSAGetLastError() << std::endl;
		system("PAUSE");
		exit(1);
	}
	/* Allocate a socket. */
	SOCKET ls;
	if ((ls = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		std::cerr << "There was an error in creating the socket.  Err no. " << WSAGetLastError() << std::endl;
		system("PAUSE");
		exit(1);
	}
	/* Bind the address to the socket. */
	if (bind(ls, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
		std::cerr << "There was an error in binding the socket address.  Err no. " << WSAGetLastError() << std::endl;
		system("PAUSE");
		exit(1);
	}
	// Indicate that we are ready to wait for connects.
	if (listen(ls, qlen) < 0) {
		std::cerr << "There was an error listening.  Err no. " << WSAGetLastError() << std::endl;
		system("PAUSE");
		exit(1);
	}
	// Return the listening socket. 
	return ls;
}