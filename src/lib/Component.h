#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <QDir>
#include <QObject>
#include <QString>
#include <QUuid>

#include <QtNetwork/QHostAddress>
#include <QtNetwork/QUdpSocket>

#include <fmi2FunctionTypes.h>


// TODO: Example of how to read config from resources directory. E.g. IP address and port numbers for UDP.

class Component
{

public:
    Component(const QString& instanceName, fmi2Type type, const QUuid& uuid, const QDir& resourcesDirectory,
        const fmi2CallbackFunctions* callbackFunctions);

    QString instanceName() const;
    fmi2Type type() const;
    QUuid uuid() const;
    QDir resourcesDirectory() const;

    void doStep();

    fmi2Real real(fmi2ValueReference reference) const;
    void setReal(fmi2ValueReference reference, fmi2Real value);

    fmi2Integer integer(fmi2ValueReference reference) const;
    void setInteger(fmi2ValueReference reference, fmi2Integer value);

    fmi2Boolean boolean(fmi2ValueReference reference) const;
    void setBoolean(fmi2ValueReference reference, fmi2Boolean value);

    fmi2String string(fmi2ValueReference reference) const;
    void setString(fmi2ValueReference reference, fmi2String value);

private:
    QString instanceName_;
    fmi2Type type_;
    QUuid uuid_;
    QDir resourcesDirectory_;
    const fmi2CallbackFunctions* callbackFunctions_;

    QHostAddress receiveAddress_ = QHostAddress::LocalHost;
    quint16 receivePort_ = 49152;
    QHostAddress transmitAddress_ = QHostAddress::LocalHost;
    quint16 transmitPort_ = 49153;

    // According to the standard, the name of the variable is unique across all different types, but the value
    // reference only needs to be unique within one type, and one value reference can be used in multiple variables.
    // Key is variable name.
    std::unordered_map<std::string, fmi2ValueReference> realInputVariables_;
    std::unordered_map<std::string, fmi2ValueReference> realOutputVariables_;

    std::unordered_map<std::string, fmi2ValueReference> integerInputVariables_;
    std::unordered_map<std::string, fmi2ValueReference> integerOutputVariables_;

    std::unordered_map<std::string, fmi2ValueReference> booleanInputVariables_;
    std::unordered_map<std::string, fmi2ValueReference> booleanOutputVariables_;

    std::unordered_map<std::string, fmi2ValueReference> stringInputVariables_;
    std::unordered_map<std::string, fmi2ValueReference> stringOutputVariables_;

    std::unordered_map<fmi2ValueReference, fmi2Real> realValues_;
    std::unordered_map<fmi2ValueReference, fmi2Integer> integerValues_;
    std::unordered_map<fmi2ValueReference, fmi2Boolean> booleanValues_;
    std::unordered_map<fmi2ValueReference, std::string> stringValues_;

    QUdpSocket udpSocket_;

    void logMessage(const fmi2Status& status, const QString& message, const QString& category = "");

    QString modelDescriptionPath(const QDir& resourcesDirectory) const;

    void parseModelDescription(const QString& filePath);
    void parseConfig(const QDir& resourcesDirectory);

    void publishInputs();
    void writeDatagram(const QByteArray& data);

    void processDatagrams();
};
