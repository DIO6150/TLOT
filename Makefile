# Compilateur
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++23 -I./include

# Dossiers
SRCDIR = .
INCDIR = ./include
BINDIR = ./bin
LIBDIR_LINUX = ./libs/linux
LIBS = -lglfw -lGL -ldl -pthread

# Fichiers sources
SOURCES = batch.cpp game_object.cpp layout.cpp main.cpp mesh_manager.cpp mesh.cpp scene.cpp shader.cpp

# Fichiers objets
OBJECTS = $(SOURCES:.cpp=.o)

# Nom du binaire
TARGET = $(BINDIR)/TheLegendOfTeto.out

all: $(BINDIR) $(TARGET)

$(BINDIR):
	mkdir -p $(BINDIR)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -L$(LIBDIR_LINUX) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
