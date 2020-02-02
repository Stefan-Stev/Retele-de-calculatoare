#include<unistd.h>
#include<stdio.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<fcntl.h>
#include<pwd.h>
#include<dirent.h>
#include<sys/stat.h>
#include <stdbool.h>
#include <time.h>
#include <sys/socket.h>
int ok=0;
int  validare(char *command)
{
	//printf("%s\n",command);
	int fd=open("login.txt",O_RDONLY);
	char ch='0';
	char anterior=' ';
	int pozitie;
	printf("Salut\n");
	while(read(fd,&ch,1)!=0)
	{
		pozitie=6;
          if(anterior==' ')
          {
          	//printf("%c",command[pozitie]);
          	while(ch==command[pozitie])
          	{
          		
          		pozitie++;
          		if(pozitie==strlen(command))
          		{
          			return 1;
          		}
          		read(fd,&ch,1);
          		anterior=ch;
          	}
          	pozitie=6;
          }
          anterior=ch;
	}
	return 0;

}
int isdirectory(char *fisier)
{
	DIR *dir;
	struct dirent *director;
    struct stat st;
   

    if (stat(fisier, &st)!=0)
    {
        printf( "Eroare la stat pentru %s :", fisier);
        perror("Cauza este");
        return -1;
    }

    switch (st.st_mode & S_IFMT)
    {
        case S_IFDIR: return 1;
        default :return 0;
    }
   
}
bool gasit(char *path,char *file_name)
{
	int lungime1=strlen(path)-1;
	int lungime2=strlen(file_name)-1;
    while(path[lungime1]!='/')//daca am atins caracterul '/' inseamna ca am dat de fisierul gasit ,numele sunt identice,cel al fisierului cautat si coada lui path
    {
    	if(path[lungime1]!=file_name[lungime2])
    		return false;
    	lungime1--;
    	lungime2--;
    }
    return true;
}
void gasire(char *path, char  *file_name)
{
    DIR *dir;
    struct dirent *director;
    struct stat st;
    char cale[10000];
    if(-1==mkfifo("fifotema",0600))
	{
		if(errno!=EEXIST)
		{
			perror("Eroare la crearea canalului 'fifo'.cauza erorii ");
			exit(5);
		}
	}
	int fd_out=open("fifotema",O_WRONLY);
   if(gasit(path,file_name)==1) //l-am gasit
	{
		struct stat st;
		if(stat(path,&st)!=0)
		{
			printf("Eroare la stat pentru %s .\n",path);
			perror("Cauza este");
			exit(10);
		}
         char verdict[1024]={'0'};
         strcpy(verdict,"Fisier gasit,cale:");
         char temporar[1024];
         strcat(verdict,path);
        // printf("%s\n",verdict);
         //trimit afisez calea
         write(fd_out,verdict,1024);
         memset(verdict,0,sizeof verdict);
         sprintf(verdict,"Dimensiunea fisierului este :%ld",st.st_size);
         //printf("%s\n",verdict);
          //strcpy(verdict," Dimensiunea fisierului este:%d",st.st_size);
         //trimit dimensiunea in bits
          write(fd_out,verdict,1024);
          memset(verdict,0,sizeof verdict);
          strcpy(verdict,"Tipul acestui fisier este");
          //tipul fisierului
          switch(st.st_mode & S_IFMT)
          {
          	case S_IFDIR:strcat(verdict," Director");write(fd_out,verdict,1024);break;
          	case S_IFREG:strcat(verdict," Regulat");write(fd_out,verdict,1024);break;
          	case S_IFIFO:strcat(verdict," FIFO");write(fd_out,verdict,1024);break;
          	default:strcat(verdict," Nestiut");write(fd_out,verdict,1024);break;


          }
           //printf("%s\n",verdict);
          memset(verdict,0,sizeof verdict);
           //strcpy(verdict,"Ultima schimbare: %s",ctime(&st.st_ctime));
          sprintf(verdict,"Ultima schimbare: %s",ctime(&st.st_ctime));
           write(fd_out,verdict,1024);
            memset(verdict,0,sizeof verdict);
           //strcpy(verdict,"Ultima accesare:%s",ctime(&st.st_atime));
           sprintf(verdict,"Ultima accesare:%s",ctime(&st.st_atime));
           write(fd_out,verdict,1024);
              memset(verdict,0,sizeof verdict);
           sprintf(verdict,"Ultima modificare: %s",ctime(&st.st_mtime));
           write(fd_out,verdict,1024);
          close(fd_out);
           exit(0);
       }
    //is_folder = isdirectory(path);

    if (isdirectory(path)==1)
    {
        if ((dir = opendir(path)) == NULL)
      {
        return ;
        }

    while ((director = readdir(dir)) != NULL)
	{
		if (strcmp(director->d_name, ".") && strcmp(director->d_name,"..") && (strlen(director->d_name) > 0 && director->d_name[0] != '.' || strlen(director->d_name) == 0))//excludem neclaritatile de intrari implicite
		{
			sprintf(cale,"%s/%s",path,director->d_name);
			gasire(cale, file_name);
		}
	}
    }
   else
   	return;
}
/*void status(char *file_name)
{
	struct stat st;
	struct passwd *pwd;
	char perm[10]="---------";
	
	if(0 != stat(file_name,&st) )
    {
        fprintf(stderr,"Eroare la stat pentru %s .\t", cale);  perror("Cauza este");
        return -1;   /* aici nu terminam executia cu exit(), ci revenim in apelant pentru a continua cu ceea ce a mai ramas de procesat 
    }
    char verdict[1000];
    strcpy(verdict,"Fisierul este de tipul:");
     switch(st.st_mode & S_IFMT)
    {
        case S_IFDIR : strcat(verdict,"")
        case S_IFREG : printf("Fisier obisnuit\n"); break;
        case S_IFLNK : printf("Link\n"); break;
        case S_IFIFO : printf("FIFO\n"); break;
        case S_IFSOCK: printf("Socket\n"); break;
        case S_IFBLK : printf("Block device\n"); break;
        case S_IFCHR : printf("Character device\n"); break;
        default: printf("Unknown file type");
    }
}*/
int main()
{
	pid_t pid;
	char command[100];
	int x;

	while(1)
	{
       printf("Introduceti numele de utilzator\n");
		scanf("%s",command);
		if(strncmp(command,"login",5)==0)
			{
				int p[2];
				if(-1==pipe(p))
				{
					perror("Eroare la crearea canaluui anonim.Cauza erorii");
					exit(2);
				}
				if(-1==(pid=fork()))
					{perror("Eroare la fork");exit(1);}
                 if(pid==0)
                 {
                 	if(validare(command)==1)
                 	{
                 		printf("Autentificare reusita\n");
                        char * mesaj="7:succces";
                        write(p[1],mesaj,strlen(mesaj));
                         exit(x);
                 	}
                 	else{
                 		do{
                 			
                 			printf("Incercati din nou\n");
                 			scanf("%s",command);
                 		}while(validare(command)==0);
                 		printf("Autentificare reusita\n");
                 	}
                 }
               else
               {
               	wait(&x);
               	char ch='0';
               	read(p[0],&ch,1);
               	if(ch=='7')
               		printf("Succes!\n");
               	label:printf("Adaugati o comanda:myfind,mystat sau quit\n");
                  scanf("%s",command);
                
                  if(strncmp(command,"myfind",6)==0)
                  {
                       if(-1==(pid=fork()))
					{perror("Eroare la fork");exit(3);}
				 if(pid==0)
				 {
                     char *nume_fisier=command+7;
                     gasire("/home",nume_fisier);
                     int x;
                     //exit(x);
				 }
				 else
				 {
				 	//wait(&x);
                    int fd_in=open("fifotema",O_RDONLY);
                    char *buff;
                    read(fd_in,buff,1024);
                    printf("%s\n",buff);
                    memset(buff,0,sizeof buff);
                    printf("\n");
                    read(fd_in,buff,1024);
                    printf("%s\n",buff);
                     memset(buff,0,sizeof buff);
                    printf("\n");
                    read(fd_in,buff,1024);
                    printf("%s\n",buff);
                     memset(buff,0,sizeof buff);
                    printf("\n");
                     read(fd_in,buff,1024);
                    printf("%s\n",buff);
                     memset(buff,0,sizeof buff);
                     read(fd_in,buff,1024);
                    printf("%s\n",buff);
                    memset(buff,0,sizeof buff);
                     read(fd_in,buff,1024);
                    printf("%s\n",buff);
                     memset(buff,0,sizeof buff);
                     goto label;
                    close(fd_in);
                    wait(NULL);//Astept fiul pentru a afisa toate informatiile si tatal sa nu plece din nou la sectiunea label
                    printf("Am afisat toate\n");
                    //wait(NULL);
                    goto label;//ma duc la sectiunea label pentru a cere noi functii:myfind,mystat,quit
				 }
				}
				//scanf("%s",command);
				if(strncmp(command,"mystat",6)==0)
				{
					int sockpair[2];
					if(socketpair(AF_UNIX,SOCK_STREAM,0,sockpair)<0)
	               {
		perror("Eroare in cadrul socketului");
		exit(13);
		
	                 }
					if(-1==(pid=fork()))
					{perror("Eroare la fork");exit(11);}
				     if(pid==0)
				     {
				     	close(sockpair[0]);
				     	char *file_name=command+7;
				     	struct stat st;
				     	struct passwd *pwd;
				     	if(0!=stat(file_name,&st))
				     	{
				     		printf("eroare la stat ");
				     		perror("Cauza erorii este");
				     		return -1;
				     	}
				     	char verdict[1024]={'0'};
				     	strcpy(verdict,"Fisierul este de tip: ");
				     	switch(st.st_mode & S_IFMT)
				     	{
				     		case S_IFDIR:strcat(verdict,"Director");break;
				     		case S_IFREG:strcat(verdict,"Fisier obisnuit");break;
				     		case S_IFLNK:strcat(verdict,"Link");break;
				     		case S_IFIFO:strcat(verdict,"FIFO");break;
				     		case S_IFSOCK:strcat(verdict,"Socket");break;
				     		case S_IFBLK:strcat(verdict,"Block device");break;
				     		case S_IFCHR:strcat(verdict,"Character device");break;
				     		default:strcat(verdict,"Unknown file");break;

				     	}
				     	write(sockpair[1],verdict,1024);
				     	memset(verdict,0,1024);
				     	char perm[10]="---------";
				     	if(S_IRUSR & st.st_mode) perm[0]='r';
                        if(S_IWUSR & st.st_mode) perm[1]='w';
                        if(S_IXUSR & st.st_mode) perm[2]='x';
                        if(S_IRGRP & st.st_mode) perm[3]='r';
                        if(S_IWGRP & st.st_mode) perm[4]='w';
                        if(S_IXGRP & st.st_mode) perm[5]='x';
                        if(S_IROTH & st.st_mode) perm[6]='r';
                        if(S_IWOTH & st.st_mode) perm[7]='w';
                        if(S_IXOTH & st.st_mode) perm[8]='x';
                        strcpy(verdict,perm);
                        write(sockpair[1],verdict,1024);
                        memset(verdict,0,1024);
                        if((pwd=getpwuid(st.st_uid))!=NULL)
                        {
                        	sprintf(verdict,"Proprietarul acestuia are numele:%s cu UID : %ld",pwd->pw_name,(long)st.st_uid);
                        	write(sockpair[1],verdict,1024);
                        }
                        else
                        {
                        	sprintf(verdict,"Proprietarul acestuia nu poate fi gasit dar are UID:%ld\n",(long)st.st_uid);
                        	write(sockpair[1],verdict,1024);

                        }
                        close(sockpair[1]);
                         exit(10);
				     }
				     else
				     {
				     	close(sockpair[1]);
				     	char buffer[1024]={0};
				     	read(sockpair[0],buffer,1024);
				     	printf("%s\n",buffer);
				     	read(sockpair[0],buffer,1024);
				     	printf("%s\n",buffer);
				     	read(sockpair[0],buffer,1024);
				     	printf("%s\n",buffer);
				     	wait(NULL);
				     	close(sockpair[0]);
				     	goto label;
				     }

				}
				if(strcmp(command,"quit")==0)
				{
					int p[2];
					//printf("Am ajuns aici");
					if(-1 == pipe(p) )
		              {
			printf( "Eroare: nu pot crea al doilea canal anonim, errno=%d\n",errno);  perror("Cauza erorii");  exit(15);
		              }
		              pid_t pid;
		              if(-1==(pid=fork()))
		              {   perror("Eroare in fork.Cauza este:");
		                  return -1;
		              }
		              	if(pid==0)
		              	{
		              		char *mesaj="12:Sunt delogat";
		              		write(p[1],mesaj,strlen(mesaj));
		              		close(p[0]);
		              		close(p[1]);
		              		int x=90;
		              		exit(x);
		              	}
		              	else
		              	{
		              		int x=90;
		              		wait(&x);//trebuie sa-mi astept fiul pentru a nu intra in while(1) din nou
		              		close(p[1]);
		              		char buffer[50]={0};
		              		read(p[0],buffer,1024);
		              		if(buffer[0]=='1' && buffer[1]=='2')
		              		{
		              			printf("%s\n",buffer+3);
		              			//ok=1;
		              			return 0;
		              		}
		              		else
		              			goto label;		              	}
		              

				
			}
				
               }
              
             }
            
	}
	
	return 0;
}
