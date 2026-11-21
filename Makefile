ifeq ($(OS),Windows_NT)
SHELL := cmd.exe
TARGET := bin/compiler.exe
MKDIR_BIN := if not exist "bin" mkdir "bin"
CLEAN_BIN := if exist "bin" rmdir /S /Q "bin"
else
TARGET := bin/compiler
MKDIR_BIN := mkdir -p bin
CLEAN_BIN := rm -rf bin
endif

CXX ?= g++
CPPFLAGS ?= -I.
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -Werror \
            -Wconversion -Wsign-conversion -Wshadow \
            -Wnon-virtual-dtor -Wold-style-cast -Woverloaded-virtual

SOURCES := main.cpp \
           frontend/ast/AST.cpp \
           frontend/ast/visitor/SemanticCheck.cpp \
           frontend/parser/Parser.cpp \
           frontend/symbol/SymbolTable.cpp

all: $(TARGET)

$(TARGET): $(SOURCES)
	@$(MKDIR_BIN)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

clean:
	@$(CLEAN_BIN)

.PHONY: all clean
