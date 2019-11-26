# Compiler settings
CC=gcc
CFLAGS=-Wall -O2 #-fdiagnostics-color=always gcc >=4.9 -std=c18 -O3 -Wextra -Wpedantic
LDFLAGS=-lm

# Source and objects settings

#SRC_EXCLUDES := test.c $(SRC_PUB) $(SRC_ADM) # Exclude specific files to re include them later
SRC := $(filter-out $(SRC_EXCLUDES), $(wildcard *.c)) 
OBJ := $(SRC:.c=.o)

EXEC=parking

# Top level rules
all: parking # Make the executable

# Object building rules
%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f *.o $(EXEC)

# Every rule that doesn't generate a same name file must be mentionned ere
.PHONY: clean
