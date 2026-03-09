#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "structure.h"
#include <string.h>




void* safe_malloc(size_t size) {
    void* ptr;
    ptr = malloc(size);
    while (ptr == NULL)
    {
        ptr = malloc(size);
    }
    return ptr;
}


    /* free vector and cords */
void free_vector( vector* vec){
        cord* current_cord;
        cord* next_cord;
        current_cord = vec->cords;
        while (current_cord != NULL) {
            next_cord = current_cord->next;
            free(current_cord);
            current_cord = next_cord;
        }
        free(vec);
}



void print_matrix(double *matrix, int rows, int cols) {
    int i, j;
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            if (j == cols - 1) {
                printf("%.4f", matrix[i * cols + j]);
            } else {
                printf("%.4f, ", matrix[i * cols + j]);
            }
        }
        printf("\n");
    }
}


/* Calculate norma 2:*/
double Euclidean_distance(vector* vec1, vector* vec2, int cols) {
    cord* cord1;
    cord* cord2;
    double sum, diff;
    int i;
    cord1 = vec1->cords;
    cord2 = vec2->cords;
    sum = 0.0;
    for (i = 0; i < cols; i++) {
        diff = cord1->value - cord2->value;
        sum += diff * diff;
        cord1 = cord1->next;
        cord2 = cord2->next;
    }
    return sum;
}

void compute_similarity(double *matrix, int rows, int cols, vector *head_vec){
    /*  Calculate and output the similarity matrix*/
    vector* current_vecI, *current_vecJ;
    int i, j;   
    current_vecI = head_vec;
    for(i=0; i<rows; i++){
        current_vecJ = head_vec;
        for(j=0; j< rows; j++){
            if (i == j) {
                matrix[i*rows + j] = 0.0; /* Similarity with itself is 0*/
            } else {
                matrix[i*rows + j] = exp(-1 * Euclidean_distance(current_vecI, current_vecJ, cols) / 2.0); 
                /* Similarity based on Euclidean distance*/
            }
            current_vecJ = current_vecJ->next;
        }
        current_vecI = current_vecI->next;
    }
}

void compute_ddg(double *matrix, int rows, int cols, double *similarity_matrix){
    /* Calculate and output the degree matrix*/
    int i, j;
    double sum;
    for(i=0; i<rows; i++){
        sum = 0.0;
        for(j=0; j<cols; j++){
            matrix[i*cols + j] = 0.0; /* Initialize degree matrix with zeros*/
            sum += similarity_matrix[i*cols + j];
        }
        matrix[i*cols + i] = sum; /* Degree is the sum of similarities*/
    }
}

void compute_norm(double *matrix, int rows, int cols, double *degree_matrix, double *similarity_matrix){
    /* Calculate and output the normalized similarity matrix*/
    int i, j;
    for(i=0; i<rows; i++){
        for(j=0; j<cols; j++){
            if (degree_matrix[i*cols + i] > 0 && degree_matrix[j*cols + j] > 0) {
                matrix[i*cols + j] = similarity_matrix[i*cols + j] / (sqrt(degree_matrix[i*cols + i] * degree_matrix[j*cols + j]));
            } else {
                matrix[i*cols + j] = 0.0; /* Avoid division by zero*/
            }
        }
    }
}



void compute_symnmf(int n, int k, double *W, double *H) {
    /* Implement the logic for symmetric non-negative matrix factorization*/
    int iter, i, j, it, l;
    double epsilon, sum_diff, tempH;
    double *HHT = (double*)safe_malloc(n * n * sizeof(double));
    double *HHTH = (double*)safe_malloc(n * k * sizeof(double));
    double *WH = (double*)safe_malloc(n * k * sizeof(double));

    iter = 300; /* defualt value */
    epsilon= 0.0001;
    sum_diff = 0.0;

    for(it = 0; it < iter; it++) {
        /*Compute HH^T*/
        for(i = 0; i < n; i++) {
            for(j = 0; j < n; j++) {
                HHT[i * n + j] = 0.0;
                for(l = 0; l < k; l++) {
                    HHT[i * n + j] += H[i * k + l] * H[j * k + l];
                }
            }
        }
        /*Compute HH^TH*/
        for(i = 0; i < n; i++) {
            for(j = 0; j < k; j++) {
                HHTH[i * k + j] = 0.0;
                for(l = 0; l < n; l++) {
                    HHTH[i * k + j] += HHT[i * n + l] * H[l * k + j];
                }
            }
        }

        /*Compute WH*/
        for(i = 0; i < n; i++) {
            for(j = 0; j < k; j++) {
                WH[i * k + j] = 0.0;
                for(l = 0; l < n; l++) {
                    WH[i * k + j] += W[i * n + l] * H[l * k + j];
                }
            }
        }

        /*Update H*/
        for(i = 0; i < n; i++) {
            for(j = 0; j < k; j++) {
                tempH = H[i * k + j] * (0.5 + 0.5 * WH[i * k + j] / (HHTH[i * k + j]));
                sum_diff += (tempH - H[i * k + j]) * (tempH - H[i * k + j]);
                H[i * k + j] = tempH;
            }
        }
       if (sum_diff < epsilon) {
            break; /* Convergence achieved*/
        }
        sum_diff = 0.0; /* Reset for next iteration*/
    }

    free(HHT);
    free(HHTH);
    free(WH);

}

void print_vectors(vector *head_vec, int rows, int cols) {
    vector *curr_vec = head_vec;
    cord *curr_cord;
    int i, j;
    for (i = 0; i < rows; i++) {
        curr_cord = curr_vec->cords;
        for (j = 0; j < cols; j++) {
            if (j == cols - 1) {
                printf("%.4f", curr_cord->value);
            } else {
                printf("%.4f, ", curr_cord->value);
            }
            curr_cord = curr_cord->next;
        }
        printf("\n");
        curr_vec = curr_vec->next;
    }
}

void free_points(vector *head_vec) {
    vector *curr_vec = head_vec;
    while (curr_vec != NULL)
    {
        vector *next_vec = curr_vec->next;
        free_vector(curr_vec);
        curr_vec = next_vec;
    }
}


int main(int argc, char *argv[])
{   /* input reading. */

    double *matrixA, *matrixD, *matrixW;
    char* goal, *file_name;
    /* inputfile reader from std_in */
    /* creating points */
     vector *head_vec, *curr_vec;
     cord *head_cord, *curr_cord;
    int rows, cols;
    double n;
    char c;
    FILE *input_file;

    rows = 0, cols = 0;

    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s wrong input\n", argv[0]);
        return 1;
    }
    goal = argv[1];
    file_name = argv[2];
    input_file = fopen(file_name, "r");
    if (input_file == NULL)
    {
        fprintf(stderr, "Error: Could not open file %s\n", file_name);
        return 1;
    }

        /* Initialize first structures */
    head_cord = safe_malloc(sizeof(cord));
    head_cord->next = NULL;

    head_vec = safe_malloc(sizeof(vector));
    head_vec->next = NULL;
    head_vec->cords = NULL; /* Vital: prevents the 'Conditional Jump' error */

    curr_vec = head_vec;
    curr_cord = head_cord;

    while (fscanf(input_file, "%lf%c", &n, &c) != EOF) {
        curr_cord->value = n;

        if (c == '\n' || c == '\r') {
            int next_char;
            if (c == '\r') fgetc(input_file); /* Handle Windows \r\n */
            
            curr_vec->cords = head_cord; /* Attach the row's coordinates */
            rows++;
            
            /* If this was the first row, we now know the final 'cols' count */
            if (rows == 1) {
                cols++; 
            }

            next_char = fgetc(input_file);
            if (next_char == EOF) break; 
            ungetc(next_char, input_file);

            /* Prep for the NEXT vector/row */
            curr_vec->next = safe_malloc(sizeof(vector));
            curr_vec = curr_vec->next;
            curr_vec->next = NULL;
            curr_vec->cords = NULL; /* Crucial for cleanup logic */

            head_cord = safe_malloc(sizeof(cord));
            curr_cord = head_cord;
            curr_cord->next = NULL;
        } else {
            /* This is a comma or space within a row */
            if (rows == 0) {
                cols++; /* Only count columns during the very first row */
            }
            curr_cord->next = safe_malloc(sizeof(cord));
            curr_cord = curr_cord->next;
            curr_cord->next = NULL;
        }
    }

    fclose(input_file);

    if(rows == 0){
        fprintf(stderr, "Error: No data points found in file %s\n", file_name);
        return 1;
    }
    matrixA = safe_malloc(rows * rows * sizeof(double));
    /* logic for the different goals*/
    if (strcmp(goal, "sym") == 0) {
        compute_similarity(&matrixA[0], rows, cols, head_vec);
        print_matrix(matrixA, rows, rows);
        free(matrixA);
    }
    else if (strcmp(goal, "ddg") == 0) {
        matrixD = safe_malloc(rows * rows * sizeof(double));
        compute_similarity(&matrixA[0], rows, cols, head_vec);
        compute_ddg(matrixD, rows, rows, matrixA);
        print_matrix(matrixD, rows, rows);
        free(matrixA);
        free(matrixD);
    }
    else if (strcmp(goal, "norm") == 0) {
        matrixD = safe_malloc(rows * rows * sizeof(double));
        matrixW = safe_malloc(rows * rows * sizeof(double));
        compute_similarity(matrixA, rows, cols, head_vec);
        compute_ddg(matrixD, rows, rows, matrixA);
        compute_norm(matrixW, rows, rows, matrixD, matrixA);
        print_matrix(matrixW, rows, rows);
        free(matrixA);
        free(matrixD);
        free(matrixW);
    }
    else{
        fprintf(stderr, "Error: Invalid goal %s\n", goal);
        return 1;
    }

    /* free memory */
    free_points(head_vec);
    return 0;
}