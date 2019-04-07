#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int main(int argc, char** argv)
{
    long niter = 1000000;
    int rank;
    double x,y;
    int i, count=0;
    double z;
    double pi;
    int size;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int recieved[size];
    long recvniter[size];
    srand(123456);
    
    if(rank != 0)
    {
        for (i=0; i<niter; ++i)
        {
            x= ((double)rand())/RAND_MAX;
            y =((double)rand())/RAND_MAX;
            z = x*x+y*y;
            if (z<=1)
            {
                count++;
            }
        }
        for(i=0; i<size; ++i)
        {
            MPI_Send(&count, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
            MPI_Send(&niter, 1, MPI_LONG, 0, 2, MPI_COMM_WORLD);
        }
    }
    else if (rank == 0)
    {
        for(i=0; i<size; ++i)
        {
            MPI_Recv(&recieved[i], size, MPI_INT,  MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&recvniter[i], size, MPI_LONG, MPI_ANY_SOURCE, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }
    
    if (rank == 0)
    {
        int finalcount = 0;
        long finalniter = 0;
        for(i = 0; i<size; ++i)
        {
            finalcount += recieved[i];
            finalniter += recvniter[i];
        }
        
        pi = ((double)finalcount/(double)finalniter)*4.0;
        printf("So diem trong vong tron la: %d\n", finalcount);
        printf("Pi: %f\n", pi);
        
    }
    
    MPI_Finalize();
    return 0;
}
