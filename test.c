#include <Python.h>
#include "sync.h"

static PyObject* sync_test(PyObject * self, PyObject* args) {
    char * filename;
    int runtime = 3;

    if (!PyArg_ParseTuple(args, "si", &filename, &runtime)) {
        return NULL;
    }
    IOFile * test_file =  io_file(filename);
    write_worker(test_file, runtime);
    io_file_reset_pos(test_file);
    read_worker(test_file, runtime);
    io_file_print_stats(test_file, runtime);
    delete_io_file(test_file);
    return Py_None;
}

static PyMethodDef myMethods[] = {
    {"sync_test", sync_test, METH_VARARGS, "runs a sync test"},
    {NULL,NULL,0,NULL}

};

static struct PyModuleDef synctest = {
    PyModuleDef_HEAD_INIT,
    "synctest",
    "io test module",
    -1,
    myMethods
};

PyMODINIT_FUNC PyInit_synctest(void){
    return PyModule_Create(&synctest);

}
