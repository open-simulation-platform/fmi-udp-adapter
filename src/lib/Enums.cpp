#include "Enums.h"

Causality causality(const QString& value)
{
    if (value == "parameter") {
        return Causality::Parameter;
    } else if (value == "calculatedParameter") {
        return Causality::CalculatedParameter;
    } else if (value == "input") {
        return Causality::Input;
    } else if (value == "output") {
        return Causality::Output;
    } else if (value == "local") {
        return Causality::Local;
    } else if (value == "independent") {
        return Causality::Independent;
    } else {
        return Causality::Local;
    }
}

Type fmiType(const QString& value) {

    if (value == "Real") {
        return Type::Real;
    } else if (value == "Integer") {
        return Type::Integer;
    } else if (value == "Boolean") {
        return Type::Boolean;
    } else if (value == "String") {
        return Type::String;
    } else {
        return Type::Unknown;
    }
}


