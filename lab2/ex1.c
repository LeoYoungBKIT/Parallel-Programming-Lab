#include <stdio.h>
#include <mpi.h>

int check_prime(int n) {
    int i;
    if (n < 2) 
        return 0;

    if ((n==2) || (n==3))
        return 1;
    
    if ((n % 2 == 0) || (n % 3 == 0))
        return 0;
    
    for (i=5; i*i < n; i+=6) 
        if ((n % i == 0) || (n % (i+2) == 0))
            return 0;
    return 1;
}

int main(int argc, char **argv) {
    int numtasks, rank, size, dest, source;
    int tag = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    int N = 1000000000;

    if (argc > 1)
        N = atoi(argv[1]);

    if (rank == 0) {
        int tid = 0;
        int num;
        int out_msg;
        for (num=2; num<N; num++) {
            out_msg = num;
            tid += 1;
            if (tid == size)
                tid = 1;
            MPI_Send(&out_msg, 1, MPI_INT, tid, tag, MPI_COMM_WORLD);
        }
        out_msg = 0;
        for (tid=1; tid<size; tid++)
            MPI_Send(&out_msg, 1, MPI_INT, tid, tag, MPI_COMM_WORLD);
    }

    if (rank != 0) {
        while (1) {
            int in_msg;
            MPI_Status status;
            MPI_Recv(&in_msg, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
            if (in_msg == 0)
                break;

            if (check_prime(in_msg))
                printf("%d ", in_msg);
        }
    }

    MPI_Finalize();
    return 0;
}