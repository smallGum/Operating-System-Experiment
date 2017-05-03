/* 
   This program uses the parent thread to receive the number
   from command line and print the Fibonacci sequence while
   the child thread will generate the Fibonacci sequence 
   corresponding to the number.
*/
#include <stdio.h>
#include <pthread.h>

#define MAX_NUM 30

// declare the Fibonacci array
int Fibonacci[30];

// get the value of fib(n)
int fib(int n) {
	if (n == 0) return 0;
	if (n == 1) return 1;
	return fib(n - 1) + fib(n - 2);
}

// generate the Fibonacci sequence with fnum numbers
void* generate(void* fnumAddr) {
	int* fnum = (int*)fnumAddr;
	int i;
	for (i = 0; i <= *fnum; ++i) {
		Fibonacci[i] = fib(i);
	}

	pthread_exit(0);
}

// print the Fibonacci sequence
void print(int fnum) {
	printf("Generated the Fibonacci sequence:\n");
	int i;
	for (i = 0; i <= fnum; ++i) {
		printf("%d ", Fibonacci[i]);
	}
}

int main() {
	int fnum;
	pthread_t tid;
	pthread_attr_t attr;

	printf("Enter the number to generate the Fibonacci sequence (must be less than 30): ");
	scanf("%d", &fnum);

	while (fnum >= MAX_NUM) {
		printf("The number must be less than 30!\n");
		printf("Enter again: ");
		scanf("%d", &fnum);
	}

	// create a child thread to generate the Fibonacci sequence
	pthread_attr_init(&attr);
	pthread_create(&tid, &attr, generate, (void*)&fnum);
	pthread_join(tid, NULL);

	print(fnum);
	printf("\n");

	return 0;
}