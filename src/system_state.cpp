#include "system_state.hpp"
#include "sle_solver.hpp"
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
    left.alloc(entityCount * 2);
    wq.alloc(entityCount * 2);
    right.alloc(constraintCapacity);
    totalEnergy.alloc(constraintCapacity);
    gradients.alloc(constraintCapacity);
    timeGradients.alloc(constraintCapacity);

    lambda.alloc(constraintCapacity);
    sleSolverBuffer = (float*) malloc(sizeof(float) * entityCount * constraintCapacity);
    sleBuffer1 = (float*) malloc(sizeof(float) * entityCount);
    sleBuffer2 = (float*) malloc(sizeof(float) * entityCount);

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

            float x = pos.values[particle];
            float y = pos.values[particle + 1];

            MatrixChunk jChunk = gradients.chunks[jChunkIndex];
            jChunk.a = x;
            jChunk.b = y;
            gradients.chunks[jChunkIndex] = jChunk;

            MatrixChunk jtChunk = timeGradients.chunks[jtChunkIndex];
            jtChunk.a = vel.values[particle];
            jtChunk.b = vel.values[particle + 1];
            timeGradients.chunks[jtChunkIndex] = jtChunk;

            // used for correction term and monitoring
            totalEnergy.values[i] = 0.5 * (x * x + y * y - 1);
        }
    }

    for (int i = 0; i < wq.length; ++i) {
        wq.values[i] = forces.values[i] / weights.values[i];
    }

    // right hand side
    gradients.mul(wq, right, MATRIX_OP_ZERO | MATRIX_OP_NEGATIVE);
    timeGradients.mul(vel, right, MATRIX_OP_NEGATIVE);
    gradients.mul(vel, right, MATRIX_OP_NEGATIVE);
    for (int i = 0; i < right.length; ++i) {
        right.values[i] -= totalEnergy.values[i];
    }

    int entityCount = 1;
    gradients.toCoefficientMatrix(
        constraintCount, 
        entityCount * 2,
        sleSolverBuffer, 
        sleBuffer1, 
        sleBuffer2
    );
    // correct for mass values here
    sleSolve(sleSolverBuffer, entityCount * 2, constraintCount, right.values, lambda.values);
    
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
