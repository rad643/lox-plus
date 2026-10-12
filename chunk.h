#ifndef CHUNK_H
#define CHUNK_H

#include "common.h"
#include "value.h"

typedef enum {
  OP_CONSTANT, // = load 
  OP_RETURN, 
} OpCode;

typedef struct{
    int count;
    int capacity;
    uint8_t * code;
    int* lines;
    ValueArray constants;
}Chunk;

void initChunk(Chunk * chunk);
void writeChunk(Chunk * chunk, uint8_t byte, int line);
// Adds a value to the chunk's constant pool and returns the index it landed at.
int addConstant(Chunk* chunk, Value value); 
void freeChunk(Chunk* chunk);

#endif