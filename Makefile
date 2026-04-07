CXX       := g++
CXX_FLAGS := -std=c++23 -Wall -Wextra -Iinclude -Ideps -Ideps/glad/include
CXX_FLAGS += -O0 -g

AR        := ar
AR_FLAGS  := rcs

SRC_DIR := src
BIN_DIR := bin
LIB_DIR := lib

EXEC_WINDOWS := $(BIN_DIR)/$(LIB_DIR)/libTetoEngine.a
LIBS_FLAGS_WINDOWS := -lglfw3 -lgdi32 -lopengl32 -lstdc++exp

EXEC_LINUX := $(BIN_DIR)/$(LIB_DIR)/tetoEngine.s
LIBS_FLAGS_LINUX := -lglfw3

SRC := $(wildcard $(SRC_DIR)/*.cpp)
SRC += deps/glad/src/glad.c
SRC += deps/stb/stb.c

OBJ := $(addprefix $(BIN_DIR)/, $(notdir $(patsubst %.cpp, %.o, $(patsubst %.c, %.o, $(SRC)))))

ifeq ($(OS), Windows_NT)
LIB_DIRS := -Llibs/windows/glfw3
LIBS	 := $(LIBS_FLAGS_WINDOWS)
BIN	     := $(EXEC_WINDOWS)
CLEAN_CMD := del /q /f $(subst /,\,$(OBJ)) $(subst /,\,$(BIN))

else
LIB_DIRS := -Llibs/linux/glfw3
LIBS	 := $(LIBS_FLAGS_LINUX)
BIN 	 := $(EXEC_LINUX)
CLEAN_CMD := rm -rf $(OBJ) $(BIN)
endif

all : $(BIN)

$(BIN) : $(OBJ)
	$(AR) $(AR_FLAGS) $(BIN) $(OBJ)

$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXX_FLAGS) -c $< -o $@

$(BIN_DIR)/%.o: deps/glad/src/%.c
	$(CXX) $(CXX_FLAGS) -c $< -o $@

$(BIN_DIR)/%.o: deps/stb/%.c
	$(CXX) $(CXX_FLAGS) -c $< -o $@

clean:
	$(CLEAN_CMD)