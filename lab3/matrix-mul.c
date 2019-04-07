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
                result[i * block_size + j] += A[i + (rank / 2) * block_size][k] * B[k][j + (rank % 2) * block_size] + A[i + (rank / 2) * block_size][k + block_size] * B[k + block_size][j + (rank % 2) * block_size];
            }
        }
    }
    
    int *recvBuffer;
    if (rank == 0)
    {
        recvBuffer = (int *)malloc(4 * block_size * block_size * sizeof(int));
    }
    MPI_Gather(result, block_size * block_size, MPI_INT, recvBuffer, block_size * block_size, MPI_INT, 0, MPI_COMM_WORLD);
    
    if (rank == 0)
    {
        int b,row,col;
        for (b = 0; b < 4; b++)
        {
            for (row = 0; row < block_size; row++)
            {
                for (col = 0; col < block_size; col++)
                {
                    C[row + b / 2 * block_size][col + (b % 2) * block_size] = recvBuffer[b * block_size * block_size + row * block_size + col];
                }
            }
        }
    }
}


