#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<stdlib.h>
#include<netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 4400
#define BUFFERSIZE 2048

#define gif "image/gif"
#define htm "text/html"
#define html "text/html"
#define jpeg "image/jpeg"
#define jpg "image/jpeg"
#define ico "image/x-icon"
#define pdf "application/pdf"
#define mp4 "video/mp4"
#define png "image/png"
#define svg "image/svg+xml"
#define xml "text/xml"
#define mp3 "audio/mpeg"

char* body =   "<!DOCTYPE html>"
                "<html>"
                    "<head><title>17020255 - Web Server</title></head>"
                    "<body>"
                        "<center>"
                            "<h1> The Web Server </h1>"
                            "<h4>Implemented using c</h4>"
                        "</center>"
                        "<hr>"
                        "<h2>What this server contains :</h2>"
                        "<h4> mp3  - audio1.mp3 </h4>"
                        "<h4> mp3  - audio2.mp3 </h4>"
                        "<h4> mp4  - video1.mp4 </h4>"
                        "<h4> mp4  - video2.mp4 </h4>"
                        "<h4> jpg  - image1.jpg </h4>"
                        "<h4> jpg  - image3.jpg </h4>"
                        "<h4> png  - image2.png </h4>"
                        "<h4> html - googleHome.html </h4>"
                        "<h4> pdf  - networking.pdf </h4>"
                        "<br><h2> to exit : /exit </h2>"
                        "<h2> copy and paste the file name on the url to visit them. </h2>"
                    "</body>"
                "</html>";

void toRespond(int,char*,void*,int,char*);

int main(int argc , char *argv[]){
    printf("\n                           The Web Server\n");
	printf("\n------------------------------------------------------------------------\n\n");


	//creating the server socket
	int server = socket(AF_INET, SOCK_STREAM, 0);
	if(server == -1){perror("\n[!] Error : Socket was not created"); return 0;}
    else { printf("~ Socket created successfully... \n"); }


	//declaring the address for the socket
    struct sockaddr_in serverAddress ;
    memset(&serverAddress,0,sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(PORT);
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);


    //Bind the socket with the local address
    if(bind(server,(struct sockaddr*)&serverAddress,sizeof(serverAddress)) == -1 ){perror("\n[!] Error : Bind unsuccessful!"); return 0 ; } 
    else {printf("~ Socket binded successfully...\n");}


    //Server Listening for incoming connections 
    if(listen(server,20)==-1){perror("\n[!] Error : Server is not listening!\n"); return 0 ;}
    else{
        printf("\n\n                     Server listening on port %d \n" , PORT );
        printf("------------------------------------------------------------------------\n\n");
    }
	
    int clientSocket;
	char buffer[BUFFERSIZE];
    char reply[BUFFERSIZE]={'\0'};
    char* message;
    char rType[16];
    char rPath[1024];
    char* mime ;
    char* statusCode; 
    char* fileCopy;
    size_t fileSize;        

    //using while loop to have an iterative server
    while (server){

        //Accepting incoming connections
        if((clientSocket= accept(server, (struct sockaddr*)&serverAddress,(socklen_t*)&serverAddress)) == -1){perror("[!] Failed to accept the Client");return -1;}  
        else{printf("New connection accepted on address : %s:4400\n\n",inet_ntoa(serverAddress.sin_addr));}

        //setting up null values on the buffer prior to read,write 
        bzero(buffer, sizeof(buffer));
        bzero(rPath,sizeof(rPath));
        bzero(rType,sizeof(rType));

        //reading the requests
        if(read( clientSocket,buffer,BUFFERSIZE)==-1){perror("[!] Read error :");} 
        //get the first two space seperated strings and store them in rType and rPath
        sscanf(buffer, "%s %s", rType, rPath);
        //get the mime type from the rPath by pointing to the first character after '.'
        mime = strrchr(rPath, '.');
        mime++ ;

        //if condition runs if the request type is GET and the requested path is just "/" 
        if(!strcmp(rType, "GET") && !strcmp(rPath, "/")){
            //select the mime type as text/html
            mime = html;
            statusCode = "HTTP/1.1 200 OK" ;
            printf("Status code : %s\n",statusCode);
            printf("Response :\n%s \n", buffer); 
            toRespond(clientSocket, statusCode,body, strlen(body),mime);
        }
        //to handle the requests from client.c 
        else if(!strcmp(rType,"Client:")){
            while(1){
                sscanf(buffer, "%s %s", rType, rPath);
                message = buffer ;
                //split the buffer and get the message to a seperate variable
                strtok_r(message, " ", &message);
                //disconnect if the message says 'return'
                if(strcmp(message,"return") == 0){
                    printf("Disconnected from %s:%d\n", inet_ntoa(serverAddress.sin_addr), ntohs(serverAddress.sin_port));
                }
                //else get a reply from server to send to the client
                else{
                    printf("Client : %s\nServer : ",message);
                    scanf("%s",reply);
                    write(clientSocket,reply,strlen(reply));
                }
                //reset the message and reply
                bzero(message, sizeof(message));
                bzero(reply,sizeof(reply));
            }
        }
        //to exit from the loop
        else if(!strcmp(rType, "GET") && !strcmp(rPath, "/exit")){
            char* msg = "<br><br><h1><center> Server Shutdown!</center></h1>" ;
            mime = html ; 
            toRespond(clientSocket,"HTTP/1.1 200 OK",msg, strlen(msg),mime);
            printf("[!] Disconnecting from server...\n");
            sleep(3);
            printf("[!] Disconnected!\n\n");
            shutdown(clientSocket,2);
	        shutdown(server,2);
	        return 0;
        }    
        else{
            //open the file with having read permissions 
            FILE *input = fopen(strtok(rPath, "/"),"r");
            //if the file cannot be found, displaying the error!
            if(input== NULL){
                statusCode = "HTTP/1.1 404 NOT FOUND";
                printf("status code : %s\n",statusCode);
                printf("Response :\n%s \n", buffer); 
                char* extend = "<br><br><h2><center> ERROR 404 : File Not Found</center></h2>" ;
                mime = html;
                toRespond(clientSocket,statusCode, extend, strlen(extend),mime);
            }
            else{
                statusCode = "HTTP/1.1 200 OK" ;
                if(strcmp(buffer,"\0")!=0){
                    printf("Status Code : %s\n",statusCode);
                    printf("Response :\n%s \n", buffer); 
                }
                //traversing to the end of the file to obtain the file size
                fseek(input,0L,SEEK_END);
                fileSize = ftell(input);
                fileCopy = malloc(fileSize+1);
                //traversing back to the begining of the file to read the file
                fseek(input,0L,SEEK_SET);
                fread(fileCopy,sizeof(char),fileSize,input);
                toRespond(clientSocket,statusCode,fileCopy,fileSize,mime);
                free(fileCopy);
                fclose(input);
            }
        }
    }
    shutdown(clientSocket,2);
	shutdown(server,2);
	return 0;
}

void toRespond(int sock, char *header, void *body,int len,char *fileType) {
    char response[len+256];
    //adding the request header to the reponse
    sprintf(response,"%s\nConnection: close\nContent-Length: %d\nContent-Type: %s\n\n",header,len,fileType);
    //appending the body to the response
    memcpy(response + strlen(response), body, len);
    if(write(sock,response,(strlen(response)+len))==-1){perror("[!] Write error :");}
}
