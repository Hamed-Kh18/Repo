#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>

void* READING_FUNC(int* socket_fd);

int main(int argc, char* argv[]){
    
    int server_port_no;
    int client_socket;
    int server_connecting;
    int len_buffer;
    char buffer[256];
    int check_use_pass =0 ;
    buffer[0]='\0';
    
    struct hostent *server;
    struct sockaddr_in server_addr;

    pthread_t thread_id;

    if (argc < 3) {         // error when argc null
        fprintf(stderr,"usage %s hostname port", argv[0]);
        exit(0);
    }

    server_port_no = atoi(argv[2]); // declare port number
    if (server_port_no == 0){

        perror("can't find port");
        exit(1);
    }

    client_socket = socket(AF_INET, SOCK_STREAM, 0); // define client socket (domain , type , protocol)
    if (client_socket <= 0) {
        
        perror("ERROR opening socket");
        exit(1);
    }

    server = gethostbyname(argv[1]);    //  declare server port
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    
    server_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr, server->h_length);
    server_addr.sin_port = htons(server_port_no);

    server_connecting = connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (server_connecting < 0) {
      perror("ERROR connecting");
      exit(1);
    }

    if (check_use_pass == 0){
        printf("username: \n");
        len_buffer = send(client_socket,buffer,strlen(buffer),0);
        check_use_pass++;
    }
    if (check_use_pass == 1)
    {
        printf ("password: \n");
        len_buffer = send(client_socket,buffer,strlen(buffer),0);
    }
       

    pthread_create(&thread_id, NULL, &READING_FUNC, (int *)client_socket);
    while (1){
        
        fgets(buffer,256,stdin);
        len_buffer = send(client_socket,buffer,strlen(buffer),0);
        if (len_buffer < 0) {
            perror("ERROR writing to socket");
            exit(1);
        }   
        buffer[0]='\0';
    }
        close(client_socket);
        return 0;
}

void* READING_FUNC(int* socket_fd){
     
    int n;
    char buffer[256];   
    while ((n = recv((long )socket_fd, buffer, 255,0))!=0){  
        buffer[n]='\0';
        fputs(buffer,stdout); // fputs vs puts ( fputs have not \n but puts have \n)
    } 
   return NULL;
}
 