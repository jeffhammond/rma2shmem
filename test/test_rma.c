#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <strings.h>
#include <assert.h>

#include <mpi.h>
#include "rma2shmem.h"
#include "verify_buffer.h"

int main(int argc, char * argv[])
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Count n = 1000;

    double * baseptr = NULL;
    MPI_Win win;
    MPI_Aint winsize = (rank==0 ? n : 0);

    /* Allocate all the window memory on rank 0 */
    MPI_Win_allocate(winsize, sizeof(double), MPI_INFO_NULL, MPI_COMM_WORLD, &baseptr, &win);
    MPI_Win_lock_all(0, win);

#if 0
    if (rank==0) {
        for (size_t i=0; i<(n/sizeof(double)); i++) {
            baseptr[i] = 0.0;
        }
        MPI_Win_sync(win);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    double * buf = NULL;
    MPI_Alloc_mem((MPI_Aint)n, MPI_INFO_NULL, &buf);
    for (size_t i=0; i<(n/sizeof(double)); i++) {
        buf[i] = 1.0;
    }

    MPI_Accumulate(buf, n/sizeof(double), MPI_DOUBLE,
                   0 /* target */, 0 /* disp */, n/sizeof(double), MPI_DOUBLE, MPI_SUM, win);
    MPI_Win_flush(0,win);

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank==0) {
        MPI_Win_sync(win);
        double expected = size;
        size_t errors = verify_doubles(baseptr, n/sizeof(double), expected);
        if (errors > 0) {
            printf("There were %zu errors!", errors);
            for (size_t i=0; i<(n/(sizeof(double))); i++) {
                printf("baseptr[%zu] = %lf (expected %lf)\n", i, baseptr[i], expected);
            }
        }
        if (errors==0) {
            printf("SUCCESS\n");
        }
    }

#endif
    MPI_Win_unlock_all(win);
    MPI_Win_free(&win);

    MPI_Finalize();

    return 0;
}
