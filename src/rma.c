#include "rma2shmem_impl.h"

/*
MPI_Win_get_attr(win, MPI_WIN_BASE, &base, &flag),
MPI_Win_get_attr(win, MPI_WIN_SIZE, &size, &flag),
MPI_Win_get_attr(win, MPI_WIN_DISP_UNIT, &disp_unit, &flag),
MPI_Win_get_attr(win, MPI_WIN_CREATE_FLAVOR, &create_kind, &flag), and
MPI_Win_get_attr(win, MPI_WIN_MODEL, &memory_model, &flag)
*/

bool RMA_Type_check(int count, MPI_Datatype datatype, size_t * bytes)
{
    int rc = MPI_SUCCESS;
    *bytes = -1;

    int size;
    rc = PMPI_Type_size(datatype, &size);
    if (rc != MPI_SUCCESS) return false;

    MPI_Aint lb; // unused

    // The extent of a datatype is defined to be the span from the first byte to the last byte
    // occupied by entries in this datatype, rounded up to satisfy alignment requirements.

    MPI_Aint extent;
    rc = PMPI_Type_get_extent(datatype, &lb, &extent);
    if (rc != MPI_SUCCESS) return false;

    if ((intptr_t)extent != (intptr_t)bytes ) {
        RMA_Message("origin bytes=%zu extent=%zu\n", bytes, extent);
        return false;
    }

    *bytes = count * size;
    return true;
}

int MPI_Put(RMA2SHMEM_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
            int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win)
{
    rma2shmem_win_extras_s * extras;
    if ( RMA_Win_get_extras(win, &extras) && (extras->shmem_window) ) {

        size_t origin_bytes, target_bytes;
        bool origin_contig = RMA_Type_check(origin_count, origin_datatype, &origin_bytes);
        bool target_contig = RMA_Type_check(target_count, target_datatype, &target_bytes);

        if (origin_contig) {
            if (target_contig) {

                void * base = NULL;
                bool rx = RMA_Win_get_base(win, &base);
                if (!rx) return MPI_ERR_OTHER;

                int disp = 1;
                rx = RMA_Win_get_disp_unit(win, &disp);
                if (!rx) return MPI_ERR_OTHER;

                // map from (win,offset) to sheap offset relative to base
                void * dest = base + (ptrdiff_t)disp * (ptrdiff_t)target_disp;

                /* MPI RMA allows underflow:
                  "The data transfer is the same as that which would occur if the origin process
                   executed a send operation with arguments origin_addr, origin_count, origin_datatype,
                   target_rank, tag, comm, and the target process executed a receive operation with arguments
                   target_addr, target_count, target_datatype, source, tag, comm, where target_addr is the
                   target buffer address computed as explained above, the
                   values of tag are arbitrary valid matching tag values,
                   and comm is a communicator for the group of win."             */
                const size_t bytes = origin_bytes;

                // TODO MPI comm / SHMEM team suppoort
                const int pe = target_rank;

                shmem_putmem_nbi(dest, origin_addr, bytes, pe);
                return MPI_SUCCESS;

            } else /* target non-contiguous */ {

                return MPI_ERR_OTHER;

                // TODO contiguous origin buffer

            }
        } else /* origin non-contiguous */ {

        }
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
