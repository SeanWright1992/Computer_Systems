//game_client.cpp
//Sean Wright
//6/1/19

#include <iostream>
#include <string>
#include <cstring>
#include <ctype.h>
#include <sys/types.h>  
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>  
#include <unistd.h>     
#include "P4.h"

using namespace std;


int main(int argc, char *argv[])
{
	if (argc < 3){ 
		cout << "Not enough arguments, please enter port and IP." << endl;
		return 0;
	}
	
	unsigned long serverIP;
    int status = inet_pton(AF_INET, argv[1], (void *)&serverIP);

    unsigned short serverPort = stoi(argv[2]);
    int serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == -1){
		cout << "Error creating socket." << endl;
		return 0;
	}

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = serverIP;
    serverAddress.sin_port = htons(serverPort);

    status = connect(serverSocket, (sockaddr *)&serverAddress, sizeof(serverAddress));
    if (status != 0){
        cout << "Error connecting to server." << endl;
        return 0;
    } else {
		cout << endl << "Connected to game server, welcome to Hangman!" << endl;
	}

    string username;
    cout << "Please enter a username: ";
    getline(cin, username);
    cout << endl;
	
    sendLong(serverSocket, long(username.length()));
    sendString(serverSocket, username);
	

	bool finished = false;
	long wordLength;
	string wordPlay;
	long result;
	long sent;
	string guess;
	bool valid;
	long turns;
	while (!finished){
		turns = recvLong(serverSocket);
		if (turns == -1){
			cout << "Error receiving turns." << endl;
			return 0;
		}
		cout << endl << "Turn " << turns << endl;
		
		wordLength = recvLong(serverSocket);
		if (wordLength == -1){
			cout << "Error receiving word length." << endl;
			return 0;
		}
		
		wordPlay = recvString(serverSocket, wordLength);
		if (wordPlay == "-1"){
			cout << "Error receiving word to play." << endl;
			return 0;
		}
		
		cout << "Word: " << wordPlay << " "<< endl;
		
		valid = false;
		while (!valid){
			cout << "Please enter a letter to guess:";
			cin >> guess;
			cout << endl;
			if (!isalpha(guess[0]) || guess.length() >1){
				cout << "Invalid guess, please guess again." << endl;
			} 
			else
				valid = true;
		} 
		sent = sendChar(serverSocket, toupper(guess[0])); 
		if (sent == -1){
			cout << "Error sending guess." << endl;
			return 0;
		}
		
		result = recvLong(serverSocket);
		if (result == -1){
			cout << "Error receiving result." << endl;
			return 0;
		}
		
		if(result == 1)
			cout << "That letter is in the word!" << endl;
		else if (result == 0)
			cout << "That letter is not in the word!" << endl;
		else if (result == 100){
			finished = true;
		} else if (result == -2)
			cout << "Invalid guess."; 
	}
	
	long finishSize = recvLong(serverSocket);
	if (finishSize == -1){
		cout << "Error receiving finish message size." << endl;
		return 0;
	}
	
	string finishMsg = recvString(serverSocket, finishSize);
	if (finishMsg == "-1"){
		cout << "Error recieving finish message." << endl;
		return 0;
	}
	
	cout << finishMsg;
	
	cout << "Leaderboard: " << endl;
	
	long leaderSize = recvLong(serverSocket);
	if (leaderSize == -1){
		cout << "Error receiving leader board size." << endl;
		return 0;
	}
	
	string leaderBoard = recvString(serverSocket, leaderSize);
	if (leaderBoard == "-1"){
		cout << "Error receiving leader board." << endl;
		return 0;
	}
	
	cout << leaderBoard;
	close(serverSocket);
	return 0;
}
