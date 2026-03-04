# Test Harness Quick Start

This directory contains a complete FMI 2.0 test harness for the HelloWorld FMU.

## What It Tests

The `test_fmu.c` file implements three test suites:

1. **Basic Lifecycle Test** - Exercises the standard FMI 2.0 lifecycle:
   - `fmiInstantiate()` - Create FMU instance
   - `fmiSetupExperiment()` - Configure experiment
   - `fmiEnterInitializationMode()` - Initialize
   - `fmiExitInitializationMode()` - Start simulation
   - `fmiTerminate()` - End simulation
   - `fmiFreeInstance()` - Clean up

2. **Simulation Stepping Test** - Runs 100 simulation steps and reads outputs:
   - Reads `counter` (Integer, vr=0)
   - Reads `time` (Real, vr=2)
   - Reads `message` (String, vr=1)

3. **Reset Test** - Verifies the FMU can reset state:
   - Steps the simulation
   - Calls `fmiReset()`
   - Verifies counter returns to 0

## Building and Running

### Using CMake (Recommended)

```bash
# From project root
mkdir build && cd build
cmake ..
cmake --build .

# Then build and run tests
cd ../test/build
cmake ..
cmake --build .
../test_fmu ../../build/binaries/libHelloWorld.so
```

### Using Make (Linux/macOS)

```bash
# From test directory
make               # Build test executable
make test          # Build FMU + test harness + run tests
make clean         # Clean up
```

### Manual Compilation (Linux)

```bash
gcc -o test_fmu test_fmu.c -ldl -lm
./test_fmu ../build/binaries/libHelloWorld.so
```

### Using the Shell Script

```bash
chmod +x run_tests.sh
./run_tests.sh
```

## Output Example

```
=========================================
FMI 2.0 Test Harness for HelloWorld FMU
=========================================
Loaded FMU library: ../build/binaries/libHelloWorld.so
All FMI 2.0 functions loaded successfully

=== Test: Basic FMI Lifecycle ===
Calling fmiInstantiate...
✓ fmiInstantiate successful
Calling fmiSetupExperiment...
✓ fmiSetupExperiment successful
...

=== Test: Simulation Stepping ===
Simulating 100 steps (0.01s each)...
Step | Time   | Counter | Message
-----|--------|---------|------------------
   0 |   0.00 |       0 | Hello World!
  10 |   0.10 |      10 | Hello World!
  20 |   0.20 |      20 | Hello World!
...

========================================
✓ ALL TESTS PASSED
========================================
```

## How It Works

1. **Dynamic Loading**: Uses `dlopen()` to load the compiled FMU `.so` at runtime
2. **Function Binding**: Maps function names to pointers (cross-platform)
3. **Variable References**: Uses the vr (value references) from modelDescription.xml:
   - `VR_COUNTER = 0` (Integer)
   - `VR_MESSAGE = 1` (String)
   - `VR_TIME = 2` (Real)
4. **FMI 2.0 Compliance**: Follows the standard FMI 2.0 specification exactly

## Extending the Tests

To add more tests:

1. Write a new test function following the pattern:
```c
int testMyFeature(FmiLibrary* lib) {
  printf("\n=== Test: My Feature ===\n");
  
  // Instantiate, setup, run tests...
  
  printf("✓ My feature test passed\n");
  return 1;  // or 0 for failure
}
```

2. Call it from `main()`:
```c
if (!testMyFeature(&lib)) {
  fprintf(stderr, "✗ My feature test FAILED\n");
  allPassed = 0;
}
```

## Troubleshooting

- **"Function not found"** - Ensure FMU was compiled correctly and has all FMI 2.0 functions
- **"Failed to load library"** - Check library path and file exists
- **Crashes during test** - Check FMU instance wasn't freed before use

## See Also

- [modelDescription.xml](../modelDescription.xml) - FMU variable definitions
- [sources/fmu.c](../sources/fmu.c) - FMU implementation
- [FMI 2.0 Specification](https://fmi-standard.org/)
