TB_DIR := src/tb
TB_MODULES := $(notdir $(basename $(wildcard $(TB_DIR)/*.sv)))

# Source and C++ files
SRC_FILES := $(wildcard src/pkg/*.sv) $(wildcard src/rtl/*.sv)
CPP_FILES := $(wildcard src/cpp/*.cc)
LIBRARIES :=
LIBRARIES += $(SPIKE)/build/libspike_dasm.a
LIBRARIES += $(SPIKE)/build/libriscv.so

INC_DIRS :=
INC_DIRS := -I$(SPIKE)/build # sadece config.h icin. genel configuration, arch gibi.
INC_DIRS += -I$(SPIKE)/riscv
INC_DIRS += -I$(SPIKE)/fesvr
INC_DIRS += -I$(SPIKE)/
INC_DIRS += -I$(SPIKE)/softfloat
INC_DIRS += -I$(SPIKE)/fdt

# Define compile and run rules for each module dynamically
define TB_RULES

clean_$(1):
	@find obj_dir_$(1) -type f ! -name "$(1)__Dpi.h" -exec rm -f {} +

compile_$(1):
	verilator -O2 -CFLAGS "$(INC_DIRS)" --Mdir obj_dir_$(1) --binary +1800-2017ext+sv $(SRC_FILES) $(TB_DIR)/$(1).sv $(CPP_FILES) $(LIBRARIES) --top $(1) --prefix $(1)  -o $(1).exe
	@echo "--compilation done--"
.PHONY: compile_$(1)

run_with_compile_$(1): compile_$(1)
	./obj_dir_$(1)/$(1).exe

run_$(1):
	./obj_dir_$(1)/$(1).exe

endef

# Generate rules
$(foreach tb,$(TB_MODULES),$(eval $(call TB_RULES,$(tb))))
