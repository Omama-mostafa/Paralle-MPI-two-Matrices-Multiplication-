#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int main(int argc, char* argv[])
{
    int rank, i, j, k, sum, noPro;
    int tag = 0;
    int *globalRes;
    FILE *fptr;

    MPI_Init(&argc, &argv);
    MPI_Status status;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &noPro);

    int row1, col1, row2, col2;
    int *matrix1, *matrix2;

    int portion, rem, choice;
    if(rank == 0)
    {
        printf("\n1-Get matrices from CONSOLE.");
        printf("\n2-Get matrices from FILE.\n");
        scanf("%d", &choice);

        if(choice == 1)
        {
            printf("\nEnter FIRST Matrix rows : ");
            scanf("%d", &row1);
            printf("\nEnter FIESR Matrix cols : ");
            scanf("%d", &col1);
            printf("\nEnter SECOND Matrix rows : ");
            scanf("%d", &row2);
            printf("\nEnter SECOND Matrix cols : ");
            scanf("%d", &col2);
        }
        else if(choice == 2)
        {
            fptr = fopen("MatrixData.txt", "r");
            fscanf(fptr, "%d", &row1);
            fscanf(fptr, "%d", &col1);
            fscanf(fptr, "%d", &row2);
            fscanf(fptr, "%d", &col2);
        }
        else
        {
            printf("\nWrong Choice!\n\n");
            return;
        }
        if(col1 != row2)
        {
            printf("\nInvalid Multiplication!\n\n");
            return;
        }
        matrix1 = (int*)malloc(row1 * col1 * sizeof(int));
        matrix2 = (int*)malloc(row2 * col2 * sizeof(int));

        if(choice == 1)
        {
            ///take matrix input
            printf("\n");
            for(i=0; i<row1; i++)
            {
                for(j=0; j<col1; j++)
                {
                    printf("Matrix_1[%d][%d] : ", i, j);
                    scanf("%d", (matrix1 + i*col1 + j));
                }
            }
            printf("\n");
            for(i=0; i<row2; i++)
            {
                for(j=0; j<col2; j++)
                {
                    printf("Matrix_2[%d][%d] : ", i, j);
                    scanf("%d", (matrix2 + i*col2 + j));
                }
            }
        }
        else if(choice == 2)
        {
            ///read matrix from file
            for(i=0; i<row1; i++)
            {
                for(j=0; j<col1; j++)
                {
                    fscanf(fptr, "%d", (matrix1 + i * col1 + j));
                }
            }
            for(i=0; i<row2; i++)
            {
                for(j=0; j<col2; j++)
                {
                    fscanf(fptr, "%d", (matrix2 + i * col2 + j));
                }
            }
        }
        portion = row1 / noPro;
        rem = row1 % noPro;
    }

    MPI_Bcast(&portion, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&row1, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&col1, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&row2, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&col2, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&matrix2[0], row2*col2, MPI_INT, 0, MPI_COMM_WORLD);

    int *subMatrix1 = (int*)malloc(portion * col1 * sizeof(int));

    MPI_Scatter(matrix1, portion*col1, MPI_INT, subMatrix1, portion*col1, MPI_INT, 0, MPI_COMM_WORLD);

    int *localRes = malloc(portion * col2 * sizeof(int));
    for(i=0; i<portion; i++)
    {
        for(j=0; j<col2; j++)
        {
            sum = 0;
            for(k=0; k<row2; k++)
            {
                sum += subMatrix1[(i * col1) + k] * matrix2[(k * col2) + j];
                //printf("\nlocal sub1[%d][%d]  = %d ", i, k, subMatrix1[(i * col1) + k]);
                //printf("\nlocal sub2[%d][%d]  = %d ", k, j, matrix2[(k * col2) + j]);
            }
            localRes[(i * col2) + j] = sum;
            //printf("\nLocal[%d][%d] = %d ", i, j, localRes[(i * col2) + j]);
        }
    }

    if(rank == 0)
        globalRes = (int*)malloc(row1 * col2 * sizeof(int));

    MPI_Gather(localRes, portion*col2, MPI_INT, globalRes, portion*col2, MPI_INT, 0, MPI_COMM_WORLD);

    if(rank == 0)
    {
        for(i = (row1 - rem); i<row1; i++)
        {
            for(j=0; j<col2; j++)
            {
                sum = 0;
                for(k=0; k < row2; k++)
                {
                    sum += matrix1[(i * col1) + k] * matrix2[(k * col2) + j];
                }
                globalRes[(i * col2) + j] = sum;
            }
        }
        printf("\n");
        for(i=0; i<row1; i++)
        {
            for(j=0; j<col2; j++)
            {
                printf("Res[%d][%d] = %d \t", i, j, globalRes[i * col2 + j]);
            }
            printf("\n");
        }
    }
    MPI_Finalize();
    fclose(fptr);
    return 0;
}

///print after read matrices

//        for(i=0; i<row1; i++)
//        {
//            for(j=0; j<col1; j++)
//            {
//                printf("\nMatrix1[%d][%d] = %d ", i, j, matrix1[i *col1 + j]);
//            }
//        }
//        printf("\n");
//        for(i=0; i<row2; i++)
//        {
//            for(j=0; j<col2; j++)
//            {
//                printf("\nMatrix2[%d][%d] = %d ", i, j, matrix2[i *col2 + j]);
//            }
//        }



///print after scatter
//    for(i=0; i<portion; i++)
//    {
//        for(j=0; j<col1; j++)
//        {
//            printf("\nsubMatrix1[%d][%d] = %d ", i, j, subMatrix1[i *col1 + j]);
//        }
//    }
//    for(i=0; i<row2; i++)
//    {
//        for(j=0; j<col2; j++)
//        {
//            printf("\nsubMatrix2[%d][%d] = %d ", i, j, matrix2[i *col2 + j]);
//        }
//    }
