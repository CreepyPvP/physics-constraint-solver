#include "system_state.hpp"
#include "sle_solver.hpp"
#include <assert.h>
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>

// #define DEBUG

void System::init() {
    int entityCount = 2;
    constraintCapacity = 3;
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
    gradients.alloc(constraintCapacity * 2);
    timeGradients.alloc(constraintCapacity * 2);

    lambda.alloc(constraintCapacity);
    sleSolverBuffer = (double*) malloc(sizeof(double) * entityCount * 2 * constraintCapacity);
    sleBuffer1 = (double*) malloc(sizeof(double) * entityCount * 2);
    sleBuffer2 = (double*) malloc(sizeof(double) * entityCount * 2);

    for (int i = 0; i < forces.length; i += 2) {
        // gravity
        forces.values[i] = 0;
        forces.values[i + 1] = -1;

        if (i == 0) {
            pos.values[i] = 1;
        } else {
            pos.values[i] = 2;
        }
        pos.values[i + 1] = 0;
        vel.values[i] = 0;
        vel.values[i + 1] = 0;
        acc.values[i] = 0;
        acc.values[i + 1] = 0;
        weights.values[i] = 1;
        weights.values[i + 1] = 1;
    }

    Constraint c0;
    c0.type = Constraint::UnitCircle;
    c0.value.unitCircle.particle = 0;
    // 0, 0 => particle 0, constraint 0
    c0.value.unitCircle.jIndex = gradients.createChunk(0, 0);
    c0.value.unitCircle.jtIndex = timeGradients.createChunk(0, 0);
    constraints[0] = c0;

    Constraint c1;
    c1.type = Constraint::Anchor;
    c1.value.anchor.particle = 2;
    c1.value.anchor.anchor = 0;
    c1.value.anchor.jIndex = gradients.createChunk(2, 1);
    c1.value.anchor.jtIndex = timeGradients.createChunk(2, 1);
    c1.value.anchor.ajIndex = gradients.createChunk(0, 1);
    c1.value.anchor.ajtIndex = timeGradients.createChunk(0, 1);
    constraints[1] = c1;

    constraintCount = 2;
}

void System::tick(double delta) {
    for (int i = 0; i < constraintCount; ++i) {
        Constraint constraint = constraints[i];

        if (constraint.type == Constraint::UnitCircle) {
            int jChunkIndex = constraint.value.unitCircle.jIndex;
            int jtChunkIndex = constraint.value.unitCircle.jtIndex;
            int particle = constraint.value.unitCircle.particle;

            double x = pos.values[particle];
            double y = pos.values[particle + 1];

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
        } else if (constraint.type == Constraint::Anchor) {
            int jChunkIndex = constraint.value.anchor.jIndex;
            int jtChunkIndex = constraint.value.anchor.jtIndex;
            int ajChunkIndex = constraint.value.anchor.ajIndex;
            int ajtChunkIndex = constraint.value.anchor.ajtIndex;
            int particle = constraint.value.anchor.particle;
            int anchorParticle = constraint.value.anchor.anchor;

            double x = pos.values[particle];
            double ax = pos.values[anchorParticle];
            double y = pos.values[particle + 1];
            double ay = pos.values[anchorParticle + 1];

            MatrixChunk jChunk = gradients.chunks[jChunkIndex];
            jChunk.a = x - ax;
            jChunk.b = y - ay;
            gradients.chunks[jChunkIndex] = jChunk;

            MatrixChunk ajChunk = gradients.chunks[ajChunkIndex];
            ajChunk.a = ax - x;
            ajChunk.b = ay - y;
            gradients.chunks[ajChunkIndex] = ajChunk;

            MatrixChunk jtChunk = timeGradients.chunks[jtChunkIndex];
            jtChunk.a = vel.values[particle] - vel.values[anchorParticle];
            jtChunk.b = vel.values[particle + 1] - vel.values[anchorParticle + 1];
            timeGradients.chunks[jtChunkIndex] = jtChunk;

            MatrixChunk ajtChunk = timeGradients.chunks[ajtChunkIndex];
            ajtChunk.a = vel.values[anchorParticle] - vel.values[particle];
            ajtChunk.b = vel.values[anchorParticle + 1] - vel.values[particle + 1];
            timeGradients.chunks[ajtChunkIndex] = ajtChunk;

            totalEnergy.values[i] = 0.5 * ((x - ax) * (x - ax) + (y - ay) * (y - ay) - 1);
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

    int entityCount = 2;
    gradients.toCoefficientMatrix(
        constraintCount, 
        entityCount * 2,
        sleSolverBuffer, 
        sleBuffer1, 
        sleBuffer2
    );
    // correct for mass values here
    sleSolve(sleSolverBuffer, constraintCount, right.values, lambda.values);
    
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
