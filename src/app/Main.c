#include <string.h>
#include <stdio.h>

#include "fmi2Functions.h"

void logMessage(fmi2ComponentEnvironment componentEnvironment, fmi2String instanceName, fmi2Status status,
                fmi2String category, fmi2String message, ...)
{
    printf("%s: %d: %s %s\n", instanceName, status, message, category);
}

int main(int argc, char* argv[])
{
    // Do some simple command line arguments parsing to get the value of --resourcesUri. Expecting something like:
    // --resourcesUri file:/home/jorn/projects/fmi-udp-adapter/src/lib/resources
    // Note that this directory doesn't need to exist, the important part is that the modelDescription.xml file is one
    // step up. And yes, it's a folder (as specified by the FMI standard), not a file.
    // In Qt Creator you can add this to Projects > Run Settings > Command line arguments

    int isResourcesUriSet = 0;
    char* resourcesUri = "";

    for (int i = 0; i < argc; ++i) {
        const char* argument = argv[i];
        if (strcmp(argument, "--resourcesUri") == 0) {
            isResourcesUriSet = 1;
            if (i + 1 < argc) {
                resourcesUri = argv[i + 1];
            }
        }
    }

    if (isResourcesUriSet == 0) {
        printf("Missing --resourcesUri command line option\n");
    }

    if (strcmp(resourcesUri, "") == 0) {
        printf("Missing value for --resourcesUri command line option\n");
    }

    printf("Hello from C Main\n");


    fmi2String typesPlatform = fmi2GetTypesPlatform();
    printf("C Main. typesPlatform: %s\n", typesPlatform);

    fmi2String version = fmi2GetVersion();
    printf("C Main. version: %s\n", version);

    fmi2CallbackFunctions callbackFunctions = {
        .logger = logMessage,
        .allocateMemory = NULL,
        .stepFinished = NULL,
        .componentEnvironment = NULL
    };

    fmi2Component component = fmi2Instantiate("FmiUdpAdapter", fmi2CoSimulation, "6dfd9809-9825-464f-a8cd-1aeb39f93d85",
        resourcesUri, &callbackFunctions, fmi2True, fmi2True);

    printf("C Main. A component have been returned\n");

    fmi2ValueReference valueReferences[] = {0, 1};
    fmi2Real desiredValues[] = {1, 2};

    fmi2SetReal(component, valueReferences, 2, desiredValues);

    fmi2Real reportedValues[2];
    fmi2GetReal(component, valueReferences, 2, reportedValues);

    for (int i = 0; i < 2; ++i) {
        printf("C Main. value: %f\n", reportedValues[i]);
    }

    fmi2DoStep(component, 0.0, 1.0, fmi2False);

    fmi2GetReal(component, valueReferences, 2, reportedValues);

    for (int i = 0; i < 2; ++i) {
        printf("C Main. value: %f\n", reportedValues[i]);
    }

    fmi2FreeInstance(component);

    return 0;
}
