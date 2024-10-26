#include "ConfigurationUtils.h"
#include "internal/string-utils.h"

ParameterInfo numericParameter(const String &name, const ParameterAttribute attribute, const int min, const int max)
{
  return customParameter(name, ParameterType::TYPE_INT, attribute, [name, min, max](const String &value) -> ValidationResult
                         {
    int result;
    if (tryGetInt(value, &result) && result >= min && result <= max)
      return ValidationResult::Success();
    return ValidationResult::Failure(name + ": value (" + value + ") is out of range [" + min + ", " + max + "]"); });
}

ParameterInfo floatParameter(const String &name, const ParameterAttribute attribute, const float min, const float max)
{
  return customParameter(name, ParameterType::TYPE_FLOAT, attribute, [name, min, max](const String &value) -> ValidationResult
                         {
    float result;
    if (tryGetFloat(value, &result) && result >= min && result <= max)
      return ValidationResult::Success();
    return ValidationResult::Failure(name + ": value (" + value + ") is out of range [" + min + ", " + max + "]"); });
}

ParameterInfo booleanParameter(const String &name, const ParameterAttribute attribute)
{
  return customParameter(name, ParameterType::TYPE_BOOL, attribute, [name](const String &value) -> ValidationResult
                         {
    if (value.equalsIgnoreCase("true") || value.equalsIgnoreCase("false"))
      return ValidationResult::Success();
    return ValidationResult::Failure(name + ": value must be true/false"); });
}

ParameterInfo optionSetParameter(const String &name, const ParameterAttribute attribute, const std::function<std::vector<String>()> optionsFunc)
{
  return customParameter(name, ParameterType::TYPE_OPTIONSET, attribute, [name, optionsFunc](const String &value) -> ValidationResult
                         {
                          if (optionsFunc == nullptr) return ValidationResult::Success();
                          const auto& options = optionsFunc();
    if (options.empty() || std::find(options.begin(), options.end(), value) != options.end())
      return ValidationResult::Success();
    return ValidationResult::Failure(name + ": " + value + " is invalid, options are: [" + vectorToString(options) + "]"); });
}

ParameterInfo stringParameter(const String &name, const ParameterAttribute attribute, const uint maxLength)
{
  return customParameter(name, ParameterType::TYPE_STRING, attribute, [name, maxLength](const String &value) -> ValidationResult
                         {
    if (value.length() <= maxLength)
      return ValidationResult::Success();
    return ValidationResult::Failure(name + ": value's length must be less than " + maxLength); });
}

ParameterInfo customParameter(const String &name, const ParameterType type, const ParameterAttribute attribute, std::function<ValidationResult(const String &value)> validationFunction)
{
  return {name, type, attribute, validationFunction};
}
