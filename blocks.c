#include "blocks.h"

// rand -> [lower, upper[
int randInt(int lower, int upper) {
	return rand()%(upper-lower) + lower;
}

void getBlockDimensionsForRotation(blockDescriptor* block, int* height, int* width) {
	if(block == NULL) {
		*height = -1;
		*width = -1;
	}
	if(block->currentRotation == 0 || block->currentRotation == 2) {
		*height = block->height;
		*width = block->width;
	} else {
		*height = block->width;
		*width = block->height;
	}
}

void generateBlocks() {
	blocks[0] = newColumnBlock();
	blocks[1] = newSquareBlock();
	blocks[2] = newLRightBlock();
	blocks[3] = newLLeftBlock();
	blocks[4] = newZBlock();
	blocks[5] = newSBlock();
	blocks[6] = newTBlock();
}

void freeBlocks() {
	for(int i = 0; i < 7; i++) {
		free(blocks[i]);
		blocks[i] = NULL;
	}
}

blockDescriptor* randomBlock() {
	blockDescriptor* block = copyBlock(blocks[randInt(0,7)]);
	block->currentRotation = randInt(0, 4);
	return block;
}

char*** allocBlockRotationsArray(blockDescriptor* block) {
	char*** array = (char***) malloc(4 * sizeof(char **));
	array[0] = (char**) malloc(block->height * sizeof(char*));
	array[2] = (char**) malloc(block->height * sizeof(char*));
	array[1] = (char**) malloc(block->width * sizeof(char*));
	array[3] = (char**) malloc(block->width * sizeof(char*));
	for(int i = 0; i < block->height; i++) {
		array[0][i] = (char*) malloc(block->width * sizeof(char));
		array[2][i] = (char*) malloc(block->width * sizeof(char));
		for(int j = 0; j < block->width; j++) {
			array[0][i][j] = ' ';
			array[2][i][j] = ' ';
		}
	}
	for(int i = 0; i < block->width; i++) {
		array[1][i] = (char*) malloc(block->height * sizeof(char));
		array[3][i] = (char*) malloc(block->height * sizeof(char));
		for(int j = 0; j < block->height; j++) {
			array[1][i][j] = ' ';
			array[3][i][j] = ' ';
		}
	}
	return array;
}

void freeBlockRotationsArray(blockDescriptor* block) {
	char*** array = block->rotations;
	for(int i = 0; i < block->height; i++) {
		free(array[0][i]);
		free(array[2][i]);
	}
	for(int i = 0; i < block->width; i++) {
		free(array[1][i]);
		free(array[3][i]);
	}
	for(int i = 0; i < 4; i++) {
		free(array[i]);
	}
	free(array);
}

blockDescriptor* copyBlock(blockDescriptor* block) {
	blockDescriptor* newBlock = (blockDescriptor*) malloc(sizeof(blockDescriptor));
	newBlock->x = block->x;
	newBlock->y = block->y;
	newBlock->height = block->height;
	newBlock->width = block->width;
	newBlock->currentRotation = block->currentRotation;
	newBlock->rotations = allocBlockRotationsArray(newBlock);
	for(int i = 0; i < block->height; i++) {
		for(int j = 0; j < block->width; j++) {
			newBlock->rotations[0][i][j] = block->rotations[0][i][j];
			newBlock->rotations[2][i][j] = block->rotations[2][i][j];
		}
	}
	for(int i = 0; i < block->width; i++) {
		for(int j = 0; j < block->height; j++) {
			newBlock->rotations[1][i][j] = block->rotations[1][i][j];
			newBlock->rotations[3][i][j] = block->rotations[3][i][j];
		}
	}
	return newBlock;
}

void freeBlock(blockDescriptor* block) {
	freeBlockRotationsArray(block);
	free(block);
}

blockDescriptor* newBlock(int height, int width) {
	blockDescriptor* block = malloc(sizeof(blockDescriptor));
	block->x = 0;
	block->y = 0;
	block->width = width;
	block->height = height;
	block->currentRotation = 0;
	block->rotations = allocBlockRotationsArray(block);
	return block;
}

blockDescriptor* newSquareBlock() {
	blockDescriptor* block = newBlock(2, 2);
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 2; j++) {
			for(int k = 0; k < 2; k++) {
				block->rotations[i][j][k] = '#';
			}
		}
	}
	return block;
}

blockDescriptor* newColumnBlock() {
	blockDescriptor* block = newBlock(4, 1);
	for(int i = 0; i < 4; i++) {
		block->rotations[0][i][0] = '#';
		block->rotations[2][i][0] = '#';
		block->rotations[1][0][i] = '#';
		block->rotations[3][0][i] = '#';
	}
	return block;
}

blockDescriptor* newLLeftBlock() {
	blockDescriptor* block = newBlock(3, 2);
	//#
	//#
	//##
	block->rotations[0][0][0] = '#';
	block->rotations[0][1][0] = '#';
	block->rotations[0][2][0] = '#';
	block->rotations[0][2][1] = '#';
	//##
	// #
	// #
	block->rotations[2][0][0] = '#';
	block->rotations[2][0][1] = '#';
	block->rotations[2][1][1] = '#';
	block->rotations[2][2][1] = '#';
	//###
	//#
	block->rotations[1][0][0] = '#';
	block->rotations[1][0][1] = '#';
	block->rotations[1][0][2] = '#';
	block->rotations[1][1][0] = '#';
	//  #
	//###
	block->rotations[3][0][2] = '#';
	block->rotations[3][1][0] = '#';
	block->rotations[3][1][1] = '#';
	block->rotations[3][1][2] = '#';
	return block;
}

blockDescriptor* newLRightBlock() {
	blockDescriptor* block = newBlock(3, 2);
	// #
	// #
	//##
	block->rotations[0][0][1] = '#';
	block->rotations[0][1][1] = '#';
	block->rotations[0][2][0] = '#';
	block->rotations[0][2][1] = '#';
	//##
	//#
	//#
	block->rotations[2][0][0] = '#';
	block->rotations[2][0][1] = '#';
	block->rotations[2][1][0] = '#';
	block->rotations[2][2][0] = '#';
	//###
	//  #
	block->rotations[1][0][0] = '#';
	block->rotations[1][0][1] = '#';
	block->rotations[1][0][2] = '#';
	block->rotations[1][1][2] = '#';
	//#
	//###
	block->rotations[3][0][0] = '#';
	block->rotations[3][1][0] = '#';
	block->rotations[3][1][1] = '#';
	block->rotations[3][1][2] = '#';
	return block;
}

blockDescriptor* newZBlock() {
	blockDescriptor* block = newBlock(2, 3);
	//##
	// ##
	block->rotations[0][0][0] = '#';
	block->rotations[0][0][1] = '#';
	block->rotations[0][1][1] = '#';
	block->rotations[0][1][2] = '#';
	//##
	// ##
	block->rotations[2][0][0] = '#';
	block->rotations[2][0][1] = '#';
	block->rotations[2][1][1] = '#';
	block->rotations[2][1][2] = '#';
	// #
	//##
	//#
	block->rotations[1][0][1] = '#';
	block->rotations[1][1][0] = '#';
	block->rotations[1][1][1] = '#';
	block->rotations[1][2][0] = '#';
	// #
	//##
	//#
	block->rotations[3][0][1] = '#';
	block->rotations[3][1][0] = '#';
	block->rotations[3][1][1] = '#';
	block->rotations[3][2][0] = '#';
	return block;
}

blockDescriptor* newSBlock() {
	blockDescriptor* block = newBlock(2, 3);
	// ##
	//##
	block->rotations[0][0][1] = '#';
	block->rotations[0][0][2] = '#';
	block->rotations[0][1][0] = '#';
	block->rotations[0][1][1] = '#';
	// ##
	//##
	block->rotations[2][0][1] = '#';
	block->rotations[2][0][2] = '#';
	block->rotations[2][1][0] = '#';
	block->rotations[2][1][1] = '#';
	//#
	//##
	// #
	block->rotations[1][0][0] = '#';
	block->rotations[1][1][0] = '#';
	block->rotations[1][1][1] = '#';
	block->rotations[1][2][1] = '#';
	//#
	//##
	// #
	block->rotations[3][0][0] = '#';
	block->rotations[3][1][0] = '#';
	block->rotations[3][1][1] = '#';
	block->rotations[3][2][1] = '#';
	return block;
}

blockDescriptor* newTBlock() {
	blockDescriptor* block = newBlock(2, 3);
	//###
	// #
	block->rotations[0][0][0] = '#';
	block->rotations[0][0][1] = '#';
	block->rotations[0][0][2] = '#';
	block->rotations[0][1][1] = '#';
	// #
	//###
	block->rotations[2][0][1] = '#';
	block->rotations[2][1][0] = '#';
	block->rotations[2][1][1] = '#';
	block->rotations[2][1][2] = '#';
	//#
	//##
	//#
	block->rotations[1][0][0] = '#';
	block->rotations[1][1][0] = '#';
	block->rotations[1][1][1] = '#';
	block->rotations[1][2][0] = '#';
	// #
	//##
	// #
	block->rotations[3][0][1] = '#';
	block->rotations[3][1][0] = '#';
	block->rotations[3][1][1] = '#';
	block->rotations[3][2][1] = '#';
	return block;
}
