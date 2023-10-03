#include "system_state.hpp"
#include <assert.h>
#include <cstdio>
#include <stdlib.h>

void System::init() {
    int entityCount = 1;
    constraintCapacity = 1;
    constraints = (Constraint*) malloc(sizeof(Constraint) * constraintCapacity);

    weights.alloc(entityCount * 2);
    pos.alloc(entityCount * 2);
    vel.alloc(entityCount * 2);
    acc.alloc(entityCount * 2);
    forces.alloc(entityCount * 2);
    correctionForces.alloc(entityCount * 2);
    lambda.alloc(entityCount * 2);
    right.alloc(constraintCapacity);
    left.alloc(entityCount * 2);
    wq.alloc(entityCount * 2);
    gradients.alloc(constraintCapacity);
    timeGradients.alloc(constraintCapacity);

    for (int i = 0; i < forces.length; i += 2) {
        // gravity
        forces.values[i] = 0;
        forces.values[i + 1] = -1;

        pos.values[i] = 1;
        pos.values[i + 1] = 0;
        vel.values[i] = 0;
        vel.values[i + 1] = 0;
        acc.values[i] = 0;
        acc.values[i + 1] = 0;
        weights.values[i] = 1;
        weights.values[i + 1] = 1;
    }

    Constraint test;
    test.type = Constraint::UnitCircle;
    test.value.unitCircle.particle = 0;
    // 0, 0 => particle 0, constraint 0
    test.value.unitCircle.jIndex = gradients.createChunk(0, 0);
    test.value.unitCircle.jtIndex = timeGradients.createChunk(0, 0);
    constraints[0] = test;
    constraintCount = 1;
}

void System::tick(float delta) {
    for (int i = 0; i < constraintCount; ++i) {
        Constraint constraint = constraints[i];

        if (constraint.type == Constraint::UnitCircle) {
            int jChunkIndex = constraint.value.unitCircle.jIndex;
            int jtChunkIndex = constraint.value.unitCircle.jtIndex;
            int particle = constraint.value.unitCircle.particle;

            MatrixChunk jChunk = gradients.chunks[jChunkIndex];
            jChunk.a = pos.values[particle];
            jChunk.b = pos.values[particle + 1];
            gradients.chunks[jChunkIndex] = jChunk;

            MatrixChunk jtChunk = timeGradients.chunks[jtChunkIndex];
            jtChunk.a = vel.values[particle];
            jtChunk.b = vel.values[particle + 1];
            timeGradients.chunks[jtChunkIndex] = jtChunk;
        }
    }

    for (int i = 0; i < wq.length; ++i) {
        wq.values[i] = forces.values[i] / weights.values[i];
    }

    // right hand side
    gradients.mul(wq, right, MATRIX_OP_ZERO | MATRIX_OP_NEGATIVE);
    timeGradients.mul(vel, right, MATRIX_OP_NEGATIVE);

    gradients.transposeCollapse(left, MATRIX_OP_ZERO);
    for (int i = 0; i < left.length; ++i) {
        left.values[i] /= weights.values[i];
    }

    // this is only for debugging purposes.
    // construct coefficient matrix instead, then use sle solver
    float value;
    Vector res;
    res.length = 1;
    res.values = &value;
    gradients.mul(left, res, MATRIX_OP_ZERO);
    float lambdaVal = right.values[0] / value;
    lambda.values[0] = lambdaVal;
    lambda.values[1] = lambdaVal;
    
    gradients.mulTranspose(lambda, correctionForces, MATRIX_OP_ZERO);

    for (int i = 0; i < pos.length; i += 2) {
        acc.values[i] = 
            (forces.values[i] + correctionForces.values[i]) / weights.values[i];
        acc.values[i + 1] = 
            (forces.values[i + 1] + correctionForces.values[i + 1]) / weights.values[i];

        vel.values[i] += delta * acc.values[i];
        vel.values[i + 1] += delta * acc.values[i + 1];
        pos.values[i] += delta * vel.values[i];
        pos.values[i + 1] += delta * vel.values[i + 1];
    }
};
