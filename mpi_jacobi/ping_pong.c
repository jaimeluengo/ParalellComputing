#include <mpi.h>
const int MAX_STRING_L = 100;
int main(void) {
    char       greeting[MAX_STRING_L];
    int        no_tasks, my_rank;
    double     elapsed_time;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &n_tasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    if (my_rank != 0) {
        /* create a message */
        
        sprintf(greeting, "Hello from process %d", my_rank);
        elapsed_time = MPI_Wtime();
        MPI_Send(greeting, strlen(greeting)+1, MPI_CHAR, 0, 0,MPI_COMM_WORLD);
        /* send to main   */
    } else{
        MPI_Recv(greeting, MAX_STRING_L, MPI_CHAR, i,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        elapsed_time = MPI_Wtime() - elapsed_time; 
        printf("Latency: %f\n", elapsed_time);
        /* matching receive */
         printf("%s\n", greeting);

    }
    MPI_Finalize();
    return 0;
}