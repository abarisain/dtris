#ifndef __BLOCKS_H
#define __BLOCKS_H

#include <stdlib.h>
#include <time.h>
typedef struct {
	int x;
	int y;
	int height;
	int width;
	int currentRotation;
	char*** rotations;
} blockDescriptor;

extern blockDescriptor* blocks[7];

int randInt(int lower, int upper);
void getBlockDimensionsForRotation(blockDescriptor* block, int* height, int* width);
void generateBlocks();
void freeBlocks();
blockDescriptor* randomBlock();
char*** allocBlockRotationsArray(blockDescriptor* block);
void freeBlockRotationsArray(blockDescriptor* block);
blockDescriptor* copyBlock(blockDescriptor* block);
void freeBlock(blockDescriptor* block);
blockDescriptor* newBlock(int height, int width);
blockDescriptor* newSquareBlock();
blockDescriptor* newColumnBlock();
blockDescriptor* newLLeftBlock();
blockDescriptor* newLRightBlock();
blockDescriptor* newSBlock();
blockDescriptor* newZBlock();
blockDescriptor* newTBlock();

#endif
