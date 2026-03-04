/*
 * FMI 2.0 Test Harness for HelloWorld FMU
 * 
 * This test program dynamically loads the compiled FMU library and exercises
 * the FMI 2.0 lifecycle using the variable references defined in modelDescription.xml
 * 
 * Compile:
 *   Linux: gcc -o test_fmu test_fmu.c -ldl -lm
 *   macOS: clang -o test_fmu test_fmu.c -ldl -lm
 * 
 * Run:
 *   Linux/macOS: ./test_fmu ../build/binaries/libHelloWorld.so
 *   Windows: test_fmu.exe ../build/binaries/HelloWorld.dll
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
  #include <windows.h>
  #define LOAD_LIBRARY(path) LoadLibraryA(path)
  #define GET_FUNCTION(lib, name) GetProcAddress((HMODULE)lib, name)
  #define CLOSE_LIBRARY(lib) FreeLibrary((HMODULE)lib)
#else
  #include <dlfcn.h>
  #define LOAD_LIBRARY(path) dlopen(path, RTLD_LAZY)
  #define GET_FUNCTION(lib, name) dlsym(lib, name)
  #define CLOSE_LIBRARY(lib) dlclose(lib)
#endif

/* ============================================================================
   FMI 2.0 Type Definitions
   ============================================================================ */

typedef unsigned int fmiValueReference;
typedef double fmiReal;
typedef int fmiInteger;
typedef const char* fmiString;
typedef char fmiBoolean;
typedef void* fmiComponent;
typedef void* fmiComponentEnvironment;

#define fmiFalse 0
#define fmiTrue 1

typedef enum {
  fmiOK,
  fmiWarning,
  fmiDiscard,
  fmiError,
  fmiFatal,
  fmiPending
} fmiStatus;

/* ============================================================================
   FMI 2.0 Function Pointers
   ============================================================================ */

typedef fmiComponent (*pfn_fmiInstantiate)(
    const char* instanceName,
    int fmuType,
    const char* fmuGUID,
    const char* fmuResourceLocation,
    fmiComponentEnvironment functions,
    fmiBoolean visible,
    fmiBoolean loggingOn);

typedef fmiStatus (*pfn_fmiSetupExperiment)(
    fmiComponent c,
    fmiBoolean toleranceDefined,
    fmiReal tolerance,
    fmiReal startTime,
    fmiBoolean stopTimeDefined,
    fmiReal stopTime);

typedef fmiStatus (*pfn_fmiEnterInitializationMode)(fmiComponent c);
typedef fmiStatus (*pfn_fmiExitInitializationMode)(fmiComponent c);

typedef fmiStatus (*pfn_fmiDoStep)(
    fmiComponent c,
    fmiReal currentCommunicationPoint,
    fmiReal communicationStepSize,
    fmiBoolean noSetFMUStatePriorToCurrentPoint);

typedef fmiStatus (*pfn_fmiGetReal)(
    fmiComponent c,
    const fmiValueReference vr[],
    size_t nvr,
    fmiReal value[]);

typedef fmiStatus (*pfn_fmiGetInteger)(
    fmiComponent c,
    const fmiValueReference vr[],
    size_t nvr,
    fmiInteger value[]);

typedef fmiStatus (*pfn_fmiGetString)(
    fmiComponent c,
    const fmiValueReference vr[],
    size_t nvr,
    fmiString value[]);

typedef fmiStatus (*pfn_fmiTerminate)(fmiComponent c);
typedef fmiStatus (*pfn_fmiFreeInstance)(fmiComponent c);

typedef fmiStatus (*pfn_fmiReset)(fmiComponent c);

/* ============================================================================
   FMI Function Container
   ============================================================================ */

typedef struct {
  pfn_fmiInstantiate fmiInstantiate;
  pfn_fmiSetupExperiment fmiSetupExperiment;
  pfn_fmiEnterInitializationMode fmiEnterInitializationMode;
  pfn_fmiExitInitializationMode fmiExitInitializationMode;
  pfn_fmiDoStep fmiDoStep;
  pfn_fmiGetReal fmiGetReal;
  pfn_fmiGetInteger fmiGetInteger;
  pfn_fmiGetString fmiGetString;
  pfn_fmiTerminate fmiTerminate;
  pfn_fmiFreeInstance fmiFreeInstance;
  pfn_fmiReset fmiReset;
} FmiLibrary;

/* ============================================================================
   Variable References (from modelDescription.xml)
   ============================================================================ */

#define VR_COUNTER 0   /* Integer output */
#define VR_MESSAGE 1   /* String output */
#define VR_TIME    2   /* Real output */

/* ============================================================================
   Helper Functions
   ============================================================================ */

const char* fmiStatusToString(fmiStatus status) {
  switch (status) {
    case fmiOK: return "OK";
    case fmiWarning: return "Warning";
    case fmiDiscard: return "Discard";
    case fmiError: return "Error";
    case fmiFatal: return "Fatal";
    case fmiPending: return "Pending";
    default: return "Unknown";
  }
}

int loadFmiLibrary(const char* libPath, FmiLibrary* lib) {
  void* handle = LOAD_LIBRARY(libPath);
  if (!handle) {
    fprintf(stderr, "ERROR: Failed to load FMU library: %s\n", libPath);
    return 0;
  }

  printf("Loaded FMU library: %s\n", libPath);

  /* Load all required FMI 2.0 functions */
  #define LOAD_FN(name) \
    lib->name = (pfn_##name)GET_FUNCTION(handle, #name); \
    if (!lib->name) { \
      fprintf(stderr, "ERROR: Function %s not found\n", #name); \
      return 0; \
    }

  LOAD_FN(fmiInstantiate);
  LOAD_FN(fmiSetupExperiment);
  LOAD_FN(fmiEnterInitializationMode);
  LOAD_FN(fmiExitInitializationMode);
  LOAD_FN(fmiDoStep);
  LOAD_FN(fmiGetReal);
  LOAD_FN(fmiGetInteger);
  LOAD_FN(fmiGetString);
  LOAD_FN(fmiTerminate);
  LOAD_FN(fmiFreeInstance);
  LOAD_FN(fmiReset);

  printf("All FMI 2.0 functions loaded successfully\n");
  return 1;
}

/* ============================================================================
   Test Cases
   ============================================================================ */

int testBasicLifecycle(FmiLibrary* lib) {
  printf("\n=== Test: Basic FMI Lifecycle ===\n");

  /* Instantiate */
  printf("Calling fmiInstantiate...\n");
  fmiComponent instance = lib->fmiInstantiate(
      "HelloWorld_Test",  /* instanceName */
      2,                  /* fmuType: 2 = CoSimulation */
      "{12345678-1234-1234-1234-123456789012}",  /* GUID from XML */
      "",                 /* fmuResourceLocation */
      NULL,               /* functions (logging) */
      fmiTrue,            /* visible */
      fmiTrue);           /* loggingOn */

  if (!instance) {
    fprintf(stderr, "ERROR: fmiInstantiate failed\n");
    return 0;
  }
  printf("✓ fmiInstantiate successful\n");

  /* Setup Experiment */
  printf("Calling fmiSetupExperiment...\n");
  fmiStatus status = lib->fmiSetupExperiment(
      instance,
      fmiTrue,   /* toleranceDefined */
      1e-6,      /* tolerance */
      0.0,       /* startTime */
      fmiTrue,   /* stopTimeDefined */
      10.0);     /* stopTime */

  if (status != fmiOK) {
    fprintf(stderr, "ERROR: fmiSetupExperiment returned %s\n", 
        fmiStatusToString(status));
    return 0;
  }
  printf("✓ fmiSetupExperiment successful\n");

  /* Enter Initialization Mode */
  printf("Calling fmiEnterInitializationMode...\n");
  status = lib->fmiEnterInitializationMode(instance);
  if (status != fmiOK) {
    fprintf(stderr, "ERROR: fmiEnterInitializationMode returned %s\n", 
        fmiStatusToString(status));
    return 0;
  }
  printf("✓ fmiEnterInitializationMode successful\n");

  /* Exit Initialization Mode */
  printf("Calling fmiExitInitializationMode...\n");
  status = lib->fmiExitInitializationMode(instance);
  if (status != fmiOK) {
    fprintf(stderr, "ERROR: fmiExitInitializationMode returned %s\n", 
        fmiStatusToString(status));
    return 0;
  }
  printf("✓ fmiExitInitializationMode successful\n");

  /* Terminate */
  printf("Calling fmiTerminate...\n");
  status = lib->fmiTerminate(instance);
  if (status != fmiOK) {
    fprintf(stderr, "ERROR: fmiTerminate returned %s\n", 
        fmiStatusToString(status));
    return 0;
  }
  printf("✓ fmiTerminate successful\n");

  /* Free Instance */
  printf("Calling fmiFreeInstance...\n");
  status = lib->fmiFreeInstance(instance);
  if (status != fmiOK) {
    fprintf(stderr, "ERROR: fmiFreeInstance returned %s\n", 
        fmiStatusToString(status));
    return 0;
  }
  printf("✓ fmiFreeInstance successful\n");

  return 1;
}

int testSimulationStep(FmiLibrary* lib) {
  printf("\n=== Test: Simulation Stepping ===\n");

  /* Instantiate */
  fmiComponent instance = lib->fmiInstantiate(
      "HelloWorld_Stepping",
      2, "{12345678-1234-1234-1234-123456789012}",
      "", NULL, fmiTrue, fmiTrue);

  if (!instance) {
    fprintf(stderr, "ERROR: fmiInstantiate failed\n");
    return 0;
  }

  /* Setup */
  fmiStatus status = lib->fmiSetupExperiment(instance, fmiTrue, 1e-6, 0.0, fmiTrue, 10.0);
  if (status != fmiOK) return 0;

  status = lib->fmiEnterInitializationMode(instance);
  if (status != fmiOK) return 0;

  status = lib->fmiExitInitializationMode(instance);
  if (status != fmiOK) return 0;

  /* Simulate 100 steps */
  printf("Simulating 100 steps (0.01s each)...\n");
  printf("Step | Time   | Counter | Message\n");
  printf("-----|--------|---------|------------------\n");

  for (int step = 0; step < 100; step++) {
    fmiReal currentTime = (fmiReal)step * 0.01;

    /* Do Step */
    status = lib->fmiDoStep(instance, currentTime, 0.01, fmiTrue);
    if (status != fmiOK) {
      fprintf(stderr, "ERROR: fmiDoStep %d returned %s\n", step, fmiStatusToString(status));
      return 0;
    }

    /* Read outputs every 10 steps */
    if (step % 10 == 0) {
      /* Get Counter */
      fmiValueReference vr1 = VR_COUNTER;
      fmiInteger counter = 0;
      status = lib->fmiGetInteger(instance, &vr1, 1, &counter);
      if (status != fmiOK) {
        fprintf(stderr, "ERROR: fmiGetInteger returned %s\n", fmiStatusToString(status));
        return 0;
      }

      /* Get Time */
      fmiValueReference vr2 = VR_TIME;
      fmiReal time = 0.0;
      status = lib->fmiGetReal(instance, &vr2, 1, &time);
      if (status != fmiOK) {
        fprintf(stderr, "ERROR: fmiGetReal returned %s\n", fmiStatusToString(status));
        return 0;
      }

      /* Get Message */
      fmiValueReference vr3 = VR_MESSAGE;
      fmiString message = "";
      status = lib->fmiGetString(instance, &vr3, 1, &message);
      if (status != fmiOK) {
        fprintf(stderr, "ERROR: fmiGetString returned %s\n", fmiStatusToString(status));
        return 0;
      }

      printf("%4d | %6.2f | %7d | %s\n", step, time, counter, message);
    }
  }

  /* Terminate */
  status = lib->fmiTerminate(instance);
  if (status != fmiOK) return 0;

  status = lib->fmiFreeInstance(instance);
  if (status != fmiOK) return 0;

  printf("\n✓ Simulation stepping test passed\n");
  return 1;
}

int testReset(FmiLibrary* lib) {
  printf("\n=== Test: Reset Functionality ===\n");

  fmiComponent instance = lib->fmiInstantiate(
      "HelloWorld_Reset",
      2, "{12345678-1234-1234-1234-123456789012}",
      "", NULL, fmiTrue, fmiFalse);

  if (!instance) {
    fprintf(stderr, "ERROR: fmiInstantiate failed\n");
    return 0;
  }

  fmiStatus status = lib->fmiSetupExperiment(instance, fmiTrue, 1e-6, 0.0, fmiTrue, 10.0);
  if (status != fmiOK) return 0;

  status = lib->fmiEnterInitializationMode(instance);
  if (status != fmiOK) return 0;

  status = lib->fmiExitInitializationMode(instance);
  if (status != fmiOK) return 0;

  /* Run a few steps */
  printf("Running 10 steps...\n");
  for (int i = 0; i < 10; i++) {
    status = lib->fmiDoStep(instance, i * 0.01, 0.01, fmiTrue);
    if (status != fmiOK) return 0;
  }

  /* Read counter before reset */
  fmiValueReference vr = VR_COUNTER;
  fmiInteger counterBefore = 0;
  lib->fmiGetInteger(instance, &vr, 1, &counterBefore);
  printf("Counter before reset: %d\n", counterBefore);

  /* Reset */
  printf("Calling fmiReset...\n");
  status = lib->fmiReset(instance);
  if (status != fmiOK) {
    fprintf(stderr, "ERROR: fmiReset returned %s\n", fmiStatusToString(status));
    return 0;
  }

  /* Re-initialize after reset */
  status = lib->fmiEnterInitializationMode(instance);
  if (status != fmiOK) return 0;

  status = lib->fmiExitInitializationMode(instance);
  if (status != fmiOK) return 0;

  /* Read counter after reset */
  fmiInteger counterAfter = 0;
  lib->fmiGetInteger(instance, &vr, 1, &counterAfter);
  printf("Counter after reset: %d\n", counterAfter);

  if (counterAfter == 0) {
    printf("✓ Reset test passed (counter reset to 0)\n");
  } else {
    printf("✗ Reset test failed (counter should be 0, got %d)\n", counterAfter);
    return 0;
  }

  lib->fmiTerminate(instance);
  lib->fmiFreeInstance(instance);

  return 1;
}

/* ============================================================================
   Main Test Runner
   ============================================================================ */

int main(int argc, char* argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <path-to-fmu-library>\n", argv[0]);
    fprintf(stderr, "Example: %s ../build/binaries/libHelloWorld.so\n", argv[0]);
    return 1;
  }

  printf("========================================\n");
  printf("FMI 2.0 Test Harness for HelloWorld FMU\n");
  printf("========================================\n");

  FmiLibrary lib = {0};

  if (!loadFmiLibrary(argv[1], &lib)) {
    fprintf(stderr, "Failed to load FMU library\n");
    return 1;
  }

  int allPassed = 1;

  /* Run tests */
  if (!testBasicLifecycle(&lib)) {
    fprintf(stderr, "✗ Basic lifecycle test FAILED\n");
    allPassed = 0;
  }

  if (!testSimulationStep(&lib)) {
    fprintf(stderr, "✗ Simulation step test FAILED\n");
    allPassed = 0;
  }

  if (!testReset(&lib)) {
    fprintf(stderr, "✗ Reset test FAILED\n");
    allPassed = 0;
  }

  printf("\n========================================\n");
  if (allPassed) {
    printf("✓ ALL TESTS PASSED\n");
  } else {
    printf("✗ SOME TESTS FAILED\n");
  }
  printf("========================================\n");

  return allPassed ? 0 : 1;
}
