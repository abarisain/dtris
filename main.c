#include <stdlib.h>
#include <curses.h>
#include <time.h>
#include <math.h>
#include "blocks.h"

#define COLLISION_NONE 0
#define COLLISION_BOARD_BORDER 1
#define COLLISION_BOARD_BORDER_BOTTOM 2
#define COLLISION_BLOCK 3

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 18
#define BOARD_POSITION_X 15
#define BOARD_POSITION_Y 2

WINDOW *mainWindow;
char gameBoard[BOARD_HEIGHT][BOARD_WIDTH];
int score;
blockDescriptor* blocks[7];
blockDescriptor* nextBlock;
blockDescriptor* currentBlock;
blockDescriptor* nextCurrentBlock; //For moving and rotating
time_t gameStartTime;
int alreadyColided; //For time based block sticking

void clearBoardLine(int row) {
	for(int i = 0; i < BOARD_WIDTH; i++) {
		gameBoard[row][i] = ' ';
	}
}

void copyBoardLine(int source, int target) {
	for(int i = 0; i < BOARD_WIDTH; i++) {
		if(source < 0) {
			gameBoard[target][i] = ' ';
		} else {
			gameBoard[target][i] = gameBoard[source][i];
		}
	}
}

int isBoardRowFull(int row) {
	for(int i = 0; i < BOARD_WIDTH; i++) {
		if(gameBoard[row][i] == ' ') {
			return 0;
		}
	}
	return 1;
}

void clearEmptyBoardLines() {
	for(int i = 0; i < BOARD_HEIGHT; i++) {
		if(isBoardRowFull(i)) {
			score += 100;
			if(i == 0) {
				//Special case, nothing to move
				clearBoardLine(0);
				continue;
			}
			for(int j = i; j >= 0; j--) {
				copyBoardLine(j - 1, j);
			}
		}
	}
}

int checkCollision(blockDescriptor* block) {
	if(block == NULL) {
		return COLLISION_NONE;
	}
	int dim1, dim2;
	getBlockDimensionsForRotation(block, &dim1, &dim2);
	//>= BOARD_HEIGHT, pas trop sur de pourquoi, mais necessaire
	if(block->x < 0 || (block->x + dim2) > BOARD_WIDTH) {
		return COLLISION_BOARD_BORDER;
	}
	if((block->y + dim1) >= BOARD_HEIGHT) {
		return COLLISION_BOARD_BORDER_BOTTOM;
	}

	for(int i = 0; i < dim1; i++) {
		for(int j = 0; j < dim2; j++) {
			char blockElement = block->rotations[block->currentRotation][i][j];
			if(blockElement != ' ' && gameBoard[block->y + i][block->x + j] != ' ') {
				return COLLISION_BLOCK;
			}
		}
	}
	return COLLISION_NONE;
}

void writeBlockToGameBoard(blockDescriptor* block) {
	if(block == NULL) {
		return;
	}
	int dim1, dim2;
	getBlockDimensionsForRotation(block, &dim1, &dim2);
	for(int i = 0; i < dim1; i++) {
		for(int j = 0; j < dim2; j++) {
			char blockElement = block->rotations[block->currentRotation][i][j];
			if(blockElement != ' ') {
				gameBoard[block->y + i][block->x + j] = blockElement;
			}
		}
	}
}

void drawBlockWithPosition(blockDescriptor* block, int x, int y) {
	if(block == NULL) {
		return;
	}
	int dim1, dim2;
	getBlockDimensionsForRotation(block, &dim1, &dim2);
	for(int i = 0; i < dim1; i++) {
		for(int j = 0; j < dim2; j++) {
			char blockElement = block->rotations[block->currentRotation][i][j];
			if(blockElement != ' ') {
				mvwprintw(mainWindow, y+i, x+j, "%c", blockElement);
			}
		}
	}
}

void drawBlock(blockDescriptor* block) {
	if(block == NULL) {
		return;
	}
	drawBlockWithPosition(block, BOARD_POSITION_X + block->x, BOARD_POSITION_Y + block->y);
}

void eraseBlockWithPosition(blockDescriptor* block, int x, int y) {
	if(block == NULL) {
		return;
	}
	int dim1, dim2;
	getBlockDimensionsForRotation(block, &dim1, &dim2);
	for(int i = 0; i < dim1; i++) {
		for(int j = 0; j < dim2; j++) {
			char blockElement = block->rotations[block->currentRotation][i][j];
			if(blockElement != ' ') {
				mvwprintw(mainWindow, y+i, x+j, "%c", ' ');
			}
		}
	}
}

void eraseBlock(blockDescriptor* block) {
	if(block == NULL) {
		return;
	}
	eraseBlockWithPosition(block, BOARD_POSITION_X + block->x, BOARD_POSITION_Y + block->y);
}

void drawNextMovableBlock() {
	if(nextCurrentBlock == NULL) {
		drawBlock(currentBlock);
		return;
	}
	//Pour l'instant on redessine tout à chaque fois
	//eraseBlock(currentBlock);
	drawBlock(nextCurrentBlock);
}

void redrawNextBlock() {
	mvwprintw(mainWindow, 4, 3, "        ");
	mvwprintw(mainWindow, 5, 3, "        ");
	mvwprintw(mainWindow, 6, 3, "        ");
	mvwprintw(mainWindow, 7, 3, "        ");
	drawBlockWithPosition(nextBlock, 4, 4);
}

void generateNextBlock() {
	if(nextBlock != NULL) {
		freeBlock(nextBlock);
	}
	nextBlock = randomBlock();
	redrawNextBlock();
}

void printScore() {
	//Offset = 8 -> 'Score : ' + 30
	mvwprintw(mainWindow, 2, 36, "%d", score);
}

void redrawGameBoard() {
	for(int i = 0; i < BOARD_HEIGHT; i++) {
		mvwprintw(mainWindow, BOARD_POSITION_Y + i, BOARD_POSITION_X - 1, "+");
		mvwprintw(mainWindow, BOARD_POSITION_Y + i, BOARD_POSITION_X + BOARD_WIDTH, "+");
		for(int j = 0; j < BOARD_WIDTH; j++) {
			mvwprintw(mainWindow, BOARD_POSITION_Y + i, BOARD_POSITION_X + j, "%c", gameBoard[i][j]);
		}
	}
	for(int i = 0; i < (BOARD_WIDTH + 2); i++) {
		mvwprintw(mainWindow, BOARD_POSITION_Y - 1 + BOARD_HEIGHT, 14 + i, "+");
	}

}

void redrawScreen() {
	mvprintw(1, 1, "dtris 0.1");
	mvwprintw(mainWindow, 2, 28, "Score : ");
	printScore();
	//mvwprintw(mainWindow, 2, 2, "Sup");
	mvwprintw(mainWindow, 2, 2, "+--Next--+");
	mvwprintw(mainWindow, 3, 2, "+        +");
	mvwprintw(mainWindow, 4, 2, "+        +");
	mvwprintw(mainWindow, 5, 2, "+        +");
	mvwprintw(mainWindow, 6, 2, "+        +");
	mvwprintw(mainWindow, 7, 2, "+        +");
	mvwprintw(mainWindow, 8, 2, "+        +");
	mvwprintw(mainWindow, 9, 2, "+--------+");
	redrawGameBoard();
	redrawNextBlock();
	drawNextMovableBlock();
	wrefresh(mainWindow);
	refresh();
}

void newGame() {
	score = 0;
	time(&gameStartTime);
	for(int i = 0; i < BOARD_HEIGHT; i++) {
		for(int j = 0; j < BOARD_WIDTH; j++) {
			gameBoard[i][j] = ' ';
		}
	}
	if(nextBlock != NULL) {
		freeBlock(nextBlock);
	}
	nextBlock = randomBlock();
	nextCurrentBlock = NULL;
	currentBlock = randomBlock();
	redrawScreen();
}

int main(int argc, char** argv) {
	int pressedKey;

	srand(time(NULL));
	generateBlocks();
	nextBlock = NULL;

	initscr();
	noecho();
	cbreak();
	curs_set(0);
	refresh();

	if(LINES < 30 || COLS < 50) {
		printf("Error : Terminal must be at least 30x50");
		endwin();
		return 0;
	}

	WINDOW* terminalWindow = newwin(LINES, COLS, 0, 0);
	box(terminalWindow, 0, 0);
	wrefresh(terminalWindow);
	mainWindow = newwin(30, 44, LINES/2-15, COLS/2-22);
	//box(mainWindow, 0, 0);
	nodelay(mainWindow, TRUE);
	keypad(mainWindow, TRUE);
	newGame();
	wrefresh(mainWindow);
	refresh();
	while(1) {
		pressedKey = wgetch(mainWindow);
		switch(pressedKey) {
			/*case 'R':
			case 'r':
				nextCurrentBlock = nextBlock;
				//Si pas NULL, alors il est free par la méthode suivante
				//Plus rapide que de le copier
				nextBlock = NULL;
				generateNextBlock();
				break;
			case 'F':
			case 'f':
				nextCurrentBlock = nextBlock;
				nextBlock = NULL;
				generateNextBlock();
				writeBlockToGameBoard(currentBlock);
				break;*/
			case ' ':
				nextCurrentBlock = copyBlock(currentBlock);
				nextCurrentBlock->currentRotation--;
				if(nextCurrentBlock->currentRotation < 0) {
					nextCurrentBlock->currentRotation = 3;
				}
				break;
			case KEY_UP:
				nextCurrentBlock = copyBlock(currentBlock);
				nextCurrentBlock->y--;
				break;
			case KEY_DOWN:
				nextCurrentBlock = copyBlock(currentBlock);
				nextCurrentBlock->y++;
				break;
			case KEY_LEFT:
				nextCurrentBlock = copyBlock(currentBlock);
				nextCurrentBlock->x--;
				break;
			case KEY_RIGHT:
				nextCurrentBlock = copyBlock(currentBlock);
				nextCurrentBlock->x++;
				break;
		}
		time_t currentTime;
		time(&currentTime);
		int timeDiff = floor(difftime(currentTime, gameStartTime));
		if(timeDiff >= 1) {
			time(&gameStartTime);
			if(nextCurrentBlock == NULL) {
				nextCurrentBlock = copyBlock(currentBlock);
			}
			nextCurrentBlock->y++;
			int collision = checkCollision(nextCurrentBlock);
			if(collision == COLLISION_BLOCK || collision == COLLISION_BOARD_BORDER_BOTTOM) {
				if(alreadyColided) {
					alreadyColided = 0;
					writeBlockToGameBoard(currentBlock);
					freeBlock(nextCurrentBlock);
					nextCurrentBlock = nextBlock;
					nextBlock = NULL;
					generateNextBlock();
				} else {
					freeBlock(nextCurrentBlock);
					nextCurrentBlock = NULL;
					alreadyColided = 1;
				}
			} else {
				alreadyColided = 0;
			}
		} else {
			if(nextCurrentBlock != NULL) {
				if(checkCollision(nextCurrentBlock)) {
					freeBlock(nextCurrentBlock);
					nextCurrentBlock = NULL;
				}
			}
		}
		clearEmptyBoardLines();
		redrawGameBoard();
		printScore();
		drawNextMovableBlock();
		if(nextCurrentBlock != NULL) {
			if(currentBlock != NULL) {
				free(currentBlock);
			}
			currentBlock = nextCurrentBlock;
			nextCurrentBlock = NULL;
		}
		wrefresh(mainWindow);
		refresh();
	}
	freeBlocks();
	endwin();
	return 0;
}
