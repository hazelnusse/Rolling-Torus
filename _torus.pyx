cdef extern from "arrayobject.h":
    cdef enum NPY_TYPES:
        NPY_BOOL
        NPY_BYTE
        NPY_UBYTE
        NPY_SHORT
        NPY_USHORT
        NPY_INT
        NPY_UINT
        NPY_LONG
        NPY_ULONG
        NPY_LONGLONG
        NPY_ULONGLONG
        NPY_FLOAT
        NPY_DOUBLE
        NPY_LONGDOUBLE
        NPY_CFLOAT
        NPY_CDOUBLE
        NPY_CLONGDOUBLE
        NPY_OBJECT
        NPY_STRING
        NPY_UNICODE
        NPY_VOID
        NPY_NTYPES
        NPY_NOTYPE

    ctypedef int npy_intp

    ctypedef extern class numpy.ndarray [object PyArrayObject]:
        cdef char *data
        cdef int nd
        cdef npy_intp *dimensions
        cdef npy_intp *strides
        cdef int flags

    object PyArray_SimpleNewFromData(int nd, npy_intp* dims, int typenum,
            void* data)
    void import_array()

cdef extern from "torus.h":
    int c_TorusEOMS "TorusEOMS"(double t, double x[8], double f[8], void *params)

def TorusEOMS(t, x, params):
    assert len(x) == 8
    assert len(params) == 3
    from numpy import zeros
    f = zeros(8, dtype="double")
    cdef double *c_x, *c_f, *c_params
    cdef int n
    numpy2c_double_inplace(x, &c_x, &n)
    numpy2c_double_inplace(f, &c_f, &n)
    numpy2c_double_inplace(params, &c_params, &n)
    r = c_TorusEOMS(t, c_x, c_f, c_params)
    assert r == 0
    return f

_AA = None

cdef void numpy2c_int_inplace(object A_n, int **A_c, int *n):
    """
    Returns the C array, that points to the numpy array (inplace).

    Only if strides != sizeof(int), the data get copied first.

    Important note: you need to use the A_c array immediately after calling
    this function in C, otherwise numpy could deallocate the array, especially
    if the _AA global variable was deallocated.
    """
    cdef ndarray A = A_n
    if not (A.nd == 1 and A.strides[0] == sizeof(int)):
        from numpy import array
        A = array(A.flat, dtype="int32")
        # this is needed so that numpy doesn't dealocate the arrays
        global _AA
        _AA = A
    n[0] = len(A)
    A_c[0] = <int *>(A.data)

cdef void numpy2c_double_inplace(object A_n, double **A_c, int *n):
    """
    Returns the C array, that points to the numpy array (inplace).

    Only if strides != sizeof(double), the data get copied first.

    Important note: you need to use the A_c array immediately after calling
    this function in C, otherwise numpy could deallocate the array, especially
    if the _AA global variable was deallocated.
    """
    cdef ndarray A = A_n
    if not (A.nd == 1 and A.strides[0] == sizeof(double)):
        from numpy import array
        A = array(A.flat, dtype="double")
        # this is needed so that numpy doesn't dealocate the arrays
        global _AA
        _AA = A
    n[0] = len(A)
    A_c[0] = <double *>(A.data)
