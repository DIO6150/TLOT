# ========================
#      PROJECT CONFIG
# ========================

# Output
BIN_WINDOWS	:= bin/teto.exe
BIN_LINUX	:= bin/teto.out

# Folders
ENGINE_SRC := src
ENGINE_INC := -Iinclude

VENDOR_SRC := vendor
VENDOR_INC := vendor

# Libs
LIBS_WINDOWS 	:= -lassimp -llua -lglfw3 -lgdi32 -lopengl32
LIBS_LINUX 	:= -lassimp -llua -lglfw3

# Windows vs Linux
ifeq ($(OS), Windows_NT)
LIBDIRS	:= -Llibs/windows/glfw3 -Llibs/windows/lua -Llibs/windows/assimp
LIBS	:= $(LIBS_WINDOWS)
BIN	:= $(BIN_WINDOWS)
else
LIBDIRS	:= -Llibs/linux/glfw3 -Llibs/linux/lua -Llibs/linux/assimp
LIBS	:= $(LIBS_LINUX)
BIN	:= $(BIN_LINUX)
endif

# Compiler
CXX      := g++
CXXFLAGS := -std=c++23 -Wall -Wextra $(ENGINE_INC) -I$(VENDOR_INC) $(GAME_INC)
CXXFLAGS += -O3

# Auto-detect .cpp files
SRC := $(wildcard $(ENGINE_SRC)/*.cpp) \
       $(wildcard $(GAME_SRC)/*.cpp) \
       $(wildcard $(VENDOR_SRC)/**/*.cpp) \
       test.cpp

# Glad
CXXFLAGS += -Ivendor/glad/include
SRC += vendor/glad/src/glad.c

# Stb
SRC += vendor/stb/stb.c

OBJ := $(SRC:.cpp=.o)
OBJ := $(OBJ:.c=.o)


# ========================
#        BUILD RULES
# ========================

all: $(BIN)

$(BIN): $(OBJ)
	$(CXX) $(OBJ) $(LIBDIRS) $(LIBS) -o $(BIN)

# Generic compilation rule
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
ifeq ($(OS), Windows_NT)
# kindof broken but fuck cmd.exe
	del /q /s src\*.o *.o  bin\teto.exe
else
	rm -rf $(OBJ) $(BIN)
endif
	

.PHONY: all clean
