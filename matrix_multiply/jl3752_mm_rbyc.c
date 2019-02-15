/*******************************************************************
 * 
 * 
 * 
 * ****************************************************************/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <sys/times.h>
#include <sys/types.h>

void print_mat(int n, double *mat[n]);

void print_mat(int n, double *mat[n]){
    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
            printf("%f ", mat[i][j]);
        }
        printf("\n");
    }
}

int main (int argc, char *argv[])
{
    int n, ntime, stime;
    struct timespec start,finish;
    if(argc < 2){
        printf("Not enough input arguments.\n");
        return -1;
    }
    n= atoi(argv[1]); //read from command line


    srand48(1); //set seed of random number generator

    double *A[n], *B[n], *C[n];

    /** Intialize matrixes **/
    for(int i=0; i<n; i++){
        A[i] = (double *) malloc(n * sizeof(double));
        B[i] = (double *) malloc(n * sizeof(double));
        C[i] = (double *) malloc(n * sizeof(double));
        for(int j=0; j<n; j++){
            A[i][j] = drand48();
            B[i][j] = drand48();
        }
    }

    clock_gettime(CLOCK_REALTIME,&start);
    /** Compute matrix multiply **/
    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
            double sum =0;
            for(int k=0; k<n; k++){
                sum += A[i][k] *B[k][j]; 
                // printf("%f->", sum);
                //Try C[i][j] +=
            }
            // printf("\n");
            C[i][j]=sum;
        }
    }

    clock_gettime(CLOCK_REALTIME,&finish);
    ntime = finish.tv_nsec - start.tv_nsec;
    stime = (int) finish.tv_sec - (int) start.tv_sec;
    printf("Time %ld, nsec %ld\n", stime, ntime);

    // print_mat(n, C);

}
