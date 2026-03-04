# FMU 2.0.4 Hello World Example

A minimal Functional Mock-up Unit (FMU) 2.0.4 starter project demonstrating CoSimulation.

**Platforms Supported:**
- ✅ Linux (x86_64, ARM 32-bit, ARM 64-bit)
- ✅ NXP i.MX95 (with FreeRTOS, QNX, or Linux)
- ✅ Windows (MSVC)
- ✅ macOS

## Project Structure

```
├── modelDescription.xml       # FMU metadata and variable definitions
├── sources/
│   └── fmu.c                 # C implementation of FMI 2.0 (portable, RTOS-friendly)
├── binaries/                 # Compiled FMU binaries (platform-specific)
├── CMakeLists.txt            # CMake build configuration
├── toolchain-arm-imx95.cmake # ARM cross-compilation toolchain
├── BUILD_INSTRUCTIONS.md     # Detailed build guide for all platforms
└── README.md                 # This file
```

## What This FMU Does

This simple example implements a basic counter that:
- Increments a counter on each simulation step
- Returns the current simulation time
- Outputs a "Hello World!" message

### Model Variables

- **counter** (Integer Output): Increments by 1 on each step
- **message** (String Output): Returns "Hello World!"
- **time** (Real Output): Current simulation time

## Building the FMU

**See [BUILD_INSTRUCTIONS.md](BUILD_INSTRUCTIONS.md) for detailed platform-specific build guides.**

### Quick Start - Linux Native Build

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

Output: `build/binaries/libHelloWorld.so`

###Portable RTOS-Friendly Implementation

This FMU is optimized for embedded and real-time systems:

- **Static Memory Pool**: Uses static allocation instead of heap (configurable)
- **platform Detection**: Automatically detects Linux, Windows, embedded, RTOS
- **RTOS Support**: Compatible with FreeRTOS, QNX, bare-metal ARM
- **Cross-Compilation**: Pre-configured for ARM/NXP i.MX95 targets
- **No Dependencies**: Pure C with standard library only
mkdir build-arm && cd build-arm
cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain-arm-imx95.cmake ..
cmake --build .
```

Output: `build-arm/binaries/libHelloWorld.so` (ARM binary)

## FMI 2.0 Implementation

This FMU implements the Co-Simulation interface with the following entry points:

- `fmiInstantiate()` - Create FMU instance
- `fmiSetupExperiment()` - Configure experiment
- `fmiEnterInitializationMode()` - Enter initialization
- `fmiExitInitializationMode()` - Exit initialization
- `fmiDoStep()` - Perform simulation step
- `fmiGetReal()` - Get real-valued outputs
- `fmiGetInteger()` - Get integer-valued outputs
- `fmiGetString()` - Get string-valued outputs
- `fmiTerminate()` - End simulation
- `fmiFreeInstance()` - Clean up

## Quick Start

1. **Build** the FMU using the commands above
2. **Load** the FMU in your FMI simulation tool (e.g., Simulink, OpenModelica, Dymola)
3. **Simulate** with appropriate step sizes and time horizons
4. **Observe** the counter incrementing and outputs changing

## Next Steps
Platform Architecture

| Platform | Compiler | Binary | Support |
|----------|----------|--------|---------|
| Linux x86_64 | gcc | .so | ✅ Full |
| Linux ARM 32-bit | arm-linux-gnueabihf-gcc | .so | ✅ Full |
| Linux ARM 64-bit | aarch64-linux-gnu-gcc | .so | ✅ Full |
| NXP i.MX95 + Linux | Cross-compiler | .so | ✅ Full |
| NXP i.MX95 + FreeRTOS | arm-none-eabi-gcc | .so | ✅ Supported |
| NXP i.MX95 + QNX | QNX toolchain | .so | ✅ Supported |
| Windows | MSVC | .dll | ✅ Full |
| macOS | clang | .dylib | ✅ Full |

## References

- [FMI Standard](https://fmi-standard.org/)
- [FMI 2.0 Specification](https://fmi-standard.org/downloads/)
- [NXP i.MX95](https://www.nxp.com/products/processors-and-microcontrollers/arm-processors/i-mx-applications-processors/imx-9-series/i-mx-95-applications-processor-family:IMX95)
- [FreeRTOS](https://www.freertos.org/)
- [QNX Neutrino RTOS](https://www.qnx.com/products/neutrino-rtos/neutrino
- Package as proper FMU archive (.zip with proper structure)

## References

- [FMI Standard](https://fmi-standard.org/)
- [FMI 2.0 Specification](https://fmi-standard.org/downloads/)
