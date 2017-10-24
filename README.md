"# Network_Chatroom" 

Collaborators:
Etgar Kaspi / Lucas Crockett / Peter Wright / Brendan Brence

The idea is for the client to collect the username and password,
then the server would receive that information and compare to the file of listed usernames/passwords.

Every time the user sends a message, the current time is logged, and the message is sent along with the timestamp of the message.

The server will read this message, and check it for errors, then send the message & timestamp to all logged in users.

The clients will receive this message as well as who it came from) and display it in the chat window.

Client-side Functions:

	Sign-In(string username, string password, string time): Get username and password, send that info to the server
	
	Sign-Out(): Tell server to sign out the user.
	
	Send message to Server(string message, string time): Write the message to the server
	
	Receive messages from Server(): The client receives message(s) from the server
	
	Display to 2nd Window(): The Client prints the received messages to the 2nd screen.

Server-Side Functions:

	Sign-In(string username, string password): Receive username and password from the client, and compare it to the usernames in the stored username/password file.
	
	Sign-Out(): Sign the user out from the user.
	
	Receive message from Client(string username, string message, string time): Receive messages from a user.
	
	Send messages to Client(string username, string message, string time): Send messages to all signed in users.
