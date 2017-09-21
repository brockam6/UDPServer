//Name of Author: Brock McCullough
//Course and Semester: CSE 434 Fall 2017
//Project part: 1
//Time Spent: 6.5 hours

/*******************************************QUESTION********************************************************************************/


/*The first difference in the TCP vs UDP servers is the use of SOCK_DGRAM (datagram) vs SOCK_STREAM (byte stream). Another difference is in the use of 'listen' & 'accept' in the TCP that blocks the process until the client connects to the server. A third difference is the use of the 'send_to' function in the UDP vs the use of 'write' in TCP. The last difference is 
 * there is no 'connect' system call when using the UDP server model.*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//Data types for system calls
#include <sys/types.h>

//Structures for sockets
#include <sys/socket.h>

//Constants and sructures needed for internet domain
#include <netinet/in.h>

void error(const char *msg)
{
 perror(msg);
 exit(1);
}

/*editBuffer method will receive the message from the client with the client id prepended to the string and parse it out, then return the message to be read in main method*/
char* editBuffer(char *bufferWithId, int idSize)
{
 idSize++;
 char *newBuffer = new char[256];
 int newBufferIndex = 0;
 for(int i=idSize;i<strlen(bufferWithId);i++)
  {
   newBuffer[newBufferIndex] = bufferWithId[i];
   newBufferIndex++;
  }
 return newBuffer;
}

int main(int argc, char *argv[])
{
  int valid = 0;
  if(argv[1] == NULL)
   {
    valid = 1;
    printf("A port number must be entered\n");
   }
  else
   {
        int portNumber;
	portNumber = atoi(argv[1]);
	printf("The socket number is: %d\n",portNumber);

	socklen_t socketInfo;
	char buffer[256];

	struct sockaddr_in serv_addr;
	struct sockaddr_in client_addr;
	int z;
	int sock;
	int newSocket;	
//Creat socket called sock using SOCK_DGRAM which uses datagrams instead of bytestream
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock < 0) //socket will return less than 0 on failure
	{
	  error("Error opening socket");
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));

	//AF_INET is used for internet file system. AF_UNIX is used for internal sockets
	serv_addr.sin_family = AF_INET;

	//port number given to the sockaddr_in structure
	serv_addr.sin_port = htons(portNumber);

	//Contains the IP address of the host
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	//Binds the socket to the address of the current host
	if(bind(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
	 error("Error on binding");	 
	}

	//Liten for the 5 (max) number of socket connections on sock
	//listen(sock,5);

	socketInfo = sizeof(client_addr);

	//accept wakes the server up when a client connects to it
	//newSocket = accept(sock, (struct sockaddr *) &client_addr, &socketInfo);
	
	//Create an array to hold the client ids from incoming clients
	int usedClients[6];

	//Create counter to keep track of index in array
	int usedIndex = 0;

	//Flag to notify duplicate client number
	int cFlag = 0;

	//Clear buffer to receive information from client
 	bzero((char *)&buffer, sizeof(buffer));

	//Message will be sent when the client id is not valid to notify client
	char message[256];
 	bzero((char *)&message, sizeof(message));
	strcpy(message,"1");
	
	//Client id given through command line arguments
	char idFromClient[256];
 	bzero((char *)&idFromClient, sizeof(idFromClient));

/*Continuously prepare to connect with clients and repsond with echo or invalid id*/
	while(valid == 0)
	{
	 newSocket = recvfrom(sock, buffer, 255, 0, (struct sockaddr *)&client_addr, &socketInfo);	
	int index = 0;
	while(buffer[index] != '-')
	{
	 idFromClient[index] = buffer[index];
	 index++;
	}

	int clientIdSize = strlen(idFromClient);
	char * tempBuffer = new char[256];
	tempBuffer = editBuffer(buffer,clientIdSize);
	strcpy(buffer,tempBuffer);

	//cFlag will be used to indicate whether client id is valid or not 
	cFlag = 0;

	//If used index array is not empty, iterate through and check if current client id
	//has already been used
	if(usedIndex != 0)
	{
	 for(int i=0;i<=usedIndex;i++)
	  {
	   if(atoi(idFromClient) == usedClients[i])
	    {
	     printf("Message from client %s: %s\n",idFromClient,buffer);
	     z = sendto(sock, message, strlen(message), 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
  	     printf("UDP Server: Duplicate client number %d\n\n", usedClients[i]);
	     cFlag = 1;
            }
	  }
	 }
 
	 usedClients[usedIndex] = atoi(idFromClient);

	 if(newSocket < 0)
	 {
  	  error("Error on accept");
	 }
	//If the vlient id is not a duplicate, respond with an echo of the message
	 else if(cFlag != 1)
	 {	
	   usedIndex++;
	   printf("Message from client %s: %s\n",idFromClient,buffer);
     z = sendto(sock, buffer, newSocket, 0, (struct sockaddr *)&client_addr, socketInfo);
	   bzero((char *)&buffer, sizeof(buffer));
	 }
	}
}
}









