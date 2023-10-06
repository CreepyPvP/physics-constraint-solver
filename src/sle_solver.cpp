#include <cassert>
#include <sle_solver.hpp>
#include <stdio.h>

static float abs(float a) {
    if (a > 0) {
        return a;
    }
    return -a;
}

// swap row j with row k
static void swapRow(float* input, float* results, int n, int j, int k) {
    for (int i = 0; i < n; ++i) {
        float tmp = input[i + j * n];
        input[i + j * n] = input[i + k * n];
        input[i + k * n] = tmp;

    }
    float tmp = results[j];
    results[j] = results[k];
    results[k] = tmp;
}

static int forwardElim(float* input, float* results, int n) {
    for (int k = 0; k < n; ++k) {
        int iMax = k;
        float vMax = input[k + n * k];

        for (int i = k + 1; i < n; ++i) {
            float value = input[k + i * n];
            if (abs(value) > abs(vMax)) {
                vMax = value;
                iMax = i;
            }
        }

        if (!vMax) {
            // this is not solvable
            return k;
        }

        if (iMax != k) {
            swapRow(input, results, n, iMax, k);
        }

        for (int i = k + 1; i < n; ++i) {
            float f = input[k + i * n] / vMax;

            input[k + i * n] = 0;
            for (int j = k + 1; j < n; ++j) {
                input[j + i * n] -= f * input[j + k * n];
            }
            results[i] -= f * results[k];
        }
    }

    return -1;
}

static void backsub(float* input, float* results, float* dest, int n) {
    for (int i = n - 1; i >= 0; --i) {
        float value = results[i] / input[i + i * n];
        dest[i] = value;

        for (int j = i - 1; j >= 0; --j) {
            results[j] -= value * input[i + j * n];
        }
    }
}

void sleSolve(float *input, int n, float* results, float *dest) {
    int flag = forwardElim(input, results, n);
    assert(flag == -1);
    backsub(input, results, dest, n);
}

