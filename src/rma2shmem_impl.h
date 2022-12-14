#ifndef RMA2SHMEM_IMPL_H
#define RMA2SHMEM_IMPL_H

#include "rma2shmemconf.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <stddef.h>

#include <mpi.h>
#include <shmem.h>

#include "rma2shmem.h"

#include "likely.h"

void RMA_Error_impl(const char *file, const int line, const char *func, const char *msg, ...);

#define RMA_Error(...) RMA_Error_impl(__FILE__,__LINE__,__func__,__VA_ARGS__)

extern int rma2shmem_key;

#endif // RMA2SHMEM_IMPL_H
