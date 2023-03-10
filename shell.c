#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "errorDefines.h"

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

int cmd_counter, nr_tokens, error;
char cwd[1024], *output, *command_line, *token;
char **tokens, **history_log;

void error_handler(int error){
    switch (error)
    {
    case 1:
        puts("Error : shell allocation error");
        break;
    case 2:
        puts("Error : unexpected event while reading the file");
        break;
    case 3:
        puts("Error : couldn't open the specified file(s)");
        break;
    case 4:
        puts("Error : couldn't create file");
        break;

    case 5:
        puts("Error : couldn't close the given file");
        break;

    case 6:
        puts("Error : couldn't remove the given file, no such file");
        break;

    case 7:
        puts("Error : cannot create directory");
        break;

    case 8:
        puts("Error : cannot delete directory, no such directory");
        break;

    case 9:
        puts("Error : fork error");
        break;

    case 10:
        puts("Error : missing pipe argument");
        break;
    
    case 11:
        puts("Statement is false");
        break;

    case 12:
        puts("Invalid number of operands for the given command");
        break;
    case 13:
        puts("Error : couldn't list current directory");
        break;
    }

}

void execute();

//clears terminal
void clear(){
    //escape sequence
    write(1, "\33[H\33[2J", 7);
}

// creaza fisier cu numele dat in directorul curent
//rez_output
void touch(char *file_name) {
	FILE *aux;
	aux=fopen(file_name, "w");
    if (!aux) 
        error = CREATE_FILE_ERROR;
    fclose(aux);
}

//rez_output
void pwd(){
    printf("%s\n", cwd);
    strcat(output, cwd);
}

// print arguments
// first token is echo
//rez_output
void echo(){
    for (int i = 1; i < nr_tokens; i++) {
        printf("%s ", tokens[i]);
        strcat(output, tokens[i]);
        strcat(output, " ");
    }
    printf("\n");
}

//rez_output
void cp(char *input_file, char *output_file) {
    // deschide fisier intrare
    int input = open(input_file, O_RDONLY);
    if (input == -1) {
        error = OPEN_FILE_ERROR;
    }
    // deschide fisier iesire
    int outputt = open(output_file, O_WRONLY);
    if (outputt == -1) {
        error = OPEN_FILE_ERROR;
    }

    // citim si copiem cate 1024 bytes
    int size = 1024;
    char buff[1024];
    int nwrite, nread;

    nread = read(input, buff, size);
    while (nread > 0) {
        nwrite = write(outputt, buff, nread);
        if (nwrite != nread) {
            error = READ_FILE_ERROR;
        }
        nread = read(input, buff, size);
    }
    
    strcat(output, "File has been copied.");
    printf("%s,\n", output);  

    // inchidem fisierele
    if (close(input) < 0) {
        error = CLOSE_FILE_ERROR;
    }
    if (close(outputt) < 0) {
        error = CLOSE_FILE_ERROR;
    }
}

//removes file from current directory
//rez_output
void rmfile(char* file_name){
    char file_path[1024];
    strcpy(file_path, cwd);
    strcat(file_path, "/");
    strcat(file_path, file_name);
    
    if(!remove(file_path))
    {
    	strcat(output, "File has been deleted.");
    	printf("%s,\n", output); 
    }
    else
    {
        error = REMOVE_FILE_ERROR;
    }
}

//creates a directory
//rez_output
void makedir(char* folder_name){
    char folder_path[1024];
    strcpy(folder_path, cwd);
    strcat(folder_path, "/");
    strcat(folder_path, folder_name);

   if(!mkdir(folder_path, 0777) == -1)
   {
   	strcat(output, "Folder has been created.");
    	printf("%s,\n", output); 
   }
   else
   {
        error = CREATE_DIRECTORY_ERROR;
   } 
}

// removes directory
void rmdr(char* folder_name){
    char folder_path[1024];
    strcpy(folder_path, cwd);
    strcat(folder_path, "/");
    strcat(folder_path, folder_name);

    if(rmdir(folder_path) == -1){
        error = REMOVE_DIRECTORY_ERROR;
    }
    else
    {
    	strcat(output, "Folder has been deleted.");
    	printf("%s,\n", output);
    }
}

void history(){

    if(cmd_counter == 0)
        puts("There are no previous commands saved in the history");

    for(int i = 0 ; i < cmd_counter ;++i){
        printf("%s", history_log[i]);
    }
}

void addToHistory(char* cmd){
    
    history_log[cmd_counter] = malloc(1024 * sizeof(char));
    strcpy(history_log[cmd_counter++], cmd);
    
}
void ls(){
    pid_t pid;

    pid = fork();
    if (pid == 0) {
        // copil
        char *argl[] = {"ls", NULL};
        int status = execvp(argl[0],argl);
        if (status < 0) {
            error = LIST_DIRECTORY_ERROR;
        }
        exit(0);
    } else if (pid > 0) {
        wait(NULL);
    } else {
        error = FORK_ERROR;
        return;
    }
}


// citim inputul intr-un buffer
char *read_line(void){

    char *line = NULL;
    size_t size = 0;
    getline(&line, &size, stdin);
    return line;
}

//tratam piepurile si operatorii logici || si &&
//parsam inputul intr-un array de tokenuri


void parse_line(){

    char* token;

    if(!tokens){
        error = SHELL_ALLOC_ERROR;
    }

    token = strtok(command_line, TOK_DELIM);
    while(token != NULL)
    {
    	//parcurgem fiecare cuvant
    	char* wrd = malloc(TOK_BUFSIZE * sizeof(char));
    	strcpy(wrd,token);
    	
    	//PIPE
    	if(!strcmp(wrd,"|"))
    	{
    		//executa comanda de dinainte de pipe
    		if(nr_tokens > 0)
    		{
    			free(output);
    			output = malloc(TOK_BUFSIZE * sizeof(char));
    			execute(tokens, nr_tokens);
    		}
    		
    		// trecem la comanda de dupa pipe
    		token = strtok(NULL, TOK_DELIM);
    		
    		// cazul in care nu mai este nimic dupa pipe "|"
    		if(token == NULL)
    		{
    			error = MISSING_PIPE_ARG;
    			continue;
    		}
    		strcpy(wrd,token);
    		
    		//trecem la a doua comanda
    		//rezultatul primei comenzi e stocat in output
    		//alteram vectorul de cuvinte astfel incat sa contina a doua comanda si rezultatul primei comenzi
    		tokens[0] = wrd; 		   //comanda a doua
            
            if(!strcmp(tokens[0], "touch")){
                token = strtok(NULL, TOK_DELIM);
                free(output);
                output = malloc(TOK_BUFSIZE * sizeof(char));
                strcpy(output, "");
                strcat(output, token);
            }
    		strcpy(tokens[1], output); //rezultatul primei comenzi
    		nr_tokens = 2;
    		
    		//executam comanda dupa pasii de mai sus
    		free(output);
    		output = malloc(TOK_BUFSIZE * sizeof(char));
    		execute(tokens, nr_tokens);
    		
    	}
    	else if(!strcmp(wrd,"||"))
    	{   
            if(strcmp(output, "")){
                free(output);
                output=malloc(TOK_BUFSIZE * sizeof(char));
            }
    		execute(tokens, nr_tokens);
    		
    		if(error != 0)
    		{
    			// ignoram erorile
    			// doar prima comanda corecta o sa fie cea rulata
    			error = 0;
    			// curatam vectorul de cuvinte de comanda care nu merge
    			nr_tokens = 0;
    			token = strtok(NULL, TOK_DELIM);
    			continue;
    		}
            else if(error == 0 && nr_tokens != 0){
                nr_tokens = 0;
                return;
            }
    		else
    		{
    			//gasim prima comanda care nu da eroare
    			//ignoram restul comenzilor
    			nr_tokens=0;
    			while(token != NULL)
    			{
    				strcpy(wrd, token);
    				if(!strcmp(wrd,"&&"))	
                        // ne oprim din parcurgerea restului comenzilor cand intalnim operatorul &&
                        break; 
                        // astfel preluam outputul primei comenzi bune din sirul comenzilor conditionate cu || si il folosim in randul comenzilor conditionate de &&
                        // acest caz trebuie tratat doar daca exista o comanda buna in sirul celorconditionate de ||, altfel sirul conditionat de && e din start fals 
    				token = strtok(NULL, TOK_DELIM);
    			}
    		}
    		
    	}
    	else if(!strcmp(wrd,"&&"))
    	{   
            if(strcmp(output, "")){
    		    free(output);
    		    output=malloc(TOK_BUFSIZE * sizeof(char));
            }

    		execute(tokens, nr_tokens);
    		
    		if(error != 0) 
                return;
    		
    		nr_tokens = 0;
    		
    	}
    	else
        	tokens[nr_tokens++] = wrd; //tinem minte tokenurile care NU sunt operatori logici sau pipe
        token = strtok(NULL, TOK_DELIM);
    }
}

//rescriem
void execute() {

 if(!strcmp(tokens[0], "clear")){
        if(nr_tokens != 1){
            error = INVALID_ARG_NUMBER;
            return;
        }
        clear();
    }

    else if(!strcmp(tokens[0], "touch")){
        if(nr_tokens != 2){
            error = INVALID_ARG_NUMBER;
            return;
        }
        touch(tokens[1]);
    }

    else if(!strcmp(tokens[0], "pwd")){
        if(nr_tokens != 1){
            error = INVALID_ARG_NUMBER;
            return;
        }
        pwd();
    }

    else if(!strcmp(tokens[0], "echo")){
        echo();
    }


    else if(!strcmp(tokens[0], "cp")){
        if(nr_tokens != 3){
            error = INVALID_ARG_NUMBER;
            return;
        }
        cp(tokens[1], tokens[2]);
    }

    else if(!strcmp(tokens[0], "rmfile")){
        if(nr_tokens != 2){
            error = INVALID_ARG_NUMBER;
            return;
        }
        rmfile(tokens[1]);
    }

    else if(!strcmp(tokens[0], "makedir")){
        if(nr_tokens != 2){
            error = INVALID_ARG_NUMBER;
            return;
        }
        makedir(tokens[1]);
    }

    else if(!strcmp(tokens[0], "rmdr")){
        if(nr_tokens != 2){
            error = INVALID_ARG_NUMBER;
            return;
        }
       rmdr(tokens[1]);
    }

    else if(!strcmp(tokens[0], "ls")){
        ls();
    }

    else if(!strcmp(tokens[0], "help")){
        if(nr_tokens != 1){
            error = INVALID_ARG_NUMBER;
            return;
        }
        help();
    }


    else if (!strcmp(tokens[0], "history")){
        if(nr_tokens != 1){
            error = INVALID_ARG_NUMBER;
            return;
        }
        history();
    }

    else if (!strcmp(tokens[0], "exit")){
        if(nr_tokens != 1){
            error = INVALID_ARG_NUMBER;
            return;
        }
        exit(EXIT_SUCCESS);
    }
    else {
        error = UNKNOWN_COMMAND;
    }
}

int main(){

    history_log = malloc(TOK_BUFSIZE * sizeof(char*));
    command_line = malloc(TOK_BUFSIZE * sizeof(char));

    while (1) 
    {
        
        output = malloc(TOK_BUFSIZE * sizeof(char));
        tokens = malloc(MAX_TOKENS * sizeof(char*));
        nr_tokens = 0;
        if(!getcwd(cwd, sizeof(cwd))){
            fprintf(stderr, "Error: getcwd() error");
            exit(EXIT_FAILURE);
        }
        printf("%s//\U0001F64F: ", cwd);

        //printf("%s/@: ", cwd);
        command_line = read_line();
        addToHistory(command_line);
    	// daca se da enter asteptam urmatoare comanda
        if (command_line == NULL) 
		{
            continue;
        }
        // luam input utilizator
        parse_line();
	
        // executa comanda daca nu am avut nicio eroare
        if(error == 0){
            if(strcmp(output, "")){
        	free(output);
        	output = malloc(TOK_BUFSIZE * sizeof(char));
            }
            if(nr_tokens){
               execute();
            }
            free(tokens);

        }
        
        //daca avem o eroare ii afisam mesajul corespunzator
        if(error != 0)
        {
        	error_handler(error);
        	error = 0;
        }
    }    return 0;

}