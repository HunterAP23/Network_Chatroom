#pragma once
#include "stdafx.h"

class ClientClass
{
public:
	bool StartConnect(SOCKET s); //connect function
	bool CSend(SOCKET s, std::string Message); //for every send message, adds timestamp and username then sends to server

	void SetUserName(std::string Name);
	void SetPassWord(std::string Pass);

	const std::string currentTime();

protected: 

private:
	//username
	std::string UserName;
	std::string Password;
};
