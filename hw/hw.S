.code16             # Set 16-bit code

.global _start       # Default entry point for the linker

_start:
    mov $0xe, %ah   # Set BIOS teletype mode

    mov $'H', %al   # Load 'H' ascii code
    int $0x10        # Issue BIOS interrupt

    mov $'e', %al   # Load 'e' ascii code
    int $0x10        # Issue BIOS interrupt

    mov $'l', %al   # Load 'l' ascii code
    int $0x10        # Issue BIOS interrupt

    mov $'l', %al   # Load 'l' ascii code
    int $0x10        # Issue BIOS interrupt

    mov $'o', %al   # Load 'o' ascii code
    int $0x10        # Issue BIOS interrupt

    mov $' ', %al   # Load ' ' ascii code
    int $0x10        # Issue BIOS interrupt

    mov $'W', %al   # Load 'W' ascii code
    int $0x10        # Issue BIOS interrupt

    mov $'o', %al   # Load 'o' ascii code
    int $0x10        # Issue BIOS interrupt

    mov $'r', %al   # Load 'r' ascii code
    int $0x10        # Issue BIOS interrupt

    mov $'l', %al   # Load 'l' ascii code
    int $0x10        # Issue BIOS interrupt

    mov $'d', %al   # Load 'd' ascii code
    int $0x10        # Issue BIOS interrupt

halt:
    hlt              # Halt the machine
    jmp halt         # Safeguard

    .fill 510 - (.-_start), 1, 0
    .word 0xaa55
