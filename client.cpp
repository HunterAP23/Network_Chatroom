#include "stdafx.h"
#include "Client.h"

//call right after connectTCP connects
bool ClientClass::StartConnect(SOCKET s)
{
	std::string ConnectMessage="";
	ConnectMessage += UserName + ":" + Password;
	std::cout << "Connecting with Username:PassWord = " << ConnectMessage << std::endl;
	send(s, ConnectMessage.c_str(), ConnectMessage.length(), NULL); // will block sending
	char* buff="";
	int returnval = 0;

	returnval = recv(s, buff, 1024, NULL); //will block waiting for response
	//error check todo return false

	std::cout << buff << std::endl;

	return true;
}

//for getting time
const std::string ClientClass::currentTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%X", &tstruct);
	return buf;
}

//for every send besides start
bool ClientClass:: CSend(SOCKET s, std::string Message)
{
	int returnvalue;
	UserName += ": " + Message + "   " + currentTime();
	returnvalue=send(s, UserName.c_str(), UserName.length(), NULL);
	//todo, errorchecking and return false
	return true;
}

void ClientClass::SetUserName(std::string name)
{
	UserName = name;
}

void ClientClass::SetPassWord(std::string Pass)
{
	Password = Pass;
}
