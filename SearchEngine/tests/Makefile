CXX?=g++

CC=$(CXX)

CPPFLAGS = -MMD -MP

CPPFLAGS += -I$(INC_DIR)
CXXFLAGS = -std=c++20
CXXFLAGS += -pedantic -Wall -Wextra -g -lpthread
CXXFLAGS += -Werror

SRC_DIR = ../src
INC_DIR = ../inc

TARGET = main

OBJECTS = main.o $(SRC_DIR)/crawl.o $(SRC_DIR)/SearchEngineServer.o
LDFLAGS += -lcurl -pthread  -lgumbo

check: $(TARGET)
	./$(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $(TARGET)

$(TARGET): $(OBJECTS) 

all: $(TARGET)

valgrind:$(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file=logV1.txt ./$(TARGET)


hell: $(TARGET)
	valgrind  --tool=helgrind -s ./$(TARGET)


stress: $(TARGET)
	@for number in $(shell seq 1 20) ; do \
        ./$(TARGET) -v ; \
    done

clean:
	@$(RM) $(OBJECTS) $(DEPENDS) $(TARGET)

DEPENDS = $(OBJECTS:%.o=%.d)
-include $(DEPENDS)
