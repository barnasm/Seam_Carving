CXX ?= g++
CCUDA ?= nvcc

# path #
SRC_PATH = src
INC_PATH = include
BUILD_PATH = build
BIN_PATH = $(BUILD_PATH)/bin
PCH_PATH = $(INC_PATH)

# executable # 
BIN_NAME = main

# extensions #
SRC_EXT = cpp
HDR_EXT = h
PCH_EXT = gch
CUDA_EXT = cu

# code lists #
# Find all source files in the source directory, sorted by
# most recently modified
SOURCES = $(shell find $(SRC_PATH) -name '*.$(SRC_EXT)' | sort -k 1nr | cut -f2-)
CUDA_SOURCES = $(shell find $(SRC_PATH) -name '*.$(CUDA_EXT)' | sort -k 1nr | cut -f2-)
# Find all header files in the include directory, sorted by
# most recently modified
INCLUDE_H = $(shell find $(INC_PATH) -name '*.$(HDR_EXT)' | sort -k 1nr | cut -f2-)
PCH_TMP = $(shell find $(PCH_PATH) -name '*.$(PCH_EXT)' | sort -k 1nr | cut -f2-)
INCLUDE_PCH = $(PCH_TMP:$(PCH_PATH)/%.$(PCH_EXT)= -include $(PCH_PATH)/%)
# Set the object file names, with the source directory stripped
# from the path, and the build path prepended in its place
OBJECTS = $(SOURCES:$(SRC_PATH)/%.$(SRC_EXT)=$(BUILD_PATH)/%.o)
PCHEADERS = $(INCLUDE_H:$(INC_PATH)/%.$(HDR_EXT)=$(PCH_PATH)/%.$(HDR_EXT).$(PCH_EXT))
CUDAO = $(CUDA_SOURCES:$(SRC_PATH)/%.$(CUDA_EXT)=$(BUILD_PATH)/%.o)
# Set the dependency files that will be used to add header dependencies
DEPS = $(OBJECTS:.o=.d)

# flags #
COMPILE_FLAGS = -std=c++17 -m64 -Wall -Wextra #-g #with -g segfault during usage precompiled headers

ifeq ($(INCLUDE_PCH),)
  INCLUDE = 
else
  INCLUDE = $(INCLUDE_PCH)
endif
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

.PHONY: clean
clean:
	@echo "Deleting $(INCLUDE_PCH) symlink"
	@$(RM) $(BIN_NAME)
	@echo "Deleting $(INCLUDE_PCH)"
	#@$(RM) -r $(INCLUDE_PCH)
	@echo "Deleting directories"
	@$(RM) -r $(BUILD_PATH)
	@$(RM) -r $(BIN_PATH)

# checks the executable and symlinks to the output
.PHONY: all
all: $(BIN_PATH)/$(BIN_NAME)
	@echo "Making symlink: $(BIN_NAME) -> $<"
	@$(RM) $(BIN_NAME)
	@ln -s $(BIN_PATH)/$(BIN_NAME) $(BIN_NAME)

# Creation of the executable
$(BIN_PATH)/$(BIN_NAME): $(OBJECTS) $(CUDAO)
	@echo "Linking: $@"
	#$(CXX) $(OBJECTS) cuda.a -L/lib64 -L/usr/local/cuda/lib64 -lcudart -o $@
	$(CXX) $(BUILD_PATH)/gpu.o $(CUDAO) $(OBJECTS) -L/lib64 -L/usr/local/cuda/lib64 -lcudart -o $@	

# Add dependency files, if they exist
-include $(INCLUDE_PCH)
-include $(DEPS)

# Source file rules
# After the first compilation they will be joined with the rules from the
# dependency files to provide header dependencies
$(BUILD_PATH)/%.o: $(SRC_PATH)/%.$(SRC_EXT)
	@echo "Compiling: $< -> $@" 
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(INCLUDES) -MP -MMD -c $< -o $@

# Precompile header file rules
.PHONY: pch
pch: $(PCHEADERS)
$(PCH_PATH)/%.$(HDR_EXT).gch: $(INC_PATH)/%.$(HDR_EXT)
	@echo "Precompiling header: $< -> $@" 
	$(CXX) $(COMPILE_FLAGS) $(INCLUDES) -x c++-header -c $< 

# gpu
.PHONY: cuda
cuda : $(CUDAO)
$(BUILD_PATH)/%.o: $(SRC_PATH)/%.$(CUDA_EXT)
	@echo "Compiling gpu: $< -> $@" 
	$(CCUDA) -ccbin g++-5 -m64 -arch=sm_61 -I/include -I./src -dc $< -o $@ -lineinfo --ptxas-options=-v --use_fast_math -L/lib64 -lcudart -lcuda
	$(CCUDA) -ccbin g++-5 -m64 -arch=sm_61 -dlink $(OBJECTS) $(CUDAO) -o $(BUILD_PATH)/gpu.o

