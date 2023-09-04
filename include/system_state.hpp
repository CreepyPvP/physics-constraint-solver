#pragma once

#define ENTITY_CAPACITY 1000

struct System {
    double p_x[ENTITY_CAPACITY];
    double p_y[ENTITY_CAPACITY];
    double v_x[ENTITY_CAPACITY];
    double v_y[ENTITY_CAPACITY];
    double a_x[ENTITY_CAPACITY];
    double a_y[ENTITY_CAPACITY];

    unsigned int entityCount;
};
