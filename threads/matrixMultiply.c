/*
   This program uses M * N worker threads to multiply 
   matrix A(M * K) and matrixB (K * N). Each thread 
   calculate one element Cij of the result matrix C.
*/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define M 3
#define K 2
#define N 3

// generate the matries
int A[M][K] = { {1, 4}, {2, 5}, {3, 6} };
int B[K][N] = { {8, 7, 6}, {5, 4, 3} };
int C[M][N];

// define the parameter struct to pass to pthread_create
typedef struct
{
	int row;
	int col;
}position;

// calculate Cij in one thread
void* calCij(void* pos) {
	position* posi = (position*)pos;
	int i;
	C[posi -> row][posi -> col] = 0;

	printf("row: %d, col: %d\n", posi -> row, posi -> col);

	for (i = 0; i < K; ++i) {
		C[posi -> row][posi -> col] += A[posi -> row][i] * B[i][posi -> col];
	}

	free(pos);
	pthread_exit(0);
}

// multiply the matries
void multiplyAB() {
	int i, j;
	pthread_t tids[M][N];
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	for (i = 0; i < M; ++i) {
		for (j = 0; j < N; ++j) {
			position* pos = (position*)malloc(sizeof(position));
			pos -> row = i;
			pos -> col = j;

			pthread_create(&tids[i][j], &attr, calCij, (void*)pos);
		}
	}

	for (i = 0; i < M; ++i) {
		for (j = 0; j < N; ++j) {
			pthread_join(tids[i][j], NULL);
		}
	}
}

// print the result matrix C
void print() {
	int i, j;
	printf("A * B =\n");
	for (i = 0; i < M; ++i) {
		for (j = 0; j < N; ++j) {
			printf("%d ", C[i][j]);
		}
		printf("\n");
	}
}

int main() {
	multiplyAB();
	print();

	return 0;
}