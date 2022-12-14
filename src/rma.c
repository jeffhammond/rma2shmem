#include "rma2shmem_impl.h"

int MPI_Put(RMA2SHMEM_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
             int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win)
{
    int rc = MPI_SUCCESS;

        rc = MPI_Put(origin_addr, origin_count, origin_datatype,
                     target_rank, target_disp, target_count, target_datatype, win);

    return rc;
}

int MPI_Get(void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
             int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win)
{
    int rc = MPI_SUCCESS;

        rc = MPI_Get(origin_addr, origin_count, origin_datatype,
                     target_rank, target_disp, target_count, target_datatype, win);

    return rc;
}

int MPI_Accumulate(RMA2SHMEM_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
                    int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype,
                    MPI_Op op, MPI_Win win)
{
    int rc = MPI_SUCCESS;

        rc = MPI_Accumulate(origin_addr, origin_count, origin_datatype,
                            target_rank, target_disp, target_count, target_datatype,
                            op, win);

    return rc;
}

int MPI_Get_accumulate(RMA2SHMEM_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
                        void *result_addr, int result_count, MPI_Datatype result_datatype,
                        int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype,
                        MPI_Op op, MPI_Win win)
{
    int rc = MPI_SUCCESS;

        rc = MPI_Get_accumulate(origin_addr, origin_count, origin_datatype,
                                result_addr, result_count, result_datatype,
                                target_rank, target_disp, target_count, target_datatype,
                                op, win);

    return rc;
}

int MPI_Rput(RMA2SHMEM_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
              int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype,
              MPI_Win win, MPI_Request *request)
{
    int rc = MPI_SUCCESS;

        rc = MPI_Rput(origin_addr, origin_count, origin_datatype,
                     target_rank, target_disp, target_count, target_datatype, win, request);

    return rc;
}

int MPI_Rget(void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
              int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype,
              MPI_Win win, MPI_Request *request)
{
    int rc = MPI_SUCCESS;

        rc = MPI_Rget(origin_addr, origin_count, origin_datatype,
                     target_rank, target_disp, target_count, target_datatype, win, request);

    return rc;
}

int MPI_Raccumulate(RMA2SHMEM_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
                      int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype,
                      MPI_Op op, MPI_Win win, MPI_Request *request)
{
    int rc = MPI_SUCCESS;

        rc = MPI_Raccumulate(origin_addr, origin_count, origin_datatype,
                            target_rank, target_disp, target_count, target_datatype,
                            op, win, request);

    return rc;
}

int MPI_Rget_accumulate(RMA2SHMEM_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
                         void *result_addr, int result_count, MPI_Datatype result_datatype,
                         int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype,
                         MPI_Op op, MPI_Win win, MPI_Request * request)
{
    int rc = MPI_SUCCESS;

        rc = MPI_Rget_accumulate(origin_addr, origin_count, origin_datatype,
                                result_addr, result_count, result_datatype,
                                target_rank, target_disp, target_count, target_datatype,
                                op, win, request);

    return rc;
}
