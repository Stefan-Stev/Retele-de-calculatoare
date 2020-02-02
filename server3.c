 
#include<stdio.h>
#include<string.h>   
#include<stdlib.h>   
#include<sys/socket.h>
#include<arpa/inet.h> 
#include<unistd.h>   
#include<pthread.h> 
#include<sqlite3.h>
#include<string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <fcntl.h>
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
 int clientCount=0;
  pthread_t thread_id[100];

char data[1024];
static int callback (void *str, int argc, char **argv, char **azColName)
{
    int i;
   char*  data = (char*) str;

    for (i = 0; i < argc; i++)
    {
        strcat(data,argv[i]);
        strcat(data,"  ");
    }
    //printf("%s\n",data);
    strcat (data, "\n");
    return 0;
}

 struct client
 {
    int index;
    int sockID;
    struct sockaddr_in clientAddr;
    int len;
 };
 struct client Client[100];
//the thread function
void *tratare(void *);
 
int main()
{
    int socket_desc , client_sock , c;
    struct sockaddr_in server , client;
     
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Nu am putut crea socketul");
    }
    puts("Socket creat");
    int  on=1;
    setsockopt(socket_desc,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
    bzero (&server, sizeof (server));
    bzero (&client, sizeof (client));
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("Bind failed. Error");
        return 1;
    }
    puts("Bind done");
     
    //Listen
    if(listen(socket_desc , 6)==-1)
    {
    	perror("Eroare la listen");

    }
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
     
     
    
    
   /* while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Connection accepted");
         
        if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &client_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
         
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( thread_id , NULL);
        puts("Handler assigned");
    }*/
     while(1)
     {
    
        Client[clientCount].sockID = accept(socket_desc, (struct sockaddr*) &Client[clientCount].clientAddr, &Client[clientCount].len);
        Client[clientCount].index = clientCount;

        pthread_create(&thread_id[clientCount], NULL, tratare, (void *) &Client[clientCount]);

      
         
     }
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
    for(int i = 0 ; i < clientCount ; i ++)
        pthread_join(thread_id[i],NULL);
     
    return 0;
}
 
/*
 * This will handle connection for each client
 * */
void * tratare(void * ClientDetail1)
{
	clientCount ++;
    struct client* Client=ClientDetail1;
    int index=Client->index;
    int clientSocket=Client->sockID;

    int read_size;
    char  client_message[2000];
     
     sqlite3* DB;
     int baza_date=0;
     
     //deschiderea bazei de date
     baza_date=sqlite3_open("example.db",&DB);
     if(baza_date)
     {
        fprintf(stderr, "error: %s\n", sqlite3_errmsg (DB));
        exit(10);
     }
     else
        printf("baza de date s-a deschis cu succes\n");

   label:
  //am cerut date despre viteza si locatia clientului
    strcpy(client_message,"SAlutari! Bine ai venit la serverul de monitorizare a traficului!\n");
    strcat(client_message,"Unde te afli si cu ce viteza mergi?\n");
    write(clientSocket,client_message,200);
//--------
   //primim viteza si adresa
    printf("Primesc informatii\n");
    fflush(stdout);
    int viteza;
    memset(client_message,0,1024);
    read(clientSocket,client_message,1024);
    viteza=atoi(client_message+7);

     char adresa[100];
     read(clientSocket,&adresa,1024);
     strcpy(adresa,adresa+7);
     printf("%d\n",viteza);
     fflush(stdout);
     printf("%s\n",adresa);
    //---
    
     fflush(stdout);
    char raspuns[1024];
    char str[1024];
    strcpy(str,"CALLBACK FUNCTION");
    sprintf(raspuns,"SELECT VITEZA FROM DRUM WHERE STRADA='%s';",adresa);
    int rc=sqlite3_exec (DB, raspuns, callback,(void*) str, NULL);
   if (rc != SQLITE_OK) 
        printf("Eroare la deschiderea bazei de date\n"); 
    else { 
        printf("Baza de date a fost accesata cu succes\n");
    }
   // printf("%s\n",str+17);
    strcpy(str,str+17);  //preiau viteza din baza de date
    int viteza_legala=atoi(str);
   printf("%d\n",viteza_legala);
   if(viteza_legala<viteza)
   {
      
      char atentionare[1024];
      strcpy(atentionare,"Limita depasita\n");
      write(clientSocket,atentionare,200);
  
   }
   else
   {
   
    char atentionare[1024];
      strcpy(atentionare,"Mergeti regulamentar\n");
      write(clientSocket,atentionare,200);
      
   }
     fflush(stdout);
   //astept cereri de la client
    while( (read_size = recv(clientSocket , client_message , 2000 , 0)) > 0 )
    {
        printf("AM primit comanda:%s\n",client_message);
          if(strcmp(client_message,"Meteo")==0)
          {
            printf("Aici1\n");
              memset(str,0,1024);
               sprintf(raspuns,"SELECT VREME FROM METEO WHERE STRADAL='%s';",adresa);
              int rc=sqlite3_exec (DB, raspuns, callback,(void*) str, NULL);
              if (rc != SQLITE_OK) 
               printf("Eroare la deschiderea bazei de date\n"); 
              else { 
                 printf("Baza de date a fost accesata cu succes\n");
                   }
              printf("ss1\n");
              printf("%s\n",str);
              write(clientSocket,str,500);

          }
          else if(strcmp(client_message,"Benzinarie")==0)
          {
            memset(str,0,1024);
            printf("Aici2\n");
            sprintf(raspuns,"SELECT * FROM BENZINARIE;");
            int rc=sqlite3_exec (DB, raspuns, callback,(void*) str, NULL);
              if (rc != SQLITE_OK) 
               printf("Eroare la deschiderea bazei de date\n"); 
              else { 
                 printf("Baza de date a fost accesata cu succes\n");
                   }
                   printf("ss2\n");
                    printf("%s\n",str);
                   write(clientSocket,str,500);

          }

          else if(strncmp(client_message,"Stare",5)==0)
          {
            memset(str,0,1024);
            printf("Aici3\n");
            char adresa2[1024]={'0'};
            strcpy(adresa2,client_message+6);//aflu starea adresei dorite de client
            printf("%s\n",adresa2);
            sprintf(raspuns,"SELECT STARE FROM DRUM WHERE STRADA='%s';",adresa2);
            int rc=sqlite3_exec (DB, raspuns, callback,(void*) str, NULL);
              if (rc != SQLITE_OK) 
               printf("Eroare la deschiderea bazei de date\n"); 
              else { 
                 printf("Baza de date a fost accesata cu succes\n");
                   }
                  printf("ss3\n");
                    printf("%s\n",str);
                   write(clientSocket,str,500); 

          }
          else if(strncmp(client_message,"Avertizare",10)==0)
          {
            memset(str,0,1024);
            printf("Aici4\n");
            char adresa2[1024];
            strcpy(adresa2,client_message+11);//aflu starea adresei dorite de client
            printf("%s\n",adresa2);
            sprintf(raspuns," UPDATE DRUM SET STARE='Blocaj in trafic' WHERE STRADA='%s';",adresa2);
            int rc=sqlite3_exec (DB, raspuns, callback,(void*) str, NULL);
            if (rc != SQLITE_OK) 
               printf("Eroare la deschiderea bazei de date\n"); 
              else { 
                 printf("Baza de date a fost accesata cu succes\n");
                   }
                  printf("ss4\n");

              strcpy(client_message,"Ati trimis avertizarea cu succes!");
              write(clientSocket,client_message,500);
            char mesaj[1024];
            sprintf(mesaj,"Accident pe :%s\n",adresa2);
            for(int i=0;i<clientCount;i++)
            {
            if(Client[i].index!=index){
              int size= write(Client[i].sockID,mesaj,1024);
              if(size<0)
              {
              	perror("Eroare");
              	exit(12);
              }
              else
              {
              	printf("Cu succes!\n");
              }
               }
            }
            printf("Am trimis la toata lumea avertismentul\n");
            
          }
          else if(strncmp(client_message,"Evenimente",10)==0)
          {
                     char sport[1024];
                     strcpy(sport,client_message+11);
                     memset(str,0,1024);
                     printf("Aici5\n");
                     printf("%s\n",sport);
                     sprintf(raspuns,"SELECT * FROM EVENIMENTE WHERE SPORT='%s' ;",sport );
                     printf("%s\n",raspuns);
                     int rc=sqlite3_exec (DB, raspuns, callback,(void*) str, NULL);
              if (rc != SQLITE_OK) 
               printf("Eroare la deschiderea bazei de date\n"); 
              else { 
                 printf("Baza de date a fost accesata cu succes\n");
                   }
                  printf("ss5\n");
                    printf("%s\n",str);
                   write(clientSocket,str,500); 

          }
          else if(strcmp(client_message,"Fara abonament")==0)
          {
                 sleep(6);
                 goto label;
          }
          else if(strcmp(client_message,"Timp expirat")==0)
          {
                 goto label;
          }
          else if(strcmp(client_message,"Actualizare")==0)
          {
          	     char mesaj[200];
          	     strcpy(mesaj,"Actualizat!");
          	     write(clientSocket,mesaj,500);
          }
          else 
          {
          	strcpy(raspuns,"Ati gresit comanda!");
          	write(clientSocket,raspuns,500);
          }
          

        
       
    }
     
    if(read_size == 0)
    {
        puts("Client deconectat");
        fflush(stdout);
        close(clientSocket);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
         
    return 0;
} 