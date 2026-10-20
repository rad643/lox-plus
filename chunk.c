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

// Writes either OP_CONSTANT or OP_CONSTANT_LONG, depending on the size of the
// returned constant pool index, followed by the index in however many bytes that opcode needs (base 256).
void writeConstant(Chunk* chunk, Value value, int line) {
    int operand=addConstant(chunk,value); // add value to the constant pool, and return its index in the pool
    if(operand>255){ // too great for uint8_t so we use 3 uint8_t bytes to store it and the longer opcode
        uint8_t lastByte=operand%256;
        operand=operand/256;
        uint8_t middleByte=operand%256;
        operand=operand/256;
        uint8_t firstByte=operand%256;
        operand=operand/256;
        writeChunk(chunk,OP_CONSTANT_LONG, line);
        writeChunk(chunk, firstByte,line);
        writeChunk(chunk, middleByte,line);
        writeChunk(chunk, lastByte,line);
    }else{ // fits within 0-255 so we use 1 uint8_t byte to store it and the shorter opcode 
        writeChunk(chunk,OP_CONSTANT,line);
        writeChunk(chunk,operand,line);
    }
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