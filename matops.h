typedef struct matrix{
	int rows, cols;
	float* arr;
} matrix;

struct SparseMatrix{
	int *row_index;
	int *col_index;
	int size;
};

struct HitsPrMatrix{
	double* hub_score;
	double* auth_score;
	double* oldpage_rank;
	double* newpage_rank;
};

float*  matrix_add(float* A, float* B, int blockSize);
float*  matrix_sub(float* A, float* B, int blockSize);
float  vector_mult(float* A, float* B, int blockSize);
struct matrix matrixMult(matrix *A, matrix *B);
struct matrix matrixNormalize(matrix *A);
float getMatrixMin(matrix *A);
float getMatrixMax(matrix *A);
float* scalar_mult(matrix* A, float val);

float* getRow(matrix *A, int row);
float* getCol(matrix *A, int col);
struct matrix matrix_copy(matrix *orig);
void initMatrix(matrix* A, int r, int c);
void printMatrix(matrix* A);

//Sparse matrix stuff
struct SparseMatrix initSparseMatrix();
void addSparseValue(struct SparseMatrix *mat, int row, int col);
void printSparseValue(struct SparseMatrix mat, int index);
struct HitsPrMatrix initHITSPRMatrix();