# CPUInfo - A dead simple CPUID utility for UEFI Shell

## Build & Run

* Install dependencies `gnu-efi, gcc, make, qemu-system-x86, ovmf`
  
  On Ubuntu, use `sudo apt install build-essentials qemu-system-x86 ovmf`

* Run `make` or `make bootable`(x86_64 only) to build
  
* Run efi shell in qemu using `make qemu` or `make qemu-nox`, or boot to app directly using `make qemu-boot` or `make qemu-nox-run`(also x86_64 only)
  
* That's it!

## Hints

* To change cpu model in qemu, add `-cpu [model]` to `QEMUOPTS` in Makefile. To get a cpu model list, use `man qemu-cpu-models`.

* To use host's cpu, add `-enable-kvm -cpu host` to `QEMUOPTS` in Makefile. You need to enable VT-x or AMD-V in your motherboard settings.

* To exit qemu-nox, use Ctrl-a x
