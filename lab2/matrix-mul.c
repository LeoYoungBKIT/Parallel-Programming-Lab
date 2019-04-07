#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 100

int A[N][N];
int B[N][N];
int C[N][N];

void BlockAlgorithm(int rank);

int main(int argc, char **argv)
{
    int i,j;
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            A[i][j] = B[i][j] = j + 1;
            C[i][j] = 0;
        }
    }
    
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (rank == 0)
    {
        double t1 = MPI_Wtime();
        BlockAlgorithm(rank);
        double t2 = MPI_Wtime();
        printf("Hoan thanh nhan 2 ma tran\nTong thoi gian la: %f (s)\n", t2 - t1);
        
        for (i = 0; i < N; i++)
        {
            for (j = 0; j < N; j++)
            {
                if (C[i][j] !=  (N * (N + 1) / 2) * (j + 1))
                {
                    printf("Ket qua sai\n");
                    return -1;
                }
            }
        }
        printf("Ket qua chinh xac\n");
    }
    else
    {
        BlockAlgorithm(rank);
    }
    
    MPI_Finalize();
    
    return 0;
}

void BlockAlgorithm(int rank)
{
    int block_size = N / 2;
    int result[block_size * block_size];
    int i,j;
    for (i = 0; i < block_size; i++)
    {
        for (j = 0; j < block_size; j++)
        {
            result[i * block_size + j] = 0;
            int k;
            for (k = 0; k < block_size; k++)
            {
                if (rank == 0)
                {
                    C[i][j] += A[i][k] * B[k][j] + A[i][k + block_size] * B[k + block_size][j];
                }
                else
                {
                    result[i * block_size + j] += A[i + (rank / 2) * block_size][k] * B[k][j + (rank % 2) * block_size] + A[i + (rank / 2) * block_size][k + block_size] * B[k + block_size][j + (rank % 2) * block_size];
                }
            }
        }
    }
    if (rank == 0)
    {
        int i;
        for (i = 0; i < 3; i++)
        {
            int *buffer = malloc(block_size * block_size * sizeof(int));
            MPI_Status status;
            MPI_Recv(buffer, block_size * block_size, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            
            int source = status.MPI_SOURCE;
            int row, col;
            for (row = 0; row < block_size; row++)
            {
                for (col = 0; col < block_size; col++)
                {
                    C[row + (source / 2) * block_size][col + (source % 2) * block_size] = buffer[row * block_size + col];
                }
            }
        }
    }
    else
    {
        MPI_Send(&result, block_size * block_size, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
}



