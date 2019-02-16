#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    double **A, **B, **C;
    int n, n_threads, b_size;
} mat_mm_t;

typedef struct 
{
    int row, col;
}arg_t;

mat_mm_t matstr;

void print_mat(int n, double *mat[n]);
int getNumBlocks(int n);

void print_mat(int n, double *mat[n]){
    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
            printf("%f ", mat[i][j]);
        }
        printf("\n");
    }
}


void *mat_mm_cyclic(void *arg)
{
    int index = (int) arg , n = matstr.n, n_threads = matstr.n_threads;

    double **A, **B, **C;
    A = matstr.A;
    B = matstr.B;
    C = matstr.C;
    long n_squared = ((long)n) * ((long)n);
    while(index < n_squared)
    {
        int r = ((int)index/n), c = ((int)index%n);
        double sum = 0;
        for(int k=0; k<n; k++){
            sum += A[r][k] * B[k][c];
            // printf("%f->", sum);
        }
        C[r][c] = sum;
        // printf("\n[%d,%d] = %f \n", r,c,sum);
        index += n_threads;
    }
    pthread_exit(NULL);
}

void *mat_mm_tile(void *arg)
{
    int index = (int) arg , n = matstr.n, 
        n_threads = matstr.n_threads, b_size = matstr.b_size;

    double **A, **B, **C;
    A = matstr.A;
    B = matstr.B;
    C = matstr.C;
    int p = n/b_size; // number of blocks per line
    long p_squared = ((long)p) * ((long)p);
    
    while(index < p_squared)
    {
        int u = ((int)index/p), v = ((int)index%p);
        int x = u*b_size, y = v*b_size;
        for(int w=0; w<p; w++){
            int z = w*b_size;
            for(int i=0; i<b_size; i++){
                for(int j=0; j<b_size; j++){
                    double sum = 0;
                    for(int k=0; k<b_size; k++){              
                        sum+= A[x +i][z+k]*B[z+k][y+j];
                    }
                    C[x +i][y+j] += sum;
                }
            }
        }
        // printf("\n[%d,%d] = %f \n", r,c,sum);
        index += n_threads;
    }
    pthread_exit(NULL);
}

/**  Returns a block size in the range (4-64) that is divider of the matrix size,
 * if it doesn't exist, it returns -1 */
int getNumBlocks(int n){
    int start = 32; //optimal block size for my Mac
    for(int i=0; i<=start; i++){
        if((start-i) >3  && n%(start-i) == 0) return (start-i);
        if(n%(start+i) == 0) return (start+i);
    }
    return -1;
}


int main(int argc, char *argv[])
{
    if(argc < 3){
        printf("Not enough input arguments.\n");
        return -1;
    }
    struct timespec start, finish;
    int rc, ntime, stime;
    int n, n_threads;
    n= atoi(argv[1]); //read from command line
    int b_size = getNumBlocks(n);
    n_threads = atoi(argv[2]); //read from command line
    void *status;
    pthread_attr_t attr;
    pthread_t thr_id[n_threads];
    double *A[n], *B[n], *C[n];

    /** Intialize matrixes **/

    srand48(1); //set seed of random number generator

    for(int i=0; i<n; i++){
        A[i] = (double *) malloc(n * sizeof(double));
        B[i] = (double *) malloc(n * sizeof(double));
        C[i] = (double *) malloc(n * sizeof(double));
        for(int j=0; j<n; j++){
            A[i][j] = drand48();
            B[i][j] = drand48();
        }
    }

    matstr.n = n;
    matstr.n_threads = n_threads;
    matstr.A = A;
    matstr.B = B;
    matstr.C = C;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    clock_gettime(CLOCK_REALTIME, &start);
    int i;

    /** Do cyclic matrix multiply or tiled **/
    if(b_size == -1){
        for (i = 0; i < n_threads; i++)
            {
                rc = pthread_create(&thr_id[i], &attr, mat_mm_cyclic, (void*) i);
            }
    }else{
        matstr.b_size = b_size;
        for (i = 0; i < n_threads; i++)
            {
                rc = pthread_create(&thr_id[i], &attr, mat_mm_tile, (void*) i);
            }
    }
  
    pthread_attr_destroy(&attr);
    for (i = 0; i < n_threads; i++)
    {
        pthread_join(thr_id[i], &status);
    }
    clock_gettime(CLOCK_REALTIME, &finish);
    ntime = finish.tv_nsec - start.tv_nsec;
    stime = (int)(finish.tv_sec - start.tv_sec);
    printf("main(): Created %d threads. Time %d, nsec %ld\n", n_threads, stime, ntime);

    /** Free matrixes **/
    for(int i=0; i<n; i++){
        free(A[i]);
        free(B[i]);
        free(C[i]);
    }

    pthread_exit(NULL);
}
