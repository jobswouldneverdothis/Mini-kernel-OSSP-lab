# MiniOS - Educational Operating System Kernel

A lightweight operating system kernel implementation designed for learning OS concepts and low-level system programming.

## Features

### Core Components
- **Boot Loader**: Custom bootloader for system initialization
- **VGA Driver**: Text-mode display driver for console output
- **Interrupt Handling**: IDT setup and interrupt service routines
- **Keyboard Driver**: PS/2 keyboard input handling
- **Memory Management**:
  - Physical memory allocator
  - Virtual memory with paging support
- **Process Management**:
  - Kernel-level process abstraction
  - Context switching

### Process Scheduling Algorithms
- **FCFS (First-Come, First-Served)**: Simple queue-based scheduling
- **Round Robin**: Time-sliced fair scheduling with configurable quantum

## System Requirements

- **OS**: Linux (Ubuntu/Debian recommended)
- **Compiler**: GCC (GNU Compiler Collection)
- **Assembler**: NASM (Netwide Assembler)
- **Emulator**: QEMU x86 emulator
- **Debugger** (optional): GDB

## Installation

Install dependencies:
```bash
sudo apt-get update
sudo apt-get install build-essential nasm gdb qemu-system-x86
```

Create mount point for floppy operations:
```bash
sudo mkdir -p /mnt/floppy
```

## Building

Compile the kernel:
```bash
make
```

Clean build artifacts:
```bash
make clean
```

## Running

Launch in QEMU emulator:
```bash
make qemu
```

For debugging with GDB:
```bash
make qemu-gdb
# In another terminal:
gdb
```

## Project Structure

```
.
├── boot/          # Bootloader code
├── kernel/        # Kernel core implementation
├── drivers/       # Device drivers (VGA, keyboard)
├── mm/            # Memory management
├── proc/          # Process and scheduling
├── include/       # Header files
└── Makefile       # Build configuration
```

## Technical Details

- **Architecture**: x86 (32-bit)
- **Boot Protocol**: Multiboot-compatible
- **Memory Model**: Higher-half kernel
- **Scheduling Quantum**: 10ms (configurable)

## Learning Resources

This project incorporates concepts from various OS development tutorials and academic resources in operating systems design.

## License

Educational use only. See LICENSE file for details.

## Contributing

This is an educational project. Suggestions and improvements are welcome through issues and pull requests.

## Acknowledgments

Built using standard OS development practices and inspired by various educational OS projects in the open-source community.