# This will build the simple SDL game
SOURCES=$(wildcard ./src/**/*.c ./src/*.c)
PNGASSETS=$(wildcard ./assets/*.png ./assets/**/*.png ./assets/**/**/*.png)
BMPTARGETS=$(patsubst %.png,%.bmp,$(PNGASSETS))
PREPASSETS=
REMASSETS=
LIBS=-framework SDL2
CC=gcc

# Simple switch between macOS / Linux TODO: Support windows builds
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	LIBS=-lSDL2
endif

# Decide if assets need to be prepared
ifeq ($(PNGASSETS),)
else
	PREPASSETS= $(foreach var,$(PNGASSETS),magick $(var) $(patsubst %.png,%.bmp,$(var));)
	REMASSETS=rm $(PNGASSETS)
endif

all: clean
all: prepassets
all: cpassets
all:
	@mkdir -p ./build/
	$(CC) $(SOURCES) $(LIBS) -g -o ./build/game.app
	cp ./build/game.app ./bin/game.app
	rm -r ./build/

build:
	@mkdir -p ./build/

cpassets:
	@mkdir -p ./bin/
	@mkdir -p ./bin/assets/
	cp -r ./assets/ ./bin/assets/

prepassets:
	$(PREPASSETS)
	$(REMASSETS)

clean:
	rm -r ./bin/
	@mkdir ./bin/
