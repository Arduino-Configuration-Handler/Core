#ifndef __H_PARAMETERS_MANAGER__
#define __H_PARAMETERS_MANAGER__
#include <WString.h>
#include <functional>
#include <map>
#include <optional>
#include "ValidationResult.h"
#include "../DataStructures.h"

/**
 * @brief Consolidates parameters from multiple configurations, offering am interface for interacting with them.
 * It supports operations such as editing parameters values and running validations, thereby simplifying the management of diverse configuration parameters.
 *
 */
class ParametersManager
{
public:
    void addParameter(const String &category, const ParameterInfo &parameter, const String &currentValue, std::function<std::vector<String>(const String &)> getOptions);

    std::vector<String> getParameterOptions(const String &category, const String &parameterName, bool refresh = false);

    std::map<String, String> getParametersValues(const String &category) const;

    const String &getOriginalValue(const String &category, const String &parameterName) const;

    void setParameterValue(const String &category, const String &parameterName, const String &value);

    const ValidationResult validateValue(const String &category, const String &parameterName, const String &value) const;

    ChainedValidationResults validateAllValues() const;

private:
    class Parameter
    {
    public:
        const ParameterInfo param;
        const String value;
        std::optional<String> newValue;

        Parameter(const ParameterInfo &parameter, const String &currentValue, const std::function<std::vector<String>(const String &)> getOptions)
            : param(parameter), value(currentValue), getOptionsForParam(getOptions), options(), optionsLoaded(false), newValue(std::nullopt) {}

        std::vector<String> getOptions(bool refresh);

    private:
        const std::function<std::vector<String>(const String &)> getOptionsForParam;
        std::vector<String> options;
        bool optionsLoaded;
    };
    std::map<String, std::map<String, Parameter>> parameters;
};

#endif