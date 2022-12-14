#
# Copyright (C) 2014. See LICENSE in top-level directory.
#

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -I$(top_srcdir)/src

lib_LTLIBRARIES = librma2shmem.la

noinst_LTLIBRARIES = librma2shmemi.la

librma2shmem_la_SOURCES =   src/win.c \
			    src/rma.c \
			    src/comm.c \
			    src/utils.c

librma2shmem_la_LDFLAGS = -version-info $(librma2shmem_abi_version)

librma2shmemi_la_SOURCES = $(librma2shmem_la_SOURCES)
librma2shmemi_la_LDFLAGS = $(librma2shmem_abi_version)

include_HEADERS = src/rma2shmem.h src/rma2shmem_impl.h

bin_PROGRAMS =
check_PROGRAMS = 
TESTS = 
XFAIL_TESTS = 

MPIEXEC = mpiexec -n 2
TESTS_ENVIRONMENT = $(MPIEXEC)

include test/Makefile.mk

.PHONY: checkprogs
checkprogs: $(check_PROGRAMS)
