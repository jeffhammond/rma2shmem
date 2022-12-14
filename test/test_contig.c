#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <mpi.h>
#include "rma2shmem.h"

static void print_typename(MPI_Datatype type)
{
    char * name;
    if      (type==MPI_CHAR              ) name="MPI_CHAR              ";
    else if (type==MPI_SIGNED_CHAR       ) name="MPI_SIGNED_CHAR       ";
    else if (type==MPI_UNSIGNED_CHAR     ) name="MPI_UNSIGNED_CHAR     ";
    else if (type==MPI_BYTE              ) name="MPI_BYTE              ";
    else if (type==MPI_WCHAR             ) name="MPI_WCHAR             ";
    else if (type==MPI_SHORT             ) name="MPI_SHORT             ";
    else if (type==MPI_UNSIGNED_SHORT    ) name="MPI_UNSIGNED_SHORT    ";
    else if (type==MPI_INT               ) name="MPI_INT               ";
    else if (type==MPI_UNSIGNED          ) name="MPI_UNSIGNED          ";
    else if (type==MPI_LONG              ) name="MPI_LONG              ";
    else if (type==MPI_UNSIGNED_LONG     ) name="MPI_UNSIGNED_LONG     ";
    else if (type==MPI_FLOAT             ) name="MPI_FLOAT             ";
    else if (type==MPI_DOUBLE            ) name="MPI_DOUBLE            ";
    else if (type==MPI_LONG_DOUBLE       ) name="MPI_LONG_DOUBLE       ";
    else if (type==MPI_LONG_LONG_INT     ) name="MPI_LONG_LONG_INT     ";
    else if (type==MPI_LONG_LONG         ) name="MPI_LONG_LONG         ";
    else if (type==MPI_UNSIGNED_LONG_LONG) name="MPI_UNSIGNED_LONG_LONG";
    else                                   name="NOT_PREDEFINED_TYPE   ";
    printf("type = %s\n", name);
    return;
}

int main(int argc, char* argv[])
{
    const int e = 60;

    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Count n = 1;
    int errors = 0;
    for (int i=0; i<e; i++) {
        if (rank==0) {

            MPI_Datatype intype = MPI_CHAR, bigtype, outtype;
            MPI_Count nout;

            if (n!=nout) {
                printf("in (%zu) and out (%zu) counts do not match!\n", (size_t)n, (size_t)nout);
                errors++;
            }
            if (intype!=outtype) {
                printf("intype (%d) and outtype (%d) do not match!\n", intype, outtype);
                print_typename(intype);
                print_typename(outtype);
                errors++;
            }
        } /* rank==0 */

        n *= 2;

    }
    MPI_Finalize();
    return errors;
}
