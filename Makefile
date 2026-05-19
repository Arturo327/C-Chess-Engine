CC = gcc
CFLAGS = -g -Iinclude -O2 -march=native
OBJ = build/main.o build/motor.o build/eval.o build/movegen.o build/get_moves.o build/get_captures.o build/make_move.o build/init.o build/sort.o build/zobrist.o build/tt.o build/bitboards.o build/see.o

all: build/R2Chess

build/R2Chess: $(OBJ)
	$(CC) $(OBJ) -lm -o build/R2Chess

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

build/bitboards.o: src/bitboards.c
	$(CC) $(CFLAGS) -c src/bitboards.c -o build/bitboards.o

build/see.o: src/see.c
	$(CC) $(CFLAGS) -c src/see.c -o build/see.o

run: build/R2Chess
	./build/R2Chess

test: build/R2Chess
	cp build/R2Chess ~/Descargas/cutechess/build/R2Chess
	cp build/R2Chess ~/Descargas/lichess-bot/engines/R2Chess
	cp build/R2Chess ~/Descargas/arenalinux_64bit_3.10beta/Engines/R2Chess
	@printf "Enter amount of rounds: "; \
	read rounds; \
	case "$$rounds" in \
		''|*[!0-9]*) \
			echo "Error: rounds must be a positive integer."; \
			exit 1 ;; \
	esac; \
	rm files/results.txt 2> /dev/null; \
	/home/artu/Descargas/cutechess/build/cutechess-cli -engine name="R2Chess" cmd="build/R2Chess" proto=uci -engine name="SF_2500" cmd="stockfish" proto=uci option.UCI_LimitStrength=true option.UCI_Elo=2500 option.Threads=1 -each tc=40/60 -rounds "$$rounds" -games 2 -repeat -pgnout files/matches.pgn >> files/results.txt & \
	echo resultados en files/results.txt y las partidas jugadas en files/matches.pgn

bench: build/R2Chess
	echo bench | ./build/R2Chess

clean:
	rm -f build/*.o build/R2Chess












