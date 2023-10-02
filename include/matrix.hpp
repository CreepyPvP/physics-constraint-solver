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
    void mul(Vector v, Vector dest);
    int createChunk(int x, int y);
};

