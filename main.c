#include <stdio.h>
/* #include <mpi.h> */
#include <math.h>
#include <stdlib.h>
#include <string.h>

void readFile(){

	FILE * fp;
	char * line_text = NULL;
	size_t len = 0;
	ssize_t read;

	fp = fopen("arxiv-citations.txt", "r");
	if (fp == NULL){
		printf("ERROR: Could not find file");
		return;
	}

	int line_num;
	line_num = 0;
	while ((read = getline(&line_text, &len, fp)) != -1) {
		printf("Line: %s\n", line_text);
	}
}
			
int main(){
	readFile();
}
