#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include <time.h>
clock_t begin, end;
double time_spent;

/*  Compile with: gcc -std=c99 -Wall -Werror -pedantic -o sudoku sudoku.c  */

static void read_file(char filename[], char pattern[9][9]) {
	FILE *fp;
	char line[BUFSIZ];

	fp = fopen(filename, "r");
	if (fp == NULL) {
		fprintf(stderr, "cannot open pattern file '%s'\n", filename);
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i < 9; i++) {
		fgets(line, sizeof line, fp);
		for (int j = 0; j < 9; j++) {
			pattern[i][j] = line[j];
		}
	}
}

static void print_pattern(char pattern[9][9]) {
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if ((j % 3 == 0) && (j > 0)) {
				printf("|");
			}
			printf("%c", pattern[i][j]);
		}
		if ((i % 3 == 2) && (i < 8)) {
			printf("\n-----------");
		}
		printf("\n");
	}
}

static void build_possibilities(char possibilities[9][9][9]) {
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			for (int k = 0; k < 9; k++) {
				possibilities[i][j][k] = (char)(((int)'0') + k + 1);
			}
		}
	}
}

static void square(int x, int y, char pattern[9][9], char possibilities[9][9][9]) {
	char holding[3][3];
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			holding[i][j] = ' ';
		}
	}
	for (int x1 = 0; x1 <= 2; x1++) {
		for (int y1 = 0; y1 <= 2; y1++) {
			if (pattern[x + x1][y + y1] != ' ') {
				holding[x1][y1] = pattern[x + x1][y + y1];
			}
		}
	}
	for (int x1 = 0; x1 <= 2; x1++) {
		for (int y1 = 0; y1 <= 2; y1++) {
			if (pattern[x + x1][y + y1] == ' ') {
				for (int x2 = 0; x2 <= 2; x2++) {
					for (int y2 = 0; y2 <= 2; y2++) {
						if (holding[x2][y2] != ' ') {
							possibilities[x + x1][y + y1][(int)(holding[x2][y2] - '0' - 1)] = ' ';
						}
					}
				}
			}
		}
	}
}

static void vertical(int y, char pattern[9][9], char possibilities[9][9][9]) {
	char holding[9] = "         ";
	
	for (int x = 0; x < 9; x++) {
		if (pattern[x][y] != ' ') {
			holding[x] = pattern[x][y];
		}
	}

	for (int x = 0; x < 9; x++) {
		if (pattern[x][y] == ' ') {
			for (int i = 0; i < 9; i++) {
				if (holding[i] != ' ') {
					possibilities[x][y][(int)(holding[i] - '0' - 1)] = ' ';
				}
			}
		}
	}
	
}


static void horizontal(int x, char pattern[9][9], char possibilities[9][9][9]) {
	char holding[9] = "         ";
	for (int y = 0; y < 9; y++) {
		if (pattern[x][y] != ' ') {
			holding[y] = pattern[x][y];
		}
	}
	
	for (int y = 0; y < 9; y++) {
		if (pattern[x][y] == ' ') {
			for (int i = 0; i < 9; i++) {
				if (holding[i] != ' ') {
					possibilities[x][y][(int)(holding[i] - '0' - 1)] = ' ';
				}
			}
		}
	}
	
}

static void first_pass(char pattern[9][9], char possibilities[9][9][9]) {
	for (int x = 0; x <= 6; x = x + 3) {
		for (int y = 0; y <= 6; y = y + 3) {
			square(x,y, pattern, possibilities);
		}
	}
	for (int x = 0; x < 9; x++) {
		horizontal(x, pattern, possibilities);
	}
	for (int y = 0; y < 9; y++) {
		vertical(y, pattern, possibilities);
	}
}

static bool change_check_pull(char pattern[9][9], char possibilities[9][9][9]) {
	bool change = false;
	
	for (int x = 0; x < 9; x++) {
		for (int y = 0; y < 9; y++) {
			if (pattern[x][y] == ' ') {
				bool numberbool = false;
				char numberchar = ' ';
				
				for (int z = 0; z < 9; z++) {
					if (possibilities[x][y][z] != ' ') {
						if (!numberbool) {
							numberbool = true;
							numberchar = possibilities[x][y][z];
						}
						else {
							numberbool = false;
							break;
						}
					}
				}
				if (numberbool) {
					pattern[x][y] = numberchar;
					change = true;
				}
			}
		}
	}
	
	return change;
}

static int check_holding(char holding[9]) {
	char compare[9] = "123456789";
	int result = 1;
	
	for (int x = 0; x < 9; x++) {
		if (holding[x] == ' ') {
			result = 0;
		}
		else {
			if (compare[(int)(holding[x] - '0' - 1)] != ' ') {
				compare[(int)(holding[x] - '0' - 1)] = ' ';
			}
			else {
				return -1;
			}
		}
	}
	return result;
}

static int check_solved_legal(char pattern[9][9]) {
	int result = 1;
	
	for (int squarex = 0; squarex <= 6; squarex += 3) {
		for (int squarey = 0; squarey <= 6; squarey += 3) {
			char holding[9] = "         ";
			for (int x = 0; x < 3; x++) {
				for (int y = 0; y < 3; y++) {
					holding[x * 3 + y] = pattern[squarex + x][squarey + y];
				}
			}
			switch (check_holding(holding)) {
				case -1 :
					return -1;
				case 0 :
					result = 0;
					break;
			}
		}
	}
	
	for (int x = 0; x < 9; x++) {
		char holding[9] = "         ";
		for (int y = 0; y < 9; y++) {
			holding[y] = pattern[x][y];
		}
		switch (check_holding(holding)) {
			case -1 :
				return -1;
			case 0 :
				result = 0;
				break;
		}
	}
	
	for (int y = 0; y < 9; y++) {
		char holding[9] = "         ";
		for (int x = 0; x < 9; x++) {
			holding[x] = pattern[x][y];
		}
		switch (check_holding(holding)) {
			case -1 :
				return -1;
			case 0 :
				result = 0;
				break;
		}
	}
	return result;
}

static int count_possibilities(char possibilities[9][9][9], int x, int y) {
	int count = 9;
	for (int z = 0; z < 9; z++) {
		if (possibilities[x][y][z] != ' ') {
			if (count == 9) {
				count = 0;
			}
			count++;
		}
	}
	return count;
}

static int try_possibility(char pattern[9][9], char possibilities[9][9][9]) {
	int iterations = 0;
	bool solved = false;
	
	int lowestx = 0;
	int lowesty = 0;
	int lowest = 9;
	
	for (int x = 0; x < 9; x++) {
		for (int y = 0; y < 9; y++) {
			if (pattern[x][y] == ' ') {
				if (lowest > count_possibilities(possibilities, x, y)) {
					lowest = count_possibilities(possibilities, x, y);
					lowestx = x;
					lowesty = y;
				}
			}
		}
	}
	
	if (lowest != 9) {
		while ((lowest > 0) && (!solved)) {
			iterations++;
			
			char testpattern[9][9];
			char testpossibilities[9][9][9];
			
			bool loadprob = false;
			
			for (int x = 0; x < 9; x++) {
				for (int y = 0; y < 9; y++) {
					testpattern[x][y] = pattern[x][y];
					for (int z = 0; z < 9; z++) {
						testpossibilities[x][y][z] = possibilities[x][y][z];
						if ((x == lowestx) && (lowesty == y) && (possibilities[x][y][z] != ' ') && (!loadprob)) {
							testpattern[x][y] = possibilities[x][y][z];
							possibilities[x][y][z] = ' ';
							testpossibilities[x][y][z] = ' ';
							loadprob = true;
						}
					}
				}
			}
			printf("\n===\n");
			print_pattern(testpattern);
			printf("\n===\n");
			switch(check_solved_legal(testpattern)) {
				case 1 :
					solved = true;
					print_pattern(testpattern);
					printf("\nSolved!\n");
					printf("\nIterations: %d\n", iterations);
					end = clock();
					time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
					printf("\nTime to run: %f\n", time_spent);
					exit(EXIT_SUCCESS);
					return iterations;
				case -1 :
					printf("\nillegal moves\n");
					break;
				case 0 :
					iterations += try_possibility(testpattern, testpossibilities);
			}
			
			lowest--;
		}
		
	}
	return iterations;
}

int main(int argc, char *argv[]) {
	char pattern[9][9];
	char possibilities[9][9][9];
	
	if (argc < 2) {
		fprintf(stderr, "Usage: %s pattern-file\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	read_file(argv[1], pattern);
	printf("Beginning Pattern:\n");
	print_pattern(pattern);
	
	switch(check_solved_legal(pattern)) {
		case 1 :
			printf("\nAlready solved!\n");
			return 0;
		case -1 :
			printf("\nUnsolvable, illegal moves\n");
			return 0;
	}
	
	build_possibilities(possibilities);

	int count = 0;
	int iterations = 1;
	do {
		first_pass(pattern, possibilities);
		count++;
	}  while (change_check_pull(pattern, possibilities));
	
	printf("\nBase Passes: %d\n\n", count);
	
	switch(check_solved_legal(pattern)) {
		case 1 :
			print_pattern(pattern);
			printf("\nSolved!\n");
			return 0;
		case -1 :
			printf("\nIllegal moves\n");
			return 0;
	}
	print_pattern(pattern);
	printf("\nBegin iterations:\n\n");
	begin = clock();
	iterations += try_possibility(pattern, possibilities);

	return 0;
}