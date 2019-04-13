CC = g++

EXE = SeamCarving

OBJ_DIR = obj

SRC = $(wildcard ./*.cpp)
OBJ = $(SRC:./%.cpp=./obj/%.o)

CPPFLAGS += 
CFLAGS += -std=c++11 -Wall

LDLIBS += $(SUBLIBS) `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0` `pkg-config --cflags --libs gtkmm-3.0` `pkg-config --cflags --libs gtk+-3.0` -lpthread

.PHONY: all clean

all: dirs $(EXE)

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ $(LDLIBS) -o $@

#test: $(TEST_DIR)/tests.cpp
#$(TEST_DIR)/tests.cpp: 

./obj/%.o: ./%.cpp
	$(CC) $(CFLAGS) $(CPPFLAGS)-c $< $(LDLIBS) -o $@		

dirs:
	mkdir -p $(OBJ_DIR)

clean:
	$(RM) -R $(OBJ_DIR) ${EXE}

