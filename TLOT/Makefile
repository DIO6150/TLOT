# ========================
#      PROJECT CONFIG
# ========================

# Output
BIN      := bin/teto.exe

# Folders
ENGINE_SRC := engine/src
ENGINE_INC := engine/include

GAME_SRC   := game/src

VENDOR_SRC := vendor
VENDOR_INC := vendor

# Libs
LIBS      := -lassimp -llua -lglfw3 -lgdi32 -lopengl32
ifeq ($(OS), Windows_NT)
LIBDIRS   := -Llibs/windows/glfw3 -Llibs/windows/lua -Llibs/windows/assimp
else
LIBDIRS   := -Llibs/linux
endif

# Compiler
CXX      := g++
CXXFLAGS := -std=c++23 -Wall -Wextra -Werror -I$(ENGINE_INC) -I$(VENDOR_INC)

# Auto-detect .cpp files
SRC := $(wildcard $(ENGINE_SRC)/*.cpp) \
       $(wildcard $(GAME_SRC)/*.cpp) \
       $(wildcard $(VENDOR_SRC)/**/*.cpp)

# Glad
CXXFLAGS += -Ivendor/glad/include
SRC += vendor/glad/src/glad.c

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
	del /q /s -rf engine\src\*.o bin\teto.exe
else
	rm -rf $(OBJ) $(BIN) bin/teto.exe
endif
	

.PHONY: all clean
