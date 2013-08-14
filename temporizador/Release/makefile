################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
temporizador.lss \

FLASH_IMAGE += \
temporizador.hex \

EEPROM_IMAGE += \
temporizador.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: temporizador.elf secondary-outputs

# Tool invocations
temporizador.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,temporizador.map -mmcu=atmega328p -o "temporizador.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

temporizador.lss: temporizador.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S temporizador.elf  >"temporizador.lss"
	@echo 'Finished building: $@'
	@echo ' '

temporizador.hex: temporizador.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex temporizador.elf  "temporizador.hex"
	@echo 'Finished building: $@'
	@echo ' '

temporizador.eep: temporizador.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex temporizador.elf  "temporizador.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: temporizador.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega328p temporizador.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(EEPROM_IMAGE)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) temporizador.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
