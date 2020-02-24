#Makefile
CompilerFlags = -std=c++11 -I. -static-libgcc -static-libstdc++
Warnings = -Wall -Wextra -Wdouble-promotion -Wswitch-default \
 -Wshadow -Wno-reorder
Sanitize = -fsanitize=address -fsanitize=undefined
TestingFlags = -Og -g
ReleaseFlags = -O3
Targets := $(wildcard ./src/*.cpp)
Include = -I ./include/ -I ./libs/ -I ./libs/eigen-3.3.7/
ProjectName = Project1.exe

ifeq ($(OS),Windows_NT)     # is Windows_NT on XP, 2000, 7, Vista, 10...
    Include += -I c:/Python27/include
else
    Include += -I/usr/include/python2.7 -lpython2.7
endif


all: $(Targets)
	g++ $(CompilerFlags) $(Warnings) $(TestingFlags) $(Targets) $(Include) -o $(ProjectName)

fast: $(Targets)
	g++ $(CompilerFlags) $(Warnings) $(ReleaseFlags) $(Targets) $(Include) -o $(ProjectName)

clean: 
	$(RM) $(ProjectName)