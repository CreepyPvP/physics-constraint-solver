#include <sle_solver.hpp>
#include <stdio.h>

void sleSolve(float *input, int width, int height, float* results, float *dest) {
    *dest = *results / *input;
}

static int forwardElim(float* mat, int width, int height);
 
static void backSub(float* mat, int width, int height);
 
void gaussianElimination(float* mat, int width, int height) {
    int singular_flag = forwardElim(mat, width, height);
 
    if (singular_flag != -1) {
        printf("Singular Matrix.\n");
        return;
    }
 
    backSub(mat, width, height);
}
 
void swap_row(float* mat, int i, int j, int width, int height, float *results) {
    for (int k=0; k <= width; ++k) {
        float tmp = mat[k + i * width];
        mat[k + i * width] = mat[k + j * width];
        mat[k + j * width] = tmp;
    }

    float tmp = results[i];
    results[i] = results[j];
    results[j] = tmp;
}
 
int forwardElim(float* mat, int width, int height) {
    for (int k = 0; k < width; ++k) {
        // Initialize maximum value and index for pivot
        int i_max = k;
        int v_max = mat[k + i_max * width];
 
        /* find greater amplitude for pivot if any */
        for (int i = k+1; i < width; ++i)
            if (abs(mat[i][k]) > v_max)
                v_max = mat[i][k], i_max = i;
 
        /* if a principal diagonal element  is zero,
         * it denotes that matrix is singular, and
         * will lead to a division-by-zero later. */
        if (!mat[k][i_max])
            return k; // Matrix is singular
 
        /* Swap the greatest value row with current row */
        if (i_max != k)
            swap_row(mat, k, i_max);
 
 
        for (int i=k+1; i<N; i++)
        {
            /* factor f to set current row kth element to 0,
             * and subsequently remaining kth column to 0 */
            double f = mat[i][k]/mat[k][k];
 
            /* subtract fth multiple of corresponding kth
               row element*/
            for (int j=k+1; j<=N; j++)
                mat[i][j] -= mat[k][j]*f;
 
            /* filling lower triangular matrix with zeros*/
            mat[i][k] = 0;
        }
 
        //print(mat);        //for matrix state
    }
    //print(mat);            //for matrix state
    return -1;
}
 
// function to calculate the values of the unknowns
void backSub(double mat[N][N+1])
{
    double x[N];  // An array to store solution
 
    /* Start calculating from last equation up to the
       first */
    for (int i = N-1; i >= 0; i--)
    {
        /* start with the RHS of the equation */
        x[i] = mat[i][N];
 
        /* Initialize j to i+1 since matrix is upper
           triangular*/
        for (int j=i+1; j<N; j++)
        {
            /* subtract all the lhs values
             * except the coefficient of the variable
             * whose value is being calculated */
            x[i] -= mat[i][j]*x[j];
        }
 
        /* divide the RHS by the coefficient of the
           unknown being calculated */
        x[i] = x[i]/mat[i][i];
    }
 
    printf("\nSolution for the system:\n");
    for (int i=0; i<N; i++)
        printf("%lf\n", x[i]);
}
