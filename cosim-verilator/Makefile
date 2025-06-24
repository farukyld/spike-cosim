TB_DIR := src/tb
TB_MODULES := $(notdir $(basename $(wildcard $(TB_DIR)/*.sv)))

# Source and C++ files
SPIKE_BUILD := build

SRC_FILES := $(wildcard src/pkg/*.sv) $(wildcard src/rtl/*.sv)
CPP_VERLTR_FILES := $(wildcard src/cpp/*.cc)
# $(info "CPP_VERLTR_FILES $(CPP_VERLTR_FILES)")
CPP_COMMON_FILES := $(wildcard ../cosim-common/*.cc)
# $(info "CPP_COMMON_FILES $(CPP_COMMON_FILES)")

LIBRARIES :=
LIBRARIES += $(SPIKE)/$(SPIKE_BUILD)/libspike_dasm.a
LIBRARIES += $(SPIKE)/$(SPIKE_BUILD)/libriscv.so
LAUNCH_OBJ:= $(SPIKE)/$(SPIKE_BUILD)/launch.o

INC_DIRS :=
INC_DIRS := -I$(SPIKE)/$(SPIKE_BUILD) # sadece config.h icin. genel configuration, arch gibi.
INC_DIRS += -I$(SPIKE)/riscv
INC_DIRS += -I$(SPIKE)/fesvr
INC_DIRS += -I$(SPIKE)/
INC_DIRS += -I$(SPIKE)/softfloat
INC_DIRS += -I$(SPIKE)/fdt
# assuming makefile is under cosim-verilator directory which is in same directory with cosim-common
INC_DIRS += -I../../cosim-common

$(LAUNCH_OBJ): $(SPIKE)/$(SPIKE_BUILD)/spike.o
	objcopy --strip-symbol=main $< $@


# Define compile and run rules for each module dynamically
define TB_RULES

clean_$(1):
	@find obj_dir_$(1) -type f ! -name "$(1)__Dpi.h" -exec rm -f {} +


compile_$(1): $(LAUNCH_OBJ)
	verilator -O2 -CFLAGS "-DDPI_HEADER=\\\"$(1)__Dpi.h\\\" -DVERILATOR $(INC_DIRS)" --Mdir obj_dir_$(1) --binary \
	+1800-2017ext+sv $(SRC_FILES) $(TB_DIR)/$(1).sv $(CPP_COMMON_FILES) $(CPP_VERLTR_FILES)\
	$(LAUNCH_OBJ) $(LIBRARIES)\
	 --top $(1) --prefix $(1)  -o $(1).exe
	@echo "--compilation done--"
.PHONY: compile_$(1)

run_with_compile_$(1): compile_$(1)
	./obj_dir_$(1)/$(1).exe

run_$(1):
	./obj_dir_$(1)/$(1).exe

endef

# Generate rules
$(foreach tb,$(TB_MODULES),$(eval $(call TB_RULES,$(tb))))
