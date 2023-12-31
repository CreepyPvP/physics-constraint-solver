#include "matrix.hpp"
#include <cstdio>
#include <stdlib.h>
#include <assert.h>

void SparseMatrix::alloc(int size) {
    chunkCount = 0;
    chunkCapacity = size;
    chunks = (MatrixChunk*) malloc(sizeof(MatrixChunk) * size);
}

void SparseMatrix::mul(Vector v, Vector dest, unsigned char flags) {
    if (flags & MATRIX_OP_ZERO) {
        for (int i = 0; i < dest.length; ++i) {
            dest.values[i] = 0;
        }
    }
    int c = 1;
    if (flags & MATRIX_OP_NEGATIVE) {
        c = -1;
    }

    for (int i = 0; i < chunkCount; ++i) {
        MatrixChunk chunk = chunks[i];
        dest.values[chunk.y] += c * chunk.a * v.values[chunk.x];
        dest.values[chunk.y] += c * chunk.b * v.values[chunk.x + 1];
    }
}

void SparseMatrix::mulTranspose(Vector v, Vector dest, unsigned char flags) {
    if (flags & MATRIX_OP_ZERO) {
        for (int i = 0; i < dest.length; ++i) {
            dest.values[i] = 0;
        }
    }
    int c = 1;
    if (flags & MATRIX_OP_NEGATIVE) {
        c = -1;
    }

    for (int i = 0; i < chunkCount; ++i) {
        MatrixChunk chunk = chunks[i];
        dest.values[chunk.x] += c * chunk.a * v.values[chunk.y];
        dest.values[chunk.x + 1] += c * chunk.b * v.values[chunk.y];
    }
}


int SparseMatrix::createChunk(int x, int y) {
    assert(chunkCount < chunkCapacity);
    chunks[chunkCount].x = x;
    chunks[chunkCount].y = y;
    return chunkCount++;
}

// buffer1 and buffer2 have length width
// this is fucking retarded
void SparseMatrix::toCoefficientMatrix(int equationCount, int width, double* dest, double* buffer1, double* buffer2) {
    for (int y = 0; y < equationCount; ++y) {
        for (int i = 0; i < width; i++) {
            buffer1[i] = 0;
        }
        for(int i = 0; i < chunkCount; ++i) {
            MatrixChunk chunk = chunks[i];
            if (chunk.y == y) {
                buffer1[chunk.x] = chunk.a;
                buffer1[chunk.x + 1] = chunk.b;
            }
        }

        for (int x = 0; x < equationCount; ++x) {
            for (int i = 0; i < width; ++i) {
                buffer2[i] = 0;
            }
            for(int i = 0; i < chunkCount; ++i) {
                MatrixChunk chunk = chunks[i];
                if (chunk.y == x) {
                    buffer2[chunk.x] = chunk.a;
                    buffer2[chunk.x + 1] = chunk.b;
                }
            }

            double accumulator = 0;
            for (int i = 0; i < width; ++i) {
                accumulator += buffer1[i] * buffer2[i];
            }
            dest[x + y * equationCount] = accumulator;
        }
    }
}


void Vector::alloc(int length) {
    this->length = length;
    this->values = (double *) malloc(length * sizeof(double));
}
