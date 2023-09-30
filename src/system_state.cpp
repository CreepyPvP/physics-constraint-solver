#include "system_state.hpp"

#include <assert.h>

void System::init() {
    entityCount = 0;
}

Entity System::add(float posX, float posY, float mass, float velX, float velY) {
    assert(entityCount < ENTITY_CAPACITY);

    Entity entity = entityCount++;
    p_x[entity] = posX;
    p_y[entity] = posY;
    v_x[entity] = velX;
    v_y[entity] = velY;
    a_x[entity] = 0;
    a_y[entity] = 0;
    m[entity] = mass;
    return entity;
}

void System::tick(float delta) {
    for (unsigned int i = 0; i < entityCount; ++i) {
        float gravityX = 0; 
        float gravityY = -0.5; 

        float a = gravityX * p_x[i] + gravityY * p_y[i]; 
        float b = m[i] * (v_x[i] * v_x[i] + v_y[i] * v_y[i]);
        float c = p_x[i] * p_x[i] + p_y[i] * p_y[i];
        float lambda = (-a - b) / c;

        a_x[i] = gravityX + lambda * p_x[i];
        a_y[i] = gravityY + lambda * p_y[i];
    }

    for (unsigned int i = 0; i < entityCount; ++i) {
        v_x[i] += delta * a_x[i] * m[i];
        v_y[i] += delta * a_y[i] * m[i];
    }
    for (unsigned int i = 0; i < entityCount; ++i) {
        p_x[i] += delta * v_x[i];
        p_y[i] += delta * v_y[i];
    }
}
