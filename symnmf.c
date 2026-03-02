#include <stdio.h>
#include <stdlib.h>
#include <math.h>


typedef struct cord{
    double value;
    struct cord *next;
} cord;
typedef struct vector{
    struct vector *next;
    struct cord *cords;
   
} vector;



void* safe_malloc(size_t size) {
    void* ptr ;
    ptr = malloc(size);
    while (ptr == NULL)
    {
        ptr = malloc(size);
    }
    return ptr;
}


    /* free vector and cords */
void free_vector(struct vector* vec){
        struct cord* current_cord = vec->cords;
        struct cord* next_cord;
        while (current_cord != NULL) {
            next_cord = current_cord->next;
            free(current_cord);
            current_cord = next_cord;
        }
        free(current_cord);
        free(vec);
}


// Turns from the linked list representation of the matrix to a 2D array representation
// Currently not used.
void get_matrix(vector* head_vec, int rows, int cols, double **matrix) {
    vector* current_vec = head_vec;
    int i = 0, j;
    while (current_vec != NULL && i < rows) {
        cord* current_cord = current_vec->cords;
        j = 0;
        while (current_cord != NULL && j < cols) {
            (*matrix)[i * cols + j] = current_cord->value;
            current_cord = current_cord->next;
            j++;
        }
        current_vec = current_vec->next;
        i++;
    }
}


void print_matrix(double *matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (j == cols - 1) {
                printf("%.4f", matrix[i * cols + j]);
            } else {
                printf("%.4f, ", matrix[i * cols + j]);
            }
        }
        printf("\n");
    }
}


// Calculate norma 2:
double Euclidean_distance(vector* vec1, vector* vec2, int cols) {
    cord* cord1 = vec1->cords;
    cord* cord2 = vec2->cords;
    double sum = 0.0;
    for (int i = 0; i < cols; i++) {
        double diff = cord1->value - cord2->value;
        sum += diff * diff;
        cord1 = cord1->next;
        cord2 = cord2->next;
    }
    return sqrt(sum);
}

void compute_similarity(double *matrix, int rows, int cols, vector *head_vec){
    //  Calculate and output the similarity matrix
    vector* current_vecI = head_vec;
    for(int i=0; i<rows; i++){
        vector* current_vecJ = head_vec;
        for(int j=0; j<cols; j++){
            if (i == j) {
                matrix[i*cols + j] = 0.0; // Similarity with itself is 0
            } else {
                matrix[i*cols + j] = exp(-1 * Euclidean_distance(current_vecI, current_vecJ, cols) / 2.0); 
                // Similarity based on Euclidean distance
            }
            current_vecJ = current_vecJ->next;
        }
        current_vecI = current_vecI->next;
    }
}

void compute_ddg(double *matrix, int rows, int cols, double *similarity_matrix){
    // Calculate and output the degree matrix
    for(int i=0; i<rows; i++){
        double sum = 0.0;
        for(int j=0; j<cols; j++){
            matrix[i*cols + j] = 0.0; // Initialize degree matrix with zeros
            sum += similarity_matrix[i*cols + j];
        }
        matrix[i*cols + i] = sum; // Degree is the sum of similarities
    }
}

void compute_norm(double *matrix, int rows, int cols, double *degree_matrix, double *similarity_matrix){
    // Calculate and output the normalized similarity matrix
    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++){
            if (degree_matrix[i*cols + i] > 0 && degree_matrix[j*cols + j] > 0) {
                matrix[i*cols + j] = similarity_matrix[i*cols + j] / (sqrt(degree_matrix[i*cols + i] * degree_matrix[j*cols + j]));
            } else {
                matrix[i*cols + j] = 0.0; // Avoid division by zero
            }
        }
    }
}



void compute_symnmf(int n, int k, double *W, double *H) {
    // Implement the logic for symmetric non-negative matrix factorization
    int iter, i, j, it, l;
    double epsilon;
    
    iter = 300; /* defualt value */
    epsilon= 0.001;

    for(it = 0; it < iter; it++) {
        double sum_diff = 0.0;
        for(i = 0; i < n; i++) {
            for(j = 0; j < k; j++) {
                double old_val = H[i * n + j];
                double numerator = 0.0;
                double denominator = 0.0;
                for(l = 0; l < n; l++) {
                    // Calculate numerator = W * H:
                    numerator += W[i * n + l] * H[l * n + j];
                    
                    // Calculate denominator = H * H^T * H:
                    double tempW = 0.0;
                    for(int m = 0; m < k; m++) {
                        tempW += H[i * n + m] * H[l * n + m];
                    }

                    denominator += tempW * H[l * n + j];
                }
                
                if(denominator > 0) {
                    H[i * n + j] = old_val * (1 - 0.5 + 0.5 * (numerator / denominator));
                } else {
                    // Not expected to happen, but in case of zero denominator, we can keep the old value or set it to a small positive number
                    H[i * n + j] = old_val;
                }
                double temp_diff = H[i * n + j] - old_val;
                sum_diff += temp_diff * temp_diff;
                
            }
        }
        if(sum_diff < epsilon) {
            break;
        }
    }

}


int main(int argc, char *argv[])
{   /* input reading. */

    double max_changed;
    double *matrixA, *matrixD, *matrixW;
     vector* next_vec;
    char* goal, *file_name;
    /* inputfile reader from std_in */
    /* creating points */
     vector *head_vec, *curr_vec;
     cord *head_cord, *curr_cord;
    int rows=0, cols=0, i;
    double n;
    char c;
    FILE *input_file;

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


    

    head_cord = safe_malloc(sizeof( cord));
    curr_cord = head_cord;
    curr_cord->next = NULL;

    head_vec = safe_malloc(sizeof( vector));
    curr_vec = head_vec;
    curr_vec->next = NULL;
    
    while (fscanf(input_file, "%lf%c", &n, &c) == 2)
    {
        if (c == '\n')
        {
            int check;
            curr_cord->value = n;
            curr_vec->cords = head_cord;
            rows++;
           /* check if end of file */
           check=getchar();
           if(check==EOF){
            break;
           }
            cols = 0;
            ungetc(check,stdin);
            
            curr_vec->next = safe_malloc(sizeof( vector));
            curr_vec = curr_vec->next;
            curr_vec->next = NULL;
            
            head_cord = safe_malloc(sizeof( cord));
            curr_cord = head_cord;
            curr_cord->next = NULL;
            continue;
        }
        cols++;
        curr_cord->value = n;
        curr_cord->next = safe_malloc(sizeof( cord));
        curr_cord = curr_cord->next;
        curr_cord->next = NULL;
    }
    fclose(input_file);
    matrixA = safe_malloc(rows * cols * sizeof(double));
    // logic for the different goals
    if (goal[0] == 's' && goal[1] == 'y' && goal[2] == 'm' && goal[3] == 'n') {
        compute_similarity(&matrixA[0], rows, cols, head_vec);
        print_matrix(matrixA, rows, cols);
    }
    else if (goal[0] == 'd' && goal[1] == 'd' && goal[2] == 'g') {
        matrixD = safe_malloc(rows * cols * sizeof(double));
        compute_similarity(&matrixA[0], rows, cols, head_vec);
        compute_ddg(matrixD, rows, cols, matrixA);
        print_matrix(matrixD, rows, cols);
    }
    else if (goal[0] == 'n' && goal[1] == 'o' && goal[2] == 'r' && goal[3] == 'm') {
        matrixD = safe_malloc(rows * cols * sizeof(double));
        matrixW = safe_malloc(rows * cols * sizeof(double));
        compute_similarity(matrixA, rows, cols, head_vec);
        compute_ddg(matrixD, rows, cols, matrixA);
        compute_norm(matrixW, rows, cols, matrixD, matrixA);
        print_matrix(matrixW, rows, cols);
    }
    else{
        fprintf(stderr, "Error: Invalid goal %s\n", goal);
        return 1;
    }

    
    
    /* free memory */
    curr_vec = head_vec;
    while (curr_vec != NULL)
    {
        next_vec = curr_vec->next;
        free_vector(curr_vec);
        curr_vec = next_vec;
    }
    free(curr_vec);
    free(matrixA);
    free(matrixD);
    free(matrixW);
    free(head_vec);
    free(head_cord);
    return 0;
}