#include "system_state.hpp"

#include <assert.h>

void System::init() {
    entityCount = 0;
}

Entity System::add(float posX, float posY, float velX, float velY) {
    assert(entityCount < ENTITY_CAPACITY);

    Entity entity = entityCount++;
    p_x[entity] = posX;
    p_y[entity] = posY;
    v_x[entity] = velX;
    v_y[entity] = velY;

    //. gravity
    a_x[entity] = 0;
    a_y[entity] = -9.85;
    return entity;
}

void System::tick(float delta) {
    for (unsigned int i = 0; i < entityCount; ++i) {
        v_x[i] += delta * a_x[i];
        v_y[i] += delta * a_y[i];
    }
    for (unsigned int i = 0; i < entityCount; ++i) {
        p_x[i] += delta * v_x[i];
        p_y[i] += delta * v_y[i];
    }
}
