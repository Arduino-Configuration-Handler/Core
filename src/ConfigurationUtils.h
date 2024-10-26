#ifndef __H_CONFIGURATION_UTILS__
#define __H_CONFIGURATION_UTILS__
#include <WString.h>
#include <functional>
#include <map>
#include <vector>
#include "DataStructures.h"
#include "StorageMedium.h"
#include "internal/ValidationResult.h"

/**
 * @brief Create a parameter of type 'integer' with the specified attribute and a validation function that ensures the value is within the range [`min`, `max`].
 *
 * @param name The parameter's name (must be unique in a configuration scope).
 * @param attribute The parameter's special attribute.
 * @param min The minimum valid value (inclusive).
 * @param max The maximum valid value (inclusive).
 * @return ParameterInfo
 */
ParameterInfo numericParameter(const String &name, const ParameterAttribute attribute, const int min, const int max);

/**
 * @brief Create a parameter of type 'float' with the specified attribute and a validation function that ensures the value is within the range [`min`, `max`].
 *
 * @param name The parameter's name (must be unique in a configuration scope).
 * @param attribute The parameter's special attribute.
 * @param min The minimum valid value (inclusive).
 * @param max The maximum valid value (inclusive).
 * @return ParameterInfo
 */
ParameterInfo floatParameter(const String &name, const ParameterAttribute attribute, const float min, const float max);

/**
 * @brief Create a parameter of type 'string' with the specified attribute and a validation function that ensures the value's length is no longer than `maxLengrh`.
 *
 * @param name The parameter's name (must be unique in a configuration scope).
 * @param attribute The parameter's special attribute.
 * @param maxLength The value's max length.
 * @return ParameterInfo
 */
ParameterInfo stringParameter(const String &name, const ParameterAttribute attribute, const uint maxLength);

/**
 * @brief Create a parameter of type 'boolean' with the specified attribute and a validation function that ensures the value is either "true" or "false".
 *
 * @param name The parameter's name (must be unique in a configuration scope).
 * @param attribute The parameter's special attribute.
 * @return ParameterInfo
 */
ParameterInfo booleanParameter(const String &name, const ParameterAttribute attribute);

/**
 * @brief Create a parameter of type 'optionset' with the specified attribute and a validation function that ensures the value is one of the valid options.
 *
 * @param name The parameter's name (must be unique in a configuration scope).
 * @param attribute The parameter's special attribute.
 * @param optionsFunc A function that returns all valid values for the parameter, may return `nullptr` in which case the validation function will always return success.
 * @return ParameterInfo
 */
ParameterInfo optionSetParameter(const String &name, const ParameterAttribute attribute, const std::function<std::vector<String>()> optionsFunc = nullptr);

/**
 * @brief Create a parameter of type 'type' with the specified attribute and the given validation function.
 *
 * @param name The parameter's name (must be unique in a configuration scope).
 * @param type The parameter's type.
 * @param attribute The parameter's special attribute.
 * @param validationFunction Function that returns success state for a valid value and failure state for invalid value.
 * @return ParameterInfo
 */
ParameterInfo customParameter(const String &name, const ParameterType type, const ParameterAttribute attribute, std::function<ValidationResult(const String &value)> validationFunction);

/**
 * @brief Struct that provides a collection of static utility functions that every configuration must implement.
 * These functions include essential operations like loading, saving, and validating configurations, ensuring consistency and reliability across different configuration types.
 *
 * @tparam T The type of the configuration object.
 */
template <typename T>
struct ConfigurationFunctions
{
    /**
     * @brief Returns the configuration's metadata.
     *
     */
    static ConfigInfo getConfigInfo();

    /**
     * @brief Get the name of the file in which the configuration is stored.
     */
    static String getConfigFileName();

    /**
     * @brief Get the valid options for a parameter of type `ParameterType::TYPE_OPTIONSET`.
     *
     * Note: This function would not be called for parameters that are not of type `ParameterType::TYPE_OPTIONSET`.
     *
     * @param parameterName The parameter for which you want the options.
     * @return std::vector<String> - A vector of all the valid options for the given parameter.
     */
    static std::vector<String> getOptionsFor(const String &parameterName);

    /**
     * @brief Writes the values in the given map into the storage medium.
     *
     * Note: the `values` map should only contain the parameters for this configuration.
     *
     * @param values A map whose keys are the parameters' names and the values are the parameters' values.
     * @param fileHandler The file handler for the configuration file.
     */
    static void save(const std::map<String, String> &values, StorageMedium::FileHandler &fileHandler);

    /**
     * @brief Loads the configuration from the storage medium as a map whose values are the parameters' names and the values are the parameters' values.
     *
     * @param fileHandler The file handler for the configuration file.
     * @return std::map<String, String> - A map whose keys are the parameters' names and the values are the parameters' values.
     */
    static std::map<String, String> loadAsMap(const StorageMedium::FileHandler &fileHandler);

    /**
     * @brief Loads the configuration from the storage medium as a concrete object.
     *
     * @param fileHandler The file handler for the configuration file.
     * @return T - An instance of the configuration type, initialized with values from the file handler.
     */
    static T loadAsObject(const StorageMedium::FileHandler &fileHandler);

    /**
     * @brief Validates that the given parameters' values are valid for configuration of type `T`.
     *
     * @param values A map whose keys are the parameters' names and the values are the parameters' values.
     * @return ValidationResult - That represents either a success or a failure, and holds the error message (incase of a failure).
     */
    static const ValidationResult validate(const std::map<String, String> &values);
};

#endif  // __H_CONFIGURATION_UTILS__
