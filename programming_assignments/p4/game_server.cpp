//game_server.cpp
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
#include <fstream>  
#include <stdlib.h>
#include <stdio.h>
#include <time.h> 
#include "P4.h"


const int minport = 10640;
const int maxport = 10649;

using namespace std;

struct leaders{
    string username = "null";
    float score = 0;
	string scoreSt;
} leaders[3];

struct ThreadArgs{
    int clientSocket;
};

pthread_mutex_t mutex;

void *threadGame(void *args);

int main(int argc, char **argv)
{
    if (argc != 2){
		cout << "Not enough arguments, please enter port." << endl;
		return 0;
	}

    short portNum = stoi(argv[1]);
    if (portNum > maxport || portNum < minport){
        cout << "Invalid port." << endl;
        return 0;
    }

    int socketNum = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socketNum == -1){
        cout << "Error creating socket." << endl;
		return 0;
	}

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(portNum);

    int status = bind(socketNum, (sockaddr *)(&serverAddress), sizeof(serverAddress));
    if (status != 0){
        cout << "Error binding." << endl;
        return 0;
    }

    status = listen(socketNum, 5);
    if (status == -1){
		cout << "Error listening." << endl;
		return 0;
    }
	

    ThreadArgs *threadArgs;

	while (true){
        cout << "Listening..." << endl; 

        sockaddr_in clientAddr;
        socklen_t addressLength = sizeof(clientAddr);
        int clientNum = accept(socketNum, (sockaddr *)(&clientAddr), &addressLength);
        if (clientNum == -1){
            cout << "Client failed to connect." << endl;
            return 0;
        } else {
            cout << "Client connected." << endl;
        }

        threadArgs = new ThreadArgs;
        threadArgs->clientSocket = clientNum;

        pthread_t threadID;
        status = pthread_create(&threadID, NULL, threadGame, (void *)(threadArgs));
        if (status != 0){
            cout << "Error creating pthread." << endl;
            return 0;
        }
    }
    return 0;
}

void *threadGame(void *args)
{
	struct ThreadArgs *threadArgs = (ThreadArgs *)args;
    int clientSocket = threadArgs->clientSocket;
    delete threadArgs;
	
	string line;
	string fileName;
	
	fileName = "/home/fac/lillethd/cpsc3500/projects/p4/words.txt";
	ifstream infile;
	infile.open(fileName);
	
	srand(time(NULL));
	int randNum = rand() % maxstrings;
	
	for (int i = 0; i <= randNum; i++){
		getline(infile, line);
	}
	
	cout << "Random word: " << line << endl;
	
    long nameSize = recvLong(clientSocket);
    if (nameSize == -1){
        pthread_detach(pthread_self());
        close(clientSocket);
        return NULL;
    }
    string username = recvString(clientSocket, nameSize);
    if (username == "-1"){
        pthread_detach(pthread_self());
        close(clientSocket);
        return NULL;
    }
	
	long turns = 0;
	long wordLength; 
	string wordPlay;
	bool finished = false;
	int result = 0;
	
	wordLength = line.length();
	for (int i = 0; i < wordLength; i++){
		wordPlay += '-';
	}
	wordPlay += '\0';
	
	char guessed[wordLength];
	strcpy(guessed, wordPlay.c_str());
	
	while (!finished){
		turns++;
		
		long sendTurns = sendLong(clientSocket, turns);
		if (sendTurns == -1){
			pthread_detach(pthread_self());
			close(clientSocket);
			return NULL;
		}
		
		long sendLength = sendLong(clientSocket, wordLength + 1);
		if (sendLength == -1){
			pthread_detach(pthread_self());
			close(clientSocket);
			return NULL;
		}
		
		long sendWord = sendString(clientSocket, wordPlay);
		if (sendWord == -1){
			pthread_detach(pthread_self());
			close(clientSocket);
			return NULL;
		}


		string guess = recvString(clientSocket, 1);
		if (guess == "-1"){
			pthread_detach(pthread_self());
			close(clientSocket);
			return NULL;
		}
		
		if (isalpha(guess[0])){
			guess = toupper(guess[0]);
			
			result = 0; 
			for (int i = 0; i < wordLength; i++){
				if (line[i] == guess[0]){
					guessed[i] = guess[0];
					result = 1; 
				}
			}
			
			wordPlay = guessed[0]; 
			for (int i = 1; i < wordLength; i++){
				wordPlay += guessed[i];
			}
			wordPlay += '\0';
			
			int same = 0; 
			for (int i = 0; i < wordLength; i++){
				if (line[i] == guessed[i])
					same++;
			}
			if (same == wordLength){
				result = 100;
				finished = true;
			}
		} else 
			result = -2;
		
		long sendResult = sendLong(clientSocket, result);
		if (sendResult == -1){
			pthread_detach(pthread_self());
			close(clientSocket);
		}
	}
	float score = (float(turns)/float(wordLength));
	char scoreSt[4];
	sprintf(scoreSt, "%3.2f", score);
	
	string finishMsg = "Congratulations! You guessed the word: " + line + " in " + 
	to_string(turns) + " turns.\nYour score is: " + scoreSt + ". \n\0";
	
	long finishLength = finishMsg.length();
	
	long sendFin = sendLong(clientSocket, finishLength);
	if (sendFin == -1){
		pthread_detach(pthread_self());
		close(clientSocket);
	}
	long sendMsg = sendString(clientSocket, finishMsg);
	if (sendMsg == -1){
		pthread_detach(pthread_self());
		close(clientSocket);
	}
	
	for (int i = 0; i < 3; i++){
        if (leaders[i].score == 0 || score < leaders[i].score){
            pthread_mutex_lock(&mutex);
			if (i == 0) { 
				for (int j = 2; j > 0; j--){
					leaders[j].username = leaders[j-1].username;
					leaders[j].score = leaders[j-1].score;
					leaders[j].scoreSt = leaders[j-1].scoreSt;
				}
				leaders[i].username = username;
				leaders[i].score = score;
				leaders[i].scoreSt = scoreSt;
				i = 3;
			}
			else if (i == 1){ 
				leaders[i+1].username = leaders[i].username;
				leaders[i+1].score = leaders[i].score;
				leaders[i+1].scoreSt = leaders[i].scoreSt;
				leaders[i].username = username;
				leaders[i].score = score;
				leaders[i].scoreSt = scoreSt;
				i = 3;
			}
			else if (i == 2){
				leaders[i].username = username;
				leaders[i].score = score;
				leaders[i].scoreSt  = scoreSt;
				i = 3;
			}		
            pthread_mutex_unlock(&mutex);
        }
    }
	
	string leaderBoard;
	for (int i = 0; i < 3; i++){
		if (leaders[i].score != 0){
			leaderBoard += to_string(i + 1) + ": " + leaders[i].username + ' ' +
			leaders[i].scoreSt + "\n";
		}
	}
	leaderBoard += '\0';
	
	long leadSize = leaderBoard.length();
	long leadSizeSend = sendLong(clientSocket, leadSize);
	if (leadSizeSend == -1){
		pthread_detach(pthread_self());
		close(clientSocket);
	}
	
	long leadMsgSend = sendString(clientSocket, leaderBoard);
	if (leadMsgSend == -1){
		pthread_detach(pthread_self());
		close(clientSocket);
	}
	
	pthread_detach(pthread_self());
	close(clientSocket);
	
	return NULL;
}
	