CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -O2 -march=native

SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, build/%.o, $(SRC))

all: build/R2Chess

build/R2Chess: $(OBJ)
	$(CC) $(OBJ) -lm -o build/R2Chess

run: build/R2Chess
	./build/R2Chess

build/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

test: build/R2Chess
	cp build/R2Chess ~/Descargas/cutechess/build/R2Chess 2> /dev/null
	cp build/R2Chess ~/Descargas/lichess-bot/engines/R2Chess 2> /dev/null
	cp build/R2Chess ~/Descargas/arenalinux_64bit_3.10beta/Engines/R2Chess 2> /dev/null
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












