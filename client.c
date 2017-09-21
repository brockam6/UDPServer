//Name of Author: Brock McCullough
//Course and Semester: CSE 434 Fall 2017
//Project part: 1
//Time Spent: 6.5 hours

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

//Data types for system calls
#include <sys/types.h>

//Definitions of structures needed for sockets
#include <sys/socket.h>

//in.h contains constants and structures needed for internet domain addresses
#include <netinet/in.h>

//netdb.h defines the structure hostent,
#include <netdb.h>

//This function is called when a system call fails. It displays a message about the error on stderr and then aborts the program.
void error(const char *msg)
{
    perror(msg);
    exit(0);
}

char* buildMessage(char *clientid, char *message)
 {
  char * newMessage = new char[256];;
  int sizeOfid = strlen(clientid);
  strcpy(newMessage, clientid);
  int messageSize = strlen(message);
  strcat(newMessage, "-");
  strcat(newMessage, message);

  return newMessage;
 }

int main(int argc, char *argv[])
{
 int invalid = 0;
 if(argv[1] == NULL || argv[2] == NULL || argv[3] == NULL)
  {
   printf("Correct arguments not entered, try again\n");
   invalid++;
  }
 int portNumber = atoi(argv[1]);
 char *clientNumber = argv[3];
 char *toSend;
//Now put the id length, followed by id number, followed by the message in the message buffer before sending to server
socklen_t serverInfo;
 
 int sock;
 int z;
 
 //serv_addr will contain address of server
 struct sockaddr_in serv_addr;

 //Will hold the name of the server
 struct hostent *hostName;

 //gethostbyname() is function that returns pointer to hostent containg host info
 hostName = gethostbyname(argv[2]);
 char buffer[256];

 sock = socket(AF_INET, SOCK_DGRAM, 0);

 if(sock < 0)
 {
  error("Error opening socket");
 }	

 if(hostName == NULL)
  {
   fprintf(stderr, "ERROR, no such host\n");
   exit(0);
  }

while(invalid == 0)
{
 //Get message to send to server
 printf("What would you like to send the server?\n");
 char message[256]; 
 fgets(message,255,stdin);

 toSend = buildMessage(clientNumber,message);
 strcpy(message, toSend);
 //bzero function clears memory space. Arg(1) is address, arg(2) is size
 bzero((char *) &serv_addr, sizeof(serv_addr)); 

 //Sets the address family used to set up the socket info struct
 serv_addr.sin_family = AF_INET;

 memcpy((void *)&serv_addr.sin_addr, hostName->h_addr_list[0], hostName->h_length);

 //Hold the port number in the socket info struct (htons is host to network)
 serv_addr.sin_port = htons(portNumber);

 if(sendto(sock, message, strlen(message), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
 {
  perror("sendto failed");
  return 0;
 } 

 //Get size of server address in preparation to receive incoming packets
 serverInfo = sizeof(serv_addr);

 //Clear out buffer to receive traffic from server
 bzero((char *)&buffer, sizeof(buffer));
 int getSocket;
 getSocket = recvfrom(sock, buffer, 255, 0, (struct sockaddr *)&serv_addr, &serverInfo);

 //Check is server finds the client id to be valid and print the respoinse from server 
 if(atoi(buffer) == 1)
 {
  printf("Client number is not valid\n");
  invalid++;
 }
 else
 {
  printf("Echo from server: %s\n",buffer); 
 }
}
}
