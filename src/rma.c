#include "rma2shmem_impl.h"

/*
MPI_Win_get_attr(win, MPI_WIN_BASE, &base, &flag),
MPI_Win_get_attr(win, MPI_WIN_SIZE, &size, &flag),
MPI_Win_get_attr(win, MPI_WIN_DISP_UNIT, &disp_unit, &flag),
MPI_Win_get_attr(win, MPI_WIN_CREATE_FLAVOR, &create_kind, &flag), and
MPI_Win_get_attr(win, MPI_WIN_MODEL, &memory_model, &flag)
*/

bool RMA_Type_check(int origin_count, MPI_Datatype origin_datatype,
                    int target_count, MPI_Datatype target_datatype,
                    size_t * bytes)
{
    int rc = MPI_SUCCESS;

    int origin_size;
    rc = PMPI_Type_size(origin_datatype, &origin_size);
    if (rc != MPI_SUCCESS) return false;
    const size_t origin_bytes = origin_count * origin_size;

    int target_size;
    rc = PMPI_Type_size(target_datatype, &target_size);
    if (rc != MPI_SUCCESS) return false;
    const size_t target_bytes = target_count * target_size;

    MPI_Aint lb; // unused

    // The extent of a datatype is defined to be the span from the first byte to the last byte
    // occupied by entries in this datatype, rounded up to satisfy alignment requirements.

    MPI_Aint origin_extent;
    rc = PMPI_Type_get_extent(origin_datatype, &lb, &origin_extent);
    if (rc != MPI_SUCCESS) return false;

    MPI_Aint target_extent;
    rc = PMPI_Type_get_extent(target_datatype, &lb, &target_extent);
    if (rc != MPI_SUCCESS) return false;

    if ((origin_bytes  != target_bytes ) || (origin_extent != target_extent) ||
        (origin_extent != origin_bytes ) || (target_extent != target_bytes )) {
        RMA_Message("origin bytes=%zu extent=%zu target bytes=%zu extent=%zu\n",
                     origin_bytes, origin_extent, target_bytes, target_extent);
        return false;
    }

    *bytes = origin_bytes;
    return true;
}

int MPI_Put(RMA2SHMEM_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
            int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win)
{
    rma2shmem_win_extras_s * extras;
    if ( RMA_Win_get_extras(win, &extras) && (extras->shmem_window) ) {

        size_t bytes;
        bool rx = RMA_Type_check(origin_count, origin_datatype, target_count, target_datatype, &bytes);
        if (!rx) return MPI_ERR_TYPE;

        void * base = NULL;
        rx = RMA_Win_get_base(win, &base);
        if (rx != 0) return MPI_ERR_OTHER;

        int disp = 1;
        rx = RMA_Win_get_disp_unit(win, &disp);
        if (rx != 0) return MPI_ERR_OTHER;

        void * dest = base + (ptrdiff_t)disp * (ptrdiff_t)target_disp;

        // TODO remap rank to pe where appropriate
        int pe = target_rank;

        shmem_putmem_nbi(dest, origin_addr, bytes, pe);
        return MPI_SUCCESS;
    }
    return PMPI_Put(origin_addr, origin_count, origin_datatype,
                    target_rank, target_disp, target_count, target_datatype,
                    win);
}

int MPI_Get(void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
            int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win)
{
    return PMPI_Get(origin_addr, origin_count, origin_datatype,
                    target_rank, target_disp, target_count, target_datatype,
                    win);
}

int MPI_Accumulate(RMA2SHMEM_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
                   int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype,
                   MPI_Op op, MPI_Win win)
{
    return PMPI_Accumulate(origin_addr, origin_count, origin_datatype,
                           target_rank, target_disp, target_count, target_datatype,
                           op, win);
}

int MPI_Get_accumulate(RMA2SHMEM_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
                       void *result_addr, int result_count, MPI_Datatype result_datatype,
                       int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype,
                       MPI_Op op, MPI_Win win)
{
    return PMPI_Get_accumulate(origin_addr, origin_count, origin_datatype,
                               result_addr, result_count, result_datatype,
                               target_rank, target_disp, target_count, target_datatype,
                               op, win);
}

int MPI_Rput(RMA2SHMEM_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
             int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype,
             MPI_Win win, MPI_Request *request)
{
    return PMPI_Rput(origin_addr, origin_count, origin_datatype,
                     target_rank, target_disp, target_count, target_datatype,
                     win, request);
}

int MPI_Rget(void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
             int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype,
             MPI_Win win, MPI_Request *request)
{
    return PMPI_Rget(origin_addr, origin_count, origin_datatype,
                     target_rank, target_disp, target_count, target_datatype,
                     win, request);
}

int MPI_Raccumulate(RMA2SHMEM_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
                    int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype,
                    MPI_Op op, MPI_Win win, MPI_Request *request)
{
    return PMPI_Raccumulate(origin_addr, origin_count, origin_datatype,
                            target_rank, target_disp, target_count, target_datatype,
                            op, win, request);
}

int MPI_Rget_accumulate(RMA2SHMEM_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
                        void *result_addr, int result_count, MPI_Datatype result_datatype,
                        int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype,
                        MPI_Op op, MPI_Win win, MPI_Request * request)
{
    return PMPI_Rget_accumulate(origin_addr, origin_count, origin_datatype,
                                result_addr, result_count, result_datatype,
                                target_rank, target_disp, target_count, target_datatype,
                                op, win, request);
    return rc;
}
