#ifndef __H_INPUT_INTERFACE__
#define __H_INPUT_INTERFACE__
#include <WString.h>
#include <map>
#include <vector>
#include "ConfigurationHandler.h"
#include "ConfigurationUtils.h"
#include "DataStructures.h"
#include "internal/ParametersManager.h"
#include "internal/ValidationResult.h"

/**
 * @brief A simple interface that allows you to get values for given configuration types and save them.
 * Abstract class designed to standardize the process of reading and modifying configurations.
 * It utilizes a template parameter to specify the types of configuration inputs, enabling flexibility when getting parameters.
 *
 * @tparam ConfigTypes The types of configurations to which you want to get values.
 */
template <typename... ConfigTypes>
class InputInterface
{
public:
    InputInterface(ConfigurationHandler &configHandler)
        : configHandler(configHandler)
    {
        static_assert(sizeof...(ConfigTypes) > 0, "At least one type must be provided");
    }

    /**
     * @brief Loads the current values for all the parameters in the configuration types from the storage medium on the configurations handler.
     *
     * Then it starts the input interface, and blocks until all input is read and valid.
     *
     */
    void initializeAndStart()
    {
        parameters = configHandler.loadParameters<ConfigTypes...>();
        (initType<ConfigTypes>(), ...);
        start();
    }

protected:
#pragma region Parameters handling functions
    void setValue(const String &category, const String &parameterName, const String &value)
    {
        parameters.setParameterValue(category, parameterName, value);
    }

    const ValidationResult validateValue(const String &category, const String &parameterName, const String &value)
    {
        return parameters.validateValue(category, parameterName, value);
    }

    std::vector<String> getOptionsFor(const String &category, const String &parameterName, const bool refresh = false)
    {
        return parameters.getParameterOptions(category, parameterName, refresh);
    }

    const String &getOriginalValue(const String &category, const String &parameterName) const
    {
        return parameters.getOriginalValue(category, parameterName);
    }
#pragma endregion

    virtual void init(const ConfigInfo &configInfo, const std::map<String, String> &currentValues) = 0;

    virtual void start() = 0;

    ChainedValidationResults validate()
    {
        ChainedValidationResults result = parameters.validateAllValues();
        // No need to run validation on the types with invalid values that must be changed anyway.
        if (result.isSuccess())
            // Append all the results to one object.
            ((result = result && validateType<ConfigTypes>()), ...);
        return result;
    }

    void save()
    {
        configHandler.saveConfiguration<ConfigTypes...>(parameters);
    }

private:
    ConfigurationHandler &configHandler;
    ParametersManager parameters;

    template <typename T>
    void initType()
    {
        ConfigInfo info = ConfigurationFunctions<T>::getConfigInfo();
        init(info, parameters.getParametersValues(info.title));
    }

    template <typename T>
    const ValidationResult validateType()
    {
        ConfigInfo info = ConfigurationFunctions<T>::getConfigInfo();
        const std::map<String, String> t = parameters.getParametersValues(info.title);
        return ConfigurationFunctions<T>::validate(t);
    }
};

#endif // __H_INPUTINTERFACE__