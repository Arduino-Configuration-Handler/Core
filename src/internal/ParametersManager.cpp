#include "ParametersManager.h"

void ParametersManager::addParameter(const String &category, const ParameterInfo &parameter, const String &currentValue, std::function<std::vector<String>(const String &)> getOptions)
{
    parameters[category].emplace(parameter.name, Parameter(parameter, currentValue, getOptions));
}

std::vector<String> ParametersManager::getParameterOptions(const String &category, const String &parameterName, bool refresh)
{
    Parameter &param = parameters.at(category).at(parameterName);
    return param.getOptions(refresh);
}

std::map<String, String> ParametersManager::getParametersValues(const String &category) const
{
    std::map<String, String> values;
    const std::map<String, Parameter> &params = parameters.at(category);
    for (const auto &[_, parameter] : params)
    {
        values[parameter.param.name] = parameter.newValue.value_or(parameter.value);
    }
    return values;
}

const String &ParametersManager::getOriginalValue(const String &category, const String &parameterName) const
{
    return parameters.at(category).at(parameterName).value;
}

void ParametersManager::setParameterValue(const String &category, const String &parameterName, const String &value)
{
    if (value.equals(getOriginalValue(category, parameterName)))
        parameters[category].at(parameterName).newValue.reset();
    else
        parameters[category].at(parameterName).newValue = value;
}

const ValidationResult ParametersManager::validateValue(const String &category, const String &parameterName, const String &value) const
{
    return parameters.at(category).at(parameterName).param.isValid(value);
}

ChainedValidationResults ParametersManager::validateAllValues() const
{
    ChainedValidationResults result;
    for (const auto &[_, parametersInCategory] : parameters)
    {
        for (const auto &[paramName, param] : parametersInCategory)
        {
            // Skip unmodified parameters
            if (!param.newValue.has_value())
                continue;
            const ValidationResult &error = param.param.isValid(param.newValue.value());
            result = result && error;
        }
    }
    return result;
}

std::vector<String> ParametersManager::Parameter::getOptions(bool refresh)
{
    if (refresh || !optionsLoaded)
    {
        options = getOptionsForParam(param.name);
        optionsLoaded = true;
    }
    return options;
}
