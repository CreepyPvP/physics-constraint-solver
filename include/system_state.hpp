#pragma once

#include "matrix.hpp"

typedef int Entity;

struct Constraint {
    enum Type {
        UnitCircle,
        Anchor
    };

    Type type;

    union Value {
        struct {
            Entity particle;
            int jIndex;
            int jtIndex;
        } unitCircle;

        struct {
            Entity particle;
            Entity anchor;

            int jIndex;
            int jtIndex;
            int ajIndex;
            int ajtIndex;
        } anchor;
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
    // C
    Vector totalEnergy;

    Vector lambda;
    float* sleSolverBuffer;
    float* sleBuffer1;
    float* sleBuffer2;

    Constraint* constraints;
    int constraintCount;
    int constraintCapacity;

    void init();
    void tick(float delta);
};
