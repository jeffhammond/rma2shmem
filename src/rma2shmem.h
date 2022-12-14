#ifndef RMA2SHMEM_H_INCLUDED
#define RMA2SHMEM_H_INCLUDED

#include <mpi.h>

#ifdef __cplusplus
extern "C" {
#endif

/* MPI-3 added const to input arguments, which causes
 * incompatibilities if RMA passes in const arguments. */
#if MPI_VERSION >= 3
#define RMA2SHMEM_CONST const
#else
#define RMA2SHMEM_CONST
#endif

/* WIN */

/* RMA */

int MPIX_Accumulate(RMA2SHMEM_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
                    int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype,
                    MPI_Op op, MPI_Win win);
int MPIX_Get(void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
             int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win);
int MPIX_Put(RMA2SHMEM_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
             int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win);
int MPIX_Get_accumulate(RMA2SHMEM_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
                        void *result_addr, int result_count, MPI_Datatype result_datatype,
                        int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype,
                        MPI_Op op, MPI_Win win);

int MPIX_Rput(RMA2SHMEM_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
              int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype,
              MPI_Win win, MPI_Request *request);
int MPIX_Rget(void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
              int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype,
              MPI_Win win, MPI_Request *request);
int MPIX_Raccumulate(RMA2SHMEM_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
                     int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype,
                     MPI_Op op, MPI_Win win, MPI_Request *request);
int MPIX_Rget_accumulate(RMA2SHMEM_CONST void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
                         void *result_addr, int result_count, MPI_Datatype result_datatype,
                         int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype,
                         MPI_Op op, MPI_Win win, MPI_Request *request);

/* COLL */

#ifdef __cplusplus
}
#endif

#endif // RMA2SHMEM_H_INCLUDED
