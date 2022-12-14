#include "rma2shmem_impl.h"

int MPI_Win_allocate(MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, void * baseptr, MPI_Win * win)
{
    int result;
    int rc = MPI_Comm_compare(MPI_COMM_WORLD, comm, &result);
    if (rc != MPI_SUCCESS) return rc;

    // need to capture disp_unit for later, i guess
    if (disp_unit != 1) {
        RMA_Error("you must use disp_unit=1 (%d)\n", disp_unit);
    }

    void * ptr = NULL;

    // MPI_SIMILAR might also work here...
    if (result == MPI_CONGRUENT) {
        const size_t bytes = size;
        const size_t align = 32;
        ptr = shmem_align(align, bytes);
    } else {
        RMA_Error("only world-like communicators are supported\n");
    }

    // we create this window so we have a valid window for any unsupported operations
    rc = MPI_Win_create(ptr, size, disp_unit, info, comm, &win);
    if (rc != MPI_SUCCESS) return rc;

    *baseptr = ptr;

    return rc;
}
