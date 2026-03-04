#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "symnmf.h"
#include "structure.h"

static PyObject* sym(PyObject* self, PyObject* args);

static PyObject* ddg(PyObject* self, PyObject* args);

static PyObject* norm(PyObject* self, PyObject* args);

static PyObject* symnf(PyObject* self, PyObject* args);

void matrix_to_points(double *matrix, vector *head_vec, int rows, int cols) {
    vector *curr_vec = head_vec;
    int i, j;
    for(i = 0; i < rows; i++) {
        cord *curr_cord = (cord*)safe_malloc(sizeof(cord));
        curr_cord->value = matrix[i * cols];
        curr_cord->next = NULL;
        curr_vec->cords = curr_cord;
        for(j = 1; j < cols; j++) {
            cord *new_cord = (cord*)safe_malloc(sizeof(cord));
            new_cord->value = matrix[i * cols + j];
            new_cord->next = NULL;
            curr_cord->next = new_cord;
            curr_cord = new_cord;
        }

        vector *new_vec = (vector*)safe_malloc(sizeof(vector));
        new_vec->next = NULL;
        curr_vec->next = new_vec;
        curr_vec = new_vec;
    }
}


void parse_points(vector *head_vec, int* n, int* dim) {
    /**parse the input from python */
    PyObject *points_obj;
    int n,dim, i,j;
    double** points;

    if(!PyArg_ParseTuple(args, "Oii", &points_obj, &dim, &n)) {
        return NULL;
    }

    /**initalize points to python.points */
    points = (double**)safe_malloc(n * sizeof(double*));
    for(i = 0; i < n; i++) {
        points[i] = (double*)safe_malloc(dim * sizeof(double));
        PyObject* point = PyList_GetItem(points_obj, i);
        for(j = 0; j < dim; j++) {
            PyObject* coord =  PyList_GetItem(point, j);
            points[i][j] = PyFloat_AsDouble(coord);
        }
    }

    /*Turn points matrix to vector*/
    matrix_to_points(&points[0][0], head_vec, n, dim);
    
        
}

/* Get the matrix from the input file and store it in a linked list representation */
/* Currently not used. */
void parse_matrix(vector *head_vec, int* rows, int* cols) {
    /**parse the input from python */
    PyObject *matrix_obj;
    int rows,cols, i,j;
    double** matrix;

    if(!PyArg_ParseTuple(args, "Oii", &matrix_obj, &rows, &cols)) {
        return NULL;
    }

    /**initalize points to python.points */
    matrix = (double**)safe_malloc(rows * sizeof(double*));
    for(i = 0; i < rows; i++) {
        matrix[i] = (double*)safe_malloc(cols * sizeof(double));
        PyObject* row = PyList_GetItem(matrix_obj, i);
        for(j = 0; j < cols; j++) {
            PyObject* value =  PyList_GetItem(row, j);
            matrix[i][j] = PyFloat_AsDouble(value);
        }
    }

    /*Turn points matrix to vector*/
    matrix_to_points(&matrix[0][0], head_vec, rows, cols);
}

PyObject* metrix_to_python(double *matrix, int rows, int cols) {
    PyObject* py_result = PyList_New(rows);
    int i, j;
    for (i = 0; i < rows; i++) {
        PyObject* py_row = PyList_New(cols);
        for (j = 0; j < cols; j++) {
            PyList_SetItem(py_row, j, PyFloat_FromDouble(matrix[i * cols + j]));
        }
        PyList_SetItem(py_result, i, py_row);
    }
    return py_result;
}

void free_points(vector *head_vec) {
    vector *curr_vec = head_vec;
    while (curr_vec != NULL) {
        vector *next_vec = curr_vec->next;
        free_vector(curr_vec);
        curr_vec = next_vec;
    }
}


static PyObject* sym(PyObject* self, PyObject* args) {
    /* Parse the input arguments from Python */
    int n, dim;
    double* matrixA;
    PyObject *py_result;
    vector *head_vec = (vector*)safe_malloc(sizeof(vector));
    head_vec->next = NULL;
    parse_points(head_vec, &n, &dim);
    /* Call the sym function and return the result to Python */
    matrixA = safe_malloc(n * n * sizeof(double));
    compute_similarity(&matrixA[0], n, dim, head_vec);

    py_result = metrix_to_python(&matrixA[0], n, n);
    free(matrixA);
    free_points(head_vec);
    return py_result;
}

static PyObject* ddg(PyObject* self, PyObject* args) {
    /* Parse the input arguments from Python */
    int n, dim;
    double* matrixD, *matrixA;
    PyObject *py_result;
    vector *head_vec = (vector*)safe_malloc(sizeof(vector));
    head_vec->next = NULL;
    parse_points(head_vec, &n, &dim);
    /* Call the ddg function and return the result to Python */
    matrixA = safe_malloc(n * n * sizeof(double));
    compute_similarity(&matrixA[0], n, dim, head_vec);
    matrixD = safe_malloc(n * n * sizeof(double));
    compute_ddg(matrixD, n, n, matrixA);
    py_result = metrix_to_python(&matrixD[0], n, n);
    free(matrixD);
    free(matrixA);
    free_points(head_vec);
    return py_result;
}

static PyObject* norm(PyObject* self, PyObject* args) {
    /* Parse the input arguments from Python */
    int n, dim;
    double* matrixD, *matrixA, *matrixW;
    PyObject *py_result;
    vector *head_vec = (vector*)safe_malloc(sizeof(vector));
    head_vec->next = NULL;
    parse_points(head_vec, &n, &dim);
    /* Call the symnmf function and return the result to Python */
    matrixA = safe_malloc(n * n * sizeof(double));
    compute_similarity(&matrixA[0], n, dim, head_vec);
    matrixD = safe_malloc(n * n * sizeof(double));
    compute_ddg(matrixD, n, n, matrixA);
    matrixW = safe_malloc(n * n * sizeof(double));
    compute_norm(matrixW, n, n, matrixD, matrixA);
    py_result = metrix_to_python(&matrixW[0], n, n);
    free(matrixW);
    free(matrixD);
    free(matrixA);
    free_points(head_vec);
    return py_result;
}



static PyObject* symnf(PyObject* self, PyObject* args) {
    /* Parse the input arguments from Python */
    int n, k, i, j;
    double *matrixH, *matrixW;
    PyObject *py_result;
    if(!PyArg_ParseTuple(args, "OOii", &matrixW, &matrixH, &n, &k)) {
        return NULL;
    }
    matrixH = safe_malloc(n * k * sizeof(double));
    matrixW = safe_malloc(n * n * sizeof(double));

    /* insert python inputs to matricies: */
    for(i = 0; i < n; i++) {
        for(j = 0; j < k; j++) {
            PyObject* value =  PyList_GetItem(matrixH, i * k + j);
            matrixH[i * k + j] = PyFloat_AsDouble(value);
        }
    }
    for(i = 0; i < n; i++) {
        for(j = 0; j < n; j++) {
            PyObject* value =  PyList_GetItem(matrixW, i * n + j);
            matrixW[i * n + j] = PyFloat_AsDouble(value);
        }
    }


    /* Call the symnmf function and return the result to Python */
    compute_symnmf(n, k, &matrixW[0], &matrixH[0]);
    py_result = metrix_to_python(&matrixH[0], n, k);
    
    free(matrixW);
    free(matrixH);


    return py_result; /* Return the Python object containing the result matrix */
}


static PyMethodDef SymNMFMethods[] = {
    {"sym", (PyCFunction)sym, METH_VARARGS, PyDoc_STR("Calculate the similarity matrix.")},
    {"ddg", (PyCFunction)ddg, METH_VARARGS, PyDoc_STR("Calculate the degree matrix.")},
    {"norm", (PyCFunction)norm, METH_VARARGS, PyDoc_STR("Calculate the normalized similarity matrix.")},
    {"symnf", (PyCFunction)symnf, METH_VARARGS, PyDoc_STR("Perform symmetric non-negative matrix factorization.")},
    {NULL, NULL, 0, NULL} /* Sentinel */
};

static struct PyModuleDef symnmfmodule = {
    PyModuleDef_HEAD_INIT,
    "symnmf", /* name of module */
    NULL, /* module documentation, may be NULL */
    -1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global  */variables.
    SymNMFMethods
};

PyMODINIT_FUNC PyInit_symnmf(void) {
    return PyModule_Create(&symnmfmodule);
}

