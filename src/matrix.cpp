#include "matrix.hpp"
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

    int d = 0;
    if (flags & MATRIX_OP_HALF_INPUT) {
        d = 1;
    }

    for (int i = 0; i < chunkCount; ++i) {
        MatrixChunk chunk = chunks[i];
        dest.values[chunk.x] += c * chunk.a * v.values[chunk.y >> d];
        dest.values[chunk.x + 1] += c * chunk.b * v.values[chunk.y >> d];
    }
}


int SparseMatrix::createChunk(int x, int y) {
    assert(chunkCount < chunkCapacity);
    chunks[chunkCount].x = x;
    chunks[chunkCount].y = y;
    return chunkCount++;
}

void SparseMatrix::transposeCollapse(Vector dest, unsigned char flags) {
    if (flags & MATRIX_OP_ZERO) {
        for (int i = 0; i < dest.length; ++i) {
            dest.values[i] = 0;
        }
    }

    for (int i = 0; i < chunkCount; ++i) {
        MatrixChunk chunk = chunks[i];
        dest.values[chunk.x] += chunk.a;
        dest.values[chunk.x + 1] += chunk.b;
    }
}

void SparseMatrix::toCoefficientMatrix(Vector v, int width, int height, float* dest) {
    for (int i = 0; i < width * height; ++i) {
        *dest = 0;
    }

    for (int i = 0; i < chunkCount; ++i) {
        MatrixChunk chunk = chunks[i];
        float value = chunk.a * v.values[chunk.x] + chunk.b * v.values[chunk.x + 1];

        int targetX = chunk.x >> 1;
        int targetY = chunk.y;

        dest[targetX + targetY * width] = value;
    }
}

void Vector::alloc(int length) {
    this->length = length;
    this->values = (float *) malloc(length * sizeof(float));
}
