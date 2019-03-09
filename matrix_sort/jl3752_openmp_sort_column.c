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


void openmp_sort_colum_crap(int n_threads, int m, int n, int *mat[n]){
	struct timespec start_omp, finish_omp;
    int maxRow = n<=m ? n : m;
    //uint64_t diff;
    int i; 
    #pragma omp parallel private(i) num_threads(n_threads)
    {
		for(i=0; i<maxRow; i++){
			int argmax_all = i, max_total = mat[i][i];
			//clock_gettime(CLOCK_PROCESS_CPUTIME_ID,  &start_omp);
			//{
			//clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &finish_omp);
			//diff = (BILLION * (finish_omp.tv_sec - start_omp.tv_sec) + finish_omp.tv_nsec - start_omp.tv_nsec);
			//printf("creating thread manager %llu\n", (long long unsigned int) diff);
				int argmax_local = i, max_local = -1;
				#pragma omp for schedule(static) nowait
				for(int j=i+1;j<n; j++){
					if(max_local < mat[j][i]){
						max_local = mat[j][i];
						argmax_local = j;
					}
				}
				#pragma omp critical
				if(max_total < max_local){
					max_total = max_local;
					argmax_all = argmax_local;
				}
				
			//#pragma omp barrier
			//Just swap if the max row is not the current one
			if(i!= argmax_all) {
				#pragma omp for schedule(static)
				for(int l=0; l<m; l++){
					int temp = mat[i][l];
					mat[i][l] = mat[argmax_all][l];
					mat[argmax_all][l] = temp;
				}
			}
		}
	}
}



void openmp_sort_colum(int n_threads, int m, int n, int *mat[n]){
    int maxRow = n<=m ? n : m;
    for(int i=0; i<maxRow; i++){
        int argmax_all = i, max_total = mat[i][i];

        #pragma omp parallel shared(argmax_all, max_total) num_threads(n_threads)
        {
			int argmax_local = i, max_local = -1;
            #pragma omp for schedule(static,1) nowait
            for(int j=i+1;j<n; j++){
                if(max_local < mat[j][i]){
                    max_local = mat[j][i];
                    argmax_local = j;
                }
            }
            #pragma omp critical
            if(max_total < max_local){
				max_total = max_local;
				argmax_all = argmax_local;
			}
        }
        //Just swap if the max row is not the current one
        if(i!= argmax_all) swap_rows_open_mp(n_threads, argmax_all, i, m, n, mat); 
     }
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

    //clock_gettime(CLOCK_REALTIME, &start_single);
    //single_thread_sort_column(m, n, A);
    //clock_gettime(CLOCK_REALTIME, &finish_single);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID,  &start_omp);
    openmp_sort_colum(n_threads, m,n,B);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &finish_omp);
    //Check if they are the same
    //compare(A,B,m,n);


     uint64_t diff; 
	diff = (BILLION * (finish_omp.tv_sec - start_omp.tv_sec) + finish_omp.tv_nsec - start_omp.tv_nsec)/MILLION;
	printf("%llu\n", (long long unsigned int) diff);
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
