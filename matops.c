#include <stdio.h>
#include <stdlib.h>
#include "matops.h"
#define INDEX(n,m,i,j) m*i + j //n=rows m=cols
#define ACCESS(A,i,j) A->arr[INDEX(A->rows, A->cols, i, j)]


float* matrix_add(float* A, float* B, int blockSize){
	int i;
	float *sum;
	sum = (float*) malloc(sizeof(float) * blockSize);

	for (i = 0; i < blockSize; i++){
		sum[i] = A[i] + B[i];	
	}

	return sum;
}
float* matrix_sub(float* A, float* B, int blockSize){
	int i;
	float *sum;
	sum = (float*) malloc(sizeof(float) * blockSize);

	for (i = 0; i < blockSize; i++){
		sum[i] = A[i] - B[i];	
	}

	return sum;
}

float vector_mult(float* A, float* B, int blockSize){
	int i;
	float sum;
	sum = 0;
	for(i = 0; i < blockSize; i++){
		sum += A[i] * B[i];	
	}
	return sum;
}
struct matrix matrixMult(matrix *A, matrix *B){
	int i, j, k;
	struct matrix C;
	initMatrix(&C, A->rows, B->cols);
	for (i = 0; i < A->rows; i++){
		for (j = 0; j < B->cols; j++){
			float sum;
			sum = 0;
			for (k = 0; k < B->rows; k++){
				sum += A->arr[i * A->cols + k] * B->arr[k * B->cols + j];
			}
			C.arr[i * C.cols + j] = sum;
		}
	}
	return C;
}

struct matrix matrixNormalize(matrix *A){
	int i, j, k;
	float mat_min = getMatrixMin(A);
	for (i = 0; i < A->rows * A->cols; i++){
		A->arr[i] = A->arr[i] - mat_min;
	}
	float mat_max = getMatrixMax(A);
	for (i = 0; i < A->rows * A->cols; i++){
		A->arr[i] = A->arr[i] / mat_max;
	}
	return *A;
}

float getMatrixMin(matrix *A){
	int i, min_index, curr_min;
	min_index = -1;
	curr_min = 0;
	for (i = 0; i < A->rows * A->cols; i++){
		if (min_index == -1){
			min_index = i;
			curr_min = A->arr[i];	
			continue;
		}
		else {
			if (A->arr[i] < curr_min){
				min_index = i;
				curr_min = A->arr[i];
				continue;
			}
		}
	}
	return A->arr[min_index];
}

float getMatrixMax(matrix *A){
	int i, max_index, curr_max;
	max_index = -1;
	curr_max = 0;
	for (i = 0; i < A->rows * A->cols; i++){
		if (max_index == -1){
			max_index = i;
			curr_max = A->arr[i];	
			continue;
		}
		else {
			if (A->arr[i] > curr_max){
				max_index = i;
				curr_max = A->arr[i];
				continue;
			}
		}
	}
	return A->arr[max_index];
}

float* getRow(matrix *A, int row){
	int index, i;
	index = INDEX(A->rows, A->cols, row, 0);	
	float *vec = malloc(A->cols * sizeof(float));

	for (i = 0; i < A->cols; i++){
		vec[i] = A->arr[index + i];
	} 

	return vec;
}

float* getCol(matrix *A, int col){
	int index, i;
	index = INDEX(A->rows, A->cols, 0, col);	
	float *vec = malloc(A->rows * sizeof(float));

	for (i = 0; i < A->rows; i++){
		vec[i] = ACCESS(A, i, col);
	} 

	return vec;
}

//Allocate memory for empty matrix with r rows and c columns
void initMatrix(matrix* A, int r, int c){
	A->rows = r;
	A->cols = c;
	A->arr = malloc(r*c*sizeof(float));

	int i,j;
	for(i=0; i<r; i++){
		for(j=0; j<c; j++){
			/* ACCESS(A,i,j) = rand() % 100 + 1; */
			/* ACCESS(A,i,j) = 0; */
		}
	}
}


void printMatrix(matrix* A){
	int i,j;
	for(i=0; i<A->rows; i++){
		for(j=0; j<A->cols; j++){
			printf("%.2f ", ACCESS(A,i,j));
		}
		//printf("\n");
		puts("");
	}
}

float* scalar_mult(matrix* A, float val){
	int i;
	float *vec = malloc(A->cols * sizeof(float)); //This should always be 1 by n
	for (i = 0; i < A->cols; i++){
		vec[i] = A->arr[i] * val;
	}
	return vec;
}

struct matrix matrix_copy(matrix *orig){
	struct matrix new_mat;
	initMatrix(&new_mat, orig->rows, orig->cols);
	int i;
	for (i = 0; i < orig->rows * orig->cols; i++){
		new_mat.arr[i] = orig->arr[i];
	}

	return new_mat;
}

//Sparse matrix stuff

struct SparseMatrix initSparseMatrix(){
	struct SparseMatrix mat;
	mat.row_index = malloc(7000000 * sizeof(int));
	mat.col_index = malloc(7000000 * sizeof(int));
	mat.size = 0;
	return mat;
}

void addSparseValue(struct SparseMatrix *mat, int row, int col){
	mat->size++;
	if (mat->size % 500000 == 0){
		printf("Size: %d\n", mat->size);
	} 
	mat->row_index[mat->size] = row;
	mat->col_index[mat->size] = col;
}

void printSparseValue(struct SparseMatrix mat, int index){
	printf("Row: %d Col: %d\n", mat.row_index[index], mat.col_index[index]);
}

struct HitsPrMatrix initHITSPRMatrix(){
	struct HitsPrMatrix mat;
	mat.hub_score = malloc(1400000 * sizeof(double));
	mat.auth_score = malloc(1400000 * sizeof(double));
	mat.oldpage_rank = malloc(1400000 * sizeof(double));
	mat.newpage_rank = malloc(1400000 * sizeof(double));
	int i;
	for(i = 0; i < 1400000; i++){
		mat.hub_score[i] = 0;
		mat.auth_score[i] = 0;
		mat.oldpage_rank[i] = 1/1354753;
		mat.newpage_rank[i] = 1/1354753;
	}
	return mat;
}