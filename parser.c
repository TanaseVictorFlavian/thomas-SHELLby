#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#define TOK_BUFSIZE 1024
// am adaugat si \n ca separator :)
#define TOK_DELIM " \t\r\b\a\n"
#define MAX_TOKENS 64

char *error;

// creaza fisier cu numele dat in directorul curent
void touch(char *file_name) {
    if (!fopen(file_name, "w")) {
        error = "can't create file.\n";
        return;
    }
}

// cp me sempai >_<
void cp(char *input_file, char *output_file) {
    // deschide fisier intrare
    int input = open(input_file, O_RDONLY);
    if (input == -1) {
        error = "cant open input file.\n";
        return;
    }
    // deschide fisier iesire
    int output = open(output_file, O_WRONLY);
    if (output == -1) {
        error = "cant open output file.\n";
        return;
    }

    // citim si copiem cate 1024 bytes
    int size = 1024;
    char buff[1024];
    int nwrite, nread;

    nread = read(input, buff, size);
    while (nread > 0) {
        nwrite = write(output, buff, nread);
        if (nwrite != nread) {
            error = "error while writing in file.\n";
            return;
        }
        nread = read(input, buff, size);
        if (nread < 0) {
            printf("eroare\n");
        }
    }

    // inchidem fisierele
    if (close(input) < 0) {
        error = "can't close input file.\n";
        return;
    }
    if (close(output) < 0) {
        error = "can't close output file.\n";
        return;
    }
}

// citim inputul userului
char *read_line(void) {
    char *line = NULL;
    size_t size = 0;
    getline(&line, &size, stdin);
    return line;
}

// parsam inputul intr-un array de tokenuri

char **parse_line(char *line) {
    int i = 0;
    char **tokens = malloc(MAX_TOKENS * sizeof(char *));
    char *token;

    if (!tokens) {
        fprintf(stderr, "shell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, TOK_DELIM);
    while (token != NULL) {
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

int main(int argc, char **argv) {
    // char **cmd;

    // cmd = malloc(MAX_TOKENS * sizeof(char **));

    // // shell main loop
    // while (1) {
    //     // luam input utilizator
    //     cmd = (parse_line(read_line()));
    //     // daca se da enter asteptam urmatoare comanda
    //     if (!cmd) {
    //         continue;
    //     }

    //     // executa comanda
    //     execute(cmd);
    // }
    // free(cmd);

    cp(argv[1], argv[2]);

    if (error) printf(error);
    return 0;
}
