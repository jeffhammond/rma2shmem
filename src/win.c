#include "rma2shmem_impl.h"

// the attribute value is a pointer to the struct (in the heap)
bool RMA_Win_get_extras(MPI_Win win, rma2shmem_win_extras_s ** extras)
{
    int flag;
    *extras = NULL;

    int rc = PMPI_Win_get_attr(win, rma2shmem_key, extras, &flag);
    if (rc != MPI_SUCCESS) return false;

    return (flag != 0);
}

bool RMA_Win_get_base(MPI_Win win, void * base)
{
        int flag;
        MPI_Aint aint;
        int rc = PMPI_Win_get_attr(win, MPI_WIN_BASE, &aint, &flag);
        if (rc != MPI_SUCCESS) return false;
        if (!flag) return false;
        *((void**)base) = (void*)aint;
        return true;
}

bool RMA_Win_get_disp_unit(MPI_Win win, int * disp)
{
        int flag;
        int rc = PMPI_Win_get_attr(win, MPI_WIN_DISP_UNIT, disp, &flag);
        if (rc != MPI_SUCCESS) return false;
        if (!flag) return false;
        return true;
}

bool RMA_Win_uses_shmem(MPI_Win win)
{
    int flag;
    rma2shmem_win_extras_s * extras;

    if ( RMA_Win_get_extras(win, &extras) ) {
        if (extras->shmem_window) {
            return true;
        }
    }
    return false;
}

static void RMA_Win_quiet(MPI_Win win)
{
    shmem_quiet();
    RMA_Message("SHMEM quiet");
}

int MPI_Win_allocate(MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, void * baseptr, MPI_Win * win)
{
    int rc = MPI_SUCCESS;

    int result;
    rc = PMPI_Comm_compare(MPI_COMM_WORLD, comm, &result);
    if (rc != MPI_SUCCESS) return rc;

    if (result != MPI_UNEQUAL) {

        MPI_Aint maxsize;
        rc = PMPI_Allreduce(&size, &maxsize, 1, MPI_AINT, MPI_MAX, comm);
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

        RMA_Message("SHMEM malloc");

        rc = PMPI_Win_create(ptr, size, disp_unit, info, comm, win);
        if (rc != MPI_SUCCESS) return rc;

        MPI_Comm dup;
        rc = PMPI_Comm_dup(comm, &dup);
        if (rc != MPI_SUCCESS) return rc;

        rma2shmem_win_extras_s * extras = malloc( sizeof(rma2shmem_win_extras_s) );
        extras->shmem_window = true;
        extras->comm         = dup;

        rc = PMPI_Win_set_attr(*win, rma2shmem_key, extras);
        if (rc != MPI_SUCCESS) return rc;

        // this nonsense is because the argument should be void ** baseptr
        // except that then users have to do more casting when they call the function
        *((void**)baseptr) = ptr;

        return MPI_SUCCESS;
    }
    return PMPI_Win_allocate(size, disp_unit, info, comm, baseptr, win);
}

int MPI_Win_free(MPI_Win * win)
{
    int rc = MPI_SUCCESS;

    rma2shmem_win_extras_s * extras;

    if ( RMA_Win_get_extras(*win, &extras) && (extras->shmem_window) ) {

        RMA_Win_quiet(*win);

        void * base;
        bool rx = RMA_Win_get_base(*win, &base);
        if (rx) {
            shmem_free(base);
            RMA_Message("SHMEM free");
        } else {
            RMA_Error("RMA_Win_get_base failed\n");
        }

        rc = PMPI_Comm_free(&(extras->comm));
        if (rc != MPI_SUCCESS) return rc;

        free(extras);
    }
    return PMPI_Win_free(win);
}

int MPI_Win_lock(int lock_type, int rank, int assert, MPI_Win win)
{
    if (RMA_Win_uses_shmem(win)) {
        if (lock_type == MPI_LOCK_EXCLUSIVE) {
            // TODO need to implement exclusive lock state
            //      targets need state for the actual lock
            //      origins need a vector to know which targets they have locked
            return MPI_ERR_RMA_SYNC;
        } else {
            return MPI_SUCCESS;
        }
    }
    return PMPI_Win_lock(lock_type, rank, assert, win);
}

int MPI_Win_unlock(int rank, MPI_Win win)
{
    if (RMA_Win_uses_shmem(win)) {
        // TODO detect incorrect usage
        RMA_Win_quiet(win);
        return MPI_SUCCESS;
    }
    return PMPI_Win_unlock(rank, win);
}

int MPI_Win_lock_all(int assert, MPI_Win win)
{
    if (RMA_Win_uses_shmem(win)) {
        // TODO detect incorrect usage
        return MPI_SUCCESS;
    }
    return PMPI_Win_lock_all(assert, win);
}

int MPI_Win_unlock_all(MPI_Win win)
{
    if (RMA_Win_uses_shmem(win)) {
        // TODO detect incorrect usage
        RMA_Win_quiet(win);
        return MPI_SUCCESS;
    }
    return PMPI_Win_unlock_all(win);
}

int MPI_Win_flush(int rank, MPI_Win win)
{
    if (RMA_Win_uses_shmem(win)) {
        // TODO detect incorrect usage
        RMA_Win_quiet(win);
        return MPI_SUCCESS;
    }
    return PMPI_Win_flush(rank, win);
}

int MPI_Win_flush_all(MPI_Win win)
{
    if (RMA_Win_uses_shmem(win)) {
        // TODO detect incorrect usage
        RMA_Win_quiet(win);
        return MPI_SUCCESS;
    }
    return PMPI_Win_flush_all(win);
}

int MPI_Win_flush_local(int rank, MPI_Win win)
{
    if (RMA_Win_uses_shmem(win)) {
        // TODO detect incorrect usage
        RMA_Win_quiet(win);
        return MPI_SUCCESS;
    }
    return PMPI_Win_flush_local(rank, win);
}

int MPI_Win_flush_local_all(MPI_Win win)
{
    if (RMA_Win_uses_shmem(win)) {
        // TODO detect incorrect usage
        RMA_Win_quiet(win);
        return MPI_SUCCESS;
    }
    return PMPI_Win_flush_local_all(win);
}

int MPI_Win_fence(int assert, MPI_Win win)
{
    rma2shmem_win_extras_s * extras;
    if (RMA_Win_get_extras(win, &extras) && (extras->shmem_window)) {
        // TODO detect incorrect usage
        RMA_Win_quiet(win);
        return PMPI_Barrier(extras->comm);
    }
    return PMPI_Win_fence(assert, win);
}

int MPI_Win_post(MPI_Group group, int assert, MPI_Win win)
{
    if (RMA_Win_uses_shmem(win)) {
        RMA_Error("PSCW is not supported and will not be supported.\n");
        return MPI_ERR_RMA_SYNC;
    }
    return PMPI_Win_post(group, assert, win);
}

int MPI_Win_start(MPI_Group group, int assert, MPI_Win win)
{
    if (RMA_Win_uses_shmem(win)) {
        RMA_Error("PSCW is not supported and will not be supported.\n");
        return MPI_ERR_RMA_SYNC;
    }
    return PMPI_Win_start(group, assert, win);
}

int MPI_Win_complete(MPI_Win win)
{
    if (RMA_Win_uses_shmem(win)) {
        RMA_Error("PSCW is not supported and will not be supported.\n");
        return MPI_ERR_RMA_SYNC;
    }
    return PMPI_Win_complete(win);
}

int MPI_Win_wait(MPI_Win win)
{
    if (RMA_Win_uses_shmem(win)) {
        RMA_Error("PSCW is not supported and will not be supported.\n");
        return MPI_ERR_RMA_SYNC;
    }
    return PMPI_Win_wait(win);
}

int MPI_Win_test(MPI_Win win, int * flag)
{
    if (RMA_Win_uses_shmem(win)) {
        RMA_Error("PSCW is not supported and will not be supported.\n");
        return MPI_ERR_RMA_SYNC;
    }
    return PMPI_Win_test(win, flag);
}
