#include <stdio.h>
#include <mpi.h>

int main(void)
{
    MPI_Init(NULL,NULL);

    MPI_Datatype dt;
    MPI_Type_vector(100, 1, 2, MPI_INT, &dt);

    int s;
    MPI_Aint lb,e;
    MPI_Type_size(dt,&s);
    printf("type size %d\n",s);
    MPI_Type_get_extent(dt,&lb,&e);
    printf("type extent %ld\n",e);

    MPI_Type_free(&dt);

    MPI_Finalize();
    return 0;
}
