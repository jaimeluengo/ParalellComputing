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
    int n, b, ntime, stime;
    struct timespec start,finish;
    if(argc < 3){
        printf("Not enough input arguments.\n");
        exit(-1);
    }
    // printf("Input positive integer defining size of the square matrix\n");
    // scanf("%d", &n);
    // printf("Input positive integer for the tile size such that the size of the matrix is divisble by it\n");
    // scanf("%d", &b);
    n = atoi(argv[1]);
    b = atoi(argv[2]);
    printf("n: %d, b:%d \n",n,b);
    if(n%b != 0){
        printf("The size of the matrix must be divisible by the block size\n");
        exit(-1);
    }

    int p = n/b; //number of blocks
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
    for(int u=0; u<p; u++){
        int x = u*b, maxi = x+b;
        for(int v=0; v<p; v++){
            int  y = v*b,  maxj = y+b; // reduce repetitive computations within loops
            for(int w=0; w<p; w++){
                int z = w*b;
                for(int i=0; i<b; i++){
                    for(int j=0; j<b; j++){
                        double sum = 0;
                        for(int k=0; k<b; k++){              
                            sum+= A[x +i][z+k]*B[z+k][y+j];
                        }
                        C[x +i][y+j] += sum;
                    }
                }
            }
        }
    }

    clock_gettime(CLOCK_REALTIME,&finish);
    ntime = finish.tv_nsec - start.tv_nsec;
    stime = (int) finish.tv_sec - (int) start.tv_sec;
    printf("Time %ld, nsec %ld\n", stime, ntime);

    for(int i=0; i<n; i++){
        free(A[i]);
        free(B[i]);
        free(C[i]);
    }

}
