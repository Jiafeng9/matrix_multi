#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "mmm.h"

/**
 * Allocate and initialize the matrices on the heap. Populate
 * the input matrices with random numbers from 0 to 99
 */
void mmm_init()
{

    // TODO
    A = (double **)malloc(size * sizeof(double *));
    B = (double **)malloc(size * sizeof(double *));
    // allocate the rest of the matrices
    SEQ_MATRIX = (double **)malloc(size * sizeof(double *));
    PAR_MATRIX = (double **)malloc(size * sizeof(double *));

    // 2d-dimensional
    for (int i = 0; i < size; i++)
    {
        A[i] = (double *)malloc(size * sizeof(double));
        B[i] = (double *)malloc(size * sizeof(double));
        SEQ_MATRIX[i] = (double *)malloc(size * sizeof(double));
        PAR_MATRIX[i] = (double *)malloc(size * sizeof(double));
    }

    // TODO
    srand((unsigned)time(NULL)); // seed the random number generator
                                 // initialize A and B with random values between 0 and 99
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            A[i][j] = rand() % 100;
            B[i][j] = rand() % 100;
        }
    }
    // initialize SEQ_MATRIX and PAR_MATRIX with 0s
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            SEQ_MATRIX[i][j] = 0;
            PAR_MATRIX[i][j] = 0;
        }
    }
}

/**
 * Reset a given matrix to zeroes (their size is in the global var)
 * @param matrix pointer to a 2D array
 */
void mmm_reset(double **matrix)
{
    // TODO
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            matrix[i][j] = 0;
        }
    }
}

/**
 * Free up memory allocated to all matrices
 * (their size is in the global var)
 */
void mmm_freeup()
{
    // TODO
    for (int i = 0; i < size; i++)
    {
        free(A[i]);
        free(B[i]);
        free(SEQ_MATRIX[i]);
        free(PAR_MATRIX[i]);
    }
    free(A);
    free(B);
    free(SEQ_MATRIX);
    free(PAR_MATRIX);
}

/**
 * Sequential MMM (size is in the global var)
 */
void mmm_seq()
{
    // TODO - code to perform sequential MMM
    for (int i = 0; i < size; i++)
    { // row
        for (int j = 0; j < size; j++)
        { // column
            for (int k = 0; k < size; k++)
            { // dot product
                SEQ_MATRIX[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

/**
 * Parallel MMM
 */

void *mmm_par(void *args)
{
    // TODO - code to perform parallel MMM
    thread_args *params = (thread_args *)args;
    // sum for "chunk" of numbers (begin + ... + end)
    for (int i = params->begin; i < params->end; i++)
    { // row
        for (int j = 0; j < size; j++)
        { // column
            for (int k = 0; k < size; k++)
            { // dot product
                PAR_MATRIX[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    return NULL;
}

/**
 * Verifies the correctness between the matrices generated by
 * the sequential run and the parallel run.
 *
 * @return the largest error between two corresponding elements
 * in the result matrices
 */
double mmm_verify()
{
    double difference = 0;

    for (int i = 0; i < size; i++)
    { // row
        for (int j = 0; j < size; j++)
        { // column
            if (SEQ_MATRIX[i][j] - PAR_MATRIX[i][j] > difference)
            {
                difference = fabs(SEQ_MATRIX[i][j] - PAR_MATRIX[i][j]);
            }
        }
    }
    // TODO
    // You may find the math.h function fabs() to be useful here (absolute value)
    return difference;
}
