# Paths
SPIKE_BUILD :=$(SPIKE)/build-cosim-io
BUILD :=.
# Source files
IOTEST_SRC := ../src/main.cc
COMMON_SRC := $(wildcard ../../cosim-common/*.cc)

# Output binary
OUT := main



# Libraries
LIBRARIES := \
	$(SPIKE_BUILD)/libspike_dasm.a \
	$(SPIKE_BUILD)/libriscv.so

# Object files
LAUNCH_OBJ := $(SPIKE_BUILD)/launch.o
IOTEST_OBJ := $(patsubst ../src/%.cc,$(BUILD)/%.o,$(IOTEST_SRC))
COMMON_OBJ := $(patsubst ../../cosim-common/%.cc,$(BUILD)/%.o,$(COMMON_SRC))

# Include directories
INC_DIRS := \
	-I$(SPIKE)/build \
	-I$(SPIKE)/riscv \
	-I$(SPIKE)/fesvr \
	-I$(SPIKE) \
	-I$(SPIKE)/softfloat \
	-I$(SPIKE)/fdt \
	-I../../cosim-common

# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra $(INC_DIRS)

# Linker flags
LDFLAGS := $(LIBRARIES)

# Default target
all: $(OUT)

$(BUILD)/%.o: ../src/%.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: ../../cosim-common/%.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Build the output binary
$(OUT): $(IOTEST_OBJ) $(COMMON_OBJ) $(LAUNCH_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)


# Build launch.o from spike.o (strip main)
$(LAUNCH_OBJ): $(SPIKE_BUILD)/spike.o
	objcopy --strip-symbol=main $< $@

# Clean up
clean:
	rm -f $(OUT) $(OBJS) $(LAUNCH_OBJ)

.PHONY: all clean
