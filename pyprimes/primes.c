#include <stdio.h>
#include <Python.h>


static PyObject *factor_out(PyObject *self, PyObject *args) {
    int32_t number;
    if (!PyArg_ParseTuple(args, "i", &number)) {
        PyErr_SetString(PyExc_TypeError, "Wrong value type. Must be unsigned number");
        return NULL;
    }
    if(number < 0){
        PyErr_SetString(PyExc_ValueError, "Negative number");
        return NULL;
    }
    PyObject *primeNumbers = PyList_New(0);

    for (uint32_t i = 2; i <= number; ++i) {
        while (!(number % i)) {
            number /= i;
            PyObject* primeNum = PyLong_FromLong(i);
            PyList_Append(primeNumbers, primeNum);
            Py_DECREF(primeNum);
        }
    }

    if (PyList_Size(primeNumbers) <= 1) {
        Py_DECREF(primeNumbers);
        return PyUnicode_FromStringAndSize("Prime!", 6);
    }
    return primeNumbers;
}


static PyMethodDef methods[] = {
        {
                .ml_name = "factor_out",
                .ml_meth = factor_out,
                .ml_flags = METH_VARARGS,
                .ml_doc = "Factor out number to primes.\nIf number already prime, return 'Prime!' string"
        },
        {NULL, NULL, 0, NULL}
};


static PyModuleDef moduleDef = {
        .m_base = PyModuleDef_HEAD_INIT,
        .m_name = "primes",
        .m_size = -1,
        .m_methods = methods,
};


PyMODINIT_FUNC PyInit_primes(void) {
    return PyModule_Create(&moduleDef);
}