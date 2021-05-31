#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <time.h>
#include <unistd.h>

#define CALC_TIME(start_s, stop_s, start_n, stop_n) (stop_s - start_s) + (stop_n - start_n) / 1E9;

/**
 * Stores the information for a single thread such as the thread ID, whether it still needs
 * to compute its share of the matrix, which row it should start on and which row its work
 * should end on. It also points to previous and next nodes in the list.
 */
struct threads
{
    int row_start, row_end; /* Compute starts on row_start and ends on row_end [row_start, row_end).*/
    pthread_t thread_id;    /* Keeps track of the threads ID. */
    struct threads *next;   /* Points to the next struct in the linked list. */
};

struct threads *thread_head; /* Points the head of a doubly linked list. */
struct threads *thread_tail; /* Points to tail of a doubly linked list. */

const int n = 1200; /* The row size for A, C, and C_1. */
const int m = 1000; /* The row size for B and the column size for A. */
const int p = 500;  /* The column size for B, C, and C_1. */

int A[1200][1000];  /* Keeps track of the matrix A. */
int B[1000][500];   /* Keeps track of the matrix B. */
int C[1200][500];   /* Keeps track of matrix multiplication using the basic algorithm. */
int C_1[1200][500]; /* Keeps track of the matrix multiplication using the enhanced algorithm. */

/**
 * Compares C and C_1, where C was calculated using a single threads and C_1 is calculated using an
 * enhanced multi-threaded algorithm. It prints out the time taken during the calculation part. The
 * thread must contain initialized timspec start and stop variables so that the time taken can be
 * determined.
 * 
 * @return true if both matrices are equal, false otherwise.
 */
bool compare_both_matrices();

/**
 * Gets a thread based on compute_next and uses the thread's information to make matrix
 * calculations that would otherwise be done a single thread. The thread should provide
 * enough information to get the row to start, the row to end on, as well as the ability
 * to store starting and ending times. Also note that the row to start on is inclusive
 * but the row to end on is exclusive.
 * 
 * @arg *compute_thread links to the thread that should be executed next.
 */
void *enhanced_mult_matrix(void *thread);

/**
 * Initializes matrices A and B where A is initialized to A[row][columm] = row + column and
 * B[row][column] = column. A and B need not be cleared before they can be initialized but
 * everything already in A and B at beginning of the function call will have been destroyed
 * by the end of the call.
 */
void initialize_matrices();

/**
 * Takes a matrix array where the calculations should be stored; starting and ending row 
 * numbers. It assumes A[1200][1000] and B[1000][500] are already initialized. Furthermore
 * any information stored in the array from [row_start, row_end) will be cleared.
 * 
 * @code array - the array where the computed information is stored in.
 * @code row_start - the row in the matrix where the calculations are started from (inclusive).
 * @code row_end - the row in the matrix where the calculations are ended on (exclusive).
 */
void multiply_matrix(int array[1200][500], int row_start, int row_end);

/**
 * Initializes num_threads number of threads in order to run an enhanced matrix calculation algorithm.
 * Matrix A[1200][1000] and B[1000][500] must already be initialized but the thread_head and thread_tail
 * pointer should be NULL otherwise all data contained within those structs will be destroyed.
 * 
 * @return the time it took run the enhanced multi-threaded matrix algorithm.
 */
double process_threads(int num_threads);
