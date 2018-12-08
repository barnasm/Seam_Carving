CXX ?= g++

# path #
SRC_PATH = src
INC_PATH = include
BUILD_PATH = build
BIN_PATH = $(BUILD_PATH)/bin
PCH_PATH = $(BUILD_PATH)/precompileHeaders

# executable # 
BIN_NAME = main

# extensions #
SRC_EXT = cpp
HDR_EXT = h

# code lists #
# Find all source files in the source directory, sorted by
# most recently modified
SOURCES = $(shell find $(SRC_PATH) -name '*.$(SRC_EXT)' | sort -k 1nr | cut -f2-)
# Find all header files in the include directory, sorted by
# most recently modified
INCLUDE_H = $(shell find $(INC_PATH) -name '*.$(HDR_EXT)' | sort -k 1nr | cut -f2-)
# Set the object file names, with the source directory stripped
# from the path, and the build path prepended in its place
OBJECTS = $(SOURCES:$(SRC_PATH)/%.$(SRC_EXT)=$(BUILD_PATH)/%.o)
PCHEADERS = $(INCLUDE_H:$(INC_PATH)/%.$(HDR_EXT)=$(PCH_PATH)/%.$(HDR_EXT).gch)
# Set the dependency files that will be used to add header dependencies
DEPS = $(OBJECTS:.o=.d)

# flags #
COMPILE_FLAGS = -std=c++17 -Wall -Wextra -g
INCLUDES = -I $(INC_PATH) -I /usr/local/include
# Space-separated pkg-config libraries used by this project
LIBS =

.PHONY: default_target
default_target: release

.PHONY: release
release: export CXXFLAGS := $(CXXFLAGS) $(COMPILE_FLAGS)
release: dirs
	@$(MAKE) all

.PHONY: dirs
dirs:
	@echo "Creating directories"
	@mkdir -p $(dir $(OBJECTS))
	@mkdir -p $(BIN_PATH)
	@mkdir -p $(PCH_PATH)
	
.PHONY: clean
clean:
	@echo "Deleting $(BIN_NAME) symlink"
	@$(RM) $(BIN_NAME)
	@echo "Deleting directories"
	@$(RM) -r $(BUILD_PATH)
	@$(RM) -r $(BIN_PATH)
	@$(RM) -r $(PCH_PATH)

# checks the executable and symlinks to the output
.PHONY: all
all: $(BIN_PATH)/$(BIN_NAME)
	@echo "Making symlink: $(BIN_NAME) -> $<"
	@$(RM) $(BIN_NAME)
	@ln -s $(BIN_PATH)/$(BIN_NAME) $(BIN_NAME)

# Creation of the executable
$(BIN_PATH)/$(BIN_NAME): $(PCHEADERS) $(OBJECTS)
	@echo "Linking: $@"
	$(CXX) $(OBJECTS) -o $@

# Add dependency files, if they exist
#-include $(PCH_PATH)
-include $(DEPS)

# Source file rules
# After the first compilation they will be joined with the rules from the
# dependency files to provide header dependencies
$(BUILD_PATH)/%.o: $(SRC_PATH)/%.$(SRC_EXT)
	@echo "Compiling: $< -> $@" 
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MP -MMD -c $< -o $@
	
# Precompile header file rules
.PHONY: pch
$(PCH_PATH)/%.$(HDR_EXT).gch: $(INC_PATH)/%.$(HDR_EXT)
	@echo "Precompiling header: $< -> $@" 
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -x c++-header $< -o $@
	

