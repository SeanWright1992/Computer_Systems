//p4.h
//Sean Wright
//6/1/19

#ifndef P4_H
#define P4_h

#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
const int maxstrings = 57127; 
using namespace std;

long recvLong(int socket)
{
    int bytesLeft = sizeof(long);
    long networkRecv;
    char *bp = (char *)&networkRecv;
    while (bytesLeft)
    {
        int bytesRecv = recv(socket, bp, bytesLeft, 0);
        if (bytesRecv <= 0)
            return -1;
        bytesLeft -= bytesRecv;
        bp += bytesRecv;
    }
    long host = ntohl(networkRecv);
    return host;
}

long sendLong(int socket, long sendAddress)
{
    long networkSend = htonl(sendAddress);
    int bytesSent = send(socket, (void *)&networkSend, sizeof(long), 0);
    if (bytesSent != sizeof(long))
        return -1;
    return 0;
}

string recvString(int socket, long recvSize)
{
    int bytesLeft = recvSize;
    char buffer[bytesLeft];
    char *bp = buffer;

    while (bytesLeft)
    {
        int bytesRecv = recv(socket, bp, bytesLeft, 0);
        if (bytesRecv <= 0)
            return "-1";
        bytesLeft -= bytesRecv;
        bp += bytesRecv;
    }
    string receivedString = buffer;
    return receivedString;
}

long sendString(int socket, string sendString)
{
    long sendSize = sendString.length();
    char buffer[sendSize];
    strcpy(buffer, sendString.c_str());
    int bytesSent = send(socket, (void *)&buffer, sendSize, MSG_DONTWAIT);
    if (bytesSent != sendSize)
        return -1; 
    return 0;
}
		
long sendChar(int socket, char sendChar)
{
	long sendSize = 1;
	char buffer = sendChar;
	int bytesSent = send(socket, (void *)&buffer, sendSize, 0);
	if (bytesSent != sendSize)
		return -1;
	return 0;
}

#endif