/* ************************* COMPILATION INSTRUCTIONS *******************************************/
/* The following options will compile this performance based matrix calculator:                 */
/* All options expect you to run the shell by typing './exec_matrix' to run it.                 */
/*   1) Make all    - Makes a zip file and creates and executable named 'exec_matrix'.          */
/*   2) Make exec_matrix   - Creates and executable named 'matrix_exec' which you can then run. */
/*   3) Make clean  - Deletes executable files and object files from folder.                    */
/*                                                                                              */
/* @author Michael Walker.                                                                      */
/* ************************* END COMPILATION INSTRUCTIONS ***************************************/

#include "matrix.h"

/* *-**-**-**-**-**-**-**-* HELPER FUNCTIONS *-**-**-**-**-**-**-**-**-**-**-**-**-**-**-**-**-* */

/**
 * Compares C and C_1, where C was calculated using a single threads and C_1 is calculated using an
 * enhanced multi-threaded algorithm. It prints out the time taken during the calculation part. The
 * thread must contain initialized timspec start and stop variables so that the time taken can be
 * determined.
 *
 * @return true if both matrices are equal, false otherwise.
 */
bool compare_both_matrices()
{
    int i, j, num_threads = 1; /* i, j are loop counter variables, num_threads is the number of threads */
    double time = 0;           /* Used to calculate the time taken during the calculation part of the program. */

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < p; j++)
        {
            if (abs(C_1[i][j] - C[i][j]) > 0.00001)
            {
                return false; /* Error encountered. */
            }
        }
    }
    printf("Successfully nade", 1, time);
    return true;
}

/**
 * Gets a thread based on compute_next and uses the thread's information to make matrix
 * calculations that would otherwise be done a single thread. The thread should provide
 * enough information to get the row to start, the row to end on, as well as the ability
 * to store starting and ending times. Also note that the row to start on is inclusive
 * but the row to end on is exclusive.
 *
 * @arg *compute_thread links to the thread that should be executed next.
 */
void *enhanced_mult_matrix(void *compute_thread)
{
    struct threads *thread = compute_thread;
    multiply_matrix(C_1, thread->row_start, thread->row_end); /* Do the calculations. */
}

/**
 * Initializes matrices A and B where A is initialized to A[row][columm] = row + column and
 * B[row][column] = column. A and B need not be cleared before they can be initialized but
 * everything already in A and B at beginning of the function call will have been destroyed
 * by the end of the call.
 */
void initialize_matrices()
{
    int i, j; /* Loop counters. */
    /* Initialize A. */
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < m; j++)
        {
            A[i][j] = i + j;
        }
    }
    /* Initialize B. */
    for (i = 0; i < m; i++)
    {
        for (j = 0; j < p; j++)
        {
            B[i][j] = j;
        }
    }
}

/**
 * Initializes num_threads number of threads in order to run an enhanced matrix calculation algorithm.
 * Matrix A[1200][1000] and B[1000][500] must already be initialized but the thread_head and thread_tail
 * pointer should be NULL otherwise all data contained within those structs will be destroyed.
 *
 * @return the time it took run the enhanced multi-threaded matrix algorithm.
 */
double process_threads(int num_threads)
{
    struct threads *thread;      /* Temporary variable used to manage thread adding, removing threads. */
    int i, j;                    /* i, j are loop counter variables.*/
    struct timespec start, stop; /* Keeps track of starting and stop times for computing time taken. */

    clock_gettime(CLOCK_REALTIME, &start); /* Get the start time. */
    for (i = 0; i < num_threads; i++)
    {
        thread = (struct threads *)malloc(sizeof(struct threads)); /* Create new thread to store thread data. */
        if (thread_head == NULL)
        {
            thread_tail = thread;
            thread->row_start = 0;
        }
        else
        {
            thread->row_start = thread_head->row_end;
            thread_head->next = thread;
        }
        thread_head = thread; /* Add to head of the linked list. */
        thread_head->row_end = (i + 1) * n / num_threads;
        pthread_create(&thread->thread_id, 0, enhanced_mult_matrix, (void *)thread_head);
    }

    /* Basically we need to wait until all threads are done with their work. */
    thread = thread_tail;
    while (thread_tail != NULL)
    {
        pthread_join(thread_tail->thread_id, NULL);
        thread = thread_tail;
        thread_tail = thread_tail->next;
        free(thread); /* Free the memory. */
    }
    clock_gettime(CLOCK_REALTIME, &stop); /* Get the end time. */
    thread_head = NULL;                   /* Set the head pointer to NULL for the next round. */
    return CALC_TIME(start.tv_sec, stop.tv_sec, start.tv_nsec, stop.tv_nsec);
}

/**
 * Takes a matrix array where the calculations should be stored; starting and ending row
 * numbers. It assumes A[1200][1000] and B[1000][500] are already initialized. Furthermore
 * any information stored in the array from [row_start, row_end) will be cleared.
 *
 * @code array - the array where the computed information is stored in.
 * @code row_start - the row in the matrix where the calculations are started from (inclusive).
 * @code row_end - the row in the matrix where the calculations are ended on (exclusive).
 */
void multiply_matrix(int array[1200][500], int row_start, int row_end)
{
    int i, j, k; /* Loop counter variables. */
    for (i = row_start; i < row_end; i++)
    {
        for (j = 0; j < p; j++)
        {
            array[i][j] = 0; /* Set the value first to 0. */
            for (k = 0; k < m; k++)
            {
                array[i][j] += A[i][k] * B[k][j]; /* Update the value. */
            }
        }
    }
}

/* *-**-**-**-**-**-**-**-**-**-* Main FUNCTION *-**-**-**-**-**-**-**-**-**-**-**-**-**-**-**-**-* */

/**
 * Takes a number via the console to indicate how many threads should be created. It then creates that
 * many threads with the first thread calculating C fully on its own and the rest of the threads divvy
 * up the work of computing C_1. Both C and C_1 get run through the same algorithm except is done using
 * a multi-threaded workload in a bit to increase performance. By the method is finished, the to matrices
 * should be compared and if an error occurred then while printing out the time taken for any given thread
 * that must be specified, otherwise it should indicate no error found.
 */
int main(int argc, char *argv[])
{
    bool no_error_found;         /* Used to indicate whether our matrix comparison algorithm found an error */
    int i;                       /* Loop counter variable */
    double time;                 /* Temporary value used to calculate the time it took to run calculations. */
    struct timespec start, stop; /* Keeps track of starting and stop times for computing time taken. */
    struct threads *thread;      /* Temporary variable used to manage thread adding, removing threads. */

    initialize_matrices(); /* Initialize A and B. */

    /* Prints the header for the thread output that indicates correctness. */
    printf("%-10s | %-15s | %10s\n", "Threads", "Seconds", "Error Exists?");

    clock_gettime(CLOCK_REALTIME, &start); /* Get the start time. */
    multiply_matrix(C, 0, n);              /* Calculate C matrix using single thread. */
    clock_gettime(CLOCK_REALTIME, &stop);  /* Get the end time. */

    /* Calculate the time it took for the matrix calculation to be done. */
    time = CALC_TIME(start.tv_sec, stop.tv_sec, start.tv_nsec, stop.tv_nsec);
    printf("%-10d | %-15f | n\\a\n", 1, time);
    fflush(NULL); /* Make sure the content actually gets written out. */

    /* Increment the number of threads by one each time. */
    for (i = 2; i <= atoi(argv[1]); i++)
    {
        time = process_threads(i); /* Process all threads including having them run the matrix calc algorithm. */

        /* Compare the matrix with single thread along with thread with multiple thread matrix. */
        no_error_found = compare_both_matrices();
        if (no_error_found)
        {
            /* No error encountered so please indicate that. */
            printf("%-10d | %-15f | No Error Found\n", i, time);
        }
        else
        {
            /* Error encountered so please indicate that. */
            printf("%-10d | %-15f | Error Found\n", i, time);
        }
    }
}
