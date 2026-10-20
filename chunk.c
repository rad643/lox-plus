#include<stdlib.h>

#include "chunk.h"
#include "memory.h"

void initChunk(Chunk * chunk){
    chunk->count=0;
    chunk->capacity=0;
    chunk->code=NULL;
    chunk->lines=NULL;
    chunk->lineCount=0;
    chunk->lineCapacity=0;
    initValueArray(&chunk->constants);
}

void writeChunk(Chunk * chunk, uint8_t byte, int line){
    // grow the dynamic bytecode instruction array 
    if (chunk->count + 1 > chunk->capacity ) { 
        int oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity);
        chunk->code = GROW_ARRAY(uint8_t, 
                                chunk->code, 
                                oldCapacity, 
                                chunk->capacity);
    }
    chunk->code[chunk->count] = byte;
    chunk->count++;
    
    // we need to check if its a new line number or we're adding bytes to the same source line
    if( chunk->lineCount>0 && line == chunk->lines[chunk->lineCount-1].value){
        chunk->lines[chunk->lineCount-1].count++;
    }else{
        // grow the dynamic line info array if we're at full lineCapacity
        if(chunk->lineCount+1 > chunk->lineCapacity){
            int oldCapacity = chunk->lineCapacity;
            chunk->lineCapacity = GROW_CAPACITY(oldCapacity);
            chunk->lines = GROW_ARRAY(Run, 
                                    chunk->lines, 
                                    oldCapacity, 
                                    chunk->lineCapacity);
        }
        chunk->lines[chunk->lineCount].value = line; // add the new source line to the array 
        chunk->lines[chunk->lineCount].count=1; // for now it only holds 1 byte
        chunk->lineCount++;
        }
}

int addConstant(Chunk* chunk, Value value) {
    writeValueArray(&chunk->constants, value);
    return chunk->constants.count - 1;
}

int getLine(Chunk* chunk, int offset){
    int total=0;
    int run=0;
    while(total<=offset){
        total+=chunk->lines[run].count;
        run++;
    }
    return chunk->lines[run-1].value;
}

void freeChunk(Chunk* chunk) {
	FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    FREE_ARRAY(Run, chunk->lines, chunk->lineCapacity);
    freeValueArray(&chunk->constants);
  	initChunk(chunk);
}