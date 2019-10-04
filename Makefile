# Compiler settings
CC=gcc
CFLAGS=-Wall -O2 #-fdiagnostics-color=always gcc >=4.9 -std=c18 -O3 -Wextra -Wpedantic
LDFLAGS=-lm -lsqlite3

# Source and objects settings

#SRC_PUB := $(wildcard ihm_*_pub.c)
SRC_PUB := $(wildcard menu_pub.c)
OBJ_PUB := $(SRC_PUB:.c=.o)
#SRC_ADM := $(wildcard ihm_*_adm.c)
SRC_ADM := $(wildcard menu_adm.c)
OBJ_ADM := $(SRC_ADM:.c=.o)

SRC_EXCLUDES := test.c $(SRC_PUB) $(SRC_ADM) # Exclude specific files to re include them later
SRC := $(filter-out $(SRC_EXCLUDES), $(wildcard *.c)) 
OBJ := $(SRC:.c=.o)

EXEC=banque

# Top level rules
all: $(EXEC)admin $(EXEC)pub # Makes the executable from object files generated by mentionned rules
admin: $(EXEC)admin
pub: $(EXEC)pub

# Object building rules
%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

$(EXEC)admin: $(OBJ) $(OBJ_ADM) # Use general objs and admin specific ones
	$(CC) -o $@ $^ $(LDFLAGS)

$(EXEC)pub: $(OBJ) $(OBJ_PUB)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f *.o $(EXEC)admin $(EXEC)pub

# Every rule that doesn't generate a same name file must be mentionned ere
.PHONY: clean

# Color management (--> built-in color support in GCC 4.9...
CLR_RS=$(\e[0m)
CLR_RED=$(\e[1;33m)
CLR_OUT=$(| sed -e "s/warning/$(CLR_RED)warning$(CLR_RS)/g")
COLORIZE=$(|& while read x ; do echo $x ; done \
| sed -e "s/.*error:.*/\x1b[1;36m&\x1b[0m/" \
-e "s/.*warning:.*/\x1b[1;36m&\x1b[0m/" \
-e "s/^\(.*\)\(required from\)/\x1b[1;36m\1\x1b[0mnote: \2/" \
-e "s/^\(.*\)\(In instantiation of\)/\x1b[1;36m\1\x1b[0mnote: \2/" \
-e "s/^\(.*\)\(In member\)/\x1b[1;36m\1\x1b[0mnote: \2/" \
| sed -e "s/error:/\x1b[1;31m&\x1b[1;36m/" \
-e "s/warning:/\x1b[1;35m&\x1b[1;36m/" \
-e "s/note:/\x1b[1;30m&\x1b[0m/")
