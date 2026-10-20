#include "common.h"
#include "chunk.h"
#include "debug.h"

int main(int argc , const char * argv[]){
    Chunk chunk;
	initChunk(&chunk);
	int i;
	for(i=0;i<300;i++){
		writeConstant(&chunk,1.2,123);
	}
	writeChunk(&chunk,OP_RETURN,124);
	disassembleChunk(&chunk, "test chunk");
	freeChunk(&chunk);
    return 0;
}