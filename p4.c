/*

Copyright 2014 Ryan Marcus

This file is part of Connect4AI.

Connect4AI is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Connect4AI is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Connect4AI.  If not, see <http://www.gnu.org/licenses/>.

 */

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define OFF_BOARD -2
#define EMPTY -1
#define LOOK_AHEAD 5
#define TABLE_SIZE 32000
#define TABLE_BIN_SIZE 10

typedef struct {
	int width;
	int height;
	int* board;
	int last_move;
	int weight;

	int refs;
} GameState;

/*@null@*/
GameState* newGameState(int width, int height) {
	int i;
	GameState* toR = (GameState*) malloc(sizeof(GameState));
	
	if (toR == NULL)
		return NULL;

	
	toR->width = width;
	toR->height = height;
	
	toR->weight = 0;
	toR->refs = 1;
	toR->last_move = 0;

	
	toR->board = (int*) malloc(sizeof(int) * width * height);
	if (toR->board == NULL) {
		free(toR);
		return NULL;
	}
	

	/*@+forloopexec@*/
	for (i = 0; i < width * height; i++) {
		toR->board[i] = EMPTY;
	}
	/*@=forloopexec@*/

	return toR;
}

void freeGameState(/*@owned@*/ GameState* gs) {
	gs->refs--;
	if (gs->refs <= 0) {
		free(gs->board);
		free(gs);
	}
}

void retainGameState(GameState* gs) {
	gs->refs++;
}

int at(GameState* gs, int x, int y) {
	if (x < 0 || y < 0)
		return OFF_BOARD;

	if (x >= gs->width || y >= gs->height)
		return OFF_BOARD;

	return gs->board[x * gs->height + y];
}

void drop(GameState* gs, int column, int player) {
	int i;
	for (i = 0; i < gs->height; i++) {
		if (at(gs, column, i) == EMPTY) {
			gs->board[column * gs->height + i] = player;
			gs->last_move = column;
			return;
		}
	}


}

int checkAt(GameState* gs, int x, int y) {
	// check across
	int found = 1;
	int curr = at(gs, x, y);
	int i;
	for (i = 0; i < 4; i++) {
		if (at(gs, x + i, y) != curr) {
			found = 0;
			break;
		}
	}

	if (found && (curr != EMPTY && curr != OFF_BOARD))
		return curr;

	// check down
	found = 1;
	for (i = 0; i < 4; i++) {
		if (at(gs, x, y + i) != curr) {
			found = 0;
			break;
		}
	}

	if (found && (curr != EMPTY && curr != OFF_BOARD))
		return curr;

	// check diag +/+
	found = 1;
	for (i = 0; i < 4; i++) {
		if (at(gs, x + i, y + i) != curr) {
			found = 0;
			break;
		}
	}

	if (found && (curr != EMPTY && curr != OFF_BOARD))
		return curr;

	// check diag -/+
	found = 1;
	for (i = 0; i < 4; i++) {
		if (at(gs, x - i, y + i) != curr) {
			found = 0;
			break;
		}
	}

	if (found == 1 && (curr != EMPTY && curr != OFF_BOARD))
		return curr;

	return 0;
}

int getIncrementForArray(int* arr, int player) {
	int toR = 0;
	int i;
	for (i = 0; i < 4; i++) {
		if (arr[i] == player) {
			toR = 1;
			continue;
		}

		if (arr[i] != player && arr[i] != EMPTY) {
			return 0;
		}
	}

	return toR;
}

int countAt(GameState* gs, int x, int y, int player) {

	// check across
	int found = 0;
	int buf[4];
	int i;

	for (i = 0; i < 4; i++) {
		buf[i] = at(gs, x + i, y);
	}
    
	found += getIncrementForArray(buf, player);

	// check down
	for (i = 0; i < 4; i++) {
		buf[i] = at(gs, x, y + i);
	}
	found += getIncrementForArray(buf, player);

	// check diag +/+
	for (i = 0; i < 4; i++) {
		buf[i] = at(gs, x + i, y + i);
	}
	found += getIncrementForArray(buf, player);

	// check diag -/+
	for (i = 0; i < 4; i++) {
		buf[i] = at(gs, x - i, y + i);
	}
	found += getIncrementForArray(buf, player);

	return found;
}

int getWinner(GameState* gs) {
	int x, y;
	int res;
	for (x = 0; x < gs->width; x++) {
		for (y = 0; y < gs->height; y++) {
			res = (int) checkAt(gs, x, y);
			if (res)
				return res;
		}
	}

	return 0;
}

int isDraw(GameState* gs) {
	int x, y;
	for (x = 0; x < gs->width; x++) {
		for (y = 0; y < gs->height; y++) {
			if (at(gs, x, y) == EMPTY)
				return 0;
		}
	}

	return 1;
}

int getHeuristic(GameState* gs, int player, int other_player) {
	int toR = 0;
	int x, y;
	for (x = 0; x < gs->width; x++) {
		for (y = 0; y < gs->height; y++) {
			toR += countAt(gs, x, y, player);
			toR -= countAt(gs, x, y, other_player);
		}
	}
	
	return toR;
}

int canMove(GameState* gs, int column) {
	int y;
	for (y = 0; y < gs->height; y++) {
		if (at(gs, column, y) == EMPTY)
			return 1;
	}

	return 0;
}

/*@null@*/
GameState* stateForMove(GameState* orig, int column, int player) {
	GameState* toR;
	if (orig == NULL || orig->board == NULL)
		return NULL;

	
	
	toR = newGameState(orig->width, orig->height);
	if (toR == NULL)
		return NULL;

	
	memcpy(toR->board, orig->board, sizeof(int) * orig->width * orig->height);
	drop(toR, column, player);
	return toR;
}

void printGameState(GameState* gs) {
	int i, x, y, toP;
	for (i = 0; i < gs->width; i++) {
		printf("%d ", i);
	}

	printf("\n");
	
	for (y = gs->height - 1; y >= 0 ; y--) {
		for (x = 0; x < gs->width; x++) {
			toP = at(gs, x, y);
			if (toP == EMPTY) {
				printf("  ");
			} else {
				printf("%d ", toP);
			}
		}
		printf("\n");
	}

	for (i = 0; i < gs->width; i++) {
		printf("%d ", i);
	}

	printf("\n\n");
}

unsigned long long hashGameState(GameState* gs) {
	unsigned long long hash = 14695981039346656037Lu;
	int i;
	for (i = 0; i < gs->width * gs->height; i++) {
		hash ^= gs->board[i];
		hash *= 1099511628211Lu;
	}
	return hash;
}

int isGameStateEqual(GameState* gs1, GameState* gs2) {
	int i;
	if (gs1->width != gs2->width || gs1->height != gs2->height)
		return 0;

	for (i = 0; i < gs1->width * gs1->height; i++) {
		if (gs1->board[i] == gs2->board[i])
			continue;
		return 0;
	}

	return 1;
}

typedef struct {
	GameState*** bins;
} TranspositionTable;


TranspositionTable* newTable() {
	int i, j;
	TranspositionTable* toR = (TranspositionTable*) malloc(sizeof(TranspositionTable));
	toR->bins = (GameState***) malloc(sizeof(GameState**) * TABLE_SIZE);
	for (i = 0; i < TABLE_SIZE; i++) {
		toR->bins[i] = (GameState**) malloc(sizeof(GameState*) * TABLE_BIN_SIZE);
		for (j = 0; j < TABLE_BIN_SIZE; j++) {
			toR->bins[i][j] = NULL;
		}
	}

	return toR;
}

GameState* lookupInTable(TranspositionTable* t, GameState* k) {
	int hv = hashGameState(k) % TABLE_SIZE;
	int i;
	GameState** bin = t->bins[hv];
	for (i = 0; i < TABLE_BIN_SIZE; i++) {
		if (bin[i] == NULL) {
			return NULL;
		}

		if (isGameStateEqual(k, bin[i])) {
			return bin[i];
		}
	}
	return NULL;
}

void addToTable(TranspositionTable* t, GameState* k) {
	int hv = hashGameState(k) % TABLE_SIZE;
	int i;
	GameState** bin = t->bins[hv];
	for (i = 0; i < TABLE_BIN_SIZE; i++) {
		if (bin[i] == NULL) {
			bin[i] = k;
			retainGameState(k);
			return;
		}
	}

	fprintf(stderr, "Overflow in hash bin %d, won't store GameState\n", hv);
}

void freeTranspositionTable(TranspositionTable* t) {
	int i, j;
	for (i = 0; i < TABLE_SIZE; i++) {
		for (j = 0; j < TABLE_BIN_SIZE; j++) {
			if (t->bins[i][j] != NULL)
				freeGameState(t->bins[i][j]);
		}
		free(t->bins[i]);
	}
	free(t->bins);

	free(t);
}

typedef struct {
	GameState* gs;
	int player;
	int other_player;
	int turn;
	
	int alpha;
	int beta;

	int best_move;

	TranspositionTable* ht;
} GameTreeNode;

GameTreeNode* newGameTreeNode(GameState* gs, int player, int other, int turn, int alpha, int beta, TranspositionTable* ht) {
	GameTreeNode* toR = (GameTreeNode*) malloc(sizeof(GameTreeNode));
	toR->gs = gs;
	toR->player = player;
	toR->other_player = other;
	toR->turn = turn;
	toR->alpha = alpha;
	toR->beta = beta;
	toR->best_move = -1;
	toR->ht = ht;
	return toR;
}

int heuristicForState(GameState* gs, int player, int other) {
	if (isDraw(gs))
		return 0;

	int term_stat = getWinner(gs);
	if (term_stat == player)
		return 1000;

	if (term_stat)
		return -1000;

	
	return getHeuristic(gs, player, other);
		     
}


// using a global instead of qsort_r because of emscripten support

GameTreeNode* g_node = NULL;


int ascComp(const void* a, const void* b) {
	GameTreeNode* node = g_node;
	return heuristicForState(*(GameState**) a, node->player, node->other_player) -
		heuristicForState(*(GameState**) b, node->player, node->other_player);
	
}

int desComp(const void* a, const void* b) {
	GameTreeNode* node = g_node;
	return heuristicForState(*(GameState**) b, node->player, node->other_player) -
		heuristicForState(*(GameState**) a, node->player, node->other_player);
	
}

int getWeight(GameTreeNode* node, int movesLeft) {
	int toR, move, best_weight;
	if (getWinner(node->gs) || isDraw(node->gs) || movesLeft == 0)
		return heuristicForState(node->gs, node->player, node->other_player);

	GameState** possibleMoves = (GameState**) malloc(sizeof(GameState*) * node->gs->width);
	int validMoves = 0;
	for (int possibleMove = 0; possibleMove < node->gs->width; possibleMove++) {
		if (!canMove(node->gs, possibleMove)) {
			continue;
		}
		
		possibleMoves[validMoves] = stateForMove(node->gs, possibleMove, (node->turn ? node->player : node->other_player));
		validMoves++;
	}

	// order possibleMoves by the heuristic
	g_node = node;
	if (node->turn) {
		// qsort_r is apparently non-standard, and won't work with emscripten. So we'll need to use a global.
		qsort(possibleMoves, validMoves, sizeof(GameState*), ascComp); 
	} else {
		qsort(possibleMoves, validMoves, sizeof(GameState*), desComp); 
	}

	best_weight = (node->turn ? INT_MIN : INT_MAX);

	for (move = 0; move < validMoves; move++) {
		// see if the game state is already in the hash table
		GameState* inTable = lookupInTable(node->ht, possibleMoves[move]);
		int child_weight;
		int child_last_move;
		if (inTable != NULL) {
			child_weight = inTable->weight;
			child_last_move = possibleMoves[move]->last_move;

		} else {
			GameTreeNode* child = newGameTreeNode(possibleMoves[move], node->player, node->other_player, !(node->turn),
							      node->alpha, node->beta, node->ht);
			child_weight = getWeight(child, movesLeft - 1);
			child_last_move = child->gs->last_move;
			free(child);
		}

	

		possibleMoves[move]->weight = child_weight;
		addToTable(node->ht, possibleMoves[move]);

		if (movesLeft == LOOK_AHEAD)
			printf("Move %d has weight %d\n", child_last_move, child_weight);

		// alpha-beta pruning
		if (!node->turn) {
			// min node
			if (child_weight <= node->alpha) {
				// MAX ensures we will never go here
				toR = child_weight;
				goto done;
			}
			node->beta = (node->beta < child_weight ? node->beta : child_weight);
		} else {
			// max node
			if (child_weight >= node->beta) {
				// MIN ensures we will never go here
				toR = child_weight;
				goto done;
			}
			node->alpha = (node->alpha > child_weight ? node->alpha : child_weight);
		}

		if (!(node->turn)) {
			// min node
			if (best_weight > child_weight) {
				best_weight = child_weight;
				node->best_move = child_last_move;
			}
		} else {
			// max node
			if (best_weight < child_weight) {
				best_weight = child_weight;
				node->best_move = child_last_move;
			}
		}

		
	}
	toR = best_weight;
done:
	for (int i = 0; i < validMoves; i++) {
		freeGameState(possibleMoves[i]);
	}

	free(possibleMoves);
	return toR;
}

int getBestMove(GameTreeNode* node, int movesLeft) {
	getWeight(node, movesLeft);
	return node->best_move;
}


// END OF API

void checkWin(GameState* gs) {
	int win = getWinner(gs);
	
	if (win) {
		printf("Game over! %d wins!\n", win);
		printGameState(gs);
		exit(0);
	}

	if (isDraw(gs)) {
		printf("Game over! Draw!\n");
		printGameState(gs);
		exit(0);
	}
	     
}

int bestMoveForState(GameState* gs, int player, int other_player, int look_ahead) {
	TranspositionTable* t1 = newTable();
	GameTreeNode* n = newGameTreeNode(gs, player, other_player, 1, INT_MIN, INT_MAX, t1);
	int move = getBestMove(n, look_ahead);
	free(n);
	freeTranspositionTable(t1);
	return move;
}

// a couple of ease-of-use functions that will run a game in global state

GameState* globalState;

void startNewGame() {
	globalState = newGameState(7, 6);
}

void playerMove(int move) {
	drop(globalState, move, 1);
}

void computerMove(int look_ahead) {
	int move = bestMoveForState(globalState, 2, 1, look_ahead);
	drop(globalState, move, 2);
}

int isGameWon() {
	return getWinner(globalState);
}

int isGameDraw() {
	return isDraw(globalState);
}

int isEmpty(int x, int y) {
	return at(globalState, x, y) == EMPTY;
}

int pieceAt(int x, int y) {
	return at(globalState, x, y);
}


int main(int argc, char** argv) {
	startNewGame();
	while (1) {
		int move;
		printf("Move? ");
		scanf("%d", &move);
	       
		playerMove(move);
 
		printGameState(globalState);
 
		checkWin(globalState);
 
		computerMove(LOOK_AHEAD);
 
		printGameState(globalState);
 
		checkWin(globalState);
	}
 
	freeGameState(globalState);
 
	return 0;
}
