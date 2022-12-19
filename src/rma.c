#include "rma2shmem_impl.h"

int MPI_Put(RMA2SHMEM_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
            int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win)
{
    rma2shmem_win_extras_s * extras;
    if ( RMA_Win_get_extras(win, &extras) && (extras->shmem_window) ) {

        bool rx;

        void * base = NULL;
        rx = RMA_Win_get_base(win, &base);
        if (!rx) return MPI_ERR_OTHER;

        int disp_unit = 1;
        rx = RMA_Win_get_disp_unit(win, &disp_unit);
        if (!rx) return MPI_ERR_OTHER;

        // TODO MPI comm / SHMEM team suppoort
        const int pe = target_rank;

        size_t origin_bytes, target_bytes;
        const bool origin_contig = RMA_Type_check(origin_count, origin_datatype, &origin_bytes);
        const bool target_contig = RMA_Type_check(target_count, target_datatype, &target_bytes);

        if (origin_contig) {
            if (target_contig) {

                // MPI RMA allows underflow:
                // "The data transfer is the same as that which would occur if the origin process
                //  executed a send operation with arguments origin_addr, origin_count, origin_datatype,
                //  target_rank, tag, comm, and the target process executed a receive operation with arguments
                //  target_addr, target_count, target_datatype, source, tag, comm, where target_addr is the
                //  target buffer address computed as explained above, the values of tag are arbitrary
                //  valid matching tag values, and comm is a communicator for the group of win."
                const size_t bytes = origin_bytes;

                // map from (win,offset) to sheap offset relative to base
                void * dest = base + (ptrdiff_t)disp_unit * (ptrdiff_t)target_disp;

                shmem_putmem_nbi(dest, origin_addr, bytes, pe);
                return MPI_SUCCESS;

            } else /* target non-contiguous */ {

                return MPI_ERR_OTHER;

                int nint, nadd, ndts, combiner;

                int rc = PMPI_Type_get_envelope(origin_datatype, &nint, &nadd, &ndts, &combiner);
                if (rc != MPI_SUCCESS) return rc;

                if (combiner == MPI_COMBINER_CONTIGUOUS) {
                    // CONTIGUOUS datatypes should be detected above
                    RMA_Error("This should not be possible\n");
                } else if (combiner == MPI_COMBINER_VECTOR) {
                    assert(nint==3);
                    assert(nadd==0);
                    assert(ndts==1);
                    int cbs[3]; /* {count,blocklength,stride} */
                    MPI_Datatype vbasetype[1];
                    MPI_Type_get_contents(origin_datatype, 3, 0, 1, cbs, NULL, vbasetype);
                    const int count       = cbs[0]; // count
                    const int blocklength = cbs[1]; // blocklength
                    const int stride      = cbs[2]; // stride
                    MPI_Datatype dt = vbasetype[0];
                    bool rx = RMA_Type_builtin(dt);
                    if (!rx) {
                        RMA_Error("Unsupported datatype\n");
                        return MPI_ERR_TYPE;
                    }
                    int type_size;
                    rc = PMPI_Type_size(dt, &type_size);
                    if (rc != MPI_SUCCESS) return rc;

                    // map from (win,offset) to sheap offset relative to base
                          int8_t * dest = base + (ptrdiff_t)disp_unit * (ptrdiff_t)target_disp;
                    const int8_t * orig = origin_addr;

                    const size_t bytes = type_size * blocklength;
                    const size_t jump  = type_size * stride;

                    for (int i=0; i<count; i++) {
                        shmem_putmem_nbi(dest, orig, bytes, pe);
                        dest += jump;
                        orig += bytes;
                    }
                    return MPI_SUCCESS;

                } else {
                    RMA_Error("Unsupported datatype\n");
                }
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
}
