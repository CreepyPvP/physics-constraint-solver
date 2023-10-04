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
            int jIndex;
            int jtIndex;
        } unitCircle;
    };

    Value value;
};

struct System {
    // J
    SparseMatrix gradients;
    // J wrt t
    SparseMatrix timeGradients;
    Vector weights;
    Vector pos;
    Vector vel;
    Vector acc;
    Vector forces;

    // tmp vectors
    Vector wq;
    Vector right;
    Vector left;
    Vector correctionForces;
    Vector lambda;
    // C
    Vector totalEnergy;

    Constraint* constraints;
    int constraintCount;
    int constraintCapacity;

    void init();
    void tick(float delta);
};
