#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define SH_TOK_BUFSIZE 64
#define SH_TOK_DELIM "\t\r\b\a"

char **shSplitLine(char * line)
{
    int bufsize = SH_TOK_BUFSIZE, counter = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if(!tokens){
        fprintf(stderr, "shell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, SH_TOK_DELIM);
    while (token != NULL){
        tokens[counter] = token;
        counter++;
        
        if(counter >= bufsize){
            bufsize += SH_TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if(!tokens){
                fprintf(stderr, "shell: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, SH_TOK_DELIM);
    }
    tokens[counter] = NULL;
    return tokens;
}