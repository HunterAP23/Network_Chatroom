
#include "stdafx.h"
//class server functions
#include "ServerClass.h"


/*Whitelist format

Username1 :: Password1
UserName2 :: Password2
UserName3 :: Password3
...
...
*/

//build map using text file of whitelist
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
			
			if (buffer.empty()==true) //was a comment line
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
	else if(WhiteListMap[UserName]==Password)
	{
		std::cout << "Yes this is a correct combination" << std:: endl;
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