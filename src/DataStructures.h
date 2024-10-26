#ifndef __H_DATA_STRUCTURES__
#define __H_DATA_STRUCTURES__

#include <WString.h>
#include <stdint.h>
#include <functional>
#include <vector>
#include "internal/ValidationResult.h"

enum class ParameterType : uint8_t
{
  TYPE_INT,
  TYPE_FLOAT,
  TYPE_BOOL,
  TYPE_STRING,
  TYPE_DATE,
  TYPE_OPTIONSET,
};

enum class ParameterAttribute : uint8_t
{
  ATTR_NONE,
  ATTR_PASSWORD,
};

/**
 * @brief Encapsulates the basic metadata for a parameter within a configuration.
 *
 */
typedef struct
{
  String name;
  ParameterType type;
  ParameterAttribute specialAttribute;
  std::function<const ValidationResult(const String &)> isValid;
} ParameterInfo;

/**
 * @brief Contains the metada of a configuration.
 *
 */
typedef struct
{
  const String title;
  const std::vector<ParameterInfo> parameters;
} ConfigInfo;

#endif