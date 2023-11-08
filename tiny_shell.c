#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#define BUFF_SIZE 100

char * get_line(){
    char* linie=NULL;
    linie=(char*)malloc(sizeof(char)*BUFF_SIZE);
    fflush(STDIN_FILENO);
    read(STDIN_FILENO, linie,BUFF_SIZE);
    //linie[strlen(linie)]='\0';
    //printf("liinie : %c\n",linie[2]);
    return linie;

}

char** parse_line(char * linie){
    char **command=NULL;
  
    command=(char**)malloc(sizeof(char*)*BUFF_SIZE);
    if(command==NULL)
    {
        printf("Malloc error!\n");
        return -1;
    }
    const char delimiter[2]=" ";
    char* token=strtok(linie, delimiter);
    int k=0;
    
    while(token!= NULL){
        command[k]=(char*)malloc(sizeof(char)*10);
        //printf("%s\n", token);
        strcpy(command[k],token);
        k++;
        token=strtok(NULL,delimiter);
    }
    
    command[k-1][strlen(command[k-1])-1]='\0';
    //printf(" parse : %s\n", command[0]);   
   command[k]=NULL;
   return command;
}

void setEnv(char** command){
    
    const char delim[2]="=";
    char * token=strtok(command[0],delim);
    char name[100]="";

    strcpy(name, token);
    token=strtok(NULL,delim);
    char value[100]="";
    strcpy(value, token);

    int ret_value=setenv(name, value,0);
    if(ret_value==-1){
        printf("Error on setenv, status: %d\n", errno);
    }
    
}

void envVars(char*** command){
    
    int i=0;
    
    while((*command)[i]!=NULL){
        if((*command)[i][0]=='$'){
            strcpy((*command)[i],(*command)[i]+1);
            strcpy((*command)[i],getenv((*command)[i]));
        }
        i++;
    }
}

void simple_cmd(char **command){
    char path[10]="/bin/";
    strcat(path,command[0]);
    int chld_pid;
    switch (chld_pid = fork())
    {
    case -1:
        printf("Error at fork!\n");
        return -1;
        break;
    
    case 0:
        if(strchr(command[0],'=')!=NULL)
            setEnv(command);
            
        else{
            envVars(&command);
        
            int exec_rez = execvp(path,command);
            if(exec_rez==-1){
            printf("Error at exec, status: %d!\n", errno);
            exit(-1);
        }
        }
        exit(0);
       
        break;
    
    default:
        int status;
        int pid=wait(&status);
        if(pid==-1){
            printf("Error at wait\n");
            return -1;
        }
        break;
    }
   
    
}

int main(){
    
    setvbuf(stdout,NULL,_IONBF,0);

    while(1) {
        printf("> ");
        char* line= get_line();
        char ** command = parse_line(line);
         
        simple_cmd(command);
        printf("main\n");
        
        free(line);
        int k=0;
        while(command[k]!=NULL){
            free(command[k]);
            k++;
        }
        free(command);
    }

   
    
    return 0;
}