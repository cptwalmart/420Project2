#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#define METAFILE_NUM_LINES 8073560
#define CITATIONFILE_NUM_LINES 10913892
#define NUM_PAPERS_IN_METADATA 1614712
#define NUM_PAPERS_IN_CITATIONS 1354753
/* #define NUM_PAPERS_IN_CITATIONS 61 */
/* #define NUM_PAPERS_IN_METADATA 30 */

struct Paper{
	char *id;
	char *words;
	struct Paper *cites;
};

int me;
int nprocs;

char fullMetaFile[METAFILE_NUM_LINES];
char fullCitationsFile[CITATIONFILE_NUM_LINES];

void readInMetadata(){

	FILE * fp;
	char * line_text = NULL;
	size_t len = 0;
	ssize_t read;

	fp = fopen("arxiv-metadata.txt", "r");
	if (fp == NULL){
		printf("ERROR: Could not find file");
		return;
	}

	int my_start, my_end, raw_block_size, leftover;
	raw_block_size = NUM_PAPERS_IN_METADATA / nprocs;
	leftover = NUM_PAPERS_IN_METADATA % nprocs;
	my_start = raw_block_size * me;
	my_end = (my_start + raw_block_size) - 1;
	if (me == nprocs - 1){ //Add on remainder
		my_end += leftover;
	}

	int line_num, onLineNum, paper_count;
	onLineNum = 0;
	paper_count = 0;
	while ((read = getline(&line_text, &len, fp)) != -1) {
		/* printf("Line: %s\n", line_text); */
		if (line_text[0] == '+'){
			onLineNum = 0;
			paper_count++;
		}
		else if (paper_count >= my_start && paper_count <= my_end){ //Ignore if not in "my" section of file
			/* printf("Node %d gets: \n", me); */
			/* printf("ID: %s\n", line_text); */
			if (onLineNum == 0){ //Get id
				/* printf("node %d ID: %s\n", me, line_text); */
				onLineNum++;
			}
			else if (onLineNum == 1){ //Get title
				/* printf("Title: %s\n", line_text); */
				/* printf("node %d Title: %s\n", me, line_text); */
				onLineNum++;
			}
			else if (onLineNum == 2){ //Get authors
				/* printf("Authors: %s\n", line_text); */
				/* printf("node %d Author: %s\n", me, line_text); */
				onLineNum++;
			}
			else if (onLineNum > 2){ //Get abstract
				/* printf("Abstract: %s\n", line_text); */
				/* printf("node %d Ab: %s\n", me, line_text); */
				onLineNum++;

			}
		}
	}
}

void readInCitations(){

	FILE * fp;
	char * line_text = NULL;
	size_t len = 0;
	ssize_t read;

	fp = fopen("arxiv-citations.txt", "r");
	if (fp == NULL){
		printf("ERROR: Could not find file");
		return;
	}

	int my_start, my_end, raw_block_size, leftover;
	raw_block_size = NUM_PAPERS_IN_CITATIONS / nprocs;
	leftover = NUM_PAPERS_IN_CITATIONS % nprocs;
	my_start = raw_block_size * me;
	my_end = (my_start + raw_block_size) - 1;
	if (me == nprocs - 1){ //Add on remainder
		my_end += leftover;
	}

	int paper_count, readingCitationsId;
	paper_count = 0;
	readingCitationsId = 0;
	while ((read = getline(&line_text, &len, fp)) != -1) {
		/* printf("Line: %s\n", line_text); */
		/* printf("On word: %d\n", paper_count); */
		if (line_text[0] == '+'){ //Trigger for reading next paper
			readingCitationsId = 0;
			paper_count++;
			/* printf("node %d reading +++: %s\n", me, line_text); */
		}
		else if (paper_count >= my_start && paper_count <= my_end){ //Ignore if not in "my" section of file
			/* printf("Entering\n"); */
			if (line_text[0] == '-' && line_text[1] == '-'){ //Trigger to start reading citations
				/* printf("node %d reading ---: %s\n", me, line_text); */
				readingCitationsId = 1;
			}
			else if (readingCitationsId){ //Read citation IDs
				/* printf("node %d reading citations:  %s\n", me, line_text); */
				
			}
			else { //Read my ID
				/* printf("node %d reading my id: %s\n", me, line_text); */

			}
		}
	}
}
void createFileLinesArray(){
	MPI_File fh1;
	MPI_File_open(MPI_COMM_WORLD, "arxiv-metadata.txt", MPI_MODE_RDWR, MPI_INFO_NULL, &fh1 );
	if (me == 0){ //Node 0 puts all words into char array
		MPI_File_read_shared(fh1, fullMetaFile, METAFILE_NUM_LINES, MPI_CHAR, NULL);
	}
	MPI_File fh2;
	MPI_File_open(MPI_COMM_WORLD, "arxiv-citations.txt", MPI_MODE_RDWR, MPI_INFO_NULL, &fh2 );
	if (me == 0){ //Node 0 puts all words into char array
		MPI_File_read_shared(fh2, fullCitationsFile, CITATIONFILE_NUM_LINES, MPI_CHAR, NULL);
	}


	int i;
	for(i = 0; i < 10; i++){
		printf("TEXT META: %c\n", fullMetaFile[i]);

	}
	for(i = 0; i < 10; i++){
		printf("TEXT CIT: %c\n", fullCitationsFile[i]);

	}
	/* fclose(fp1); */
	/* fclose(fp2); */
}
			
int main(){
	MPI_Init(NULL, NULL);
	MPI_Comm world = MPI_COMM_WORLD;
	MPI_Comm_size(world,  &nprocs);
	MPI_Comm_rank(world, &me);

	/* createFileLinesArray(); */

	readInMetadata();
	readInCitations();

	MPI_Finalize();
	return 0;
}
