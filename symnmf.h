#ifndef SYMNMF_H
#define SYMNMF_H
double** compute_similarity(double** X, int n, int d);

double** compute_ddg(double** A, int n);

double** compute_norm(double** A, int n); 

double** update_h(double** W, double** H, int n, int k);
/**need to check */
void free_matrix(double** matrix, int rows);

#endif