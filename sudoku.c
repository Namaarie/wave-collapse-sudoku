#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define BLK "\e[1;30m"
#define RED "\e[1;31m"
#define GRN "\e[1;32m"
#define YEL "\e[1;33m"
#define BLU "\e[1;34m"
#define MAG "\e[1;35m"
#define CYN "\e[1;36m"
#define WHT "\e[1;37m"

// NUMBER SPECIFICATION: abc,def,ghi,jkl,mno
// a: control number
// b: value
// c: entropy
// d: x
// e: y
// f: quadrant
// g-o: possibilities

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

void PrintBoard(int64_t board[]){
	char val[24]; // https://stackoverflow.com/questions/8257714/how-to-convert-an-int-to-string-in-c
	for(int y = 0; y < 9; y++){
		if(y % 3 == 0){
			print("*------*------*------*\n", RED);
		}
		for(int x = 0; x < 9; x++){
			sprintf(val, "%lld", board[9*y+x]);
			if(x % 3 == 0){
				print("|", RED);
			}
			if(val[1] != '0'){
				printf("%c ", val[1]);
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

int64_t SetValue(int n, int x, int y){
	char s[] = "109000123456789";
	s[1] = (char)n + 48;
	if(n != 0){
		// control chars
		// no entropy and possibilities
		s[2] = 0;
		for(int i = 6; i < 15; i++){
			s[i] = 0;
		}
	}
	s[3] = (char)x + 48;
	s[4] = (char)y + 48;
	s[5] = 0; // quadrant
	return atoll(s);
}

int main(int argc, char **argv){
	int64_t board[81];
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

	// print arguments list
	printf("%d ARGUMENTS: ", argc);
	for(int i = 0; i < argc; i++){
		printf("%s ", argv[i]);
	}
	return 0;
}
