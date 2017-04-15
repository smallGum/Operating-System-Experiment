#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
	int pid1 = -1, pid2 = -1;
	pid1 = fork();                        // create first child process
	if (pid1) { pid2 = fork(); }      // create second child process

	// print different character according to different process
	if (pid1 != 0 && pid2 != 0) { printf("a\n"); }
	else if (pid1 == 0) { printf("b\n"); }
	else if (pid2 == 0) { printf("c\n"); }

	return 0;
}