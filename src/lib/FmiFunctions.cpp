#include <QDebug>
#include <QDir>
#include <QUrl>
#include <QUuid>
#include <string>

#include "fmi2Functions.h"

#include <Component.h>
#include <iostream>

// This is the implementation of the functions defined in the FMI 2.0 API header file fmi2Functions.h.

fmi2String fmi2GetTypesPlatform()
{
    return fmi2TypesPlatform;
}

fmi2String fmi2GetVersion()
{
    return fmi2Version;
}

fmi2Status fmi2SetDebugLogging(fmi2Component component, fmi2Boolean loggingOn, size_t categoriesSize,
    const fmi2String categories[])
{
    Q_UNUSED(component)
    Q_UNUSED(loggingOn)
    Q_UNUSED(categoriesSize)
    Q_UNUSED(categories)

    return fmi2OK;
}

fmi2Component fmi2Instantiate(fmi2String instanceName, fmi2Type type, fmi2String uuid, fmi2String resourcesUri,
    const fmi2CallbackFunctions* callbackFunctions, fmi2Boolean visible, fmi2Boolean loggingOn)
{
    Q_UNUSED(visible)
    Q_UNUSED(loggingOn)

    // Only support for co-simulation.
    if (type != fmi2CoSimulation) {
        return NULL;
    }

    // The FMI standard is refering to IETF RFC3986, and says the string fmuResourceLocation (resourcesUri) should be a
    // folder, which is not a file, and shows the following examples:
    // "file:///C:/temp/MyFMU/resources" and "file:/C:/temp/MyFMU/resources".

    // https://tools.ietf.org/html/rfc8089#appendix-E.2
    // E.2.  DOS and Windows Drive Letters
    // Lists the following "file:c:/path/to/file" and "file:///c:/path/to/file"
    // So according to this the second variant in the FMI standard is incorrect, and it will not work here.
    QDir resourcesDirectory = QDir(QUrl(resourcesUri).toLocalFile());

    return new Component(instanceName, type, QUuid(uuid), resourcesDirectory, callbackFunctions);
}

void fmi2FreeInstance(fmi2Component component)
{
    if (!component) {
        return;
    }

    qDebug() << "About to delete component";
    delete component;
}

fmi2Status fmi2SetupExperiment(fmi2Component component, fmi2Boolean toleranceDefined, fmi2Real tolerance,
    fmi2Real startTime, fmi2Boolean stopTimeDefined, fmi2Real stopTime)
{
    Q_UNUSED(component)
    Q_UNUSED(toleranceDefined)
    Q_UNUSED(tolerance)
    Q_UNUSED(startTime)
    Q_UNUSED(stopTimeDefined)
    Q_UNUSED(stopTime)

    return fmi2OK;
}

fmi2Status fmi2EnterInitializationMode(fmi2Component component)
{
    Q_UNUSED(component)

    return fmi2OK;
}

fmi2Status fmi2ExitInitializationMode(fmi2Component component)
{
    Q_UNUSED(component)

    return fmi2OK;
}

fmi2Status fmi2Terminate(fmi2Component component)
{
    Q_UNUSED(component)

    return fmi2OK;
}

fmi2Status fmi2Reset(fmi2Component component)
{
    Q_UNUSED(component)

    return fmi2OK;
}

fmi2Status fmi2GetReal(fmi2Component component, const fmi2ValueReference references[], size_t referencesSize,
    fmi2Real values[])
{
    const auto typedComponent = static_cast<Component*>(component);

    for (decltype(referencesSize) i = 0; i < referencesSize; ++i) {
        values[i] = typedComponent->real(references[i]);
    }

    return fmi2OK;
}

fmi2Status fmi2GetInteger(fmi2Component component, const fmi2ValueReference references[], size_t referencesSize,
    fmi2Integer values[])
{
    const auto typedComponent = static_cast<Component*>(component);

    for (decltype (referencesSize) i = 0; i < referencesSize; ++i) {
        values[i] = typedComponent->integer(references[i]);
    }

    return fmi2OK;
}

fmi2Status fmi2GetBoolean(fmi2Component component, const fmi2ValueReference references[], size_t referencesSize,
    fmi2Boolean values[])
{
    const auto typedComponent = static_cast<Component*>(component);

    for (decltype (referencesSize) i = 0; i < referencesSize; ++i) {
        values[i] = typedComponent->boolean(references[i]);
    }

    return fmi2OK;
}

fmi2Status fmi2GetString(fmi2Component component, const fmi2ValueReference references[], size_t referencesSize,
    fmi2String values[])
{
    const auto typedComponent = static_cast<Component*>(component);

    for (decltype (referencesSize) i = 0; i < referencesSize; ++i) {
        values[i] = typedComponent->string(references[i]);
    }

    return fmi2OK;
}

fmi2Status fmi2SetReal(fmi2Component component, const fmi2ValueReference references[], size_t referencesSize,
    const fmi2Real values[])
{
    if (!component) {
        return fmi2Warning;
    }

    const auto typedComponent = static_cast<Component*>(component);

    for (decltype(referencesSize) i = 0; i < referencesSize; ++i) {
        typedComponent->setReal(references[i], values[i]);
    }

    return fmi2OK;
}

fmi2Status fmi2SetInteger(fmi2Component component, const fmi2ValueReference references[], size_t referencesSize,
    const fmi2Integer values[])
{

    if (!component) {
        return fmi2Warning;
    }

    const auto typedComponent = static_cast<Component*>(component);

    for (decltype(referencesSize) i = 0; i < referencesSize; ++i) {
        typedComponent->setInteger(references[i], values[i]);
    }

    return fmi2OK;
}

fmi2Status fmi2SetBoolean(fmi2Component component, const fmi2ValueReference references[], size_t referencesSize,
    const fmi2Boolean values[])
{
    if (!component) {
        return fmi2Warning;
    }

    const auto typedComponent = static_cast<Component*>(component);

    for (decltype(referencesSize) i = 0; i < referencesSize; ++i) {
        typedComponent->setBoolean(references[i], values[i]);
    }

    return fmi2OK;
}

fmi2Status fmi2SetString(fmi2Component component, const fmi2ValueReference references[], size_t referencesSize,
    const fmi2String values[])
{

    if (!component) {
        return fmi2Warning;
    }

    const auto typedComponent = static_cast<Component*>(component);

    for (decltype(referencesSize) i = 0; i < referencesSize; ++i) {
        typedComponent->setString(references[i], values[i]);
    }

    return fmi2OK;
}

// The functions fmi2GetFMUstate, fmi2SetFMUstate, fmi2FreeFMUstate, fmi2SerializedFMUstateSize, fmi2SerializeFMUstate
// and fmi2DeSerializeFMUstate are not implemented, so make sure to not explicitly set
// /fmiModelDescription/CoSimulation canGetAndSetFMUstate and canSerializeFMUstate attributes to true in the
// modelDescription.xml file.

// The function fmi2GetDirectionalDerivative is not implemented, so make sure to not explicitly set
// /fmiModelDescription/CoSimulation providesDirectionalDerivative attribute to true in the
// modelDescription.xml file.

fmi2Status fmi2SetRealInputDerivatives(fmi2Component component, const fmi2ValueReference references[],
    size_t referencesSize, const fmi2Integer order[], const fmi2Real values[])
{
    Q_UNUSED(component)
    Q_UNUSED(references)
    Q_UNUSED(referencesSize)
    Q_UNUSED(order)
    Q_UNUSED(values)

    return fmi2OK;
}

fmi2Status fmi2GetRealOutputDerivatives(fmi2Component component, const fmi2ValueReference references[],
    size_t referencesSize, const fmi2Integer order[], fmi2Real values[])
{
    Q_UNUSED(component)
    Q_UNUSED(references)
    Q_UNUSED(referencesSize)
    Q_UNUSED(order)
    Q_UNUSED(values)

    return fmi2OK;
}

fmi2Status fmi2DoStep(fmi2Component component, fmi2Real currentPoint, fmi2Real stepSize,
    fmi2Boolean noSetFmuStatePriorToCurrentPoint)
{
    Q_UNUSED(currentPoint)
    Q_UNUSED(stepSize)
    Q_UNUSED(noSetFmuStatePriorToCurrentPoint)

    if (!component) {
        return fmi2Warning;
    }


    const auto typedComponent = static_cast<Component*>(component);

    // Just for testing.
//    typedComponent->setReal(0, typedComponent->real(0) + 1.0);
//    typedComponent->setReal(1, typedComponent->real(1) - 1.0);

    typedComponent->doStep();

    return fmi2OK;
}

fmi2Status fmi2CancelStep(fmi2Component component)
{
    Q_UNUSED(component)

    return fmi2OK;
}

fmi2Status fmi2GetStatus(fmi2Component component, const fmi2StatusKind kind, fmi2Status* value)
{
    Q_UNUSED(component)
    Q_UNUSED(kind)
    Q_UNUSED(value)

    return fmi2OK;
}

fmi2Status fmi2GetRealStatus(fmi2Component component, const fmi2StatusKind kind, fmi2Real* value)
{
    Q_UNUSED(component)
    Q_UNUSED(kind)
    Q_UNUSED(value)

    return fmi2OK;
}

fmi2Status fmi2GetIntegerStatus(fmi2Component component, const fmi2StatusKind kind, fmi2Integer* value)
{
    Q_UNUSED(component)
    Q_UNUSED(kind)
    Q_UNUSED(value)

    return fmi2OK;
}

fmi2Status fmi2GetBooleanStatus(fmi2Component component, const fmi2StatusKind kind, fmi2Boolean* value)
{
    Q_UNUSED(component)
    Q_UNUSED(kind)
    Q_UNUSED(value)

    return fmi2OK;
}

fmi2Status fmi2GetStringStatus(fmi2Component component, const fmi2StatusKind kind, fmi2String* value)
{
    Q_UNUSED(component)
    Q_UNUSED(kind)
    Q_UNUSED(value)

    return fmi2OK;
}
