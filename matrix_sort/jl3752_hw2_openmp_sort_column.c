/** compile with the following command:
 * gcc -std=gnu99 -o sort_colum -fopenmp jl3752_openmp_sort_column.c -O3
 * Run with following command:
 * ./sort_colum   <rows> <columns> <number of threads>
 * Example:
 * ./sort_colum 20000 20000 8
 * */

#include <omp.h>
#include <stdio.h>
#include <stdint.h> /* for uint64 definition */
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#define BILLION 1000000000L
#define MILLION 1000000L
void single_thread_sort_column(int m, int n, int *mat[n]);
void single_thread_sort_block(int m, int n, int *mat[n]);



void swap_rows(int row_i, int row_j, int n_cols, int n, int *mat[n]){
    for(int i=0; i<n_cols; i++){
        int temp = mat[row_i][i];
        mat[row_i][i] = mat[row_j][i];
        mat[row_j][i] = temp;
    }
}

void swap_rows_open_mp(int n_threads, int row_i, int row_j, int n_cols, int n, int *mat[n]){

     #pragma omp parallel num_threads(n_threads)
     {
        #pragma omp for schedule(static,1)
		for(int i=0; i<n_cols; i++){
			int temp = mat[row_i][i];
			mat[row_i][i] = mat[row_j][i];
			mat[row_j][i] = temp;
		}
	}
}


void print_mat(int n, int *mat[n])
{
    printf("\n");
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%d ", mat[i][j]);
        }
        printf("\n");
    }
}


void single_thread_sort_column(int m, int n, int *mat[n]){
    int maxRow = n<=m ? n : m;
    for(int i=0; i<maxRow; i++){
        int argmax = i, max = mat[i][i];
        for(int j=i+1; j<n; j++){
            if(max < mat[j][i]){
                max = mat[j][i];
                argmax = j;
            }
        }
       swap_rows(argmax, i, m, n, mat); 
    }

}

void openmp_sort_colum(int n_threads, int m, int n, int *mat[n]){

    int maxRow = n<=m ? n : m;
    int argmax_all, max_total = -1;
    double start_time = omp_get_wtime();
    #pragma omp parallel shared(argmax_all, max_total) num_threads(n_threads)
    {
		for(int i=0; i<maxRow; i++){
			#pragma omp single 
			{
				argmax_all = i, max_total = mat[i][i];
			}
			int argmax_local = i, max_local = -1;
			
			#pragma omp for schedule(static) nowait
			for(int j=i+1;j<n; j++){
				if(max_local < mat[j][i]){
					max_local = mat[j][i];
					argmax_local = j;
				}
			}
			#pragma omp critical
			{
				if(max_total < max_local){
					max_total = max_local;
					argmax_all = argmax_local;
				}
			}
			
			#pragma omp barrier

			#pragma omp for schedule(static)
			for(int l=0; l<m; l++){
				int temp = mat[i][l];
				mat[i][l] = mat[argmax_all][l];
				mat[argmax_all][l] = temp;
			}

		}
	}
	double time = omp_get_wtime() - start_time;
	//printf("%f\n", time*1000);
}

void compare(int *A[], int *B[], int m, int n){
    for(int i=0; i<m; i++){
        for(int j=0; j<n; j++){ 
            if(A[i][j] != B[i][j]){
                printf("They are different!\n");
                return;
            }
        }
    }
    //printf("They are the same!\n");
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        printf("Not enough input arguments.\n");
        return -1;
    }
    struct timespec start_single, start_omp, finish_single, finish_omp;
    int rc, stime;
    unsigned long ntime;
    int m, n, n_threads;
    n = atoi(argv[1]); //read from command line
    m = atoi(argv[2]); //read from command line
    n_threads = atoi(argv[3]); //read from command line

    void *status;
    int *A[n], *B[n];

    /** Intialize matrixes **/

    srand(2); //last digit of my netid is 2

    for (int i = 0; i < n; i++)
    {
        A[i] = (int *)malloc(m * sizeof(int));
        B[i] = (int *)malloc(m * sizeof(int));
        for (int j = 0; j < m; j++)
        {
            A[i][j] = rand();
            B[i][j] = A[i][j];
        }
    }

    //double start_time = omp_get_wtime();
    //single_thread_sort_column(m, n, A);
	//double time = omp_get_wtime() - start_time;
	//printf("%f\n", time*1000);
    //clock_gettime(CLOCK_MONOTONIC,  &start_omp);
    openmp_sort_colum(n_threads, m,n,B);
    //clock_gettime(CLOCK_MONOTONIC, &finish_omp);
    //Check if they are the same
    //compare(A,B,m,n);


     //uint64_t diff; 
	//diff = (BILLION * (finish_omp.tv_sec - start_omp.tv_sec) + finish_omp.tv_nsec - start_omp.tv_nsec)/MILLION;
	//printf("%llu\n", (long long unsigned int) diff);
    //print single threaded time
    //ntime = finish_single.tv_nsec - start_single.tv_nsec;
    //stime = (int)(finish_single.tv_sec - start_single.tv_sec);
    //printf("Single threaded:%d\n", stime);

	//print omp time
    //ntime = (finish_omp.tv_nsec - start_omp.tv_nsec)/1000000; //ms
    //stime = (int)(finish_omp.tv_sec - start_omp.tv_sec);
    //printf("Multi threaded: %lu  %d\n", ntime, stime);

    /** Free matrixes **/
    for (int i = 0; i < n; i++)
    {
        free(A[i]);
        free(B[i]);
    }
}
