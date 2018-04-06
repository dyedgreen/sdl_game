# This will build the simple SDL game
SOURCES=$(wildcard ./src/**/*.c ./src/*.c)
PNGASSETS=$(wildcard ./assets/*.png ./assets/**/*.png)
BMPTARGETS=$(patsubst %.png,%.bmp,$(PNGASSETS))
PREPASSETS=
REMASSETS=
LIBS=-framework SDL2
CC=gcc

OSFILES=$(wildcard .DS_Store ./src/.DS_Store ./src/**/.DS_Store ./assets/.DS_Store ./assets/**/.DS_Store)
RMOSFILES=

RMOLDBUILDS=

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

# Decide if os files need to be cleared
ifeq ($(OSFILES),)
else
	REMOSFILES= $(foreach var,$(OSFILES),rm $(var);)
endif

# Decide if old builds need to be cleared
ifneq ($(wildcard ./bin/.),)
	RMOLDBUILDS= rm -r ./bin/
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
	$(RMOLDBUILDS)
	@mkdir ./bin/
	$(REMOSFILES)
