#include "matrix.hpp"
#include <stdlib.h>
#include <assert.h>

void SparseMatrix::alloc(int size) {
    chunkCount = 0;
    chunkCapacity = 0;
    chunks = (MatrixChunk*) malloc(sizeof(MatrixChunk) * size);
}

void SparseMatrix::mul(Vector v, Vector dest) {
    for (int i = 0; i < dest.length; ++i) {
        dest.values[i] = 0;
    }

    for (int i = 0; i < chunkCount; ++i) {
        MatrixChunk chunk = chunks[i];
        dest.values[chunk.y] += chunk.a * v.values[chunk.x];
        dest.values[chunk.y] += chunk.b * v.values[chunk.x + 1];
    }
}


int SparseMatrix::createChunk(int x, int y) {
    assert(chunkCount < chunkCapacity);
    chunks[chunkCount].x = x;
    chunks[chunkCount].y = y;
    return chunkCount++;
}

void Vector::alloc(int length) {
    this->length = length;
    this->values = (float *) malloc(length * sizeof(float));
}
