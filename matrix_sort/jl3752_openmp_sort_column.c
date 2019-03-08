/** compile with the following command:
 * gcc filename.c -fopenmp
 * gcc -g -o pt jl3752_mm_pt.c -O3 -pthread
 * Run with following command:
 * ./pt <matrix size> <number of threads>
 * Example:
 * ./pt 2048 8
 * */

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


void print_mat(int n, double *mat[n])
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%f ", mat[i][j]);
        }
        printf("\n");
    }
}


void single_thread_sort(double *mat[], int m, int n){
 int curr_locations[n]; // holds rows locations of the matrix at each stage in the sorting
    for(int i=0; i< n; i++){
        curr_locations[i]=i;
    }
    int maxRow = n<=m ? n : m;
    for(int i=0; i<maxRow; i++){
        int argmax = i, max = mat[i][i];
        for(int j=i+1; j<n; j++){
            if(max < mat[j][i]){
                max = mat[j][i];
                argmax = j;
            }
        }
       swap(mat, argmax, i, m); 
    }

}

void swap(double *mat[], int row_i, int row_j, int n_cols){
    for(int i=0; i<n_cols; i++){
        int temp = mat[row_i][i];
        mat[row_i][i] = mat[row_j][i];
        mat[row_j][i] = temp;
    }
}


void openmp_sort(double *mat[], int m, int n){
//  int curr_locations[n]; // holds rows locations of the matrix at each stage in the sorting
//     for(int i=0; i< n; i++){
//         curr_locations[i]=i;
//     }
//     //find new locations of rows
//     int num_row_blocks = m%n_threads == 0 ? m/n_threads : m/n_threads +1;
//     //If not square matrix just sort until minimum between number of rows and cols
//     int num_col_blocks = n%n_threads == 0 ? n/n_threads : n/n_threads +1;
//     //Take the minimum of both
//     int num_blocks = num_row_blocks<num_col_blocks ? num_row_blocks : num_col_blocks;
//     for(int i=0; i<num_blocks; i++){
//         int start_block = i*n_threads;
//         #pragma omp parallel {
//             int thread_ID = omp_get_thread_num();
//             max_heaps[thread_ID] = HeapInit(thread_ID+1); //allow one more element in the heap
//         }

//         /** Fill up heaps with maximum elements */   
//         #pragma omp parallel {
//             /* Obtain and print thread id */
//             int thread_ID = omp_get_thread_num();
//             // printf("Hello World from thread = %d\n", tid);
//             int start_line = start_block+tid;
//             //fill up the heap
//             for(j=start_line; j<start_line+tid j<n && start_line<m; j++){
//                 insert_heap(max_heaps[thread_ID], A[curr_locations[j]][start_line]);
//             }
//             //finish traversing the column
//             for(j=start_line+tid; j<n && start_line<m; j++){
//                 insert_heap(max_heaps[thread_ID], A[curr_locations[j]][start_line]);
//                 delete_max_heap(max_heaps[thread_ID]);
//             }
//             /* All threads join master thread and terminate */
//         }

//         /** Do the swapping */   
//         #pragma omp parallel {
//             /* Obtain and print thread id */
//             int thread_ID = omp_get_thread_num();
//             // printf("Hello World from thread = %d\n", tid);
//             int start_line = start_block+tid;
//             //fill up the heap
//             for(j=start_line; j<start_line+tid j<n && start_line<m; j++){
//                 insert_heap(max_heaps[thread_ID], A[curr_locations[j]][start_line]);
//             }
//             //finish traversing the column
//             for(j=start_line+tid; j<n && start_line<m; j++){
//                 insert_heap(max_heaps[thread_ID], A[curr_locations[j]][start_line]);
//                 delete_max_heap(max_heaps[thread_ID]);
//             }
//             /* All threads join master thread and terminate */
//         }

//         #pragma omp parallel {
//             int thread_ID = omp_get_thread_num();
//             free(max_heaps[thread_ID]);
//         }
//     }
}


int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        printf("Not enough input arguments.\n");
        return -1;
    }
    struct timespec start, finish;
    int rc, ntime, stime;
    int m, n, n_threads;
    n = atoi(argv[1]); //read from command line
    m = atoi(argv[2]); //read from command line
    n_threads = atoi(argv[3]); //read from command line

    void *status;
    double *A[n];

    /** Intialize matrixes **/

    srand48(2); //last digit of my netid is 2

    for (int i = 0; i < n; i++)
    {
        A[i] = (double *)malloc(m * sizeof(double));
        for (int j = 0; j < m; j++)
        {
            A[i][j] = drand48();
        }
    }

    clock_gettime(CLOCK_REALTIME, &start);

    single_thread_sort(A, m, n);
    clock_gettime(CLOCK_REALTIME, &finish);
    ntime = finish.tv_nsec - start.tv_nsec;
    stime = (int)(finish.tv_sec - start.tv_sec);
    printf("main(): Created %d threads. Time %d, nsec %ld\n", n_threads, stime, ntime);

    /** Free matrixes **/
    for (int i = 0; i < n; i++)
    {
        free(A[i]);
        free(B[i]);
        free(C[i]);
    }

    pthread_exit(NULL);
}
