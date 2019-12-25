TARGET       = cpuinfo.efi
OBJS         = main.o file.o util.o firmware.o cpu.o

ARCH         = $(shell uname -m | sed s,i[3456789]86,ia32,)
EFIINC       = /usr/include/efi
EFIINCS      = -I$(EFIINC) -I$(EFIINC)/$(ARCH) -I$(EFIINC)/protocol
LIB          = /usr/lib
EFILIB       = /usr/lib
EFI_CRT_OBJS = $(EFILIB)/crt0-efi-$(ARCH).o
EFI_LDS      = $(EFILIB)/elf_$(ARCH)_efi.lds

CC           = gcc
CFLAGS       = $(EFIINCS) -fno-stack-protector -fpic -fshort-wchar -mno-red-zone -Wall -Wextra
ifeq ($(ARCH),x86_64)
  CFLAGS += -DEFI_FUNCTION_WRAPPER
endif

LDFLAGS      = -nostdlib -znocombreloc -T $(EFI_LDS) -shared -Bsymbolic -L $(EFILIB) -L $(LIB) $(EFI_CRT_OBJS)

target: $(TARGET)

bootable: target
	mkdir -p ./EFI/BOOT
	cp ./$(TARGET) ./EFI/BOOT/BOOTX64.EFI

%.so: $(OBJS)
	ld $(LDFLAGS) $(OBJS) -o $@ -lefi -lgnuefi

%.efi: %.so
	objcopy -j .text -j .sdata -j .data -j .dynamic \
		-j .dynsym  -j .rel -j .rela -j .reloc \
		--target=efi-app-$(ARCH) $^ $@

main.o: main.c
file.o: file.c
util.o: util.c
firmware.o: firmware.c
cpu.o: cpu.c

QEMUOPTS = -L /usr/share/OVMF -bios OVMF_CODE.fd -net none -hda fat:rw:.

qemu: target
	qemu-system-$(ARCH) $(QEMUOPTS)

qemu-nox: target
	@echo "*********************************"
	@echo "*** Use ${bold}\e[38;2;255;0;0;1mCtrl-a x\e[0m${normal} to exit qemu ***"
	@echo "*********************************"
	qemu-system-$(ARCH) -nographic $(QEMUOPTS)

qemu-boot: bootable qemu
qemu-nox-boot: bootable qemu-nox

clean:
	rm -rf ./EFI NvVars $(OBJS) $(TARGET) cpuinfo.txt
