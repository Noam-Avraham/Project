#ifndef SYMNMF_H
#define SYMNMF_H
double** compute_similarity(double *matrix, int rows, int cols, vector *head_vec);

double** compute_ddg(double *matrix, int rows, int cols, double *similarity_matrix);

double** compute_norm(double *matrix, int rows, int cols, double *degree_matrix, double *similarity_matrix); 

double** compute_symnmf(int n, int k, double *W, double *H);
/**need to check */
void free_matrix(double** matrix, int rows);

#endif