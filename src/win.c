#include "rma2shmem_impl.h"

int MPI_Win_allocate(MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, void * baseptr, MPI_Win * win)
{
    int rc = MPI_SUCCESS;

    int result;
    rc = PMPI_Comm_compare(MPI_COMM_WORLD, comm, &result);
    if (rc != MPI_SUCCESS) return rc;

    if ((result == MPI_CONGRUENT) || (result == MPI_SIMILAR)) {

        MPI_Aint maxsize;
        rc = MPI_Allreduce(&size, &maxsize, 1, MPI_AINT, MPI_MAX, comm);
        if (rc != MPI_SUCCESS) return rc;

        void * ptr = NULL;
        const size_t bytes = maxsize;
#ifdef HAVE_SHMEM_ALIGN
        const size_t align = 32;
        ptr = shmem_align(align, bytes);
#else
        ptr = shmem_malloc(bytes);
#endif
        if (ptr == NULL) return MPI_ERR_NO_MEM;

        rc = PMPI_Win_create(ptr, size, disp_unit, info, comm, win);
        if (rc != MPI_SUCCESS) return rc;

        rma2shmem_win_extras_s * extras = malloc( sizeof(rma2shmem_win_extras_s) );
        extras->shmem_window = true;

        rc = PMPI_Win_set_attr(*win, rma2shmem_key, extras);
        if (rc != MPI_SUCCESS) return rc;

        // this nonsense is because the argument should be void ** baseptr
        // except that then users have to do more casting when they call the function
        *((void**)baseptr) = ptr;

    } else {
        return MPI_Win_allocate(size, disp_unit, info, comm, baseptr, win);
    }
    return rc;
}


int MPI_Win_free(MPI_Win * win)
{
    int rc = MPI_SUCCESS;

    int flag;
    rma2shmem_win_extras_s * extras;

    rc = MPI_Win_get_attr(*win, rma2shmem_key, &extras, &flag);
    if (rc != MPI_SUCCESS) return rc;

    if (flag) {
        if (extras->shmem_window) {
            MPI_Aint aint;
            rc = MPI_Win_get_attr(*win, MPI_WIN_BASE, &aint, &flag);
            void * base = (void*)aint;
            shmem_free(base);
        }
    }

    return PMPI_Win_free(win);
}
