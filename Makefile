CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -I./src
LDFLAGS = -lmingw32 -lSDL2main -lSDL2

SRCDIR = src
OBJDIR = obj
BINDIR = bin
TARGET = $(BINDIR)/game.exe

SRC := $(wildcard $(SRCDIR)/*.cpp)
OBJ := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRC))

all: $(OBJDIR) $(BINDIR) $(TARGET)

$(OBJDIR):
	if not exist $(OBJDIR) mkdir $(OBJDIR)

$(BINDIR):
	if not exist $(BINDIR) mkdir $(BINDIR)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	if exist $(OBJDIR) rmdir /S /Q $(OBJDIR)
	if exist $(TARGET) del $(TARGET)

run: $(TARGET)
	.\$(TARGET)

.PHONY: all clean run
