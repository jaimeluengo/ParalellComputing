/** compile with the following command:
 * gcc -std=gnu99 -o sort_block -fopenmp jl3752_hw2_openmp_sort_block.c -O3
 * Run with following command:
 * ./sort_block <rows> <columns> <number of threads>
 * Example:
 * ./sort_block 2000 2000 8
 * */

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

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

void swap_cols(int col_i, int col_j, int n_rows, int *mat[n_rows]){
    for(int i=0; i<n_rows; i++){
        int temp = mat[i][col_i];
        mat[i][col_i] = mat[i][col_j];
        mat[i][col_j] = temp;
    }
}


void swap_cols_open_mp(int n_threads, int col_i, int col_j, int n_rows, int *mat[n_rows]){
    #pragma omp parallel num_threads(n_threads)
    {
		#pragma omp for schedule(static,1)
		for(int i=0; i<n_rows; i++){
			int temp = mat[i][col_i];
			mat[i][col_i] = mat[i][col_j];
			mat[i][col_j] = temp;
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

void single_thread_sort_block(int m, int n, int *mat[n]){
    int maxRow = n<=m ? n : m;
    for(int i=0; i<maxRow; i++){
        int colmax = i, rowmax = i, max = mat[i][i];
        for(int k = i; k<m; k++){
            for(int j=i; j<n; j++){
                if(max < mat[j][k]){
                    max = mat[j][k];
                    colmax = k;
                    rowmax = j;
                }
            }
        }
       if(i != rowmax) swap_rows(rowmax, i, m, n, mat); 
       if(i != colmax) swap_cols(colmax, i, n, mat); 
    }
}



void omp_sort_block(int n_threads, int m, int n, int *mat[n]){
    int maxRow = n<=m ? n : m;
    double start_time = omp_get_wtime();
    int colmax_total, rowmax_total, max_total;
    #pragma omp parallel shared(colmax_total, rowmax_total, max_total) num_threads(n_threads)
    {
		for(int i=0; i<maxRow; i++){
				#pragma omp single 
				{
					colmax_total = i, rowmax_total = i, max_total = mat[i][i];
				}
				int colmax_local = i, rowmax_local = i, max_local = -1;
				#pragma omp for schedule(static) collapse(2) nowait
				for(int k = i; k<m; k++){
					for(int j=i; j<n; j++){
						if(max_local < mat[j][k]){
							max_local = mat[j][k];
							colmax_local = k;
							rowmax_local = j;
						}
					}
				}
				
				#pragma omp critical
				if(max_total < max_local){
					max_total = max_local;
					colmax_total = colmax_local;
					rowmax_total = rowmax_local;
				}
			
			#pragma omp barrier

		   	#pragma omp for schedule(static)
			for(int l=0; l<n; l++){
				int temp = mat[l][i];
				mat[l][i] = mat[l][colmax_total];
				mat[l][colmax_total] = temp;
			}
			#pragma omp for schedule(static)
			for(int l=0; l<m; l++){
				int temp = mat[i][l];
				mat[i][l] = mat[rowmax_total][l];
				mat[rowmax_total][l] = temp;
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
    printf("They are the same!\n");
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        printf("Not enough input arguments.\n");
        return -1;
    }
    struct timespec start_single, start_omp, finish_single, finish_omp;
    int rc, ntime, stime;
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
    //single_thread_sort_block(m, n, A);
	//double time = omp_get_wtime() - start_time;
	//printf("%f\n", time*1000);
    //clock_gettime(CLOCK_REALTIME, &start_omp);
    omp_sort_block(n_threads, m,n,B);
    //clock_gettime(CLOCK_REALTIME, &finish_omp);
    
    //Check if they are the same
    //compare(A,B,m,n);

    
    //print single threaded time
    //ntime = finish_single.tv_nsec - start_single.tv_nsec;
    //stime = (int)(finish_single.tv_sec - start_single.tv_sec);
    //printf("Single threaded: Time %d, nsec %ld\n", stime, ntime);

	//print omp time
    //ntime = finish_omp.tv_nsec - start_omp.tv_nsec;
    //stime = (int)(finish_omp.tv_sec - start_omp.tv_sec);
    //printf("Multi threaded:Time %d, nsec %ld\n", stime, ntime);

    /** Free matrixes **/
    for (int i = 0; i < n; i++)
    {
        free(A[i]);
        free(B[i]);
    }
}
