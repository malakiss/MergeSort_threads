#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

typedef struct {
    int i; //row kam
    int j; // which col
    int n; // 3adad el elements ely hay3mlhom dot product 
} data; //beta3 dotproductOP (each element in o/p matrix)

typedef struct {
    int i; // row kam
    int colsA; 
    int colsB; 
} data2; //beta3 method 2 (each row in a thread)

int **matrixA;
int **matrixB;
int **resultMatrix;
int **resultMatrix2;


void readMatrix(FILE *file, int rows, int cols, int **matrix) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fscanf(file, "%d", &matrix[i][j]);
        }
    }
}

void printMatrixToFile(FILE *file, int rows, int cols, int **matrix) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fprintf(file, "%d ", matrix[i][j]);
        }
        fprintf(file, "\n");
    }
}

// function ely bthseb kol element b thread lewaahdo 
void *dotProductOp(void *args) {
    data *d = (data *)args; //casting arg to data struct 
    int i = d->i;
    int j = d->j;
    int n = d->n;

    resultMatrix[i][j] = 0; //ba intialise kol element b zero 
    for (int k = 0; k < n; k++) {
        resultMatrix[i][j] += matrixA[i][k] * matrixB[k][j];
    }

    free(d); //freeeeee!!!!!!!!!!!!
    pthread_exit(NULL);
}

// operation ely bthseb kol row f thread lewahdo hayb2a 3adadhom
//ad rows of output matrix 
void *dotProductOp2(void *args) {
    data2 *d = (data2 *)args;
    int i = d->i; //thread rakam i (any row fel result matrix)
    int colsA = d->colsA; //equivelent to rowsB 
    int colsB = d->colsB; //num of cols  in resultmatrix 

    for (int j = 0; j < colsB; j++) {  //outer loop: bt3ady 3ala kol element fel row i
        resultMatrix2[i][j] = 0; 
        for (int k = 0; k < colsA; k++) { // inner loop :bthseb el dotproduct veta3 kol element resultMatrix2[i][j]
            resultMatrix2[i][j] += matrixA[i][k] * matrixB[k][j];
        }
    }

    free(d);
    pthread_exit(NULL);
}

int main() {
    FILE *file = fopen("matrices.txt", "r");
    FILE *outFile = fopen("output.txt", "w");
    if (file == NULL || outFile == NULL) {
        perror("Error opening file");
        return 1;
    }

    int rowsA, colsA, rowsB, colsB;

    // MATRIX A READING
    fscanf(file, "%d %d", &rowsA, &colsA);
    matrixA = (int **)malloc(rowsA * sizeof(int *)); //ALLOCATE memory for 2d matrix 
    for (int i = 0; i < rowsA; i++) {
        matrixA[i] = (int *)malloc(colsA * sizeof(int));
    }
    readMatrix(file, rowsA, colsA, matrixA);

    // MATRIX B READING
    fscanf(file, "%d %d", &rowsB, &colsB);
    if (colsA != rowsB) {
        fprintf(stderr, "Wrong entry of dimensions!!!.\n");
        fclose(file);
        fclose(outFile);
        return 1;
    }

    matrixB = (int **)malloc(rowsB * sizeof(int *));
    for (int i = 0; i < rowsB; i++) {
        matrixB[i] = (int *)malloc(colsB * sizeof(int));
    }
    readMatrix(file, rowsB, colsB, matrixB);

    // INTIALISE 2 RESULT MATRICES FOR EACH OPERATION
    resultMatrix = (int **)malloc(rowsA * sizeof(int *));
    resultMatrix2 = (int **)malloc(rowsA * sizeof(int *));
    for (int i = 0; i < rowsA; i++) {
        resultMatrix[i] = (int *)malloc(colsB * sizeof(int));
        resultMatrix2[i] = (int *)malloc(colsB * sizeof(int));
    }

    // Op1 : kol element f thread:
    clock_t start1 = clock();
    //allocate 2d array for threads ashan kol element f thread lewahdo
    pthread_t **threads = (pthread_t **)malloc(rowsA * sizeof(pthread_t *)); 
    for (int i = 0; i < rowsA; i++) {
        threads[i] = (pthread_t *)malloc(colsB * sizeof(pthread_t));
    }

    for (int i = 0; i < rowsA; i++) {
        for (int j = 0; j < colsB; j++) {
            data *d = (data *)malloc(sizeof(data));
            d->i = i; //which row
            d->j = j;//which col
            d->n = colsA; //num of elements in thread
            pthread_create(&threads[i][j], NULL, dotProductOp, d);
        }
    }

    for (int i = 0; i < rowsA; i++) {
        for (int j = 0; j < colsB; j++) {
            pthread_join(threads[i][j], NULL); //SYNC!!!!!!!
        }
    }
    clock_t end1 = clock();
    double elapsed1 = (double)(end1 - start1) / CLOCKS_PER_SEC;
// akteb fel file 
    fprintf(outFile, "Element wise Result Matrix:\n");
    printMatrixToFile(outFile, rowsA, colsB, resultMatrix);
    fprintf(outFile, "Elapsed Time: %.4f seconds\n\n", elapsed1);

//------------------------------------------------------------------------------------------------------------------
    
    // Op2: kol row f thread
    clock_t start2 = clock();
    //only 1d array of thread ashan howa kol threda byhseb row kamel
    pthread_t *rowThreads = (pthread_t *)malloc(rowsA * sizeof(pthread_t)); 
    for (int i = 0; i < rowsA; i++) {
        data2 *d2 = (data2 *)malloc(sizeof(data2));
        d2->i = i;
        d2->colsA = colsA;
        d2->colsB = colsB;
        pthread_create(&rowThreads[i], NULL, dotProductOp2, d2);
    }

    for (int i = 0; i < rowsA; i++) {
        pthread_join(rowThreads[i], NULL);
    }
    clock_t end2 = clock();
    double elapsed2 = (double)(end2 - start2) / CLOCKS_PER_SEC;

    fprintf(outFile, "Row wise Result Matrix:\n");
    printMatrixToFile(outFile, rowsA, colsB, resultMatrix2);
    fprintf(outFile, "Elapsed Time: %.4f seconds\n\n", elapsed2);

    // FREEEE ALLLL
    for (int i = 0; i < rowsA; i++) {
        free(matrixA[i]);
        free(matrixB[i]);
        free(resultMatrix[i]);
        free(resultMatrix2[i]);
        free(threads[i]);
    }
    free(matrixA);
    free(matrixB);
    free(resultMatrix);
    free(resultMatrix2);
    free(threads);
    free(rowThreads);
    fclose(file);
    fclose(outFile);
    return 0;
}
