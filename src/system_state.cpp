#include "system_state.hpp"
#include <assert.h>
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
    right.alloc(constraintCount);
    left.alloc(entityCount * 2);
    wq.alloc(entityCount * 2);
    gradients.alloc(constraintCapacity);
    timeGradients.alloc(constraintCapacity);

    for (int i = 0; i < forces.length; i += 2) {
        // gravity
        forces.values[i] = 0;
        forces.values[i + 1] = -1;

        pos.values[i] = 0;
        pos.values[i + 1] = 1;
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
};
