#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define TOK_BUFSIZE 1024
// am adaugat si \n ca separator :)
#define TOK_DELIM " \t\r\b\a\n"
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

// executa comanda
// momentan: fara pipe, &&, || sau redirectionari
void execute(char *cmd[]) {
    pid_t pid;

    pid = fork();
    if (pid == 0) {
        // copil
        int status = execvp(cmd[0], cmd);
        if (status < 0) {
            printf("Eroare in timpul executiei comenzii.\n");
        }
        exit(0);
    } else if (pid > 0) {
        wait(NULL);
    } else {
        printf("Eroare la fork.\n");
        return;
    }
}


int main() {
    char **cmd;

    cmd = malloc(MAX_TOKENS * sizeof(char **));

    // shell main loop
    while (1) {
        // luam input utilizator
        cmd = (parse_line(read_line()));

        // daca se da enter asteptam urmatoare comanda
        if (!cmd) {
            continue;
        }

        // executa comanda
        execute(cmd);
    }
    free(cmd);
    return 0;
}
