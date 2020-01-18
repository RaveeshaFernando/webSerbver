#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<stdlib.h>
#include<netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 4209
#define BUFFERSIZE 2048
#define LA INADDR_ANY

#define css "text/css"
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
                        "<h4> mp3  - song.mp3 </h4>"
                        "<h4> mp3  - song2.mp3 </h4>"
                        "<h4> mp4  - video.mp4 </h4>"
                        "<h4> jpg  - image.jpg </h4>"
                        "<h4> html - page.html </h4>"
                        "<h4> pdf  - readme.pdf </h4>"
                        "<h2> copy and paste the file name on the url to visit them. </h2>"
                    "</body>"
                "</html>";
char *mime;

void toRespond(int,char*,void*,int);

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


    //Listening
    if(listen(server,20)==-1){perror("\n[!] Error : Server is not listening!\n"); return 0 ;}
    else{
        printf("\n\n                     Server listening on port %d \n" , PORT );
        printf("------------------------------------------------------------------------\n\n");
    }
	
    int clientSocket;
	char buffer[BUFFERSIZE]={'\0'};
    char rType[16]={'\0'};
    char rPath[1024]={'\0'};
    char* statusCode ;
    char reply[BUFFERSIZE]={'\0'};
    pid_t childpid ;

    while (1){
        //accepting incoming connections
        if((clientSocket= accept(server, (struct sockaddr*)&serverAddress,(socklen_t*)&serverAddress)) == -1){perror("[!] Failed to accept the Client");return -1;}  
        else{printf("New connection accepted on address : %s : %d\n\n",inet_ntoa(serverAddress.sin_addr),ntohs(serverAddress.sin_port));}


        // read( clientSocket,buffer,BUFFERSIZE); 
        // sscanf(buffer, "%s %s", rType, rPath);
        // mime = strrchr(rPath, '.');
        // mime++ ;
        
        if((childpid = fork()) == 0){
			while(1){
				read( clientSocket,buffer,BUFFERSIZE); 
                sscanf(buffer, "%s %s", rType, rPath);
                mime = strrchr(rPath, '.');
                mime++ ;
        
                if (!strcmp(rType, "Client:")){
                    char* message = buffer; 
                    strtok_r(message, " ", &message);
                    if(strcmp(message,"return") == 0){
                        printf("Disconnected from %s:%d\n", inet_ntoa(serverAddress.sin_addr), ntohs(serverAddress.sin_port));
                    }
                    else{
                        printf("Client : %s\nServer : ",message);
                        scanf("%s",reply);
                        send(clientSocket,reply,strlen(message), 0);
                        bzero(buffer, sizeof(buffer));
                        bzero(message, sizeof(message));
                        bzero(reply,sizeof(reply));
                    }
                }
                else if(!strcmp(rType, "GET") && !strcmp(rPath, "/")){
                    char *data = body ;
                    mime = html;
                    statusCode = "HTTP/1.1 200 OK" ;
                    printf("status code : %s\n",statusCode);
                    printf("%s \n", buffer); 
                    toRespond(clientSocket, statusCode, data, strlen(data));
                }
                else{
                    FILE *input = fopen(strtok(rPath, "/"),"r");
                    char *source;
                    size_t bufsize, readBytes;
                    if (input != NULL){
                        statusCode = "HTTP/1.1 200 OK" ;
                        printf("status code : %s\n",statusCode);
                        printf("%s \n", buffer); 
                            
                        if (fseek(input, 0L, SEEK_END) == 0){
                            bufsize = ftell(input);
                            source = malloc((ftell(input) + 1));
                            fseek(input, 0L, SEEK_SET);    
                            fread(source, sizeof(char),bufsize,input);
                            toRespond(clientSocket,statusCode, source,ftell(input));
                        }
                        free(source);
                        fclose(input);
                    }
                    else{
                        statusCode = "HTTP/1.1 404 NOT FOUND";
                        printf("status code : %s\n",statusCode);
                        printf("%s \n", buffer); 
                        char* extend = "<h2><center> HTTP/1.1 404 :  File Not Found</center></h2>" ;
                        mime = html;
                        toRespond(clientSocket, "HTTP/1.1 404 NOT FOUND", extend, strlen(extend));
                    } 
                }  
			}
		}


        // if (!strcmp(rType, "Client:")){
        //     char* message = buffer; 
        // 	strtok_r(message, " ", &message);
        //     if(strcmp(message,"return") == 0){
        //         printf("Disconnected from %s:%d\n", inet_ntoa(serverAddress.sin_addr), ntohs(serverAddress.sin_port));
        //     }
        //     else{
        //         printf("Client : %s\nServer : ",message);
        //         scanf("%s",reply);
        //         send(clientSocket,reply,strlen(message), 0);
        //         bzero(buffer, sizeof(buffer));
        //         bzero(message, sizeof(message));
        //         bzero(reply,sizeof(reply));
        //     }
        // }
        // else if(!strcmp(rType, "GET") && !strcmp(rPath, "/")){
        //     char *data = body ;
        //     mime = html;
        //     statusCode = "HTTP/1.1 200 OK" ;
        //     printf("status code : %s\n",statusCode);
        //     printf("%s \n", buffer); 
        //     toRespond(clientSocket, statusCode, data, strlen(data));
        // }
        // else{
        //     FILE *input = fopen(strtok(rPath, "/"),"r");
        //     char *source;
        //     size_t bufsize, readBytes;
        //     if (input != NULL){
        //         statusCode = "HTTP/1.1 200 OK" ;
        //         printf("status code : %s\n",statusCode);
        //         printf("%s \n", buffer); 
                    
        //         if (fseek(input, 0L, SEEK_END) == 0){
        //             bufsize = ftell(input);
        //             source = malloc((ftell(input) + 1));
        //             fseek(input, 0L, SEEK_SET);    
        //             fread(source, sizeof(char),bufsize,input);
        //             toRespond(clientSocket,statusCode, source,ftell(input));
        //         }
        //         free(source);
        //         fclose(input);
        //     }
        //     else{
        //         statusCode = "HTTP/1.1 404 NOT FOUND";
        //         printf("status code : %s\n",statusCode);
        //         printf("%s \n", buffer); 
        //         char* extend = "<h2><center> HTTP/1.1 404 :  File Not Found</center></h2>" ;
        //         mime = html;
        //         toRespond(clientSocket, "HTTP/1.1 404 NOT FOUND", extend, strlen(extend));
        //     } 
        // }  
    }
    close(clientSocket);
	close(server);
	return 0;
}

void toRespond(int sock, char *header, void *body,int len) {
    char response[len+256] ; 
    sprintf(response,"%s\nConnection: close\nContent-Length: %d\nContent-Type: %s\n\n",header,len,mime);
    memcpy(response + strlen(response), body, len);
    write(sock,response,strlen(response) + len);
}
