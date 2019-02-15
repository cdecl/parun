CC = g++
CC_FLAGS = -std=c++11 -pedantic -Wall -O2 -I./clipp/include -I./ThreadPool
LD_LIBS = -pthread

EXEC = parun
SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

# Main target
$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXEC) $(LD_LIBS)

%.o: %.cpp
	$(CC) -c $(CC_FLAGS) $< -o $@

clean:
	rm -f $(EXEC) $(OBJECTS)
	
	