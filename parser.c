#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define TOK_BUFSIZE 1024
// am adaugat si \n ca separator :)
//nu cred daca \r e necesar drept separator
#define TOK_DELIM " \t\b\a\n"
#define MAX_TOKENS 64   

//definesc global pt ca am nevoie pt parser pt pipe
char **cmd;
char *output, *token, *history_log, cwd[1024], *cmd_number,*command_line;
int nr_tokens, error_nr, got_error, cmd_counter; //got_error e variabila care e pasata functiei parse_error pentru a afisa cauza erorri


//citim inputul userului
char * read_line(void)
{

    char *line = NULL;
    size_t size = 0;
    getline(&line, &size, stdin);
    return line;
}

void parse_error(int error_code)
{
	//aici erorile cu codul lor

}

// executa comanda
void execute(char **tokens, int nr_args) 
{
 	//tratarea erorilor, in special nr de arg sa fie bun
}

//parsam inputul si tratam pipe, &&, ||
void parse_line(char *line){

    char** tokens = malloc(MAX_TOKENS * sizeof(char*));
    char* token;

    if(!tokens){
        fprintf(stderr, "shell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, TOK_DELIM);
    int nr_tokens=0;
    while(token != NULL)
    {
    	//parcurgem fiecare cuvant
    	char* wrd= malloc(TOK_BUFSIZE * sizeof(char))
    	strcpy(wrd,token);
    	
    	//PIPE
    	if(!strcmp(wrd,"|"))
    	{
    		//executa comanda de dinainte de pipe
    		if(nr_tokens>0)
    		{
    			free(output);
    			output=malloc(TOK_BUFSIZE * sizeof(char))
    			execute(tokens, nr_tokens);
    		}
    		
    		//trecem la comanda de dupa pipe
    		token = strtok(NULL, TOK_DELIM);
    		
    		// cazul in care nu mai este nimic dupa pipe "|"
    		if(token==NULL)
    		{
    			//error msg
    			continue;
    		}
    		strcpy(wrd,token);
    		
    		//trecem la a doua comanda
    		//rezultatul primei comenzi e stocat in output
    		//alteram vectorul de cuvinte astfel incat sa contina a doua comanda si rezultatul primei comenzi
    		tokens[0]=wrd; 		   //comanda a doua
    		strcpy(tokens[1],output); //rezultatul primei comenzi
    		nr_tokens = 2;
    		
    		//executam comanda dupa pasii de mai sus
    		free(output);
    		output=malloc(TOK_BUFSIZE * sizeof(char))
    		execute(tokens, nr_tokens);
    		
    	}
    	else if(!strcmp(wrd,"||"))
    	{
    		free(output);
    		output=malloc(TOK_BUFSIZE * sizeof(char));
    		execute(tokens, nr_tokens);
    		
    		if(got_error!=0)
    		{
    			//ignoram erorile
    			//doar prima comanda corecta o sa fie cea rulata
    			got_error=0;
    			//curatam vectorul de cuvinte de comanda care nu merge
    			nr_tokens = 0;
    			tokens=strtok(NULL, TOK_DELIM);
    			continue;
    		}
    		else
    		{
    			//gasim prima comanda care nu da eroare
    			//ignoram restul comenzilor
    			nr_tokens=0;
    			while(token!=NULL)
    			{
    				strcpy(wrd, token);
    				if(!strcmp(wrd,"&&"))	break; // ne oprim din parcurgerea restului comenzilor cand intalnim operatorul &&
    							       // astfel preluam outputul primei comenzi bune din sirul comenzilor conditionate cu || si il folosim in randul comenzilor conditionate de &&
    							       //acest caz trebuie tratat doar daca exista o comanda buna in sirul celorconditionate de ||, altfel sirul conditionat de && e din start fals 
    				tokens=strtok(NULL, TOK_DELIM);
    			}
    			if(token==NULL) got_error=-1; //inseamna ca nicio comanda nu a fost buna, rezultatul total e fals
    		}
    		
    	}
    	else if(!strcmp(wrd,"&&"))
    	{
    		free(output);
    		output=malloc(TOK_BUFSIZE * sizeof(char));
    		execute(tokens, nr_tokens);
    		
    		if(got_error!=0) break;
    		
    		nr_tokens = 0;
    		
    	}
    	else
        	tokens[nr_tokens++] = wrd; //tinem minte tokenurile care NU sunt operatori logici sau pipe
        token = strtok(NULL, TOK_DELIM);
    }
}


int main() 
{
    history_log = malloc(TOK_BUFSIZE *100* sizeof(char));
    output = malloc(TOK_BUFSIZE * sizeof(char));
    // shell main loop
    while (1) 
    {
    	command_line=read_line();

		if(getcwd(cwd,sizeof(cwd))!=NULL)
		{
			fprintf(stderr,"Error:getcwd() error");
			exit(EXIT_FAILURE);
		}
    	
    	// daca se da enter asteptam urmatoare comanda
        if (command_line==NULL) 
		{
            continue;
        }

		cmd_counter++;
		cmd_number=malloc(5*sizeof(char));

		sprintf(cmd_number,"%d", cmd_counter)

		strcat(history_log, cmd_number);
		strcat(history_log, ": ");
		strcat(history_log, command_line);
		strcat(history_log,"\n");
    	
        // luam input utilizator
        parse_line(command_line);

       	//pe aici trebuie adaugat comamand_line in history
       	//putem sa le si numerotam, dar trebuie sa o salvam drept char ca sa o afisam
	
        // executa comanda daca nu am avut nicio eroare
        if(got_error!=0)
        {
        	free(output);
        	output = malloc(TOK_BUFSIZE * sizeof(char));
        	execute(tokens, nr_tokens);
        }
        
        //daca avem o eroare ii afisam mesajul corespunzator
        if(got_error!=0)
        {
        	parse_error(got_error);
        	got_error = 0;
        }
    }
    
    return 0;
}
