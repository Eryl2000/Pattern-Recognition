EXE = Project3.exe

SRC_DIR = src
OBJ_DIR = obj

SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CompilerFlags = -std=gnu++11 -I. -static-libgcc -static-libstdc++
Warnings = -Wall -Wextra -Wdouble-promotion -Wswitch-default \
 -Wshadow -Wno-reorder
Sanitize = -fsanitize=address -fsanitize=undefined
TestingFlags = -Og -g
ReleaseFlags = -O3
Targets := $(wildcard ./src/*.cpp)
Include = -I ./include/ -I ./libs/ -I ./libs/eigen-3.3.7/

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJ)
	g++ $(CompilerFlags) $(Warnings) $(TestingFlags) $(Include) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	g++ $(CompilerFlags) $(Warnings) $(TestingFlags) $(Include) -c $< -o $@

reset: clean all

clean:
	$(RM) -rf $(OBJ_DIR) $(EXE)