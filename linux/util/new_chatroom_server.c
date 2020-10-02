#include <stdio.h> 
#include <string.h> //strlen 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h> //close 
#include <arpa/inet.h> //close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <pthread.h>

int CLIENT_BROADCAST[10]={0};   // client array 0 = empty client  1 = define a client
int CLIENT_DATA_BROADCST[10];   // client counter

typedef struct {
    char *col1;
    char *col2;
} USER_DATA;

struct client_information {     // define clients information
    int new_socket;
    int client_number;
    int PORT_client;
    int user_pass_counter;
    pthread_mutex_t *lock;
    USER_DATA *user_pass_data;
};

void* CLIENT_FUNC (struct client_information *client_data);             // In this function, the server communicates with the client
void* BROADCAST_FUNC(char* copy_buffer , int speaker , int my_data);    // In this function, the server broadcasting the client message to another 
char* mystrdup (const char *s);
USER_DATA* READ_USER_PASS_FUNC(int check_input,char** file_data , int* ROW);

int main(
    int argc,
    char** argv
){
/* read from text file and creat username and password array */
    USER_DATA* test = NULL;
    int ROW=0;
    char** file_data = malloc((argc+1) * sizeof *file_data);
    int check_input = argc;
    for(int i = 0; i < argc; ++i)
    {
        size_t length = strlen(argv[i])+1;
        file_data[i] = malloc(length);
        memcpy(file_data[i], argv[i], length);
    }
    file_data[argc] = NULL;
    test = READ_USER_PASS_FUNC(check_input, file_data[1] , &ROW);

    for(int i = 0; i < argc; ++i) // free file_data[i]
    {
        free(file_data[i]);
    }
    free(file_data);

    int server_port = 5001;            // define server port
    int server_socket;
    int new_sock_fd;
    int server_addr_len;
    int client_no=0;
    int thread_value;
    int client_port;

    struct sockaddr_in server_addr;    // create information (domain , ip , port) for server
    struct sockaddr_in cli_addr[10];    // create information for clients
    struct client_information client_data;

    // struct checking_pass username_passwords;
    pthread_t client_thread[10];
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    
    server_socket = socket(AF_INET , SOCK_STREAM , 0); // define domain() , type(TCP) , protocol for server
	if( server_socket == 0){ 
		perror("socket failed"); 
		exit(EXIT_FAILURE);
	}

    server_addr.sin_family = AF_INET;           // domain
	server_addr.sin_addr.s_addr = INADDR_ANY;   // localhost IP 
	server_addr.sin_port = htons( server_port );       // server port 
	server_addr_len = sizeof(server_addr); 

    if ( bind(server_socket , (const struct sockaddr *)&server_addr, server_addr_len ) < 0 ){ // bind server with defined addresses
		perror("bind failed"); 
		exit(EXIT_FAILURE);
	}

	printf("Listener on port %d \n", server_port);

	if ( listen(server_socket, 3) < 0 ){    // Is the maximum length that the queue of pending connections may grow to. If this value is 3
		perror("listen"); 
		exit(EXIT_FAILURE); 
	}

    while (1){
        new_sock_fd = accept(server_socket, (struct sockaddr *)&cli_addr[client_no] , (socklen_t*)&server_addr_len ); // server accept new client
		if( new_sock_fd < 0 ) {
			perror ("failed");
			exit(EXIT_FAILURE);
		}
        for (int k = 0; k < 10; k++)
        {
            if (CLIENT_BROADCAST[k]==0){
                client_no = k;
                break;
            }   
        }
        client_port = ntohs(cli_addr[client_no].sin_port);
        printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_sock_fd , inet_ntoa(cli_addr[client_no].sin_addr) ,  client_port); 

        client_data.new_socket = new_sock_fd;  // declare client data in client_data structure
        client_data.client_number = client_no;
        client_data.PORT_client = client_port;
        client_data.lock = &mutex;
        client_data.user_pass_data = test;
        client_data.user_pass_counter = ROW;

        pthread_mutex_lock(&mutex);  // lock all threads during creat new client
        thread_value =pthread_create(&client_thread[client_no], NULL, &CLIENT_FUNC, (void *)&client_data);  // creat thread for each client
        if(thread_value != 0){
			perror("pthread_create() error");
        	exit(EXIT_FAILURE);	
		}
        pthread_mutex_unlock(&mutex); // unlock all threads
    }
        for(int i = 0; i < ROW; ++i)
    {
        free(test[i].col1);
        free(test[i].col2);
    }
    free(test);
}


void* CLIENT_FUNC(
    struct client_information *client_data
){
    int my_data =client_data->new_socket;
    int my_number =client_data->client_number;
    int my_port = client_data-> PORT_client;
    int check_user_counter = client_data->user_pass_counter;
    USER_DATA *user_pass = client_data->user_pass_data; 
	int n;
	char buffer[256];
    CLIENT_BROADCAST[my_number]=1;

    n = recv( my_data , buffer , sizeof(buffer) ,0);
    buffer[n-1]='\0';

    for (size_t i = 0; i < check_user_counter; i++)
    {
        if(strcmp(buffer,user_pass[i].col1)==0){
            n = recv( my_data , buffer , sizeof(buffer) ,0);
            buffer[n-1]='\0';
            if (strcmp(buffer,user_pass[i].col2)==0){
                break;
            }
        }
        else if (i != check_user_counter-1){
            continue;
        }
        CLIENT_BROADCAST[my_number]=0;
        printf("disconnecting : %d\n",my_port);
        close( my_data );
        pthread_exit(NULL);
    }
    CLIENT_DATA_BROADCST[my_number]=my_data;
    while (1)
	{
   		n = recv( my_data , buffer , sizeof(buffer) ,0); // recive data from client
        buffer[n]='\0';  // The first byte after the last character resets to zero
		if (n == 0){ 
			CLIENT_BROADCAST[my_number]=0;
            printf("disconnecting : %d\n",my_port);
            close( my_data );
			break;
		}
   		else if (n < 0){
      		perror("ERROR reading from socket");
      		exit(1);
   		}
        pthread_mutex_lock(client_data->lock);          // lock all threads during broadcasting message
        BROADCAST_FUNC(buffer , my_number ,my_data);
        pthread_mutex_unlock(client_data->lock);        // unlock threads

    }
    pthread_exit(NULL);
}

void* BROADCAST_FUNC(
    char* copy_buffer ,
    int speaker ,
    int my_data
){

	char buf[256];
    
	for (int j = 0; j < 10; j++){
		if (j == speaker){
            continue;
        }    
        if (CLIENT_BROADCAST[j]==1){
            snprintf(buf, 12, "client %d:", speaker);
			strcat (buf,copy_buffer);
			write(CLIENT_DATA_BROADCST[j],buf,sizeof(buf));
		}	
	}	
}

char* mystrdup (const char *s)
{
    if (!s)     /* validate s not NULL */
        return NULL;

    size_t len = strlen (s);            /* get length */
    char *sdup = malloc (len + 1);      /* allocate length + 1 */

    if (!sdup)          /* validate */
        return NULL;

    return memcpy (sdup, s, len + 1);   /* pointer to copied string */ 
}

USER_DATA* READ_USER_PASS_FUNC(int check_input, char** file_data , int* ROW){

    USER_DATA *data = NULL;
    size_t arrsz = 2;
    size_t line = 0;
    size_t row = 0;
    int maximum = 256;
    char buf[maximum];
    FILE *fp;
    if (check_input>1){
        fp = fopen (file_data, "r");
    }
    if (!fp) {  /* validate file open for reading */
        perror ("file open failed");
        exit(1);;
    }

    /* allocate an 'arrsz' initial number of struct */
    if (!(data = malloc (arrsz * (sizeof* data)))) {
        perror ("malloc-data");
        exit(1);
    }

    while (fgets (buf, maximum, fp)) {         /* read each line from file */
        char c1[maximum], c2[maximum];
        size_t len = strlen (buf);          /* length for validation */

        line++;     /* increment line count */

        /* validate line fit in buffer */
        if (len && buf[len-1] != '\n' && len == maximum - 1) {
            fprintf (stderr, "error: line %zu exceeds maximum chars.\n", line);
            exit(1);;
        }

        if (row == arrsz) { /* check if all pointers used */
            arrsz ++;
            data = realloc (data,arrsz*(sizeof *data));
                 /* update arrsz to reflect new allocation */
        }

        /* buf declare into var */
        if (sscanf (buf, "%254s %254s",c1, c2) != 2) {
            fprintf (stderr, "error: invalid format line %zu\n", line);
            continue;   /* get next line */
        }

        /* allocate copy strings, assign allocated blocks to pointers */
        if (!(data[row].col1 = mystrdup (c1))) { 
            fprintf (stderr, "error: malloc-c1 line %zu\n", line);
            break;   
        }
        if (!(data[row].col2 = mystrdup (c2))) { 
            fprintf (stderr, "error: malloc-c1 line %zu\n", line);
            break; 
        }
        row++;     

    }
    fclose (fp);

    *ROW = row;
    //for (size_t i = 0; i < row; i++){
    //    printf ("%-4s %-10s\n", data[i].col1, data[i].col2);
    //}
    return data;
}






