#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>

#define TOK_BUFSIZE 1024
#define TOK_DELIM " \t\r\b\a\n"
#define MAX_TOKENS 64   


void help(){
    puts("Commands listed below:\n");
    puts("clear: clears the terminal");
    puts("pwd: returns the current directory path");
    puts("rmfile: removes a specified file from the current directory");
    puts("makedir: creates a directory in the current directroy");
    puts("history: lists previous commands");
    puts("echo: writes arguments to the standard output");
    puts("touch: creates an empty file in the current directory");
    puts("ls: lists all files and directories from the current directory");
    puts("cp: copies the content of a file into another file\n");

    
}

int cmd_counter;
char cwd[1024], *history_log, *shell_output;
char ** cmd_special_chars;

//clears terminal
void clear(){
    //escape sequence
    write(1, "\33[H\33[2J", 7);
}


void pwd(){
    printf("%s\n", cwd);
}


//removes file from current directory
void rmfile(char* file_name){
    char file_path[1024];
    strcpy(file_path, cwd);
    strcat(file_path, "/");
    strcat(file_path, file_name);
    
    if(remove(file_path)){
        fprintf(stderr,"Error: cannot remove %s, no such file", file_name);
        exit(EXIT_FAILURE);
    }
}

//creates a directory
void makedir(char* folder_name){
    char folder_path[1024];
    strcpy(folder_path, cwd);
    strcat(folder_path, "/");
    strcat(folder_path, folder_name);

   if(mkdir(folder_path, 0777) == -1){
        fprintf(stderr, "Error: cannot create directory");
        exit(EXIT_FAILURE);

   } 
}
// removes directory
void rmdr(char* folder_name){
    char folder_path[1024];
    strcpy(folder_path, cwd);
    strcat(folder_path, "/");
    strcat(folder_path, folder_name);

    if(rmdir(folder_path) == -1){
        fprintf(stderr, "Error: cannot remove %s, no such directory", folder_name);
        exit(EXIT_FAILURE);
    }
}

void history(){
    printf("%s\n", history_log);
}

void addToHistory(char** cmd){
    
    for(int i = 0 ; cmd[i] ; ++i)
        strcat(history_log, cmd[i]);
        strcat(history_log, " ");
    strcat(history_log, "\n");

}

//citim inputul userului

char *read_line(void){

    char *line = NULL;
    size_t size = 0;
    getline(&line, &size, stdin);
    return line;
}

//parsam inputul intr-un array de tokenuri


char **parse_line(char *line){

    cmd_counter ++;
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

int main(){

    char ** cmd;
    history_log = malloc(TOK_BUFSIZE * sizeof(char));
    cmd = malloc(MAX_TOKENS * sizeof(char**));
    
    cmd = (parse_line(read_line()));


    // shell_output = malloc(TOK_BUFSIZE * sizeof(char));
 
    // aflam path-ul

    if(!getcwd(cwd, sizeof(cwd))){
        fprintf(stderr, "Error: getcwd() error");
        exit(EXIT_FAILURE);
    }


    // shell main loop
    // while (1) {
    //     // luam input utilizator
    //     cmd = (parse_line(read_line()));
    //     //  adaugam comanda in istoric
    //     addToHistory(cmd);
        
    //     // daca se da enter asteptam urmatoare comanda
    //     if (!cmd) {
    //         continue;
    //     }

    //     // executa comanda
    //     execute(cmd);
    // }
    // free(cmd);

    return 0;

}

