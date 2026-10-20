#include <stdio.h>

#include "debug.h"
#include "value.h"

void disassembleChunk(Chunk* chunk, const char* name) {
  printf("== %s ==\n", name);

  for (int offset = 0; offset < chunk->count;) {
    offset = disassembleInstruction(chunk, offset);
  }
}

static int constantInstruction(const char* name, Chunk* chunk, int offset){
  uint8_t constantPoolIndex = chunk->code[offset + 1];
  printf("operation=%s constantPoolIndex=%d ", name, constantPoolIndex);
  printValue(chunk->constants.values[constantPoolIndex]);
  printf("\n");
  return offset+2;
}

static int constantLongInstruction(const char * name, Chunk * chunk, int offset){
  uint8_t firstByte= chunk->code[offset+1];
  uint8_t secondByte= chunk->code[offset+2];
  uint8_t thirdByte= chunk->code[offset+3];
  int constantPoolIndex = (firstByte*256*256) + (secondByte*256) + (thirdByte);
  printf("operation=%s constantPoolIndex=%d ", name, constantPoolIndex);
  printValue(chunk->constants.values[constantPoolIndex]);
  printf("\n");
  return offset+4;
}

static int simpleInstruction(const char* name, int offset) {
  printf("operation=%s\n", name);
  return offset + 1;
}

int disassembleInstruction(Chunk* chunk, int offset) {
  printf("offset=%d sourceLine=%d ", offset , getLine(chunk,offset) );
  uint8_t opCode = chunk->code[offset];
  switch (opCode) {
    case OP_CONSTANT:
      return constantInstruction("OP_CONSTANT", chunk, offset);
    case OP_CONSTANT_LONG:
      return constantLongInstruction("OP_CONSTANT_LONG", chunk, offset);
    case OP_RETURN:
      return simpleInstruction("OP_RETURN", offset);
    default:
      printf("Unknown opcode %d\n", opCode);
      return offset + 1;
  }
}