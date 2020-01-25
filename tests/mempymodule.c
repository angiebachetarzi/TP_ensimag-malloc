#include <python3.5m/Python.h>
#include "../src/mem.h"

static PyObject *
mempy_alloc(PyObject *self, PyObject *args) {
  unsigned long taille;
  unsigned long addr;

  if (!PyArg_ParseTuple(args, "k", &taille))
        return NULL;
  addr = (unsigned long) emalloc(taille);
  return PyLong_FromUnsignedLong(addr);
}

static PyObject *
mempy_free(PyObject *self, PyObject *args) {
  unsigned long addr;

  if (!PyArg_ParseTuple(args, "k", & addr))
        return NULL;
  efree((void*) addr);
  return PyLong_FromLong(0);
}

static PyMethodDef EnsiAllocMethods[] = {
    {"alloc",  mempy_alloc, METH_VARARGS,
     "allocate a bloc of the argument size, return the pointer value"},
    {"free",  mempy_free, METH_VARARGS,
     "free a bloc with the argument pointer value"},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};



static struct PyModuleDef mempymodule = {
   PyModuleDef_HEAD_INIT,
   "ensialloc",   /* name of module */
   NULL, /* module documentation, may be NULL */
   -1,       /* size of per-interpreter state of the module,
                or -1 if the module keeps state in global variables. */
   EnsiAllocMethods
};

PyMODINIT_FUNC
PyInit_mempy(void)
{
    return PyModule_Create(&mempymodule);
}
