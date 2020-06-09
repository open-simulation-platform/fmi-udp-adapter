#include "Component.h"

#include <QDebug>
#include <QXmlStreamReader>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QNetworkDatagram>
#include <QJsonObject>
#include <QJsonDocument>

#include "Enums.h"

Component::Component(const QString& instanceName, fmi2Type type, const QUuid& uuid, const QDir& resourcesDirectory,
                     const fmi2CallbackFunctions* callbackFunctions)
    : instanceName_(instanceName),
      type_(type),
      uuid_(uuid),
      resourcesDirectory_(resourcesDirectory),
      callbackFunctions_(callbackFunctions)
{
    logMessage(fmi2OK, "Hello from Component::Component");

    parseModelDescription(modelDescriptionPath(resourcesDirectory));

    parseConfig(resourcesDirectory);

    // Note that since this will not run in an QCoreApplication we can't connect to the readyRead signal, so poll in
    // doStep.
    if (!udpSocket_.bind(receiveAddress_, receivePort_)) {
        logMessage(fmi2Warning, QString("Failed to bind UDP socket to port %1").arg(QString::number(receivePort_)));
    }
}

QString Component::instanceName() const
{
    return instanceName_;
}

fmi2Type Component::type() const
{
    return type_;
}

QUuid Component::uuid() const
{
    return uuid_;
}

QDir Component::resourcesDirectory() const
{
    return resourcesDirectory_;
}

void Component::doStep()
{
    processDatagrams();

    publishInputs();
}

fmi2Real Component::real(fmi2ValueReference reference) const
{
    const auto result = realValues_.find(reference);
    if (result != realValues_.end()) {
        return result->second;
    } else {
        return 0.0;
    }
}

void Component::setReal(fmi2ValueReference reference, fmi2Real value)
{
    realValues_[reference] = value;
}

fmi2Integer Component::integer(fmi2ValueReference reference) const
{
    const auto result = integerValues_.find(reference);
    if (result != integerValues_.end()) {
        return result->second;
    } else {
        return 0;
    }
}

void Component::setInteger(fmi2ValueReference reference, fmi2Integer value)
{
    integerValues_[reference] = value;
}

fmi2Boolean Component::boolean(fmi2ValueReference reference) const
{

    const auto result = booleanValues_.find(reference);
    if (result != booleanValues_.end()) {
        return result->second;
    } else {
        return false;
    }
}

void Component::setBoolean(fmi2ValueReference reference, fmi2Boolean value)
{
    booleanValues_[reference] = value;
}

fmi2String Component::string(fmi2ValueReference reference) const
{
    const auto result = stringValues_.find(reference);
    if (result != stringValues_.end()) {
        return result->second.c_str();
    } else {
        return "";
    }
}

void Component::setString(fmi2ValueReference reference, fmi2String value)
{
    stringValues_[reference] = std::string(value);
}

void Component::logMessage(const fmi2Status& status, const QString& message, const QString& category)
{
    callbackFunctions_->logger(NULL, instanceName_.toStdString().c_str(), status, category.toStdString().c_str(),
                               message.toStdString().c_str());
}

QString Component::modelDescriptionPath(const QDir& resourcesDirectory) const
{
    // We don't have any path to the modelDescription.xml file, but according to the standard it should be one level up
    // from the optional 'resources' directory, so if that's provided this should work fine.

    QDir fmuDirectory = QDir(resourcesDirectory);
    fmuDirectory.cdUp();
    if (fmuDirectory.exists()) {
        return QString("%1/modelDescription.xml").arg(fmuDirectory.path());
    } else {
        qWarning() << "Missing modelDescription path.";
    }

    return QString();
}

void Component::parseModelDescription(const QString& filePath)
{
    qDebug() << "filePath:" << filePath;
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Could not open file." << file.errorString();
        return;
    }

    QXmlStreamReader xmlReader(&file);

    while (!xmlReader.atEnd()) {
        const auto tokenType = xmlReader.readNext();

        if (tokenType == QXmlStreamReader::StartElement && xmlReader.name() == "ScalarVariable") {
            const auto attributes = xmlReader.attributes();

            const auto name = attributes.value(QLatin1String("name")).toString().toStdString();
            const auto causality = ::causality(attributes.value(QLatin1String("causality")).toString());
            const auto valueReference = attributes.value(QLatin1String("valueReference")).toUInt();

            xmlReader.readNextStartElement();
            const auto start = xmlReader.attributes().value(QLatin1String("start"));

            if (xmlReader.name() == QLatin1String("Real")) {
                if (causality == Causality::Input) {
                    realInputVariables_[name] = valueReference;
                    realValues_[valueReference] = start.toDouble();
                } else if (causality == Causality::Output) {
                    realOutputVariables_[name] = valueReference;
                }

            }

            if (xmlReader.name() == QLatin1String("Integer")) {
                if (causality == Causality::Input) {
                    integerInputVariables_[name] = valueReference;
                    integerValues_[valueReference] = start.toInt();
                } else if (causality == Causality::Output){
                    integerOutputVariables_[name] = valueReference;
                }

            }

            if(xmlReader.name() == QLatin1String("Boolean")) {
                if (causality == Causality::Input) {
                    booleanInputVariables_[name] = valueReference;
                    booleanValues_[valueReference] = (start.toString() == "true");
                } else if (causality == Causality::Output) {
                    booleanOutputVariables_[name] = valueReference;
                }

            }

            if(xmlReader.name() == QLatin1String("String")) {
                if (causality == Causality::Input) {
                    stringInputVariables_[name] = valueReference;
                    stringValues_[valueReference] = start.toString().toStdString();
                } else if (causality == Causality::Output) {
                    stringOutputVariables_[name] = valueReference;
                    stringValues_[valueReference] = "";
                }
            }
        }
    }
}

void Component::parseConfig(const QDir& resourcesDirectory)
{
    QFile file(QString("%1/Config.json").arg(resourcesDirectory.path()));

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << QLatin1String("Could not open file.") << file.errorString();
        return;
    }

    auto jsonDocument = QJsonDocument::fromJson(file.readAll());
    if (jsonDocument.isNull()) {
        logMessage(fmi2Warning, QLatin1String("Failed to parse JSON."));
        return;
    }

    const auto json = jsonDocument.object();

    const auto receiveAddress = json.value(QLatin1String("receiveAddress"));
    if (receiveAddress != QJsonValue::Undefined && receiveAddress.isString()) {
        receiveAddress_ = QHostAddress(receiveAddress.toString());
    }

    const auto receivePort = json.value(QLatin1String("receivePort"));
    if (receivePort != QJsonValue::Undefined && receivePort.isDouble()) {
        receivePort_ = static_cast<quint16>(receivePort.toDouble());
    }

    const auto transmitAddress = json.value(QLatin1String("transmitAddress"));
    if (transmitAddress != QJsonValue::Undefined && transmitAddress.isString()) {
        transmitAddress_ = QHostAddress(transmitAddress.toString());
    }

    const auto transmitPort = json.value(QLatin1String("transmitPort"));
    if (transmitPort != QJsonValue::Undefined && transmitPort.isDouble()) {
        transmitPort_ = static_cast<quint16>(transmitPort.toDouble());
    }
}

void Component::publishInputs()
{
    for (const auto& variable : realInputVariables_) {
        const auto result = realValues_.find(variable.second);
        if (result == realValues_.end()) {
            continue;
        }

        QJsonObject json;
        json.insert(QLatin1String("name"), QString::fromStdString(variable.first));
        json.insert(QLatin1String("type"), "Real");
        json.insert(QLatin1String("value"), result->second);

        writeDatagram(QJsonDocument(json).toJson(QJsonDocument::Compact));
    }

    for (const auto& variable : integerInputVariables_) {
        const auto result = integerValues_.find(variable.second);
        if(result == integerValues_.end()) {
            continue;
        }

        QJsonObject json;
        json.insert(QLatin1String("name"), QString::fromStdString(variable.first));
        json.insert(QLatin1String("type"), "Integer");
        json.insert(QLatin1String("value"), result->second);

        writeDatagram(QJsonDocument(json).toJson(QJsonDocument::Compact));
    }

    for (const auto& variable : booleanInputVariables_) {
        const auto result = booleanValues_.find(variable.second);
        if(result == booleanValues_.end()) {
            continue;
        }

        QJsonObject json;
        json.insert(QLatin1String("name"), QString::fromStdString(variable.first));
        json.insert(QLatin1String("type"), "Boolean");
        json.insert(QLatin1String("value"), result->second);

        writeDatagram(QJsonDocument(json).toJson(QJsonDocument::Compact));
    }


    for (const auto& variable : stringInputVariables_) {
        const auto result = stringValues_.find(variable.second);
        if(result == stringValues_.end()) {
            continue;
        }

        QJsonObject json;
        json.insert(QLatin1String("name"), QString::fromStdString(variable.first));
        json.insert(QLatin1String("type"), "String");
        json.insert(QLatin1String("value"), QString(result->second.c_str()));

        writeDatagram(QJsonDocument(json).toJson(QJsonDocument::Compact));
    }
}

void Component::writeDatagram(const QByteArray& data)
{
    // To listen use: nc -ul 49153
    udpSocket_.writeDatagram(data, transmitAddress_, transmitPort_);
}

void Component::processDatagrams()
{
    // To manually test use something like: nc -u localhost 49152
    // And some example of valid JSON. Use valueReference and type to set the value (remember that according to the
    // standard a value can be referenced by multiple variables of the same type).
    // {"name":"output1","value":9}

    while (udpSocket_.hasPendingDatagrams()) {
        QNetworkDatagram datagram = udpSocket_.receiveDatagram();

        auto jsonDocument = QJsonDocument::fromJson(datagram.data().constData());
        if (jsonDocument.isNull()) {
            logMessage(fmi2Warning, "Failed to parse JSON.");
            continue;
        }

        const auto json = jsonDocument.object();

        const auto name = json.value(QLatin1String("name"));
        if (name == QJsonValue::Undefined || !name.isString()) {
            logMessage(fmi2Warning, QLatin1String("JSON key 'name' is missing or have invalid type."));
            continue;
        }

        const auto value = json.value(QLatin1String("value"));
        if (value == QJsonValue::Undefined) {
            logMessage(fmi2Warning, QLatin1String("JSON key 'value' is missing."));
            continue;
        }

        const auto type = json.value(QLatin1String("type"));
        if (type == QJsonValue::Undefined || !type.isString()) {
            logMessage(fmi2Warning, QLatin1String("JSON key 'type' is missing or have invalid type"));
            continue;
        } else {

            const auto variableType = fmiType(type.toString());
            switch(variableType) {
            case Type::Real:
            {
                const auto doubleVariable = realOutputVariables_.find(name.toString().toStdString());
                if (doubleVariable == realOutputVariables_.end()) {
                    logMessage(fmi2Warning, QLatin1String("Name not found."));
                    continue;
                } else {
                    setReal(doubleVariable->second, value.toDouble());
                }
            }
                break;
            case Type::Integer:
            {
                const auto integerVariable = integerOutputVariables_.find(name.toString().toStdString());
                if (integerVariable == integerOutputVariables_.end()) {
                    logMessage(fmi2Warning, QLatin1String("Name not found."));
                    continue;
                } else {
                    setInteger(integerVariable->second, value.toInt());
                }
            }
                break;
            case Type::Boolean:
            {
                const auto boolVariable = booleanOutputVariables_.find(name.toString().toStdString());
                if(boolVariable == booleanOutputVariables_.end()) {
                    logMessage(fmi2Warning, QLatin1String("Name not found."));
                    continue;
                } else {
                    setBoolean(boolVariable->second, value.toBool());
                }
                break;
            }
            case Type::String:
            {
                const auto stringVariable = stringOutputVariables_.find(name.toString().toStdString());
                if (stringVariable == stringOutputVariables_.end()) {
                    logMessage(fmi2Warning, QLatin1String("Name not found."));
                    continue;
                } else {
                    setString(stringVariable->second, value.toString().toStdString().c_str());
                }
            }
                break;
            case Type::Unknown:
                logMessage(fmi2Warning, QLatin1String("Unknown Variable Type"));
                continue;
            }
        }
    }
}
