#define MATRIX_OP_ZERO 1 << 0
#define MATRIX_OP_NEGATIVE 1 << 1

// a 2x1 matrix slice
// here example with x = 1, y = 2
//
// 0 0 0 0
// 0 0 0 0
// 0 a b 0
//
struct MatrixChunk {
    int x;
    int y;
    float a;
    float b;
    int owner;
};

struct Vector {
    float* values;
    int length;

    void alloc(int length);
};

struct SparseMatrix {
    MatrixChunk* chunks;
    int chunkCapacity;
    int chunkCount;

    void alloc(int length);
    int createChunk(int x, int y);

    void mul(Vector v, Vector dest, unsigned char flags);
    // multiply the transpose matrix with an identity vector
    void transposeCollapse(Vector dest, unsigned char flags);
};

