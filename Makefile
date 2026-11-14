CC      = gcc
CFLAGS  = -O2 -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=199309L
SRC     = $(wildcard src/*.c)
OBJ     = $(SRC:.c=.o)
BIN     = experimento
BASE    = base.txt

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Atalhos para os 3 volumes (resultados vao para /resultados)
run100k: $(BIN)
	mkdir -p resultados && ./$(BIN) $(BASE) 100000 resultados/100k.csv

run1m: $(BIN)
	mkdir -p resultados && ./$(BIN) $(BASE) 1000000 resultados/1m.csv

run10m: $(BIN)
	mkdir -p resultados && ./$(BIN) $(BASE) 10000000 resultados/10m.csv

clean:
	rm -f $(OBJ) $(BIN)

.PHONY: all clean run100k run1m run10m
