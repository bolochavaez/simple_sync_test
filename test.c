#include <Python.h>
#include "sync.h"

//reminder of IOFile thing
//typedef struct io_file {
//    int fd;
//    int current_position;
//    unsigned long long int writes;
//    unsigned long long int reads;
//} IOFile;
//


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


static PyObject* spawn_write_worker(PyObject * self, PyObject* args) {
    char * filename;
    if (!PyArg_ParseTuple(args, "s", &filename)) {
        return NULL;
    }
    IOFile * test_file =  io_file(filename);
    return Py_BuildValue("{si,si,si,si}", "fd", test_file->fd, "c_pos", test_file->current_position, "writes", test_file->writes, "reads", test_file->reads);
}

static PyMethodDef myMethods[] = {
    {"sync_test", sync_test, METH_VARARGS, "runs a sync test"},
    {"spawn_write_worker", spawn_write_worker, METH_VARARGS, "spawn a write worker"},
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
