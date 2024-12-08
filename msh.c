//P2-SSOO-23/24

// MSH main file
// Write your msh source code here

//include "parser.h"
#include <stddef.h>			/* NULL */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_COMMANDS 8


// files in case of redirection
char filev[3][64];

//to store the execvp second parameter
char *argv_execvp[8];

void siginthandler(int param)
{
	printf("****  Exiting MSH **** \n");
	//signal(SIGINT, siginthandler);
	exit(0);
}

/* myhistory */

/* myhistory */

struct command
{
  // Store the number of commands in argvv
  int num_commands;
  // Store the number of arguments of each command
  int *args;
  // Store the commands
  char ***argvv;
  // Store the I/O redirection
  char filev[3][64];
  // Store if the command is executed in background or foreground
  int in_background;
};

int history_size = 20;
struct command * history;
int head = 0;
int tail = 0;
int n_elem = 0;

void free_command(struct command *cmd)
{
    if((*cmd).argvv != NULL)
    {
        char **argv;
        for (; (*cmd).argvv && *(*cmd).argvv; (*cmd).argvv++)
        {
            for (argv = *(*cmd).argvv; argv && *argv; argv++)
            {
                if(*argv){
                    free(*argv);
                    *argv = NULL;
                }
            }
        }
    }
    free((*cmd).args);
}

void store_command(char ***argvv, char filev[3][64], int in_background, struct command* cmd)
{
    int num_commands = 0;
    while(argvv[num_commands] != NULL){
        num_commands++;
    }

    for(int f=0;f < 3; f++)
    {
        if(strcmp(filev[f], "0") != 0)
        {
            strcpy((*cmd).filev[f], filev[f]);
        }
        else{
            strcpy((*cmd).filev[f], "0");
        }
    }

    (*cmd).in_background = in_background;
    (*cmd).num_commands = num_commands-1;
    (*cmd).argvv = (char ***) calloc((num_commands) ,sizeof(char **));
    (*cmd).args = (int*) calloc(num_commands , sizeof(int));

    for( int i = 0; i < num_commands; i++)
    {
        int args= 0;
        while( argvv[i][args] != NULL ){
            args++;
        }
        (*cmd).args[i] = args;
        (*cmd).argvv[i] = (char **) calloc((args+1) ,sizeof(char *));
        int j;
        for (j=0; j<args; j++)
        {
            (*cmd).argvv[i][j] = (char *)calloc(strlen(argvv[i][j]),sizeof(char));
            strcpy((*cmd).argvv[i][j], argvv[i][j] );
        }
    }
}


/**
 * Get the command with its parameters for execvp
 * Execute this instruction before run an execvp to obtain the complete command
 * @param argvv
 * @param num_command
 * @return
 */
void getCompleteCommand(char*** argvv, int num_command) {
	//reset first
	for(int j = 0; j < 8; j++)
		argv_execvp[j] = NULL;

	int i = 0;
	for ( i = 0; argvv[num_command][i] != NULL; i++)
		argv_execvp[i] = argvv[num_command][i];
}
int accsum = 0;
/**
 * Main sheell  Loop  
 */
int main(int argc, char* argv[])
{
	/**** Do not delete this code.****/
	int end = 0; 
	int executed_cmd_lines = -1;
	char *cmd_line = NULL;
	char *cmd_lines[10];

	if (!isatty(STDIN_FILENO)) {
		cmd_line = (char*)malloc(100);
		while (scanf(" %[^\n]", cmd_line) != EOF){
			if(strlen(cmd_line) <= 0) return 0;
			cmd_lines[end] = (char*)malloc(strlen(cmd_line)+1);
			strcpy(cmd_lines[end], cmd_line);
			end++;
			fflush (stdin);
			fflush(stdout);
		}
	}

	/*********************************/

	char ***argvv = NULL;
	int num_commands;

	history = (struct command*) malloc(history_size *sizeof(struct command));
	int run_history = 0;
	struct command *cmd;
	

	while (1) 
	{
		int count_command = 1;
		int status = 0;
		int command_counter = 0;
		int in_background = 0;
		signal(SIGINT, siginthandler);

		if (run_history)
    {
        run_history=0;
		count_command = 0;
		if (cmd){
			argvv=cmd->argvv;
			command_counter = cmd->num_commands;
		}
    }
    else{
        // Prompt 
        write(STDERR_FILENO, "MSH>>", strlen("MSH>>"));

        // Get command
        //********** DO NOT MODIFY THIS PART. IT DISTINGUISH BETWEEN NORMAL/CORRECTION MODE***************
        executed_cmd_lines++;
        if( end != 0 && executed_cmd_lines < end) {
            command_counter = read_command_correction(&argvv, filev, &in_background, cmd_lines[executed_cmd_lines]);
        }
        else if( end != 0 && executed_cmd_lines == end)
            return 0;
        else
            command_counter = read_command(&argvv, filev, &in_background); //NORMAL MODE
    }
		//************************************************************************************************


		/************************ STUDENTS CODE ********************************/
     
     
	   if (command_counter > 0) {
			if (command_counter > MAX_COMMANDS){
				printf("Error: Maximum number of commands is %d \n", MAX_COMMANDS);
			}
			else {
				// Print command
				//print_command(argvv, filev, in_background);
			}
		}
         for (int i =0; i< command_counter; i++){ 
            //here we store commands in array argvv
		    getCompleteCommand(argvv,i); 
		}

        //mycalc
		if(strcmp(argvv[0][0], "mycalc") == 0) {
			
			if (argvv[0][1] != NULL && argvv[0][2] != NULL && argvv[0][3] != NULL) { //checks if the tree arguments exist.
				if (strcmp(argvv[0][2], "add") == 0) { //addition case
					
					int add1 = atoi(argvv[0][1]); // transforms the 1st argument from string to int
					int add2 = atoi(argvv[0][3]); // transforms the 3rd argument from string to int
					int result = add1 + add2; // executing the addition
					
					accsum += result; // adds the result to the accumulator variable accsum
					char array[20];
					sprintf(array, "%d", accsum); // the accsum int is transformed into string and stored in the array
					const char *value = array; // the string of the sum is assigned to the variable value

					if (setenv("Acc", value, 1) < 0) { // the enviroment variable Acc is set to the calculated value
						perror("There has been an error with the enviroment variable Acc\n"); // printed message in case of error
					}

					fprintf(stderr, "[OK] %d + %d = %d; Acc %s \n", add1, add2, result, getenv("Acc")); // prints the result of the sum and the value of Acc
					
				} else if (strcmp(argvv[0][2], "mul") == 0) { // multiplication case
					
					int num1 = atoi(argvv[0][1]); // transforms the 1st argument from string to int
					int num2 = atoi(argvv[0][3]);// transforms the 3rd argument from string to int
					int result = num1 * num2; //executes the multiplication

					fprintf(stderr, "[OK] %d * %d = %d \n", num1, num2, result); //prints the result with the standard error output
					
				} else if (strcmp(argvv[0][2], "div") == 0) {
			
					int num1 = atoi(argvv[0][1]);// transforms the 1st argument from string to int
					int num2 = atoi(argvv[0][3]);// transforms the 3rd argument from string to int
					if (num2 != 0) { // in case num2 (denominator) is not zero . 
						int result = num1 / num2; // executes the division
						int remainder = num1 % num2; // obtains the reminder of the division

						fprintf(stderr, "[OK] %d / %d = %d; Remainder %d \n", num1, num2, result, remainder); // prints the result of the division
					
					} else {
						// error if the denominator is 0 
						perror("ERROR. Division by 0");
					}
				} else {
					// Error in case there are three arguments but none of them is add , div , mult
					printf("[ERROR] The structure of the command is mycalc <operand_1> <add/mul/div> <operand_2>\n");
			
				}
			} else {
				//Error in case there are not three arguments
				printf("[ERROR] The structure of the command is mycalc <operand_1> <add/mul/div> <operand_2>\n");
			}
		}

		else if(strcmp(argvv[0][0], "myhistory") == 0) { //myhistory function
			if (argvv[0][1] == NULL) {
				// Listing the last 20 commands 
				for (int i = 0; i < n_elem; i++) { //loop for iterating among the commands 
					fprintf(stderr,"%d ", i); // prints the command
                	for (int j=0; j<history[i].num_commands; j++){ //loop for iterating among each command of the current history entry
						for (int k=0; k<history[i].args[j]; k++){ // loop for iterating through each argument of the current command
							fprintf(stderr,"%s", history[i].argvv[j][k]); //prints the argument of the current command
							if (k!=history[i].args[j] -1){
								fprintf(stderr," ");
							}
						}
						if (j!= history[i].num_commands -1 ){
							fprintf(stderr," | "); // adding a pipe symbol between each command except the last one
						}
					}
					if(strcmp(history[i].filev[0],"0") !=0){ //checking if there is an input redirection
						fprintf(stderr, " < %s",history[i].filev[0]); // prints the input redirection
					}
					if(strcmp(history[i].filev[1],"0") !=0){ // checks if there is an ouput redirection
						fprintf(stderr, " > %s",history[i].filev[1]); //prints the output redirection
					}
					if(strcmp(history[i].filev[2],"0") !=0){ // checks if there is an error redirection
						fprintf(stderr, " ! > %s",history[i].filev[2]); //prints the error redirection 
					}
					if (history[i].in_background){ // checks if the command has been run on background
						fprintf(stderr," &");
					}
					fprintf(stderr,"\n");
            	}
			} else if (argvv[0][1] != NULL && atoi(argvv[0][1]) >= 0 && atoi(argvv[0][1]) < n_elem) {// checks if the index is inside the valid range and if there exist a 2nd arg
				int index = atoi(argvv[0][1]);// converts 2nd argument from string into int
				if (index >= 0 && index < n_elem) { // checks if the index is out of range
					fprintf(stderr, "Running command %d\n", index); 
					cmd = &history[index]; // set the command to be executed .
					run_history = 1;
				}
			} 
			else {
				// Command out of range
				printf("ERROR: Command not found\n");

			}
		}
		else {
			// Storing each executed command in the history
			if (n_elem < 20 && count_command) {
				if (n_elem == 0) {
					head = tail = 0; // Initialize head and tail
				}
				store_command(argvv, filev, in_background, &history[tail]); // uses store command function to store the current command
				tail = (tail + 1) % history_size;// updates the tail after storing a new command 
				if (n_elem < history_size) {
					n_elem++; // incrementig the size of the history
				}
			}
			else if (n_elem >= 20) { // there are already 20 elements
				free_command(&history[head]); // eliminating the command that is in the head
				store_command(argvv, filev, in_background, &history[tail]); // stores the new command 
				head = (head + 1) % history_size; // update the head after having eliminating the previous one
				tail = (tail + 1) % history_size; // updates the tail 
			}


			//simple commmand
			if (command_counter == 1) {
				int pid, status, fd=0;
				pid = fork(); // creating a child process

				// Redirects 0(input) ,1 (output) ,2 (error)
					if (strcmp(filev[0], "0")!= 0){ // If there is an input file, close STDOUT input & open it
						if ((close(0))<0){
						perror("Error closing default descriptor 0"); // prints an error if closing descriptor fails
						}
						if (fd = open(filev[0],O_RDONLY,0666)<0){ // open the input file for reading
						perror("Error opening file");} // in case there is an error 
					}
					if (strcmp(filev[1], "0")!= 0){ // If there is an output file, close STDOUT & open file
						if ((close(1))<0){ // prints an error if closing descriptor fails
						perror("Error closing default descriptor 1");
						}
						if (fd = open(filev[1],O_WRONLY|O_CREAT|O_TRUNC,0666)<0){ // open the ouput fie for writing
						perror("Error opening file");} // message in case of error
					}			
					if (strcmp(filev[2], "0")!= 0){ // If there is error file, close error and open file
						if ((close(2))<0){
						perror("Error closing default descriptor 2");
						}
						if (fd = open(filev[2],O_WRONLY|O_CREAT|O_TRUNC,0666)<0){ // opens the error file for writing
						perror("Error opening file");}
					}	

				switch (pid){ //choosing the process depending on the pid 
				
				case -1:  //error case
				
					perror("Error in fork");
					return -1;

					
				case 0:  //child
				
					execvp(argvv[0][0],argvv[0]); // a new program is executed 
					perror("Error in exec"); // in case of failure
					break;
				
				default:  //parent proces

					if(in_background !=1){  // back to the parent waiting
						while(pid !=wait (&status) );
						
					} else{     // back to the parent without waiting
						printf("Pid = [%d]\n",getpid()); // prints the id of the child
					} 
				}
			}

			//multiple commands
			else if(command_counter>1) {
				int fdes=0;
						int fd[2];
						int pid, status;
				
				
				if (strcmp(filev[0], "0")!= 0){ // If there is an input file, close STDOUT input & open it
					if ((close(0))<0){
					perror("There has been an error closing default descriptor 0");// prints an error if closing descriptor fails
					}
					if (fdes = open(filev[0],O_RDONLY,0666)<0){//open the input file for reading
					perror("There has been an error opening the file");}// in case there is an error 
				}
				if (strcmp(filev[1], "0")!= 0){ // If there is an output file, close STDOUT & open file
					if ((close(1))<0){// prints an error if closing descriptor fails
					perror("There has been an rror closing default descriptor 1");
					}
					if (fdes = open(filev[1],O_WRONLY|O_CREAT|O_TRUNC,0666)<0){
					perror("There has been an error opening file");}// in case there is an error 
				}					
				if (strcmp(filev[2], "0")!= 0){ // If error file, close error and open error file
					if ((close(2))<0){
					perror("There has been an error closing default descriptor 2");
					}
					if (fdes = open(filev[2],O_WRONLY|O_CREAT|O_TRUNC,0666)<0){// opens the error file for writing
					perror("There has been an error opening file");}// in case there is an error 
				}	

				int newinput = dup(STDIN_FILENO);//duplicates and assigns to the varible newimput the standard input file descriptor

				for (int j = 0; j < command_counter; j++) { // loop from 0 to the number of commands 
								if (j != command_counter - 1) { // if we are not yet on the last command
									if (pipe(fd) < 0) { // creation of a pipe for the communication between commands 
									perror("There has been an error when creating the pipe\n");// an error occurred
									exit(0);// in case of error it goes out of the program
									}
							}
							

								
								pid = fork();// creating a child process
								if (pid==0){ //child case
								
						
						if (dup2(newinput,STDIN_FILENO) < 0){;//duplicates the file descriptor to STDIN_FILENO. It replaces the child process input with the one of the previous sequence
							perror("There has been an error trying to duplicate the file descriptor");	// message in case of error when duplication
						}

						if (close(newinput)<0){ // closes the file descriptor in child's process
							perror("There has been an error closing the file descriptor"); // message in case of error closing it.
						}
						
						if (j!= command_counter-1){ //while it is not the last command
							if (dup2(fd[1],STDOUT_FILENO)<0){ //duplicates the file descriptor  to the  standard output file descriptor
							perror("There has been an error duplicating the file descriptor"); // message in case of error
							}
							if (close(fd[1])<0){//closes the write end of the pipe
								perror("There has been an error closing the file");// message in case of error
							}
							if (close(fd[0])<0){//closes the read end of the pipe
								perror("There has been an error closing the descriptor");
							}
						}

						if(execvp(argvv[j][0],argvv[j]) <0) { // executes the command specified by argvv[j][0], replacing the current process with the specified command
							perror("There has been an error trying to execute the command");//message in case of error
							exit(0);//in case of error , gets out of the program
						} 
						break;

					} else if(pid<0){
						perror("There has been an error in the fork"); // if there is an error while doing the fork()
					} else{ // father's case
						
						if (close(newinput)<0){ //closes the file descriptor newimput
							perror("There has been an error closing the descriptor");// error message 
						}

						if (j!= command_counter-1){ // while it is not the last command 

							if ((newinput = dup(fd[0])) < 0){;//duplicating the read end of the pipe and assigning it to newimput
								perror("There has been an error when trying to duplicate the descriptor");// message in case of failure	
							}
							
							if (dup(fd[0])<0){ // duplicates the read end of the pipe
								perror("There has been an error when trying to duplicate the descriptor");// message in case of error 	
							}
							if (close(fd[1])<0){//closes the write end of the pipe 
								perror("There has been an error when trying to close the descriptor");// message in case of error
							}
						}
						
							
					}// close switch
				} // close for
					if (fdes!=0){// If fdes is not equal to zero a file has been opened and must be closed		
						if((close(fdes)) < 0){ // closes the file descriptor
							perror("There has been an error closing the file");}//message in case of error
				
					}

					if(in_background !=1){ // parent process has to wait for the end of the child process since there is no background
						while(pid != wait (&status));//parents process waits and saves the status of the child process
						
					}
					else{ // parent process is done in background
					printf("Pid = [%d]\n",getpid());}// returns the ID of the current process
									
							
					
				}
			
		}
	}

}

       

