#include <stdio.h>

int main() 
{
	char srcFile[30];
	char desFile[30];
	char content;
	FILE* psrcFile;
	FILE* pdesFile;

	printf("Enter the source file name: ");
	scanf("%s", srcFile);

	printf("Enter the destination file name: ");
	scanf("%s", desFile);

	if (!(psrcFile = fopen(srcFile, "r"))) {
		fprintf(stderr, "The source file doesn\'t exist!\n");
		exit(-1);
	}

	if (pdesFile = fopen(desFile, "r")) {
		fprintf(stderr, "The destination file has existed!\n");
		exit(-1);
	}

	pdesFile = fopen(desFile, "w");

	while ((content = fgetc(psrcFile)) != EOF) {
		fputc(content, pdesFile);
	}

	fclose(psrcFile);
	fclose(pdesFile);

	printf("File copy completed!\n");

	return 0;
}