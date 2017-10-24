#pragma once
#include "stdafx.h"

struct client;

//maximum clients we will serve
const int MAX_CLIENTS = 64;

//class ServerClass
//Handles all server functions, including password checking, etc.
class ServerClass
{
public:

	//ServerClass();



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
	std::string m_WhiteList="WhiteList.txt"; //default val
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