CXX = g++

SRC_DIR := ../src
OBJ_DIR := ./obj
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

CXXFLAGS = -Wall -F /Library/Frameworks
LDFLAGS = -I/usr/local/include/SDL2 -D_THREAD_SAFE -L/usr/local/lib --libs sdl2 SDL2_mixer SDL2_image SDL2_ttf SDL2_gfx openal libavcodec libavfilter libavformat libavutil

all: main

main: $(OBJ_FILES)
	$(CXX) $(LDFLAGS) -o $@ $^
	
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

clean:
	rm $@ $^