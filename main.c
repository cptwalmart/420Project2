#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "hash.h"

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

int blocksize = 0;

char fullMetaFile[METAFILE_NUM_LINES];
char fullCitationsFile[CITATIONFILE_NUM_LINES];

char *normalize(char *word) {
  char *ret = calloc(strlen(word), sizeof(char));
  int i, j = 0;
  for(i = 0; i < strlen(word); i++) {
    if(isalnum(word[i])) {
      ret[j++] = tolower(word[i]);
    }
  }
  return ret;
}

/* struct Paper papers */

void printPaper(struct Paper p){
	printf("ID: %s\n", p.id);
	printf("Title: %s\n", p.title);
	printf("Authors: %s\n", p.authors);
	printf("Abstract: %s\n", p.abstract);
}

struct Paper *readInMetadata(){

	FILE * fp;
	char * line_text = NULL;
	size_t len = 0;
	ssize_t read;

	fp = fopen("arxiv-metadata.txt", "r");
	if (fp == NULL){
		printf("ERROR: Could not find file");
		return 0;
	}

	int my_start, my_end, raw_block_size, leftover;
	raw_block_size = NUM_PAPERS_IN_METADATA / nprocs;
	leftover = NUM_PAPERS_IN_METADATA % nprocs;
	my_start = raw_block_size * me;
	my_end = (my_start + raw_block_size) - 1;
	/* my_end = 10000; */
	blocksize = my_end - my_start;
	if (me == nprocs - 1){ //Add on remainder
		my_end += leftover;
	}

	struct Paper *myPapers = malloc(sizeof(struct Paper) * blocksize);

	int onLineNum, paper_count, num_grabbed_papers;
	onLineNum = 0;
	num_grabbed_papers = 0;
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
				/* char tmp_id[strlen(line_text)]; */
				/* myPapers[num_grabbed_papers].id = malloc(sizeof(line_text)); */
				/* strcpy(tmp_id, line_text); */
				myPapers[num_grabbed_papers].id = malloc(sizeof(char*)*strlen(line_text));
				strcpy(myPapers[num_grabbed_papers].id, line_text);
				myPapers[num_grabbed_papers].id[strlen(line_text) - 1] = 0; /* strip newline */
				onLineNum++;
			}
			else if (onLineNum == 1){ //Get title
				/* char tmp_title[strlen(line_text)]; */
				myPapers[num_grabbed_papers].title = malloc(sizeof(char*)*strlen(line_text));
				strcpy(myPapers[num_grabbed_papers].title, line_text);
				onLineNum++;
			}
			else if (onLineNum == 2){ //Get authors
				myPapers[num_grabbed_papers].authors = malloc(sizeof(char*)*strlen(line_text));
				strcpy(myPapers[num_grabbed_papers].authors, line_text);
				onLineNum++;
			}
			else if (onLineNum > 2){ //Get abstract
				myPapers[num_grabbed_papers].abstract = malloc(sizeof(char*)*strlen(line_text));
				strcpy(myPapers[num_grabbed_papers].abstract, line_text);
				/* printPaper(myPapers[num_grabbed_papers]); */
				num_grabbed_papers++;

			}
		}
	}
	return myPapers;
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

//Not used
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

void hashtable_print_contents(struct hashtable *h) {
  uint32_t bucket_idx;
  fprintf(stderr, "Hashtable at %p:\n", h);
  for(bucket_idx = 0; bucket_idx < HASH_BUCKETS; bucket_idx++) {
    if(h->buckets[bucket_idx]) {
      fprintf(stderr, "  Bucket %d:\n", bucket_idx);
      struct hashbucket *p = h->buckets[bucket_idx];
      while(p) {
	fprintf(stderr, "    Key '%s':\n", p->key);
	struct paper_list *l = p->value;
	while(l) {
	  fprintf(stderr, "      Paper '%s'\n", l->id);
	  l = l->next;
	}
	p = p->next;
      }
    }
  }
}


int main(){
	MPI_Init(NULL, NULL);
	MPI_Comm world = MPI_COMM_WORLD;
	MPI_Comm_size(world,  &nprocs);
	MPI_Comm_rank(world, &me);

	/* createFileLinesArray(); */

	fprintf(stderr, "Reading papers...\n");
	struct Paper *papers;
	papers = readInMetadata();
	fprintf(stderr, "Hashing papers...\n");
	struct hashtable h;
	hashtable_init(&h);
	
	int i;
	for(i = 0; i < blocksize; i++) {
	  char *wptr = strdup(papers[i].abstract), *sep;
	  char *orig_wptr = wptr;
	  while((sep = strchr(wptr, ' '))) {
	    *sep = '\0';
	    char *normalized = normalize(wptr);
	    if(strlen(normalized) > 3) {
	      hashtable_append(&h, normalized, papers[i].id);
	    } else {
	      free(normalized);
	    }
	    wptr = sep + 1;
	  }
	  free(orig_wptr);
	  if(i % 1000 == 999) {
	    fprintf(stderr, " %d / %d\r", i, blocksize);
	    fflush(stderr);
	  }
	}
	fprintf(stderr, "\n");
	/* readInCitations(); */

	MPI_Finalize();
	return 0;
}
