#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<stdlib.h>
#include<netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 4201
#define BUFFERSIZE 2048


int main(int argc, char** argv){
    
    //creating the client socket
	int client = socket(AF_INET, SOCK_STREAM, 0);
	if(client == -1){perror("\n[!] Error : Client Socket was not created"); return 0;}
    else { printf("~ Client Socket created successfully... \n"); }


	//declaring the address for the client socket
    struct sockaddr_in clientAddress ;
    memset(&clientAddress,0,sizeof(clientAddress));
    clientAddress.sin_family = AF_INET;
	clientAddress.sin_port = htons(PORT);
	clientAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    if(connect(client,(struct sockaddr *)&clientAddress,sizeof(clientAddress))<0){perror("\n[!] Error : Connection unsuccessful!"); return 0 ; } 
    else {printf("~ connected to the server successfully...\n");};
    
    printf("\nType anything to send messages to the server or type 'return' to disconnet");
    printf("\n--------------------------------------------------------------------------\n");

    char buffer[BUFFERSIZE];
    char message[BUFFERSIZE-8];

    while(1){
        strcpy(buffer,"Client: ") ;
        printf("%s",buffer);
		scanf("%s", message);
        strcat(buffer,message);
		send(client,buffer,strlen(buffer),0);

		if(strcmp(message,"return") == 0){
			close(client);
			printf("\n--------------------------------------------------------------------------\n");
			printf("\n[!] Disconnecting from server...\n");
			printf("[!] Disconnected!\n\n");
			return 0 ;
		}

		if(recv(client, buffer,BUFFERSIZE, 0)==-1){perror("\nError : Cannot receive data from the server"); return 0;}
		else{printf("Server:%s from the server\n", buffer);}
    }
    return 0 ;
}