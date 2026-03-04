#ifndef STRUCTURE_H
#define STRUCTURE_H

typedef struct cord{
    double value;
    struct cord *next;
} cord;
typedef struct vector{
    struct vector *next;
    struct cord *cords;
   
} vector;
#endif