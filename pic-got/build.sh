#!/bin/bash

set -xe
code="$PWD"

build_firm="false"
run_debug="false"

firmopts="-mcpu=cortex-m33 -mfloat-abi=hard -mfpu=fpv5-sp-d16 -Tstm32h562vgt6.ld -ggdb -Wall -Wextra -fjump-tables -nolibc --specs=nosys.specs -nostartfiles -mthumb -I$code/generated/"
slotopts="-mcpu=cortex-m4 -mfloat-abi=soft -Tslot.ld -ggdb -Wall -Wextra -fjump-tables -nolibc --specs=nosys.specs -nostartfiles -fPIC -msingle-pic-base -mpic-register=r9  -mthumb"

while [ $# -gt 0 ]; do
	case "$1" in
		slot)
			arm-none-eabi-gcc $slotopts $code/slot.c -o build/slot.elf
			arm-none-eabi-objcopy -O binary --set-section-flags .bss=alloc,load,content build/slot.elf build/slot.bin
			xxd -i -n slot build/slot.bin > generated/slot.h
			sed -i "s/unsigned char slot\[\]/const unsigned char slot\[\] __attribute__((used,section(\".slot\")))/g" generated/slot.h
			;;
		firm)
			build_firm="true"
			;;
		debug)
			run_debug="true"
			;;
		*)
			echo "Command not supported"
			exit -1
			;;
	esac
	shift
done



cd . > /dev/null

if [ "$build_firm" = "true" ]; then
	arm-none-eabi-gcc $firmopts main.c startup_stm32h562vgt6.c -o build/out.elf
fi

cd $code > /dev/null

if [ "$run_debug" = "true" ]; then 
	gf2 build/out.elf \
		-ex "target extended-remote /dev/ttyBmpGdb" \
		-ex "monitor auto_scan"\
		-ex "attach 1"\
		-ex "load"\
		-ex "b main"\
		-ex "set mem inaccessible-by-default off"\
		-ex "run"
fi
