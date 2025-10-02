CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -Wpedantic -I./include
LDFLAGS = -pthread

# Build configurations
DEBUG_FLAGS = -g -O0 -DDEBUG
RELEASE_FLAGS = -O3 -DNDEBUG -march=native

# Directories
SRCDIR = src
INCLUDEDIR = include
BUILDDIR = build
BINDIR = bin

# Files
SOURCES = $(SRCDIR)/FSHR_DERIBIT_Main.cpp
OBJECTS = $(BUILDDIR)/FSHR_DERIBIT_Main.o
EXECUTABLE = $(BINDIR)/deribit_order_passer

# Default target
all: release

# Debug build
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(EXECUTABLE)

# Release build
release: CXXFLAGS += $(RELEASE_FLAGS)
release: $(EXECUTABLE)

# Create directories
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

# Build executable
$(EXECUTABLE): $(OBJECTS) | $(BINDIR)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Build complete: $@"

# Compile source files
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(BUILDDIR) $(BINDIR)

# Run the application
run: release
	./$(EXECUTABLE)

# Build and run in debug mode
run-debug: debug
	./$(EXECUTABLE)

.PHONY: all debug release clean run run-debug