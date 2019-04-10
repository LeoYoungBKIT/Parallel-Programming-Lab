#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <malloc.h>
#include <time.h>

void slave_code();

int main(int argc, char **argv) {
    int rank, size;
    int* sample_per_slave;

    srand(time(NULL));

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int N = 10000000;
    if (argc > 1) N = atoi(argv[1]);

    sample_per_slave = (int *) malloc(size * sizeof(int));
    int n = N / (size - 1);
    int i;
    sample_per_slave[0] = 0;
    for (i=1; i<size-1; i++) {
        sample_per_slave[i] = n;
    }
    sample_per_slave[size - 1] = N - n * (size - 2);

    if (rank == 0) {
        int i;
        MPI_Status status;
        double t1, t2;

        t1 = MPI_Wtime();

        for (i=1; i<size; i++) {
            MPI_Send(&sample_per_slave[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        int count = 0;
        int recv_msg;
        for (i=1; i<size; i++) {
            MPI_Recv(&recv_msg, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
            count += recv_msg;
        }

        t2 = MPI_Wtime();

        float pi = count * 4.0 / N;
        printf("Time: %f\n", t2-t1);
        printf("So diem trong vong tron la: %d\n", count);
        printf("Tong so diem: %d\n", N);
        printf("Pi = %f\n", pi);
      }
    else
    {
        slave_code();
    }

    MPI_Finalize();
    return 0;
}

void slave_code()
{
    int recv_msg;
    MPI_Status status;
    MPI_Recv(&recv_msg, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

    int i,count = 0;
    for (i=0; i<recv_msg; i++) {
        double x= ((double)rand())/RAND_MAX;
        double y =((double)rand())/RAND_MAX;
        double z = x*x+y*y;
        if (z<=1) count++;
    }

    MPI_Send(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
}