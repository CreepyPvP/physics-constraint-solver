#include "system_state.hpp"
#include <assert.h>
#include <stdlib.h>

void System::init() {
    int entityCount = 8;
    constraintCapacity = 8;
    constraints = (Constraint*) malloc(sizeof(Constraint) * constraintCapacity);

    weights.alloc(entityCount * 2);
    pos.alloc(entityCount * 2);
    vel.alloc(entityCount * 2);
    acc.alloc(entityCount * 2);
    forces.alloc(entityCount * 2);
    tmp.alloc(entityCount * 2);
    gradients.alloc(constraintCapacity);

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
    }

    Constraint test;
    test.type = Constraint::UnitCircle;
    test.value.unitCircle.particle = 0;
    // 0, 0 => particle 0, constraint 0
    test.value.unitCircle.chunkIndex = gradients.createChunk(0, 0);
    constraints[0] = test;
    constraintCount = 1;
}

void System::tick(float delta) {
    for (int i = 0; i < constraintCount; ++i) {
        Constraint constraint = constraints[i];

        if (constraint.type == Constraint::UnitCircle) {
            MatrixChunk chunk = 
                gradients.chunks[constraint.value.unitCircle.chunkIndex];
            chunk.a = pos.values[constraint.value.unitCircle.particle];
            chunk.b = pos.values[constraint.value.unitCircle.particle + 1];
        }
    }

    gradients.mul(vel, tmp);
};
