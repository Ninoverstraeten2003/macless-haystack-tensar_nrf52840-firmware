# --- BUILD TARGETS ---
.PHONY: default clean build blink_uf2 generate_key openhaystack

default: build

# --- TOOLCHAIN / PATHS ---
OUTPUT_DIR   ?= compiled
PROJECT_NAME ?= nrf52840_firmware
SDK_ROOT     ?= ./nrf5x-base/sdk/nRF5_SDK_15.3.0_59ac345

# --- KEY GENERATION ---
# Override by passing BASE64_KEY=<your key> on the command line, e.g.:
#   make BASE64_KEY=abc123== build
BASE64_KEY   ?=
GENERATED_KEY_HEADER := public_key.h

CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

# --- SOURCES ---
SRC_FILES := \
	main.c \
	ble_stack.c \
	openhaystack.c \
	battery.c \
	$(SDK_ROOT)/modules/nrfx/mdk/system_nrf52840.c \
	$(SDK_ROOT)/components/boards/boards.c \
	$(SDK_ROOT)/components/softdevice/common/nrf_sdh.c \
	$(SDK_ROOT)/components/softdevice/common/nrf_sdh_ble.c \
	$(SDK_ROOT)/components/libraries/util/app_error.c \
	$(SDK_ROOT)/components/libraries/util/app_error_weak.c \
	$(SDK_ROOT)/components/libraries/util/app_util_platform.c \
	$(SDK_ROOT)/components/libraries/atomic/nrf_atomic.c \
	$(SDK_ROOT)/components/libraries/experimental_section_vars/nrf_section_iter.c \
	$(SDK_ROOT)/components/libraries/strerror/nrf_strerror.c

# --- STARTUP ---
ASM_FILES := custom_startup.S

# --- INCLUDE PATHS ---
CFLAGS += -I. \
	-I$(SDK_ROOT)/components/boards \
	-I$(SDK_ROOT)/components/softdevice/s140/headers \
	-I$(SDK_ROOT)/components/softdevice/s140/headers/nrf52 \
	-I$(SDK_ROOT)/components/softdevice/common \
	-I$(SDK_ROOT)/components/ble/common \
	-I$(SDK_ROOT)/components/libraries/util \
	-I$(SDK_ROOT)/components/libraries/atomic \
	-I$(SDK_ROOT)/components/libraries/balloc \
	-I$(SDK_ROOT)/components/libraries/memobj \
	-I$(SDK_ROOT)/components/libraries/log \
	-I$(SDK_ROOT)/components/libraries/log/src \
	-I$(SDK_ROOT)/external/fprintf \
	-I$(SDK_ROOT)/components/libraries/timer \
	-I$(SDK_ROOT)/components/libraries/strerror \
	-I$(SDK_ROOT)/components/libraries/ringbuf \
	-I$(SDK_ROOT)/components/libraries/experimental_section_vars \
	-I$(SDK_ROOT)/components \
	-I$(SDK_ROOT)/modules/nrfx/mdk \
	-I$(SDK_ROOT)/modules/nrfx \
	-I$(SDK_ROOT)/modules/nrfx/hal \
	-I$(SDK_ROOT)/integration/nrfx/legacy \
	-I$(SDK_ROOT)/integration/nrfx \
	-I$(SDK_ROOT)/components/libraries/delay \
	-I$(SDK_ROOT)/components/toolchain/cmsis/include \
	-DCUSTOM_BOARD_INC=nicenano

# --- FLAGS ---
CFLAGS += -mcpu=cortex-m4 -mthumb -mfloat-abi=soft \
	-Os -g3 -ffunction-sections -fdata-sections \
	-DNRF52840_XXAA -DSOFTDEVICE_PRESENT -DS140 -DNRF_SD_BLE_API_VERSION=6

LDFLAGS += -mcpu=cortex-m4 -mthumb -mfloat-abi=soft \
	-T nrf52840.ld -Wl,--gc-sections --specs=nosys.specs \
	-Wl,--entry=Reset_Handler

# --- OBJECTS ---
OBJ_FILES := \
	$(patsubst %.S,$(OUTPUT_DIR)/%.o,$(ASM_FILES)) \
	$(patsubst %.c,$(OUTPUT_DIR)/%.o,$(notdir $(SRC_FILES)))

VPATH := $(dir $(SRC_FILES))

# --- KEY GENERATION TARGET ---
# `generate_key` is phony so it ALWAYS re-runs binary.py, even when
# public_key.h already exists on disk (e.g. when a new BASE64_KEY is given).
generate_key:
	@echo "Generating $(GENERATED_KEY_HEADER)..."
	@python3 binary.py $(if $(BASE64_KEY),$(BASE64_KEY),)

# Used by the build dependency path: only generates the header when it is
# missing, avoiding unnecessary rebuilds during a normal `make build`.
$(GENERATED_KEY_HEADER):
	@echo "Generating $(GENERATED_KEY_HEADER)..."
	@python3 binary.py $(if $(BASE64_KEY),$(BASE64_KEY),)

# --- COMPILE C ---
# All .o files depend on the generated header so the key is always fresh.
$(OUTPUT_DIR)/%.o: %.c $(GENERATED_KEY_HEADER) | $(OUTPUT_DIR)
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# --- COMPILE ASM ---
$(OUTPUT_DIR)/%.o: %.S | $(OUTPUT_DIR)
	@echo "Assembling $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# --- LINK + OUTPUTS ---
build: $(OBJ_FILES)
	@echo "Linking..."
	@$(CC) $(LDFLAGS) $(OBJ_FILES) -o $(OUTPUT_DIR)/$(PROJECT_NAME).elf
	@echo "Generating HEX..."
	@$(OBJCOPY) -O ihex $(OUTPUT_DIR)/$(PROJECT_NAME).elf $(OUTPUT_DIR)/app.hex
	@echo "Build Complete."

# --- UF2 TARGET ---
# Regenerates the key header first, then builds and converts to UF2.
# Pass BASE64_KEY=<key> to use a custom key, otherwise the default is used.
openhaystack:
	@$(MAKE) generate_key $(if $(BASE64_KEY),BASE64_KEY=$(BASE64_KEY),)
	@$(MAKE) build
	@echo "Converting app.hex to UF2..."
	@python3 uf2conv.py -c -f 0xADA52840 $(OUTPUT_DIR)/app.hex -o flash.uf2
	@echo "DONE! Copy 'flash.uf2' to your drive."

$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)

clean:
	@echo "Cleaning up..."
	rm -rf $(OUTPUT_DIR)
	rm -f flash.uf2
	rm -f $(GENERATED_KEY_HEADER)