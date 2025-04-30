# Paths
SPIKE_BUILD :=$(SPIKE)/build-cosim-io
BUILD :=.

# Output binaries
SIMPLE  := simple
SENDER := sender
RECVER := receiver

# Source files
IOTEST_SRC := $(patsubst %,../src/%.cc,$(SIMPLE) $(SENDER) $(RECVER))
COMMON_SRC := $(wildcard ../../cosim-common/*.cc)

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
all: $(SIMPLE) $(SENDER) $(RECVER)

$(BUILD)/%.o: ../src/%.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: ../../cosim-common/%.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Build the output binary
$(BUILD)/%: $(BUILD)/%.o $(COMMON_OBJ) $(LAUNCH_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)


# Build launch.o from spike.o (strip main)
$(LAUNCH_OBJ): $(SPIKE_BUILD)/spike.o
	objcopy --strip-symbol=main $< $@

# Clean up
clean:
	rm -f $(BUILD)/$(SIMPLE) $(BUILD)/$(SENDER) $(BUILD)/$(RECVER) $(COMMON_OBJ) $(IOTEST_OBJ)

.PHONY: all clean

INSTRUCTIONS ?= 266447791
run_with_default_cosim_args:
	export COSIM_ARGS="--instructions=$(INSTRUCTIONS) --log-commits $(SPIKE_TEST)" && \
	$(BUILD)/$(SIMPLE)

run:
	$(BUILD)/$(SIMPLE)

time_with_default_cosim_args:
	export COSIM_ARGS="--instructions=$(INSTRUCTIONS) --log-commits $(SPIKE_TEST)" && \
	echo $$COSIM_ARGS && \
	bash -c "time $(BUILD)/$(SIMPLE)"

time:
	bash -c "time $(BUILD)/$(SIMPLE)"
