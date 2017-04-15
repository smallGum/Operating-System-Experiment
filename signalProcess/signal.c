#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void waiting();
void stop();

int wait_mark;

void main() {
	int pid1, pid2;

	// make sure to create the child process 1 successfully
	while ((pid1 = fork()) == -1);
	if (pid1 > 0) {
		// make sure to create the child process 2 successfully
		while ((pid2 = fork()) == -1);

		// choose the parent process to catch "ctrl + C" signal
		// and send signals to child processes
		if (pid2 > 0) {
			wait_mark = 1;
			signal(SIGQUIT, SIG_IGN);
			signal(SIGINT, stop);     // when the "ctrl + C" signal is received, excute stop function
			waiting();                // wait until the "ctrl + C" signal is received

			// send different signals to the two child processes
			kill(pid1, 16);
			kill(pid2, 17);
			// wait until the child processes received the signals
			wait(0);
			wait(0);

			// print message when all the child processes stop
			printf("parent process is killed!\n");
			exit(0);
		}
		else {   // the child process 2 deals with the signal 17
			wait_mark = 1;
			signal(SIGINT, SIG_IGN);
			signal(SIGQUIT, SIG_IGN);
			signal(17, stop);
			waiting();      

			// print the message when the signal 17 is received.
			printf("child process 2 is killed by parent!\n");
			exit(0);
		}
	}
	else {   // the child process 1 deals with the signal 16
		wait_mark = 1;
		signal(SIGINT, SIG_IGN);
		signal(SIGQUIT, SIG_IGN);
		signal(16, stop);
		waiting();

		// print the message when the signal 17 is received.
		printf("child process 1 is killed by parent!\n");
		exit(0);
	}
}

void waiting() {
	// death loop when wait_mark == 1
	while (wait_mark);
}
void stop() {
	// set wait_mark = 0 and stop the death loop
	wait_mark = 0;
}