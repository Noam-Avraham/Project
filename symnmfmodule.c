#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "symnmf.h"
#include "structure.h"

static PyObject* sym(PyObject* self, PyObject* args);

static PyObject* ddg(PyObject* self, PyObject* args);

static PyObject* norm(PyObject* self, PyObject* args);

static PyObject* symnmf(PyObject* self, PyObject* args);


int parse_points(vector *head_vec, int* n, int* dim, PyObject* args) {
    /**parse the input from python */
    PyObject *points_obj;
    int i,j, temp_n, temp_dim;
    vector *curr_vec;

    if(!PyArg_ParseTuple(args, "Oii", &points_obj, &temp_dim, &temp_n)) {
        return 1;
    }
    *n = temp_n;
    *dim = temp_dim;

    /* Get vector straight from python*/
    curr_vec = head_vec;
    for(i = 0; i < *n; i++) {
        vector *new_vec;
        cord *curr_cord = (cord*)safe_malloc(sizeof(cord));
        curr_cord->next = NULL;
        curr_vec->cords = curr_cord;
        PyObject* point = PyList_GetItem(points_obj, i);
        for(j = 0; j < *dim; j++) {
            cord *new_cord = (cord*)safe_malloc(sizeof(cord));
            PyObject* coord =  PyList_GetItem(point, j);
            curr_cord->value = PyFloat_AsDouble(coord);
            new_cord->next = NULL;
            curr_cord->next = new_cord;
            curr_cord = new_cord;
        }
        new_vec = (vector*)safe_malloc(sizeof(vector));
        new_vec->next = NULL;
        curr_vec->next = new_vec;
        curr_vec = new_vec;
    }

    return 0;
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




static PyObject* sym(PyObject* self, PyObject* args) {
    /* Parse the input arguments from Python */
    int n, dim;
    double* matrixA;
    PyObject *py_result;
    vector *head_vec = (vector*)safe_malloc(sizeof(vector));
    head_vec->next = NULL;
    if (parse_points(head_vec, &n, &dim, args) == 1) {
        return NULL;
    }
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
    if (parse_points(head_vec, &n, &dim, args) == 1) {
        return NULL;
    }
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
    if (parse_points(head_vec, &n, &dim, args) == 1) {
        return NULL;
    }
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



static PyObject* symnmf(PyObject* self, PyObject* args) {
    /* Parse the input arguments from Python */
    int n, k, i, j;
    double *matrixH, *matrixW;
    PyObject *py_result, *matrixH_obj, *matrixW_obj;
    if(!PyArg_ParseTuple(args, "OOii", &matrixW_obj, &matrixH_obj, &n, &k)) {
        return NULL;
    }
    matrixH = (double*)safe_malloc(n * k * sizeof(double));
    matrixW = (double*)safe_malloc(n * n * sizeof(double));
    /* insert python inputs to matricies: */
    for(i = 0; i < n; i++) {
        PyObject* arrayH = PyList_GetItem(matrixH_obj, i);
        PyObject* arrayW = PyList_GetItem(matrixW_obj, i);

        for(j = 0; j < k; j++) {
            PyObject* value =  PyList_GetItem(arrayH, j);
            matrixH[i * k + j] = PyFloat_AsDouble(value);
        }
        for(j = 0; j < n; j++) {
            PyObject* value =  PyList_GetItem(arrayW, j);
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
    {"symnmf", (PyCFunction)symnmf, METH_VARARGS, PyDoc_STR("Perform symmetric non-negative matrix factorization.")},
    {NULL, NULL, 0, NULL} /* Sentinel */
};

static struct PyModuleDef symnmfmodule = {
    PyModuleDef_HEAD_INIT,
    "symnmf", /* name of module */
    NULL, /* module documentation, may be NULL */
    -1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global  variables. */
    SymNMFMethods
};

PyMODINIT_FUNC PyInit_symnmf_module(void) {
    return PyModule_Create(&symnmfmodule);
}

