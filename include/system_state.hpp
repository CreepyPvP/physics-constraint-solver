#pragma once

#include "matrix.hpp"

typedef int Entity;

struct Constraint {
    enum Type {
        UnitCircle
    };

    Type type;

    union Value {
        struct {
            Entity particle;
            int chunkIndex;
        } unitCircle;
    };

    Value value;
};

struct System {
    SparseMatrix gradients;
    Vector weights;
    Vector pos;
    Vector vel;
    Vector acc;
    Vector forces;
    Vector tmp;

    Constraint* constraints;
    int constraintCount;
    int constraintCapacity;

    void init();
    void tick(float delta);
};
