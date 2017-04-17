#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

// This program create a shared Memory between the parent and the child process, 
// in which the parent process get the parameter from command line and the child
// process calculate the Fibonacci sequence using the parameter.

// define the symbol that allows the user to read from and write to the shared memory
#define PERM S_IRUSR|S_IWUSR
// define the max length of the Fibonacci sequence
#define MAX_SEQUENCE 15

// structure of the shared data
typedef struct {
	long fib_sequence[MAX_SEQUENCE];
	int sequence_size;
}shared_data;

// Function to calculate the Fibonacci number
long fib(long n) {
	if (n ==  0) { return 0; }
	if (n == 1) { return 1; }
	return fib(n - 1) + fib(n - 2);
}

int main() {
	key_t shmid;
	shared_data *p_addr, *c_addr;
	pid_t pid;
	int fibNum;
	int i;

	// accept the parameter passed on the command line
	printf("Enter the number to calculte the Fibonacii sequence: (must be < 15)\n");
	scanf("%d", &fibNum);
	// check if the parameter is < MAX_SEQUENCE
	while (fibNum < 0 || fibNum >= 15) {
		printf("The number must be >= 0 and < 15! Enter again: \n");
		scanf("%d", &fibNum);
	}

	// create a shared-memory segment of size shared_data
	if ((shmid = shmget(IPC_PRIVATE, sizeof(shared_data), PERM)) == -1) {
		fprintf(stderr, "Create Share Memory Error: %d\n\a", strerror(errno));
		exit(1);
	}

	// create the child process
	pid = fork();
	// attach the shared-memory segment to the address space of parent process
	if (pid > 0) {
		p_addr = (shared_data*)shmat(shmid, 0, 0);
		wait(NULL);

		// print the Fibonacii sequence from the shared-memory 
		for (i = 0; i < p_addr -> sequence_size; ++i) {
			printf("%ld ", p_addr -> fib_sequence[i]);
		}
		printf("\n");
		
		// detach the shared memory form the parent process
		if (shmdt(p_addr) == -1) { fprintf(stderr, "Unable to detach\n"); }
		// release the shared memory
		if (shmctl(shmid, IPC_RMID, 0) < 0) {
			printf("shmctl error:%d\n", strerror(errno));
			return -1;
		}
	}
	else if (pid == 0) {
		c_addr = (shared_data*)shmat(shmid, 0, 0);
		if (c_addr == (shared_data*) - 1) printf("shm shmat is fail\n");
		c_addr -> sequence_size = fibNum;

		// calculate the Fibonacii sequence
		for (i = 0; i < c_addr -> sequence_size; ++i) {
			c_addr -> fib_sequence[i] = fib(i);
		}
		// detach the shared memory form the child process
		if (shmdt(c_addr) == -1) { fprintf(stderr, "Unable to detach\n"); }
	}

	return 0;
}