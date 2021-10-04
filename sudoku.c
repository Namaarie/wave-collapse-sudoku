#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

#define BLK "\e[1;30m"
#define RED "\e[1;31m"
#define GRN "\e[1;32m"
#define YEL "\e[1;33m"
#define BLU "\e[1;34m"
#define MAG "\e[1;35m"
#define CYN "\e[1;36m"
#define WHT "\e[1;37m"
#define RELATED_AMT 28

// NUMBER SPECIFICATION: abc,def,ghi,jkl,mno
// a: control number
// b: value
// c: entropy
// d: x
// e: y
// f: quadrant
// g-o: possibilities

typedef struct Tile{
	int x;
	int y;
	int quadrant;
	int value;
	int entropy;
	int possibilities[9];
	int related[RELATED_AMT];
} Tile;

void PrintBoardBig(int64_t board[]){
	printf(RED);
	printf("-------------------------------------"); // vertical separation between large tile
	for(int y = 0; y < 9; y++){
		for(int o = 0; o < 3; o++){
			printf("\n"); // inbetween inner rows
			for(int x = 0; x < 9; x++){
				if(x % 3 == 0){
					printf(RED);
				}
				printf("|");
				for(int i = 0; i < 3; i++){
					printf(WHT);
					printf("%lld ", board[9*y+x]);
				}
			}
			printf(RED);
			printf("|");
		}
		printf(WHT);
		if(y % 3 == 2){
			printf(RED);
		}
		printf("\n-------------------------------------"); // vertical separation between large tile
	}
}

void print(char *string, char *color){
	printf("%s", color);
	printf("%s", string);
	printf(WHT);
}

void PrintBoard(Tile *board){
	for(int y = 0; y < 9; y++){
		if(y % 3 == 0){
			print("*------*------*------*\n", RED);
		}
		for(int x = 0; x < 9; x++){
			int value = board[9*y+x].entropy;
			if(x % 3 == 0){
				print("|", RED);
			}
			if(value != 0){
				printf("%d ", value);
			}else{
				printf(". ");
			}
		}
		print("|", RED);
		printf("\n");
	}
	print("*------*------*------*\n", RED);
}

void Clear(){
	printf("\e[1;1H\e[2J");
}

int GetQuadrant(int x, int y){
	// calculate quadrant
	if(abs(x - 4) <= 1){
		// center column
		if(abs(y - 4) <= 1){
			// center center
			return 4;
		}else if(y - 4 < 0){
			// top center
			return 1;
		}else{
			// bottom center
			return 7;
		}
	}else if(x - 4 < 0){
		// left column
		if(abs(y - 4) <= 1){
			// left center
			return 3;
		}else if(y - 4 < 0){
			// top left
			return 0;
		}else{
			// bottom left
			return 6;
		}
	}else{
		// right column
		if(abs(y - 4) <= 1){
			// right center
			return 5;
		}else if(y - 4 < 0){
			// top right
			return 2;
		}else{
			// bottom right
			return 8;
		}
	}
}

Tile SetValue(int n, int x, int y){
	Tile tile;
	tile.value = n;
	if(n != 0){
		// control chars
		// no entropy and possibilities
		tile.entropy = 0;
		for(int i = 0; i < 9; i++){
			tile.possibilities[i] = 0;
		}
	}else{
		tile.entropy = 9;
		for(int i = 0; i < 9; i++){
			tile.possibilities[i] = i + 1;
		}
	}
	tile.x = x;
	tile.y = y;

	tile.quadrant = GetQuadrant(x, y);

	// calculate related tiles
	int myIndex = 9 * y + x;
	int relatedLength = 0;
	// gets column
	for(int i = 0; i < 9; i++){
		int index = 9 * i + x;
		if(index != myIndex){
			tile.related[relatedLength] = index;
			relatedLength++;
		}
	}
	// get row
	for(int i = 9*y; i < 9*y+8; i++){
		if(i != myIndex){
			tile.related[relatedLength] = i;
			relatedLength++;
		}
	}

	// get quadrant
	for(int yy = 0; yy < 9; yy++){
		for(int xx = 0; xx < 9; xx++){
			if(GetQuadrant(xx, yy) == tile.quadrant && 9*yy+xx != myIndex){
				tile.related[relatedLength] = 9*yy+xx;
				relatedLength++;
			}
		}
	}
	//printf("RELATED LENGTH: %d\n", relatedLength);

	return tile;
}

void Collapse(int index, Tile *board){
	// get related tiles, and drop thir entropy
	printf("COLLAPSING %d\n", index);
	for(int i = 0; i < 81; i++){
		printf("i: %d, e: %d, v: %d\n", i, board[i].entropy, board[i].value);
	}
	for(int i = 0; i < RELATED_AMT; i++){
		Tile currentTile = board[index];
		Tile relatedTile = board[currentTile.related[i]];
		if(relatedTile.possibilities[currentTile.value-1] != 0){
			relatedTile.possibilities[currentTile.value-1] = 0;
			if(relatedTile.entropy > 0){
				relatedTile.entropy--;
			}
			printf("amt: %d ", i);
			printf("e: %d, v: %d\n", relatedTile.entropy, currentTile.value);
			for(int x = 0; x < 9; x++){
				// printf("i: %d, p: %d ", relatedTile.y * 9 + relatedTile.x, relatedTile.possibilities[x]);
			}
		}
		if(relatedTile.entropy == 1){
			printf("ENTROPY 1");
			for(int j = 0; j < 9; j++){
				if(relatedTile.possibilities[j] != 0){
					relatedTile.value = relatedTile.possibilities[j];
					relatedTile.possibilities[j] = 0;
					relatedTile.entropy = 0;
				}
			}
			Collapse(9*relatedTile.y+ relatedTile.x, board);
		}
		board[index] = currentTile;
		board[currentTile.related[i]] = relatedTile;
	}
}

int main(int argc, char **argv){
	Tile board[81];
	char a[] = "9715..842..69...1....8.2..95.....79...76.83...28.....57..1.5....4...91..819..7254";
	Clear();
	for(int y = 0; y < 9; y++){
		for(int x = 0; x < 9; x++){
			if(a[9*y+x] != '.'){
				board[9*y+x] = SetValue(a[9*y+x] - 48, x, y);
			}else{
				board[9*y+x] = SetValue(0, x, y);
			}
		}
	}
	PrintBoard(board);

	// SOLVE
	for(int i = 0; i < 81; i++){
		if(board[i].entropy == 0){
			Collapse(i, board);
		}
	}
	PrintBoard(board);
	// print arguments list
	printf("%d ARGUMENTS: ", argc);
	for(int i = 0; i < argc; i++){
		printf("%s ", argv[i]);
	}
	return 0;
}
