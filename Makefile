
CC = g++
CCFLAGS = -Wall -Wextra -std=c++17 -g3

LIBS = -lGL -lGLU -lglut

PATH_SRC = ./src
PATH_INC = ./inc
PATH_BIN = ./bin
PATH_TEST = ./test

.PHONY: all
all:
	mkdir -p $(PATH_BIN)
	@echo
	@echo "*** Compiling object files ***"
	@echo "***"
	make $(OBJS)
	@echo "***"

.PHONY: clean
clean:
	@echo
	@echo "*** Purging binaries ***"
	@echo "***"
	rm -rv $(PATH_BIN)
	@echo "***"

$(PATH_BIN)/%.exe: $(PATH_TEST)/%.cpp $(OBJS)
	$(CC) -I $(PATH_INC) $(DEFINED) $(CCFLAGS) $< $(OBJS) $(LIBS) -o $@

VISUALS_DEP = $(addprefix $(PATH_INC)/, visuals.hpp) $(PATH_SRC)/visuals.cpp

$(PATH_BIN)/visuals.o: $(VISUALS_DEP)
	$(CC) -I $(PATH_INC) $(DEFINED) $(CCFLAGS) $(PATH_SRC)/visuals.cpp -c -o $(PATH_BIN)/visuals.o


OBJS = $(addprefix $(PATH_BIN)/,  visuals.o)
