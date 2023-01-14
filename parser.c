#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define TOK_BUFSIZE 1024
#define TOK_DELIM " \t\r\b\a"
#define MAX_TOKENS 64   


//citim inputul userului
char * read_line(void){

    char *line = NULL;
    size_t size = 0;
    getline(&line, &size, stdin);
    return line;
}

//parsam inputul intr-un array de tokenuri

char **parse_line(char *line){

    int i = 0;
    char** tokens = malloc(MAX_TOKENS * sizeof(char*));
    char* token;

    if(!tokens){
        fprintf(stderr, "shell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, TOK_DELIM);
    while(token != NULL){
        
        tokens[i++] = token;
        token = strtok(NULL, TOK_DELIM);
    }
    tokens[i] = NULL;
    return tokens;
}


int main(){

    char ** cmd;

    cmd = malloc(MAX_TOKENS * sizeof(char**));
    
    cmd = (parse_line(read_line()));

    for(int i = 0 ; cmd[i] ; ++i)
        printf("%s ", cmd[i]);
    return 0;

}

