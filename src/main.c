#include "motor.h"
#include <stdio.h>
#include <stdint.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

int parse_square(const char *s) {
    	if (s[0] < 'a' || s[0] > 'h') return -1;
    	if (s[1] < '1' || s[1] > '8') return -1;
    	int col = s[0] - 'a';      // 0-7
    	int row = s[1] - '1';      // 0-7
    	return row * 8 + col;
}

void get_move (Move *move, Pos *pos) {
	char from[8], to[8];
    	int sq;
	int a;

    	do {
        	printf("from: ");
        	a = scanf("%7s", from);
        	sq = parse_square(from);
        	if (sq == -1) printf("casilla invalida, usa formato e2\n");
    	} while (sq == -1);
    	move->from = sq;

    	do {
        	printf("to: ");
        	a = scanf("%7s", to);
        	sq = parse_square(to);
        	if (sq == -1) printf("casilla invalida, usa formato e4\n");
    	} while (sq == -1);
    	move->to = sq;

    	move->pieza = pos->board[move->from];
    	move->capture = pos->board[move->to];
}

void print_square(int sq) {
    	printf("%c%c", 'a' + (sq & 7), '1' + (sq >> 3));
}

void print_move(Move *m) {
    	printf("bot: ");
    	print_square(m->from);
    	printf(" -> ");
    	print_square(m->to);
    	printf("\n");
}

int main () {
	generate_horse_table();
	generate_king_table();
	Pos pos;
	reset(&pos);
	int depth;
	char color;

	Move move;
	printf("bot de ajedrez iniciado\nintroduzca una profundidad: ");
	int a = scanf("%d", &depth);
	/*
	printf("¿blancas o negras? introduzca 'b' o 'n': ");
	a = scanf(" %c", &color);
	int withe = (color == 'b');
	*/
	int my_turn = 1;

	while (1) {
		move = bot_move(depth, my_turn, &pos);
		print_move(&move);
		apply_move(&move, &pos);
		my_turn = !my_turn;
	}

	return 0;
}
