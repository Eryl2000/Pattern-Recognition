#Makefile
CompilerFlags = -std=c++11 -I. -static-libgcc -static-libstdc++
Warnings = -Wall -Wextra -Wdouble-promotion -Wswitch-default \
 -Wconversion -Wuseless-cast -Wshadow
Sanitize = -fsanitize=address -fsanitize=undefined
TestingFlags = -Og -g
ReleaseFlags = -O3
Targets := $(wildcard ./src/*.cpp)
Include = -I ./include/ -I ./eigen-3.3.7/
ProjectName = Project1


all: $(Targets)
	g++ $(CompilerFlags) $(Warnings) $(TestingFlags) $(Targets) $(Include) -o $(ProjectName)

fast: $(Targets)
	g++ $(CompilerFlags) $(Warnings) $(ReleaseFlags) $(Targets) $(Include) -o $(ProjectName)

clean: 
	$(RM) $(ProjectName)