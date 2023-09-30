#pragma once

#define ENTITY_CAPACITY 5

typedef unsigned int Entity;

struct System {
    float p_x[ENTITY_CAPACITY];
    float p_y[ENTITY_CAPACITY];
    float v_x[ENTITY_CAPACITY];
    float v_y[ENTITY_CAPACITY];
    float a_x[ENTITY_CAPACITY];
    float a_y[ENTITY_CAPACITY];

    unsigned int entityCount;

    void init();
    Entity add(float p_x, float p_y, float v_x, float v_y);
    void tick(float delta);
};
