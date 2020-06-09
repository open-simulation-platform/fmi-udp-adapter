#pragma once

#include <QString>

enum class Causality
{
    Parameter,
    CalculatedParameter,
    Input,
    Output,
    Local,
    Independent
};

Causality causality(const QString& value);


enum class Type {
    Real,
    String,
    Integer,
    Boolean,
    Unknown
};

Type fmiType(const QString& value);
