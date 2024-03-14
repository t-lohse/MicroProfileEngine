# Makefile for building C++ projects with automatic wildcard handling

# Compiler settings - Can be customized.
CXX = g++
CXXFLAGS = -Wall -Wno-packed-bitfield-compat -g --std=c++20
#LDFLAGS = -flto

# Project settings
TARGET = engine
SRCS := $(wildcard *.cpp)
HDRS := $(wildcard *.h)
OBJS := $(SRCS:.cpp=.o)

# Phony targets
.PHONY: all clean

# Main rule
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

# Compile and generate dependency info
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

# Clean build files
clean:	
	rm -f $(TARGET) $(OBJS) $(DIRS)

# Include the generated dependency files
-include $(OBJS:.o=.d)
