/**
* This program creates a simple shell to read
* and execute instructions from command-line. 
*/

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

// The max number of characters next command could have
#define MAX_LINE 80

char commands[10][MAX_LINE];   // global variable to store the recent 10 commands entered
int numCommand;                // the actual number of the commands stored in commands[] 

void initCommands() {
	int i;
	for (i = 0; i < 10; ++i) {
		commands[i][0] = '\0';
	}
	numCommand = 0;
}

// display all recent commands. (at most 10)
void disAllRecentCommands() {
	int i;
	printf("\n");
	for (i = (numCommand - 1 + 10) % 10; (i + 10) % 10 != numCommand && commands[(i + 10) % 10][0] != '\0'; --i) {
		printf("%s", commands[(i + 10) % 10]);
	}
	if (commands[numCommand][0] != '\0') { printf("%s", commands[numCommand]); }
	//printf("%s", commands[(numCommand - 2 + 10) % 10]);
	//printf("%s", commands[(numCommand - 3 + 10) % 10]);
	printf(" COMMAND->");
	fflush(stdout);
}

// execute the command whose first letter is x where x is in "r x"
int excuteXCommand(char *inputBuffer, char x) {
	int i;
	for (i = (numCommand - 1 + 10) % 10; (i + 10) % 10 != numCommand && commands[(i + 10) % 10][0] != '\0'; --i) {
		if (x == commands[(i + 10) % 10][0]) {
			strncpy(inputBuffer, commands[(i + 10) % 10], strlen(commands[(i + 10) % 10]));
			return strlen(commands[(i + 10) % 10]);
		}
	}
	if (x == commands[numCommand][0]) {
		strncpy(inputBuffer, commands[numCommand], strlen(commands[numCommand]));
		return strlen(commands[numCommand]);
	}

	return -1;
}

/**
* setup() reads in the next command line, separating it into 
* distinct tokens using whitespace as delimiters.
* setup() modifies the args parameter so that it holds pointers 
* to the null-terminated strings that are the tokens in the most
* recent user command line.
* setup() calls exit() when "ctrl D" is entered
*
* @param inputBuffer the most recent user command line
* @param args the string array to store the parameters of the command
* @param background flag to represent whether the program should run in background
*/
void setup(char inputBuffer[], char *args[], int *background) {
	int length;                  // number of characters in the command
	int i;                       // loop counter
	int start;                   // location of the first character in the command
	int ct;                      // location in args[] for the next argument

	ct = 0;

	// read the command and save it into inputBuffer
	length = read(STDIN_FILENO, inputBuffer, MAX_LINE);

	if (inputBuffer[0] == 'r' && length > 2) {
		length = excuteXCommand(inputBuffer, inputBuffer[2]);
		inputBuffer[length] = '\0';
		printf(" COMMAND-> %s", inputBuffer);
	}

	start = -1;
	if (length == 0) { exit(0); }   // enter 'ctrl D', exit the program
	if (length < 0) {
		perror("error reading the command");
		exit(-1);                   // when error occurs, quit the shell
	}

	// store the latest command to commands[]
	strcpy(commands[numCommand], inputBuffer);
	commands[numCommand][length] = '\0';
	numCommand = (numCommand + 1) % 10;

	if (inputBuffer[0] == 'r' && length == 2) {
		strncpy(inputBuffer, commands[(numCommand - 2 + 10) % 10], strlen(commands[(numCommand - 2 + 10) % 10]));
		length = strlen(commands[(numCommand - 2 + 10) % 10]);
		inputBuffer[length] = '\0';
		printf(" COMMAND-> %s", inputBuffer);
	}

	// scan the inputBuffer
	for (i = 0; i < length; i++) {
		switch (inputBuffer[i]) {
			// if space and tab are read, save a new argument to args
			case ' ':
			case '\t':
				if (start != -1) {
					args[ct] = &inputBuffer[start];
					ct++;
				}
				inputBuffer[i] = '\0';   //null-terminated character to end the argument
				start = -1;
				break;

			// if a new line is read, the command is ended
			case '\n':
				if (start != -1) {
					args[ct] = &inputBuffer[start];
					ct++;
				}
				inputBuffer[i] = '\0';
				args[ct] = NULL;   // arguments ends 
				break;

			// if other characters are read, change the start
			default:
				if (start == -1) { start = i; }
				if (inputBuffer[i] == '&') {
					*background = 1;
					inputBuffer[i] = '\0';
				}
		}
	}
	args[ct] = NULL;
}

int main() {
  char inputBuffer[MAX_LINE];    // buffer to hold command entered
  int background;                // equals 1 if a command is followed by '&'
  char *args[MAX_LINE / 2 + 1];  // command line arguments
  pid_t pid;

  initCommands();
  signal(SIGINT, disAllRecentCommands);

  while (1) {
    background = 0;   // default to run the command on the foreground
    printf(" COMMAND->");
    fflush(stdout);

    setup(inputBuffer, args, &background);

    // create the child process
    // the parent process will wait the child process to finish executing the command
    // if background == 0
    if ((pid = fork()) == -1) { printf("Fork Error.\n"); }
    if (pid == 0) { execvp(args[0], args); exit(0); }
    if (background == 0) { waitpid(pid); }
  }

  return 0;
}