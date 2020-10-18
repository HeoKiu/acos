#include <stdio.h>
#include <Python.h>

PyObject *getMatrixItem(PyObject *matrix, uint32_t x, uint32_t y) {
    if (y < PyList_Size(matrix) && x < PyList_Size(PyList_GetItem(matrix, y))) {
        return PyList_GetItem(PyList_GetItem(matrix, y), x);
    }
    return PyFloat_FromDouble(0);
}

static PyObject *dot(PyObject *self, PyObject *args) {
    PyObject *matrix1, *matrix2, *result;
    uint32_t size;
    PyArg_ParseTuple(args, "IOO", &size, &matrix1, &matrix2);

    result = PyList_New(size);
    for (uint32_t i = 0; i < size; ++i) {
        PyObject *resultRow = PyList_New(size);
        for (uint32_t j = 0; j < size; ++j) {
            double value = 0;
            for (uint32_t k = 0; k < size; ++k) {
                value += PyFloat_AsDouble(getMatrixItem(matrix1, k, i)) *
                         PyFloat_AsDouble(getMatrixItem(matrix2, j, k));
            }
            PyList_SetItem(resultRow, j, PyFloat_FromDouble(value));
        }
        PyList_SetItem(result, i, resultRow);
    }

    return result;
}


static PyMethodDef methods[] = {
        {
                .ml_name = "dot",
                .ml_meth = dot,
                .ml_flags = METH_VARARGS,
                .ml_doc = "Do something very useful"
        },
        {NULL, NULL, 0, NULL}
};


static PyModuleDef moduleDef = {
        .m_base = PyModuleDef_HEAD_INIT,
        // имя модуля
        .m_name = "matrix",
        .m_size = -1,
        .m_methods = methods,
};


PyMODINIT_FUNC PyInit_matrix(void) {
    return PyModule_Create(&moduleDef);
}