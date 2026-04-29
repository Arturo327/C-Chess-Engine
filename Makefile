CC = gcc
CFLAGS = -Iinclude -O2 -march=native
OBJ = build/main.o build/motor.o build/eval.o build/movegen.o build/get_moves.o build/get_captures.o build/make_move.o build/init.o build/sort.o build/zobrist.o build/tt.o

all: build/chess

build/chess: $(OBJ)
	$(CC) $(OBJ) -o build/chess

build/main.o: src/main.c
	$(CC) $(CFLAGS) -c src/main.c -o build/main.o

build/motor.o: src/motor.c
	$(CC) $(CFLAGS) -c src/motor.c -o build/motor.o

build/eval.o: src/eval.c
	$(CC) $(CFLAGS) -c src/eval.c -o build/eval.o

build/movegen.o: src/movegen.c
	$(CC) $(CFLAGS) -c src/movegen.c -o build/movegen.o

build/get_moves.o: src/get_moves.c
	$(CC) $(CFLAGS) -c src/get_moves.c -o build/get_moves.o

build/get_captures.o: src/get_captures.c
	$(CC) $(CFLAGS) -c src/get_captures.c -o build/get_captures.o

build/make_move.o: src/make_move.c
	$(CC) $(CFLAGS) -c src/make_move.c -o build/make_move.o

build/init.o: src/init.c
	$(CC) $(CFLAGS) -c src/init.c -o build/init.o

build/sort.o: src/sort.c
	$(CC) $(CFLAGS) -c src/sort.c -o build/sort.o

build/zobrist.o: src/zobrist.c
	$(CC) $(CFLAGS) -c src/zobrist.c -o build/zobrist.o

build/tt.o: src/tt.c
	$(CC) $(CFLAGS) -c src/tt.c -o build/tt.o

run: build/chess
	./build/chess

clean:
	rm -f build/*.o build/chess
