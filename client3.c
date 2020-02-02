#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdbool.h>
#include <time.h>
int ok=0;
int main()
{
int socket_desc, val;
struct sockaddr_in client_addr;
char buffer[256];
socket_desc = socket(AF_INET, SOCK_STREAM, 0);
if(socket_desc==-1)
{
    perror("Eroare la scoket().\n");
}

printf("Enter the port number\n");
int port;
scanf("%d", &port);
client_addr.sin_family = AF_INET;
client_addr.sin_addr.s_addr =  INADDR_ANY; 
client_addr.sin_port = htons(port);

if(connect(socket_desc, (struct sockaddr*)&client_addr, sizeof(client_addr)) == 0)
	printf("CONNECT STATE: Connected to Server on port %d\n", port);
else
	printf("Connection to server failed !\n");
fflush(stdout);
/*while(1) 
{
	
	printf("Message to server: ");
	bzero(buffer,256);
	scanf("%s", buffer);
	
	write(socket_desc,buffer,strlen(buffer));
	bzero(buffer,256);
	
	read(socket_desc,buffer,255);
	printf("Message from server: %s\n",buffer);
	
}*/
     pid_t pid;
     int p[2];
    if (pipe(p) < 0) 
        {
        	perror("Eroare la pipe");
        }
     if(-1==(pid=fork()))
     {
     	perror("Eroare la fork");
     	exit(1);
     }
     if(pid==0)
     {
     	//fiu 
     	int read_size=0;
        char verdict[1024]={'0'};
        char buffer_temporar[1024]={'0'};
     	while((read_size=read(socket_desc,buffer,1024))>0)
     	{
           
            if(strncmp(buffer,"Accident",8)==0)
            {

                strcpy(buffer_temporar,buffer);
                ok=1;
                //write(p[1],&ok,4);
            }
            else{
            //in caz de accident, afisez accidentul
            strcat(buffer,"\n");
            strcat(buffer,buffer_temporar);
            buffer_temporar[0]='\0';

           //afisez informatiile primite de la server
     		printf("%s\n",buffer);
            fflush(stdout);
          memset(verdict,0,1024);
     		ok=1;
     		write(p[1],&ok,4); 

        }
            

             
     	}
     	if(read_size<=0)
     	{
              perror("Eroare la citire");
     	}


     } 
     else if(pid>0)
     {
     	fflush(stdout);
     	//tatal 
     	char message[100];
     	int ok2;
   //-----tastez viteza---
        label:
     	read(p[0],&ok2,4); //pentru ca procesul tata sa nu mearga mai departe necontrolat
     	fflush(stdout);
     	int viteza;
     	printf("Tastati viteza:\n");
     	scanf("%d",&viteza);
     	sprintf(message,"Viteza:%d",viteza);
     	write(socket_desc,message,1024);
//------------
     	//------tastez adresa-----
     	char adresa[200];
     	printf("Tastati adresa:\n");
     	scanf("%s",adresa);
     	bzero(message,0);
     	sprintf(message,"Adresa:%s",adresa);
     	write(socket_desc,message,1024);
     	//----
     	//------cer informatii cotidiene-----
        read(p[0],&ok2,4);
     	printf("Aveti abonament?\n");
     	char raspuns[10];
     	scanf("%s",raspuns);
     	if(strcmp(raspuns,"Da")==0)
     	{
            clock_t start ,end;
            double cpu_time_used;
            start=clock();
            do{
             
                                 
             printf("Ce doriti?\n");
             char raspuns[100];
             scanf("%s",raspuns);

             //----ma deconectez daca clientul vrea asta
             if(strcmp(raspuns,"Logout")==0)
                {    printf("V-ati deconectat!\n");

                    return 0;close(socket_desc);}
                    //--------


             write(socket_desc,raspuns,200);
             read(p[0],&ok2,4);
             end=clock();
             double cpu_time_used=((double)(end-start))/CLOCKS_PER_SEC*10000;
             printf("%f\n",cpu_time_used);
              if(cpu_time_used>20.0)
                {printf("Am trecut de limita de timp\n");
                  break;
                }
              }while(1);
              printf("Sesiunea a expirat. Retrimiteti datele dvs!\n");
              sleep(3);
              char raspuns[100];
              strcpy(raspuns,"Timp expirat");
              write(socket_desc,raspuns,200);
              goto label;

     	}
        else
        {
            printf("Client fara abonament. Nu puteti accesa alte informatii!\n");
           char raspuns[100];
           strcpy(raspuns,"Fara abonament");
           write(socket_desc,raspuns,200);
           sleep(20);

           goto label;
        }


     }

	close(socket_desc);
	return 0;	
}