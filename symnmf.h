#ifndef SYMNMF_H
#define SYMNMF_H
#include "structure.h"
void compute_similarity(double *matrix, int rows, int cols, vector *head_vec);

void compute_ddg(double *matrix, int rows, int cols, double *similarity_matrix);

void compute_norm(double *matrix, int rows, int cols, double *degree_matrix, double *similarity_matrix); 

void compute_symnmf(int n, int k, double *W, double *H);

void* safe_malloc(size_t size);

void free_vector( vector* vec);

void print_matrix(double *matrix, int rows, int cols);

void print_vectors(vector *head_vec, int rows, int cols);

void free_points(vector *head_vec);

#endif