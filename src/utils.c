#include "rma2shmem_impl.h"

/* Raise an internal fatal RMA error.
 *
 * @param[in] file Current file name (__FILE__)
 * @param[in] line Current line numeber (__LINE__)
 * @param[in] func Current function name (__func__)
 * @param[in] msg  Message to be printed
 * @param[in] code Exit error code
 */
void RMA_Error_impl(const char *file, const int line, const char *func, const char *msg, ...)
{
    va_list ap;
    int  disp;
    char string[500];

    disp  = 0;
    va_start(ap, msg);
    disp += vsnprintf(string, 500, msg, ap);
    va_end(ap);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    fprintf(stderr, "[%d] RMA Internal error in %s (%s:%d)\n[%d] Message: %s\n",
            rank, func, file, line, rank, string);
    MPI_Abort(MPI_COMM_WORLD, 100);
}

/* Message something to terminal.
 *
 * @param[in] file Current file name (__FILE__)
 * @param[in] line Current line numeber (__LINE__)
 * @param[in] func Current function name (__func__)
 * @param[in] msg  Message to be printed
 * @param[in] code Exit error code
 */
void RMA_Message_impl(const char *file, const int line, const char *func, const char *msg, ...)
{
    va_list ap;
    int  disp;
    char string[500];

    disp  = 0;
    va_start(ap, msg);
    disp += vsnprintf(string, 500, msg, ap);
    va_end(ap);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    fprintf(stderr, "[%d] RMA Message in %s (%s:%d)\n[%d]: %s\n",
            rank, func, file, line, rank, string);
}

// the attribute value is a pointer to the struct (in the heap)
bool RMA_Win_get_extras(MPI_Win win, rma2shmem_win_extras_s ** extras)
{
    int flag;
    *extras = NULL;

    int rc = PMPI_Win_get_attr(win, rma2shmem_key, extras, &flag);
    if (rc != MPI_SUCCESS) return false;

    return (flag != 0);
}

#if 0
MPI_Win_get_attr(win, MPI_WIN_BASE, &base, &flag),
MPI_Win_get_attr(win, MPI_WIN_SIZE, &size, &flag),
MPI_Win_get_attr(win, MPI_WIN_DISP_UNIT, &disp_unit, &flag),
MPI_Win_get_attr(win, MPI_WIN_CREATE_FLAVOR, &create_kind, &flag), and
MPI_Win_get_attr(win, MPI_WIN_MODEL, &memory_model, &flag)
#endif

bool RMA_Win_get_size(MPI_Win win, MPI_Aint * base)
{
        int flag;
        MPI_Aint aint;
        int rc = PMPI_Win_get_attr(win, MPI_WIN_SIZE, &aint, &flag);
        if (rc != MPI_SUCCESS) return false;
        if (!flag) return false;
        *base = aint;
        return true;
}

bool RMA_Win_get_base(MPI_Win win, void * base)
{
        int flag;
        MPI_Aint aint;
        int rc = PMPI_Win_get_attr(win, MPI_WIN_BASE, &aint, &flag);
        if (rc != MPI_SUCCESS) return false;
        if (!flag) return false;
        *((void**)base) = (void*)aint;
        return true;
}

bool RMA_Win_get_disp_unit(MPI_Win win, int * disp)
{
        int flag;
        int rc = PMPI_Win_get_attr(win, MPI_WIN_DISP_UNIT, disp, &flag);
        if (rc != MPI_SUCCESS) return false;
        if (!flag) return false;
        return true;
}

bool RMA_Type_builtin(MPI_Datatype dt)
{
    int ni, na, nd, c;
    int rc = PMPI_Type_get_envelope(dt, &ni, &na, &nd, &c);
    return (c == MPI_COMBINER_NAMED);
}

bool RMA_Type_check(int count, MPI_Datatype datatype, size_t * bytes)
{
    int rc = MPI_SUCCESS;
    *bytes = -1;

    int size;
    rc = PMPI_Type_size(datatype, &size);
    if (rc != MPI_SUCCESS) return false;

    MPI_Aint lb; // unused

    // The extent of a datatype is defined to be the span from the first byte to the last byte
    // occupied by entries in this datatype, rounded up to satisfy alignment requirements.

    MPI_Aint extent;
    rc = PMPI_Type_get_extent(datatype, &lb, &extent);
    if (rc != MPI_SUCCESS) return false;

    if ((intptr_t)extent != (intptr_t)bytes ) {
        RMA_Message("origin bytes=%zu extent=%zu\n", bytes, extent);
        return false;
    }

    *bytes = count * size;
    return true;
}
