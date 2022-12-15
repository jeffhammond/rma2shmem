#ifndef RMA2SHMEM_IMPL_H
#define RMA2SHMEM_IMPL_H

#include "rma2shmemconf.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <stddef.h>

#include <mpi.h>
#include <shmem.h>

#include "rma2shmem.h"

#include "likely.h"

void RMA_Error_impl(const char *file, const int line, const char *func, const char *msg, ...);
void RMA_Message_impl(const char *file, const int line, const char *func, const char *msg, ...);

#define RMA_Error(...) RMA_Error_impl(__FILE__,__LINE__,__func__,__VA_ARGS__)

#define RMA_Message(...) RMA_Message_impl(__FILE__,__LINE__,__func__,__VA_ARGS__)

extern int rma2shmem_key;

typedef struct
{
    bool            shmem_window;
    int             sync_state;  // TODO
    MPI_Comm        comm;
}
rma2shmem_win_extras_s;

bool RMA_Win_get_extras(MPI_Win win, rma2shmem_win_extras_s ** extras);
bool RMA_Win_uses_shmem(MPI_Win win);
bool RMA_Win_get_base(MPI_Win win, void * base);
bool RMA_Win_get_disp_unit(MPI_Win win, int * disp);

#endif // RMA2SHMEM_IMPL_H
