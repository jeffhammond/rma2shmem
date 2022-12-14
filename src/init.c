#include "rma2shmem_impl.h"

int MPI_Init(int *argc, char ***argv)
{
    int rc = PMPI_Init(argc, argv);
    shmem_init();
    return rc;
}

int MPI_Finalize(void)
{
    shmem_finalize();
    return PMPI_Finalize();
}

int MPI_Init_thread(int *argc, char ***argv, int required, int *provided)
{
    int shmem_requested, shmem_provided;

    int rc = PMPI_Init_thread(argc, argv, required, provided);
    if (rc != MPI_SUCCESS) return rc;

    if (       required  ==   MPI_THREAD_SINGLE) {
         shmem_requested =  SHMEM_THREAD_SINGLE;
    } else if (required  ==   MPI_THREAD_FUNNELED) {
         shmem_requested =  SHMEM_THREAD_FUNNELED;
    } else if (required  ==   MPI_THREAD_SERIALIZED) {
         shmem_requested =  SHMEM_THREAD_SERIALIZED;
    } else if (required  ==   MPI_THREAD_MULTIPLE) {
         shmem_requested =  SHMEM_THREAD_MULTIPLE;
    }

    int rx = shmem_init_thread(shmem_requested, &shmem_provided);
    if (rx != 0) return MPI_ERR_OTHER;

    if (       shmem_provided == SHMEM_THREAD_SINGLE) {
                     provided =    MPI_THREAD_SINGLE;
    } else if (shmem_provided == SHMEM_THREAD_FUNNELED) {
                     provided =    MPI_THREAD_FUNNELED;
    } else if (shmem_provided == SHMEM_THREAD_SERIALIZED) {
                     provided =    MPI_THREAD_SERIALIZED;
    } else if (shmem_provided == SHMEM_THREAD_MULTIPLE) {
                     provided =    MPI_THREAD_MULTIPLE;
    }

    return rc;
}
