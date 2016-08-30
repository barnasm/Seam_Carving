CC = clang++#g++

EXE = "Seam\ Carving"

SRC = $(wildcard ./*.cpp)
OBJ = $(SRC:./%.cpp=./obj/%.o)

CPPFLAGS += 
CFLAGS += -std=c++11 -Wall

LDLIBS += $(SUBLIBS) `pkg-config --cflags gtk+-3.0``pkg-config --libs gtk+-3.0` `pkg-config --cflags --libs gtkmm-3.0``pkg-config --cflags --libs gtk+-3.0`

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ $(LDLIBS) -o $@

#test: $(TEST_DIR)/tests.cpp
#$(TEST_DIR)/tests.cpp: 

./obj/%.o: ./%.cpp
	$(CC) $(CFLAGS) $(CPPFLAGS)-c $< $(LDLIBS) -o $@		

clean:
	$(RM) $(OBJ)
