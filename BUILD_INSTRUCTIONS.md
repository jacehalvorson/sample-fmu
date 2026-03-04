# Building HelloWorld FMU for Linux and NXP i.MX95

## Prerequisites

### For Linux (x86_64, ARM)
```bash
# Ubuntu/Debian
sudo apt-get install build-essential cmake git

# For ARM cross-compilation to i.MX95
sudo apt-get install gcc-arm-linux-gnueabihf  # For 32-bit ARM
# OR
sudo apt-get install gcc-aarch64-linux-gnu    # For 64-bit ARM (aarch64)
```

### For NXP i.MX95 RTOS
- NXP MCUXpresso IDE (includes toolchain) or
- Yocto build system with appropriate SDK
- Custom toolchain with arm-linux-gnueabihf or aarch64-linux-gnu compiler

## Building for Linux (Native)

### On Linux x86_64
```bash
cd c:\Users\tankt\projects\fmu
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

Output: `build/binaries/libHelloWorld.so`

### On Linux ARM (32-bit)
```bash
cd c:\Users\tankt\projects\fmu
mkdir build
cd build
cmake ..
cmake --build .
```

## Building for NXP i.MX95 (Cross-Compilation)

The project includes a pre-configured toolchain file for ARM platforms.

### Using the Provided Toolchain

1. **Edit the toolchain file** for your specific setup:
   ```bash
   # Open toolchain-arm-imx95.cmake and:
   # - Update CMAKE_C_COMPILER path
   # - Set correct sysroot path if needed
   # - Configure FREERTOS_DIR or QNX_BASE if using RTOS
   ```

2. **Build with cross-toolchain**:
   ```bash
   cd c:\Users\tankt\projects\fmu
   mkdir build-arm
   cd build-arm
   cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain-arm-imx95.cmake ..
   cmake --build .
   ```

   Output: `build-arm/binaries/libHelloWorld.so`

### Custom Toolchain Configuration

If using a different toolchain:

```bash
cmake -DCMAKE_TOOLCHAIN_FILE=/path/to/your/toolchain.cmake \
      -DCMAKE_SYSTEM_PROCESSOR=aarch64 \
      -DCMAKE_C_COMPILER=aarch64-linux-gnu-gcc \
      ..
cmake --build .
```

## Building with FreeRTOS Support

If targeting FreeRTOS on NXP i.MX95:

```bash
cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain-arm-imx95.cmake \
      -DFREERTOS_DIR=/path/to/freertos \
      ..
cmake --build .
```

## Building with QNX Support

If targeting QNX on NXP i.MX95:

```bash
cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain-arm-imx95.cmake \
      -DQNX_BASE=/opt/qnx \
      ..
cmake --build .
```

## Verifying the Build

### Check Binary Architecture (Linux)
```bash
# After building
file build/binaries/libHelloWorld.so
```

Expected output for different platforms:
- **Linux x86_64**: `ELF 64-bit LSB shared object, x86-64`
- **Linux ARM 32-bit**: `ELF 32-bit LSB shared object, ARM`
- **Linux ARM 64-bit**: `ELF 64-bit LSB shared object, ARM aarch64`

## Testing the FMU

### Linux Native Test
```bash
# Copy the FMU to a test directory
cp build/binaries/libHelloWorld.so ~/fmu-test/

# Use with your FMI simulator (e.g., with Python, OpenModelica, etc.)
```

### Deploying to i.MX95 Target

1. **Transfer the compiled FMU**:
   ```bash
   scp build-arm/binaries/libHelloWorld.so user@imx95-target:/home/user/
   ```

2. **On the target device**:
   ```bash
   # Verify binary
   file libHelloWorld.so
   
   # Run with your FMI co-simulation framework
   ```

## Advanced: Building Documentation

The code includes platform detection macros:

| Macro | Meaning | Platform |
|-------|---------|----------|
| `FMU_PLATFORM_LINUX` | Linux platform | Linux (any arch) |
| `FMU_PLATFORM_WINDOWS` | Windows platform | Windows |
| `FMU_PLATFORM_EMBEDDED` | Embedded system | ARM/RTOS |
| `FMU_RTOS_FREERTOS` | FreeRTOS present | With FreeRTOS |
| `FMU_RTOS_QNX` | QNX present | With QNX |

## Troubleshooting

### "Compiler not found"
```bash
# Install cross-compiler
sudo apt-get install gcc-arm-linux-gnueabihf

# Or verify path
which arm-linux-gnueabihf-gcc
```

### "CMake: No suitable compiler found"
```bash
# Force compiler in toolchain
export CC=arm-linux-gnueabihf-gcc
export CXX=arm-linux-gnueabihf-g++
cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain-arm-imx95.cmake ..
```

### "Invalid shared object" on target
- Verify binary architecture matches target
- Check glibc/libc compatibility
- Ensure all dependencies are available on target

## Next Steps

1. Integrate with NXP MCUXpresso environment
2. Add FreeRTOS task integration
3. Implement real-time constraints
4. Add target-specific I/O integration
