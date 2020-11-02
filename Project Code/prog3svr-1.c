#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 4096


typedef struct
{
    struct sockaddr_in addr; 
    int connfd;              
    int uid;                
    char name[64];          
} structureClient;

static _Atomic unsigned int NoofClients = 0;
static int uid = 10;


structureClient *clients[MAX_CLIENTS];

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;



void AddToQueue(structureClient *cl)
{
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) 
    {
        if (!clients[i])
	 {
            clients[i] = cl;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void DeleteQueue(int uid)
{
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) 
    {
        if (clients[i]) {
            if (clients[i]->uid == uid) 
	    {
                clients[i] = NULL;
                break;
            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}



void Text_all(char *s)
{
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i <MAX_CLIENTS; ++i)
     {
        if (clients[i]) {
            if (write(clients[i]->connfd, s, strlen(s)) < 0) {
                perror("Write to descriptor failed");
                break;
            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}


void Text_self(const char *s, int connfd)
{
    if (write(connfd, s, strlen(s)) < 0) 
     {
        perror("Write to descriptor failed");
        exit(-1);
    }
}

void Textclient(char *s, char *name)
{
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i){
        if (clients[i]) 
	{ 
	
            if (strncmp(clients[i]->name,name,strlen(name)) == 0) 
	    {
		
                if (write(clients[i]->connfd, s, strlen(s))<0)
		 {
                    perror("Write to descriptor failed");
                    break;
                }
            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}


void DisplayClients(int connfd)
{
    char s[64];

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        if (clients[i])
	 {
	    Text_self("FD		USERNAME\n", connfd);
	    Text_self("---------------------------------------------------------\n", connfd);
            sprintf(s, "%d 		%s\r\n",clients[i]->connfd,clients[i]->name);
            Text_self(s, connfd);
	    Text_self("---------------------------------------------------------\n", connfd);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
   }


void newline(char *s)
{
    while (*s != '\0') {
        if (*s == '\r' || *s == '\n') {
            *s = '\0';
        }
        s++;
    }
}

void Address(struct sockaddr_in addr)
{
    printf("%d.%d.%d.%d",
        addr.sin_addr.s_addr & 0xff,
        (addr.sin_addr.s_addr & 0xff00) >> 8,
        (addr.sin_addr.s_addr & 0xff0000) >> 16,
        (addr.sin_addr.s_addr & 0xff000000) >> 24);
}

void *clientHandler(void *arg)
{
    char buff_out[BUFFER_SIZE];
    char buff_in[BUFFER_SIZE / 2];
    int rlen;

    NoofClients++;
    structureClient *cli = (structureClient *)arg;

     printf(" Client (1):Connection accepted \n");
      printf(" Client (1):Connection Handler Assigned \n");
  
    sprintf(buff_out, "JOIN %s Request Accepted\r\n", cli->name);
    Text_all(buff_out);
    
    /* Receive input from client */
    while ((rlen = read(cli->connfd, buff_in, sizeof(buff_in) - 1)) > 0) 
    {
        buff_in[rlen] = '\0';
        buff_out[0] = '\0';
        newline(buff_in);

        /* Ignore empty buffer */
        if (!strlen(buff_in))
        {
            continue;
        }

            char *command, *param;
            command = strtok(buff_in," ");
            if (!strcmp(command, "QUIT")) 
	    {
                break;
            } 
	    else if (!strcmp(command, "BCST")) 
	    {
                param = strtok(NULL, " ");
                    if (param)
		    {
			
                        sprintf(buff_out, "[PM][%s]", cli->name);
                        while (param != NULL) 
			{
                            strcat(buff_out, " ");
                            strcat(buff_out, param);
                            param = strtok(NULL, " ");
                        }
                        strcat(buff_out, "\r\n");
                        Text_all(buff_out);
                    } 
		   else 
		    {
                        Text_self("<< message cannot be null\r\n", cli->connfd);
                    }

            }
	    else if (!strcmp(command,"MESG")) 
	    {
                char username[32];
                param = strtok(NULL, " ");
                if (param) 
		{
			
			memset(username,'\0',sizeof(username));
			strcpy(username,param);
			
                    int uid = atoi(param);
                    param = strtok(NULL, " ");
                    if (param)
		    {
			
                        sprintf(buff_out, "FROM [%s]", cli->name);
                        while (param != NULL) 
			{
                            strcat(buff_out, " ");
                            strcat(buff_out, param);
                            param = strtok(NULL, " ");
                        }
                        strcat(buff_out, "\r\n");
                        Textclient(buff_out,username);
                    } 
		   else 
		    {
                        Text_self(" message cannot be null\r\n", cli->connfd);
                    }

                } 
		else 
		{
                    Text_self("username cannot be null\r\n", cli->connfd);
                }

            } 
	   else if(!strcmp(command, "LIST")) 
	   {
		printf(" Client ( %d): LIST\n", cli->uid-9);
                sprintf(buff_out, "clients %d\r\n", NoofClients);
                Text_self(buff_out, cli->connfd);
                DisplayClients(cli->connfd);
            }
	    else 
	    {
		printf(" Client ( %d):unrecognizable messeage.Discarding UNKNOWN message \n", cli->uid-9);
                Text_self(" unrecognizable messeage\r\n", cli->connfd);
            }

    }

    close(cli->connfd);

   
    DeleteQueue(cli->uid);
    printf("Client %d : QUIT\n", cli->uid-9);
    printf(" Client ( %d): Disconnecting User \n", cli->uid-9);
    free(cli);
    NoofClients--;
    pthread_detach(pthread_self());

    return NULL;
}


int main(int argc, char *argv[])
{


    int listenfd = 0, connfd = 0,ret=0;
    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;
    pthread_t tid;
     char buff[50],buff1[50];
    if(argc!=2)
    {
	printf("Usage Error:PortNum Not entered\n");
	exit(0);
     }
    
   
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr =INADDR_ANY;
    serv_addr.sin_port = htons(atoi(argv[1]));

   
    signal(SIGPIPE, SIG_IGN);

  
    if (bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Socket binding failed");
        return EXIT_FAILURE;
    }

  
    if (listen(listenfd, 10) < 0) {
        perror("Socket listening failed");
        return EXIT_FAILURE;
    }

    printf("Waiting for Connection\n");

   
    while (1) 
{
        socklen_t clilen = sizeof(cli_addr);
        connfd = accept(listenfd, (struct sockaddr*)&cli_addr, &clilen);
	memset((void *)buff,'\0',sizeof(buff));
	if(ret=read(connfd, buff, sizeof(buff) - 1)>=0)
	{ 
           
	}
	
	if(strncmp(buff,"JOIN",4)==0)
        {
          int i=0,j=0;
		for(i=5;buff[i]!='\0';i++)
		{
			buff1[j++]=buff[i];
		}
		
		if ((NoofClients + 1) == MAX_CLIENTS) {
		    printf("<< Maximum clients reached\n");
		    printf("<< Reject ");
		    Address(cli_addr);
		    printf("\n");
		    close(connfd);
		    continue;
		}

		
		structureClient *cli = (structureClient *)malloc(sizeof(structureClient));
		cli->addr = cli_addr;
		cli->connfd = connfd;
		cli->uid = uid++;
     
		sprintf(cli->name, "%s", buff1);

		
		AddToQueue(cli);
		pthread_create(&tid, NULL, &clientHandler, (void*)cli);

		
		sleep(2);
	}
    }
   
    return EXIT_SUCCESS;
}
