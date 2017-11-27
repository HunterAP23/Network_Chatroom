#pragma once
#include <string>
#include <WinSock2.h>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <algorithm>

struct client;

//maximum clients we will serve
const int MAX_CLIENTS = 64;
//maximum line size to be read/sent at once
const int LINESZ = 2048;
//maximum number of clients allowed to be waiting for a connection
const int QUEUESZ = 10;
//prefix applied to all messages sent by the server.
const std::string PREFIX = ":";

//class ServerClass
//Handles all server functions, including password checking, etc.
class ServerClass
{
public:

	//ServerClass();
	//requests service and listening port
	void InitServer();

	//main server routine
	bool receiveMsg(SOCKET ls);

	//sends an error message to all clients if the server experienced a terminal error.
	void serverFailure(std::vector<SOCKET> c) {
		char buff[LINESZ] = "500: Internal Server Error";
		sendAll(c, &buff[0], strlen(buff));
	}

	//sends a message, str, to all connected users.
	bool sendAll(std::vector<SOCKET> c, const char * str, int nb);

	static SOCKET passiveTCP(char *service, int qlen);


	bool LoginAttempt(client& x);
	//getter setter for whitelistpath
	std::string GetWhiteList();
	void SetWhiteList(std::string NewPath);

	//for map version, builds whitelist from text file at ?server start? or have a function to UpdateWhiteList mid server
	void LoadWhiteList();

	//when a new client connects, calls other functions to complete login procedure
	bool NewConnect(SOCKET s);
	//part of handshake process, checks if client has updated security code
	bool CheckSecurityCode(int code);
	//checks WhiteList for username + password combination returns true if found false if not
	bool CheckWhiteList(std::string UserName, std::string PassWord);

protected:



private:
	std::map<std::string, std::string> WhiteListMap;
	// path for where the WhiteList (assortment of usernames and passwords
	std::string m_WhiteList = "WhiteList.txt"; //default val
	int keycode = 1312; //default val
	SOCKET ListeningSocket; //store our listening socket
							//connected client array
							//SOCKET SOC[MAX_CLIENTS]; 
	std::vector<client> SOC;
};

struct client
{
public:
	SOCKET s; //socket this client is on, identifying feature I guess? 
	enum ClientStatus { Code, PassWord, LoggedIn }; //login status, code means they need to supply the code, password for loggin in process
													//logged in means they have fully logged in and are participating in the chat and recieving messages

protected:

private:

};