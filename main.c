#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "hash.h"
#include "matops.h"

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
};

struct Result{
	char *id;
	double pagerank
};

int me;
int nprocs;

int blocksize = 0;

char fullMetaFile[METAFILE_NUM_LINES];
char fullCitationsFile[CITATIONFILE_NUM_LINES];

struct hashtable id_map; //Store id -> index map
struct SparseMatrix adj_mat;
struct HitsPrMatrix hitsPr_mat;

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
int getIndexFromId(char *id){
	FILE * fp;
	char * line_text = NULL;
	size_t len = 0;
	ssize_t read;

	fp = fopen("id_map.txt", "r");
	if (fp == NULL){
		printf("ERROR: Could not find file: id_map.txt");
		return -1;
	}

	while ((read = getline(&line_text, &len, fp)) != -1) {
		if (line_text[0] == id[0]){
			int i, match;
			i = 0;
			match = 1;
			while (line_text[i] != ':'){
				if (line_text[i] != id[i]){
					match = 0;
					break;
				}
				i++;
			}
			if (match == 1){
				char num_str[15];
				i++;
				int j;
				j = 0;
				while (line_text[i] != '\0'){
					num_str[j] = line_text[i];		
					j++;
					i++;
				}
				int num = atoi(num_str);
				/* printf("Found ID: %d\n", num); */
				fclose(fp);
				return num;
			}
		}
	}
	fclose(fp);
	printf("Failed to find index for ID: %s\n", id);
	return -1;
}
struct Paper *readInMetadata(){

	FILE * fp;
	char * line_text = NULL;
	size_t len = 0;
	ssize_t read;

	fp = fopen("arxiv-metadata.txt", "r");
	if (fp == NULL){
		printf("ERROR: Could not find file: arxiv-metadata.txt");
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
	/* FILE *f = fopen("id_map.txt", "w"); */
	/* if (f == NULL) */
	/* { printf("Error opening file!\n"); */
	/* 	exit(1); */
	/* } */
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

				//Create id->row index map. 
				/* char *line = malloc(100); */
				/* strcat(line, line_text); */
				/* strtok(line, "\n"); */
				/* strcat(line, ":"); */
				/* char tmp[15]; */
				/* sprintf(tmp, "%d", paper_count); */
				/* strcat(line, tmp); */

				/* /1* const char *text = "Write this to the file"; *1/ */
				/* fprintf(f, "%s\n", line); */
				/* free(line); */

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
	/* fclose(f); */
	return myPapers;
}

// TEST
void hashtable_print_contents(struct hashtable *h) {
  uint32_t bucket_idx = 38;
  fprintf(stderr, "Hashtable at %p:\n", h);
  //for(bucket_idx = 0; bucket_idx < HASH_BUCKETS; bucket_idx++) {
   // if(h->buckets[bucket_idx]) {
      fprintf(stderr, "  Bucket %d:\n", bucket_idx);
      struct hashbucket *p = h->buckets[bucket_idx];
      while(p) {
	fprintf(stderr, "    Key '%s':\n", p->key);
	struct paper_list *l = p->value;
	while(l) {
	  fprintf(stderr, "      Paper '%s'\n", l->id);
	  l = l->next;
	//}
	p = p->next;
      }
   // }
  }
}

void createAdjMatrix(){

	FILE * fp;
	char * line_text = NULL;
	size_t len = 0;
	ssize_t read;

	fp = fopen("arxiv-citations.txt", "r");
	if (fp == NULL){
		printf("ERROR: Could not find file: arxiv-citations.txt");
		return;
	}

	int my_start, my_end, raw_block_size, leftover;
	/* raw_block_size = NUM_PAPERS_IN_CITATIONS / nprocs; */
	raw_block_size = NUM_PAPERS_IN_CITATIONS / nprocs; //Changed since this doesnt need to be in parallel anymore
	leftover = NUM_PAPERS_IN_CITATIONS % nprocs;
	my_start = raw_block_size * me;
	my_end = (my_start + raw_block_size) - 1;
	if (me == nprocs - 1){ //Add on remainder
		my_end += leftover;
	}

	int paper_count, citation_count, readingCitationsId;
	paper_count = 0;
	citation_count = 0;
	readingCitationsId = 0;

	FILE *f = fopen("sparse_mat.txt", "w");
	if (f == NULL){
		printf("Error opening file!\n");
		exit(1);
	}

	hashtable_init(&id_map);
	citation_count = 0;

	while ((read = getline(&line_text, &len, fp)) != -1){
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
				/* fprintf(f, "%d", myIndex); */
				/* fprintf(f, "%s", ","); */
				/* fprintf(f, "%d\n", myCitationIndex); */
				
			}
			else { //Read my ID
				/* printf("node %d reading my id: %s\n", me, line_text); */
				/* myIndex = getIndexFromId(line_text); */
				char str[100];
				sprintf(str, "%d", paper_count);
				strtok(line_text, "\n"); //remove end of line character
				hashtable_append(&id_map, line_text, str); 

			}
		}
	}
	printf("Num citations: %d\n", citation_count);
	fclose(fp);
	
	FILE * fp2;
	line_text = NULL;
	len = 0;
	ssize_t read2;
	fp2 = fopen("arxiv-citations.txt", "r");
	if (fp2 == NULL){
		printf("ERROR: Could not find file: arxiv-citations.txt");
		return;
	}

	int myIndex, myCitationsIndex;

	paper_count = 0;
	citation_count = 0;
	readingCitationsId = 0;

	adj_mat = initSparseMatrix();

	hitsPr_mat = initHITSPRMatrix();

	while ((read2 = getline(&line_text, &len, fp2)) != -1){
		if (line_text[0] == '+'){ //Trigger for reading next paper
			readingCitationsId = 0;
			paper_count++;
		}
		else if (paper_count >= my_start && paper_count <= my_end){ //Ignore if not in "my" section of file
			if (line_text[0] == '-' && line_text[1] == '-'){ //Trigger to start reading citations
				readingCitationsId = 1;
			}
			else if (readingCitationsId){ //Read citation IDs
				/* printf("node %d reading citations:  %s\n", me, line_text); */
				strtok(line_text, "\n"); //remove end of line character
				struct paper_list *l = hashtable_get(&id_map, line_text);
				if (l ==  NULL){
					continue;
				}
				myCitationsIndex = atoi(l->id);
				addSparseValue(&adj_mat, myIndex, myCitationsIndex);
				hitsPr_mat.hub_score[myIndex]++;
				hitsPr_mat.auth_score[myCitationsIndex]++;

				citation_count++;
				/* printf("my index: %d my citation index: %d\n", myIndex, myCitationsIndex); */
			}
			else { //Read my ID
				strtok(line_text, "\n"); //remove end of line character
				struct paper_list *l = hashtable_get(&id_map, line_text);
				myIndex = atoi(l->id);

			}
		}
	}
	printSparseValue(adj_mat, 39);
	printSparseValue(adj_mat, 40);
	printf("HubScore: %f\n", hitsPr_mat.hub_score[38]);
	printf("AuthorityScore: %f\n", hitsPr_mat.auth_score[38]);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Insert PageRank Iteration Loop here.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int i;
	int bINDEXb = 0;
	int SearchIndex;
	double tmpPR = 0;
	int k;
	//printf("Citation #%d HubScore: %.0f, AuthorityScore: %.0f, PageRank: %f\n", 38, hitsPr_mat.hub_score[38], hitsPr_mat.auth_score[38], hitsPr_mat.newpage_rank[38]);
	for(k = 0; k < 5; k++){
		if(k % 2 == 0){
			for(i = 0; i < 1305086; i++){
				int j;
				// if(i > 1300000){
				// 	printf("%d %d %d\n",k, i, bINDEXb);
				// }
				if((hitsPr_mat.auth_score[i] > 0) && (hitsPr_mat.hub_score[i] > 0)){
					for(j = 0; j < hitsPr_mat.auth_score[i]; j++){
						SearchIndex = adj_mat.col_index[bINDEXb];
						if(hitsPr_mat.hub_score[SearchIndex] > 0){
							tmpPR = tmpPR + (hitsPr_mat.oldpage_rank[SearchIndex] / hitsPr_mat.hub_score[SearchIndex]);
						}
						bINDEXb++;
					}
					hitsPr_mat.newpage_rank[i] = tmpPR;
				}
				bINDEXb++;
			}
		}

		else{
			for(i = 0; i < 1305086; i++){
				int j;
				// if(i % 100000 == 0){
				// 	printf("%d %d\n",k, i);
				// }
				if((hitsPr_mat.auth_score[i] > 0) && (hitsPr_mat.hub_score[i] > 0)){
					for(j = 0; j < hitsPr_mat.auth_score[i]; j++){
						SearchIndex = adj_mat.col_index[bINDEXb];
						if(hitsPr_mat.hub_score[SearchIndex] > 0){
							tmpPR = tmpPR + (hitsPr_mat.newpage_rank[SearchIndex] / hitsPr_mat.hub_score[SearchIndex]);
						}
						bINDEXb++;
					}
					hitsPr_mat.oldpage_rank[i] = tmpPR;
				}
				bINDEXb++;
			}
		}
		bINDEXb = 0;
		tmpPR = 0;
	}

	// struct paper_list *l = hashtable_get(&h, "alg-geom/9412017");
	
	// printf("Value: %s\n", l->id);
	//hashtable_print_contents(&h);
	//struct SparseMatrix *ptr = &adj_mat;
	//ptr = ptr + atoi(l->id);

	int aINDEXa;
	for(aINDEXa = 0; aINDEXa < 40; aINDEXa++){
		printf("Citation #%d HubScore: %.0f, AuthorityScore: %.0f, PageRank: %E\n", aINDEXa, hitsPr_mat.hub_score[aINDEXa], hitsPr_mat.auth_score[aINDEXa], hitsPr_mat.newpage_rank[aINDEXa]);
	}
	// printSparseValue(adj_mat, aINDEXa);
	// aINDEXa++;
	// printSparseValue(adj_mat, aINDEXa);
	// aINDEXa++;
	// printSparseValue(adj_mat, aINDEXa);
	// printf("HubScore: %f\n", hitsPr_mat.hub_score[38]);
	// printf("AuthorityScore: %f\n", hitsPr_mat.auth_score[38]);

	fclose(f);
}

void sortResults(struct Result *results, int size){
	int i, j;
	for (i = 0; i < size-1; i++){
		for (j = 0; j < size-i-1; j++){
			if (results[j].pagerank > results[j+1].pagerank){
				struct Result tmp = results[j];
				results[j] = results[j+1];
				results[j+1] = tmp;
			}
		}
	}
}

void searchWord(struct hashtable h, char *word){
	int curr_result = 0;

	char *normalized = normalize(word);
	int bucketSize = getBucketSize(&h, normalized);
	struct Result *results = malloc (bucketSize * sizeof(struct Result));

	struct paper_list *l = hashtable_get(&h, normalized); 


	while(l){
		struct paper_list *paper_index = hashtable_get(&id_map, l->id);
		if (paper_index ==  NULL){
			l = l->next;
			results[curr_result].id = "nil";
			results[curr_result].pagerank = 0;
			curr_result++;
			continue;
		}
		int myIndex = atoi(paper_index->id);
		double pr = hitsPr_mat.oldpage_rank[myIndex];
		results[curr_result].id = l->id;
		results[curr_result].pagerank = pr;
		curr_result++;

		l = l->next;
	}

	sortResults(results, bucketSize);
	int p, pagesToShow;
	pagesToShow = 5;
	if (bucketSize < 5){
		pagesToShow = bucketSize;
	}

	printf("========== Results for %s ==========\n", normalized);
	for (p = bucketSize- 1; p >  bucketSize - pagesToShow; p--){
		printf("ID: %s\n", results[p].id);
		printf("Pagerank: %f\n", results[p].pagerank);
		printf("------------------------------\n");
	}
	free(results);

}

int main(){
	MPI_Init(NULL, NULL);
	MPI_Comm world = MPI_COMM_WORLD;
	MPI_Comm_size(world,  &nprocs);
	MPI_Comm_rank(world, &me);

	fprintf(stderr, "%d Reading papers...\n", me);
	struct Paper *papers;
	papers = readInMetadata();
	fprintf(stderr, "%d Hashing papers...\n", me);
	struct hashtable h;
	hashtable_init(&h);

	int i;
	//ONLY DOING HALF PAPERS. DONT FORGET TO REMOVE!!
	for(i = 0; i < blocksize/8; i++) {
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
	}
	fprintf(stderr, "%d Serializing hash...\n", me);
	int length = hashtable_serialized_length(&h);
	fprintf(stderr, "%d Hashtable size: %d\n", me, length);
	char *serialized = malloc(length);
	serialize_hashtable(&h, serialized);

	fprintf(stderr, "%d Serialization complete.\n", me);

	int *serialized_sizes;
	if(me == 0) {
		serialized_sizes = calloc(nprocs, sizeof(int));
	}
	MPI_Gather(&length, 1, MPI_INT, serialized_sizes, 1, MPI_INT, 0, world);
	if(me == 0) {
		int i;
		for(i = 0; i < nprocs; i++) {
			fprintf(stderr, "Size of %d's table: %d\n", i, serialized_sizes[i]);
		}
	}
	if(me == 0) {
		int i;
		for(i = 1; i < nprocs; i++) {
			fprintf(stderr, "Receiving from %d\n", i);
			char *recv_serial = calloc(serialized_sizes[i], 1);
			MPI_Recv(recv_serial, serialized_sizes[i], MPI_BYTE, i, 0, world, MPI_STATUS_IGNORE);
			fprintf(stderr, "Merging from %d\n", i);
			deserialize_hashtable(recv_serial, &h);
			free(recv_serial);
		}
	} else {
		MPI_Send(serialized, length, MPI_BYTE, 0, 0, world);
	}

	if (me == 0){
		createAdjMatrix();

		printf("Google: ");
		fflush( stdout );

		char phrase[100];
		char tmp[100];
		fgets(phrase, 100, stdin);
		strcpy(tmp, phrase);

		char* token = strtok(phrase, " "); 

		int numLoops = 0;
	  
		while (token != NULL) {  
			searchWord(h, token);
			token = strtok(NULL, " "); 
			numLoops++;
		} 
		if (numLoops > 1){ //The user entered multiple words. Do seach on full string
			searchWord(h, tmp);
		}
		printf("INPUT SIZE: %d and word: %s\n", strlen(tmp), tmp);
	}
	
	MPI_Finalize();
	return 0;
}
