#Makefile
CompilerFlags = -std=gnu++11 -I. -static-libgcc -static-libstdc++
Warnings = -Wall -Wextra -Wdouble-promotion -Wswitch-default \
 -Wshadow -Wno-reorder
Sanitize = -fsanitize=address -fsanitize=undefined
TestingFlags = -Og -g
ReleaseFlags = -O3
Targets := $(wildcard ./src/*.cpp)
Include = -I ./include/ -I ./libs/ -I ./libs/eigen-3.3.7/
ProjectName = Project2.exe


all: $(Targets)
	g++ $(CompilerFlags) $(Warnings) $(TestingFlags) $(Targets) $(Include) -o $(ProjectName)

fast: $(Targets)
	g++ $(CompilerFlags) $(Warnings) $(ReleaseFlags) $(Targets) $(Include) -o $(ProjectName)

clean: 
	$(RM) $(ProjectName)