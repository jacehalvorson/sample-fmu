/* 
 * Simple FMU 2.0.4 Hello World Example
 * This is a minimal CoSimulation FMU that demonstrates basic FMI 2.0 functionality
 * 
 * Portable for:
 * - Linux (x86_64, ARM)
 * - RTOS on NXP i.MX95 (FreeRTOS, QNX, etc.)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Platform/OS detection */
#if defined(__linux__) || defined(linux) || defined(__linux)
  #define FMU_PLATFORM_LINUX 1
#elif defined(_WIN32) || defined(_WIN64) || defined(__WINDOWS__)
  #define FMU_PLATFORM_WINDOWS 1
#elif defined(__ARMCC_VERSION) || defined(__GNUC__)
  #define FMU_PLATFORM_EMBEDDED 1
#endif

/* RTOS detection */
#if defined(FREERTOS_KERNEL_VERSION_MAJOR) || defined(configUSE_PREEMPTION)
  #define FMU_RTOS_FREERTOS 1
#elif defined(__QNX__)
  #define FMU_RTOS_QNX 1
#endif

/* FMI 2.0 Common types */
typedef unsigned int fmiValueReference;
typedef double fmiReal;
typedef int fmiInteger;
typedef const char* fmiString;
typedef char fmiBoolean;
typedef void* fmiComponent;
typedef void* fmiComponentEnvironment;

#define fmiFalse 0
#define fmiTrue 1

/* FMI 2.0 Status codes */
typedef enum {
  fmiOK,
  fmiWarning,
  fmiDiscard,
  fmiError,
  fmiFatal,
  fmiPending
} fmiStatus;

/* Instance structure */
#define MAX_INSTANCES 4
#define MESSAGE_LENGTH 64

typedef struct {
  fmiReal currentTime;
  fmiInteger counter;
  char message[MESSAGE_LENGTH];
  fmiBoolean loggingOn;
  fmiBoolean inUse;
} HelloWorldData;

/* Static instance pool for RTOS environments without heap */
static HelloWorldData instancePool[MAX_INSTANCES] = {{0}};

/* Initialize the instance pool */
static void initializeInstancePool(void) {
  static int poolInitialized = 0;
  if (!poolInitialized) {
    memset(instancePool, 0, sizeof(instancePool));
    poolInitialized = 1;
  }
}

/* Logging utility */
static void logMessage(fmiComponentEnvironment env, const char* message) {
  /* Basic logging - platform independent */
  #ifdef FMU_RTOS_FREERTOS
    /* FreeRTOS: could use xTaskGetTickCount() for timestamp */
    printf("[HelloWorld RTOS] %s\n", message);
  #elif FMU_RTOS_QNX
    /* QNX specific logging could go here */
    printf("[HelloWorld QNX] %s\n", message);
  #else
    /* Linux/generic platforms */
    printf("[HelloWorld] %s\n", message);
  #endif
}

/* FMI 2.0 API Implementation */

/* Instantiate FMU */
fmiComponent fmiInstantiate(
    const char* instanceName,
    int fmuType,
    const char* fmuGUID,
    const char* fmuResourceLocation,
    fmiComponentEnvironment functions,
    fmiBoolean visible,
    fmiBoolean loggingOn) {
  
  HelloWorldData* data = NULL;
  int i;
  
  /* Initialize pool on first call */
  initializeInstancePool();
  
  /* Try to use static pool first (RTOS-friendly) */
  for (i = 0; i < MAX_INSTANCES; i++) {
    if (!instancePool[i].inUse) {
      data = &instancePool[i];
      data->inUse = fmiTrue;
      break;
    }
  }
  
  /* Fallback to heap allocation if no pool space (Linux/desktop) */
  if (data == NULL) {
    #ifdef FMU_PLATFORM_LINUX
      data = (HelloWorldData*)malloc(sizeof(HelloWorldData));
      if (!data) {
        logMessage(functions, "Failed to allocate memory");
        return NULL;
      }
    #else
      logMessage(functions, "No free instances in static pool");
      return NULL;
    #endif
  }
  
  /* Initialize data */
  data->currentTime = 0.0;
  data->counter = 0;
  strncpy(data->message, "Hello World!", MESSAGE_LENGTH - 1);
  data->message[MESSAGE_LENGTH - 1] = '\0';
  data->loggingOn = loggingOn;
  
  if (loggingOn) {
    logMessage(functions, "FMU instantiated");
  }
  
  return (fmiComponent)data;
}

/* Initialize FMU */
fmiStatus fmiSetupExperiment(
    fmiComponent c,
    fmiBoolean toleranceDefined,
    fmiReal tolerance,
    fmiReal startTime,
    fmiBoolean stopTimeDefined,
    fmiReal stopTime) {
  
  HelloWorldData* data = (HelloWorldData*)c;
  data->currentTime = startTime;
  data->counter = 0;
  
  printf("FMU Setup: startTime=%.2f, stopTime=%.2f\n", startTime, stopTime);
  
  return fmiOK;
}

/* Enter initialization mode */
fmiStatus fmiEnterInitializationMode(fmiComponent c) {
  printf("FMU entering initialization mode\n");
  return fmiOK;
}

/* Exit initialization mode */
fmiStatus fmiExitInitializationMode(fmiComponent c) {
  printf("FMU exiting initialization mode\n");
  return fmiOK;
}

/* Perform simulation step */
fmiStatus fmiDoStep(
    fmiComponent c,
    fmiReal currentCommunicationPoint,
    fmiReal communicationStepSize,
    fmiBoolean noSetFMUStatePriorToCurrentPoint) {
  
  HelloWorldData* data = (HelloWorldData*)c;
  
  data->currentTime = currentCommunicationPoint + communicationStepSize;
  data->counter++;
  
  if (data->counter % 10 == 0) {
    printf("HelloWorld FMU Step: time=%.2f, counter=%d\n", 
           data->currentTime, data->counter);
  }
  
  return fmiOK;
}

/* Get real output value */
fmiStatus fmiGetReal(
    fmiComponent c,
    const fmiValueReference vr[],
    size_t nvr,
    fmiReal value[]) {
  
  HelloWorldData* data = (HelloWorldData*)c;
  
  for (int i = 0; i < nvr; i++) {
    if (vr[i] == 2) {  /* time */
      value[i] = data->currentTime;
    } else {
      return fmiError;
    }
  }
  
  return fmiOK;
}

/* Get integer output value */
fmiStatus fmiGetInteger(
    fmiComponent c,
    const fmiValueReference vr[],
    size_t nvr,
    fmiInteger value[]) {
  
  HelloWorldData* data = (HelloWorldData*)c;
  
  for (int i = 0; i < nvr; i++) {
    if (vr[i] == 0) {  /* counter */
      value[i] = data->counter;
    } else {
      return fmiError;
    }
  }
  
  return fmiOK;
}

/* Get string output value */
fmiStatus fmiGetString(
    fmiComponent c,
    const fmiValueReference vr[],
    size_t nvr,
    fmiString value[]) {
  
  HelloWorldData* data = (HelloWorldData*)c;
  
  for (int i = 0; i < nvr; i++) {
    if (vr[i] == 1) {  /* message */
      value[i] = (fmiString)data->message;
    } else {
      return fmiError;
    }
  }
  
  return fmiOK;
}

/* Terminate FMU */
fmiStatus fmiTerminate(fmiComponent c) {
  HelloWorldData* data = (HelloWorldData*)c;
  printf("FMU terminating. Final counter: %d\n", data->counter);
  return fmiOK;
}

/* Free FMU */
fmiStatus fmiFreeInstance(fmiComponent c) {
  if (c != NULL) {
    HelloWorldData* data = (HelloWorldData*)c;
    
    /* Check if this instance is from the static pool */
    if (data >= &instancePool[0] && data < &instancePool[MAX_INSTANCES]) {
      /* Instance from pool - just mark as unused */
      data->inUse = fmiFalse;
    } else {
      /* Heap-allocated instance (Linux) */
      #ifdef FMU_PLATFORM_LINUX
        free(c);
      #endif
    }
  }
  return fmiOK;
}

/* Reset FMU */
fmiStatus fmiReset(fmiComponent c) {
  HelloWorldData* data = (HelloWorldData*)c;
  data->currentTime = 0.0;
  data->counter = 0;
  return fmiOK;
}
