
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <unistd.h> 
#include <sys/wait.h>
#include <signal.h>
#include <dirent.h>
#include <fcntl.h>
char* args[512];
char buffer[2048];
int argcount = 0;
pid_t childpid;
int termstatus = 0;
char* OutputfileName = NULL;
char* InputfileName = NULL;
int foreground = 0;
int background = 0;
int sigtstp = 0;
int fd;
struct sigaction SIGINT_action = { 0 };
struct sigaction SIGTSTP_action = { 0 };


void catchSIGSTP() { // catches ^z
	if (background == 0) { // we are in fg mode 
		background = 1;
		foreground = 1;
		char* msg = "Entering foreground - only mode(&is now ignored)\n";
		write(1, msg,49 );
		fflush(stdout);
	}
	else  //Exiting foreground-only mode
	{
		background = 0;	// bg is 0 so we are in the fg				
		foreground = 0; // set fg only mode to 0
		char* msg1 = "Exiting foreground-only mode\n";
		write(1, msg1, 31);
		fflush(stdout);
	}

}

void parseCmd() {
	fgets(buffer, 2048, stdin);// read in max of 2048 chars into buffer
	char* saveptr;
	strtok_r(buffer, "\n", &saveptr); // get rid of the newline 	
	char* indArgs = strtok_r(buffer, " ", &saveptr); //get the first argument
	while (indArgs != NULL) { 
		 if (strcmp(indArgs , ">") == 0) {
			indArgs = strtok_r(NULL, " ", &saveptr); // skip the > so its not an arg
			OutputfileName = indArgs; //setthe outputfile name to whatever comes after >
			indArgs = strtok_r(NULL, " ", &saveptr); //get next arg
		
			
		}
		 else if (strcmp(indArgs, "<") == 0) {
			indArgs = strtok_r(NULL, " ", &saveptr); // see above
			InputfileName = indArgs;
			indArgs = strtok_r(NULL, " ", &saveptr);
			

		}
		else if (strcmp(indArgs, "&") == 0) {
			 if (foreground == 1) {
				 background = 0;
				  printf("[ background mode: Ignored \n");
				fflush(stdout);
			 }
			 background = 1; // if we find a & that means its the end of the line and the process wil run in the background
			/* printf("Now entering background mode: \n");
			 fflush(stdout);*/
			 break; 

		 }
		else {	 	
			args[argcount] = indArgs; // populate array with our arguments
			indArgs = strtok_r(NULL, " ", &saveptr); //get next
			argcount++; 	
		}	
	}	
}



void cd() {
	if (strcmp(args[0], "cd") == 0 && argcount == 1) { //if first argument is equal to cd
		chdir(getenv("HOME")); // get env variable home and change directory
		
	}
	else {
		chdir(args[1]); // change directory to the one specified in argument 1
	}
}



void status() {
	
	//pid = waitpid(getpid(), &termstatus, 0);
	if (WIFEXITED(termstatus) == 1 ) {
		printf(" %d exited  with status %d\n", getpid(), WEXITSTATUS(termstatus));
	}
	else {
		printf(" %d exited abnormally due to signal %d\n", getpid(), WTERMSIG(termstatus));
	}
}



void otherCMD()
{
	  childpid = fork();
	
		switch (childpid) {
		case -1: // if child pid is -1 then there was an error forking
			perror("fork()");
			exit(1);
			break;
		case 0: // this is the child	
			if (background == 0) { //in foreground
				SIGINT_action.sa_handler = SIG_DFL;
				sigfillset(&SIGINT_action.sa_mask);
				sigaction(SIGINT, &SIGINT_action, NULL);
				if (InputfileName != NULL) {

					fd = open(InputfileName, O_RDONLY); // open input file name for read only

					if (fd == -1) { // error opening the file
						perror("open()");
						exit(1);

					}
					if (dup2(fd, 0) == -1) { // dup 2 error
						perror("dup2 error");
						exit(2);
					}

					close(fd); //close the file stream
				}
				else if (OutputfileName != NULL) {
					fd = open(OutputfileName, O_WRONLY | O_CREAT | O_TRUNC, 0644); // open output file name and set permissions
					if (fd == -1) {// error opening the file
						perror("open()");
						exit(1);

					}
					if (dup2(fd, 1) == -1) {// dup 2 error
						perror("dup2 error");
						exit(2);
					}
					close(fd);  //close the file stream
				}
			}
			else  { 
				
				// if we are in the background then redirect output to dev null
				fd = open("/dev/null", O_RDONLY); // open up dev/null
				if (fd == -1) {  // error opening the file
					perror("open()");
					exit(1);
				}
				if (dup2(fd, 0) == -1) { // dup 2 error
					perror("dup2 error");
					exit(2);
				}
				close(fd); //close the file stream
			}	
			 if (execvp(args[0], args)) {
				// exec whatever is the first argument
				perror("execvp"); // exec error
				termstatus = 1;
				exit(1);
				break;
			}		
		default: //this is the parent	
			if (background == 1) {
				printf("Background PID: %d\n", childpid);
				fflush(stdout);
			}
			if (background == 0) {
				childpid = waitpid(childpid, &termstatus, 0); // wait on child to exit
				if (WIFEXITED(termstatus) == 0) {
					printf("Terminated by signal: %d \n", WEXITSTATUS(termstatus));
					fflush(stdout);
				}

			}			
		}	
}

int main() {
	

	SIGINT_action.sa_handler = SIG_IGN;
	sigfillset(&SIGINT_action.sa_mask);
	sigaction(SIGINT, &SIGINT_action, NULL);

	SIGTSTP_action.sa_handler = catchSIGSTP;
	sigfillset(&SIGTSTP_action.sa_mask);
	SIGTSTP_action.sa_flags = SA_RESTART;
	sigaction(SIGTSTP, &SIGTSTP_action, NULL); // this segfaults 

	while (1) {
		printf(":");
		fflush(stdout);
		parseCmd();

		childpid = waitpid(-1, &termstatus, WNOHANG);


		if (childpid > 0) {
			if (WIFEXITED(termstatus)) { //termination success
				printf("Exit status: %d\n", WEXITSTATUS(termstatus));
			}
			else { 
				printf("Background PID: %d Terminating signal: %d\n", childpid, termstatus); // terminated by signal 
			}
			childpid = waitpid(-1, &termstatus, WNOHANG); // wait on the next child if there is none return imediately
		}


		if (strcmp(args[0], "\n") == 0 || strstr(args[0], "#")) { // checks for comment or empty line
			
			
		}


		else if (strcmp(args[0], "cd") == 0) { // executes cd command
			cd();
		}


		else if (strcmp(args[0], "status") == 0) { // executes status command
			status();
		}


		else if (strcmp(args[0], "exit") == 0 && argcount == 1) { // exits program
			exit(0);
		}


		else {
			otherCMD(); // execute all other commands
		}


		argcount = 0; //resets our global variables
		memset(args, 0, sizeof(args));
		InputfileName = NULL;
		OutputfileName = NULL;
		background = 0;
		foreground = 0;

		SIGINT_action.sa_handler = SIG_DFL;
		sigfillset(&SIGINT_action.sa_mask);
		sigaction(SIGINT, &SIGINT_action, NULL);
		
		
	}
}
