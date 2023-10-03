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

void Vector::alloc(int length) {
    this->length = length;
    this->values = (float *) malloc(length * sizeof(float));
}
