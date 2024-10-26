#ifndef __H_STRING_UTILS__
#define __H_STRING_UTILS__
#include <WString.h>
#include <vector>

/**
 * @brief Concatenates all the strings in the vector to one String object.
 * The concatenated values are separated using the `separator` string.
 * 
 * @param vec The stings you wish to concatenate.
 * @param separator The separator string used to separate the vector elements in the final string.
 * @return A String object containing all the elements from the strings vector separated by the given separator.
 */
String vectorToString(const std::vector<String> &vec, const String &separator = ", ");
/**
 * @brief Tries to convert a substring of a given string to an integer number.
 * 
 * @param value The complete string object.
 * @param offset The start point for the substring.
 * @param length The number of characters for the substring.
 * @param result The output of the conversion if it was successful, otherwise contains garbage.
 * @return `true` if the substring was converted successfully, `false` otherwise.
 */
bool tryGetInt(const String &value, const int offset, const int length, int *result);
/**
 * @brief Tries to convert a given string to an integer number.
 * 
 * @param value The string to be converted to an int.
 * @param result The output of the conversion if it was successful, otherwise contains garbage.
 * @return `true` if the substring was converted successfully, `false` otherwise.
 */
bool tryGetInt(const String &value, int *result);
/**
 * @brief Tries to convert a given string to a float.
 * 
 * @param value The string to be converted to a float.
 * @param result The output of the conversion if it was successful, otherwise contains garbage.
 * @return `true` if the substring was converted successfully, `false` otherwise.
 */
bool tryGetFloat(const String &value, float *result);

#endif