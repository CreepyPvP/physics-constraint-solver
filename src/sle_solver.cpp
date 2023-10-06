#include <sle_solver.hpp>
#include <stdio.h>

void sleSolve(float *input, int width, int height, float* results, float *dest) {
    dest[0] = results[0] / input[0];
    dest[1] = results[1] / input[1];
}

