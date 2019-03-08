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
     int temp = 0;
     #pragma omp parallel private(temp) num_threads(n_threads)
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
    int temp = 0;
    #pragma omp parallel private(temp) num_threads(n_threads)
    {
		#pragma omp for schedule(static,1)
		for(int i=0; i<n_rows; i++){
			temp = mat[i][col_i];
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
       swap_rows(rowmax, i, m, n, mat); 
       swap_cols(colmax, i, n, mat); 
    }
}



void openmp_sort_colum(int n_threads, int m, int n, int *mat[n]){
 int curr_locations[n]; // holds rows locations of the matrix at each stage in the sorting
    for(int i=0; i< n; i++){
        curr_locations[i]=i;
    }
    int maxRow = n<=m ? n : m;
    for(int i=0; i<maxRow; i++){
        int argmax = i, max = mat[i][i];

        #pragma omp parallel shared(argmax, max) num_threads(n_threads)
        {
            #pragma omp for schedule(static,1)
            for(int j=i+1;j<n; j++){
                #pragma omp critical
                if(max < mat[j][i]){
                    max = mat[j][i];
                    argmax = j;
                }
            }
        }
        swap_rows(argmax, i, m, n, mat); 

     }
}


void omp_sort_block(int n_threads, int m, int n, int *mat[n]){
    int maxRow = n<=m ? n : m;
    for(int i=0; i<maxRow; i++){
        int colmax = i, rowmax = i, max = mat[i][i];
        #pragma omp parallel shared(colmax, rowmax, max, mat) num_threads(n_threads)
        {
			#pragma omp for schedule(static,n_threads) collapse(2)
			for(int k = i; k<m; k++){
				for(int j=i; j<n; j++){
                    #pragma omp critical
					if(max < mat[j][k]){
						max = mat[j][k];
						colmax = k;
						rowmax = j;
					}
				}
			}
		}
       //swap_rows_open_mp(n_threads, rowmax, i, m, n, mat); 
       swap_rows(rowmax, i, m, n, mat); 
       //swap_cols_open_mp(n_threads, colmax, i, n, mat); 
       swap_cols(colmax, i, n, mat); 
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
    printf("They are the same!\n");
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
    //print_mat(n, B);
    //print_mat(n, A);
    clock_gettime(CLOCK_REALTIME, &start);
    single_thread_sort_block(m, n, A);
    printf("\n");
    omp_sort_block(n_threads, m,n,B);
    compare(A,B,m,n);
    clock_gettime(CLOCK_REALTIME, &finish);
    //print_mat(n, B);
    //print_mat(n, A);
    ntime = finish.tv_nsec - start.tv_nsec;
    stime = (int)(finish.tv_sec - start.tv_sec);
    printf("main(): Created %d threads. Time %d, nsec %ld\n", n_threads, stime, ntime);

    /** Free matrixes **/
    for (int i = 0; i < n; i++)
    {
        free(A[i]);
        free(B[i]);
    }
}
