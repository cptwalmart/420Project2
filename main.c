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
	char *title;
	char *authors;
	char *abstract;
	/* struct Paper *cites; */
};

int me;
int nprocs;

char fullMetaFile[METAFILE_NUM_LINES];
char fullCitationsFile[CITATIONFILE_NUM_LINES];

/* struct Paper papers */

void printPaper(struct Paper p){
	/* printf("ID: %s\n", p.id); */
	printf("ID: ");
	int i;
	for (i = 0; i < 9; i++){
		printf("%c", p.id[i]);
	}
	printf("\n");
	/* printf("Title: %s\n", p.title); */
	/* printf("Authors: %s\n", p.authors); */
	/* printf("Abstract: %s\n", p.abstract); */
}

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

	int my_start, my_end, raw_block_size, leftover, final_block_size;
	raw_block_size = NUM_PAPERS_IN_METADATA / nprocs;
	leftover = NUM_PAPERS_IN_METADATA % nprocs;
	my_start = raw_block_size * me;
	my_end = (my_start + raw_block_size) - 1;
	my_end = 1;
	final_block_size = my_end - my_start;
	if (me == nprocs - 1){ //Add on remainder
		my_end += leftover;
	}

	struct Paper paper;
	struct Paper myPapers[final_block_size];

	char *id, *title, *authors, *abstract;
	id = (char*)malloc(1);
	title = (char*)malloc(1);
	authors = (char*)malloc(1);
	abstract = (char*)malloc(1);

	int line_num, onLineNum, paper_count, num_grabbed_papers;
	onLineNum = 0;
	num_grabbed_papers = 0;
	paper_count = 0;
	while ((read = getline(&line_text, &len, fp)) != -1) {
		/* printf("Line: %s\n", line_text); */
		if (line_text[0] == '+'){
			onLineNum = 0;
			paper_count++;
			struct Paper tmp;
			paper = tmp;

		}
		else if (paper_count >= my_start && paper_count <= my_end){ //Ignore if not in "my" section of file
			/* printf("Node %d gets: \n", me); */
			/* printf("ID: %s\n", line_text); */
			if (onLineNum == 0){ //Get id
				char tmp_id[strlen(line_text)];
				/* myPapers[num_grabbed_papers].id = malloc(sizeof(line_text)); */
				strcpy(tmp_id, line_text);
				myPapers[num_grabbed_papers].id = tmp_id;
				printf("TMP ID: %s\n", myPapers[num_grabbed_papers].id);
				/* myPapers[paper_count].id = line_text; */
				/* char tmp_id[strlen(line_text)]; */
				/* /1* tmp_id = malloc(sizeof(line_text)); *1/ */
				/* int i; */
				/* for (i = 0; i < strlen(line_text); i++){ */
				/* 	tmp_id[i] = line_text[i]; */
				/* } */
				/* /1* strcpy(tmp_id, line_text); *1/ */
				/* paper.id = tmp_id; */
				/* printf("node %d ID: %s\n", me, line_text); */
				/* free(id); */
				/* id = malloc(sizeof(line_text)); */
				/* id = line_text; */
				/* printf("id: %s\n", line_text); */
				/* paper.id = line_text; */
				/* char *tmp; */
				/* id = malloc(sizeof(line_text)); */
				/* strcpy(id, line_text); */
				/* id = line_text; */
				/* printf("id test: %s\n", id); */ 
				/* paper.id = tmp; */
				/* free(id); */
				/* id = malloc(sizeof(line_text)); */
				/* strcpy(id, line_text); */
				onLineNum++;
			}
			else if (onLineNum == 1){ //Get title
				char tmp_title[strlen(line_text)];
				/* myPapers[num_grabbed_papers].id = malloc(sizeof(line_text)); */
				strcpy(tmp_title, line_text);
				myPapers[num_grabbed_papers].title = tmp_title;
				printf("TMP TITLE: %s\n", myPapers[num_grabbed_papers].title);
				printf("TMP IDDD: %s\n", myPapers[num_grabbed_papers].id);
				/* myPapers[num_grabbed_papers].title = malloc(sizeof(line_text)); */
				/* strcpy(myPapers[num_grabbed_papers].title, line_text); */
				/* myPapers[num_grabbed_papers].title = line_text; */
				/* char *tmp_title; */
				/* tmp_title = malloc(sizeof(line_text)); */
				/* /1* strcpy(tmp_title, line_text); *1/ */
				/* paper.title = tmp_title; */
				/* printf("Title: %s\n", line_text); */
				/* free(title); */
				/* title = malloc(sizeof(line_text)); */
				/* title = line_text; */
				/* printf("title: %s\n", title); */
				/* printf("node %d Title: %s\n", me, line_text); */
				/* paper.title = line_text; */
				/* char *tmp; */
				/* paper.title = malloc(sizeof(line_text)); */
				/* paper.title = line_text; */
				/* printf("id test1: %s\n", id); */ 
				/* paper.title = tmp; */
				/* free(title); */
				/* title = malloc(sizeof(line_text)); */
				/* strcpy(title, line_text); */
				onLineNum++;
			}
			else if (onLineNum == 2){ //Get authors
				char tmp_authors[strlen(line_text)];
				/* myPapers[num_grabbed_papers].id = malloc(sizeof(line_text)); */
				strcpy(tmp_authors, line_text);
				myPapers[num_grabbed_papers].authors = tmp_authors;
				printf("TMP AUTHORS: %s\n", myPapers[num_grabbed_papers].authors);
				/* myPapers[num_grabbed_papers].authors = malloc(sizeof(line_text)); */
				/* strcpy(myPapers[num_grabbed_papers].authors, line_text); */
				/* myPapers[num_grabbed_papers].authors = line_text; */
				/* char *tmp_author; */
				/* tmp_author = malloc(sizeof(line_text)); */
				/* /1* strcpy(tmp_author, line_text); *1/ */
				/* paper.authors = tmp_author; */
				/* printf("Authors: %s\n", line_text); */
				/* free(authors); */
				/* authors = malloc(sizeof(line_text)); */
				/* authors = line_text; */
				/* printf("authors: %s\n", authors); */
				/* printf("node %d Author: %s\n", me, line_text); */
				/* paper.authors = line_text; */
				/* char *tmp; */
				/* paper.authors = malloc(sizeof(line_text)); */
				/* paper.authors = line_text; */
				/* printf("id test2: %s\n", id); */ 
				/* free(authors); */
				/* authors = malloc(sizeof(line_text)); */
				/* strcpy(authors, line_text); */
				/* paper.authors = tmp; */
				onLineNum++;
			}
			else if (onLineNum > 2){ //Get abstract
				char tmp_abstract[strlen(line_text)];
				/* myPapers[num_grabbed_papers].id = malloc(sizeof(line_text)); */
				strcpy(tmp_abstract, line_text);
				myPapers[num_grabbed_papers].abstract = tmp_abstract;
				printf("TMP ABSTRACT: %s\n", myPapers[num_grabbed_papers].abstract);
				/* myPapers[num_grabbed_papers].abstract = malloc(sizeof(line_text)); */
				/* strcpy(myPapers[num_grabbed_papers].abstract, line_text); */
				/* myPapers[num_grabbed_papers].abstract = line_text; */
				/* char *tmp_abstract; */
				/* tmp_abstract = malloc(sizeof(line_text)); */
				/* /1* strcpy(tmp_abstract, line_text); *1/ */
				/* paper.abstract = tmp_abstract; */
				/* printf("Abstract: %s\n", line_text); */
				/* free(abstract); */
				/* abstract = malloc(sizeof(line_text)); */
				/* abstract = line_text; */
				/* printf("abstract: %s\n", abstract); */
				/* printf("node %d Ab: %s\n", me, line_text); */
				/* paper.abstract = line_text; */
				/* struct Paper p; //= {"test1", "test2", "test3", "test4"}; */
				/* /1* myPapers[num_grabbed_papers] = {id, title, authors, abstract}; *1/ */
				/* myPapers[num_grabbed_papers] = p; */
				/* printPaper(myPapers[num_grabbed_papers]); */
				/* char *tmp; */
				/* paper.abstract = malloc(sizeof(line_text)); */
				/* paper.abstract = line_text; */				
				/* printf("id test3: %s\n", id); */ 

				/* p.id = id; */ 
				/* p.title = line_text; */ 
				/* p.authors = line_text; */ 
				/* p.abstract = line_text; */ 
				/* printPaper(p); */
				/* paper.abstract = tmp; */
				/* free(abstract); */
				/* abstract = malloc(sizeof(line_text)); */
				/* strcpy(abstract, line_text); */
				/* printf("id: %s\n", id); */
				/* printf("title: %s\n", title); */
				/* printf("authors: %s\n", authors); */
				/* printf("abstract: %s\n", abstract); */
				/* struct Paper p; */
				/* p.id = id; */
				/* p.title = title; */
				/* p.authors = authors; */
				/* p.abstract = abstract; */
				printPaper(myPapers[0]);
				num_grabbed_papers++;

			}
		}
	}
	/* if (me == 0){ */
		/* int i; */
		/* for(i = 0; i < my_end - my_start; i++){ */
		/* 	printPaper(myPapers[i]); */
		/* } */
	/* } */
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
	/* readInCitations(); */

	MPI_Finalize();
	return 0;
}
