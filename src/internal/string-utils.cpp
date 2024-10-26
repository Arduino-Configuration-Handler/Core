#include <cmath>
#include <ctype.h>
#include "string-utils.h"

String vectorToString(const std::vector<String> &vec, const String &separator)
{
    String result;
    size_t vectorSize = vec.size();
    for (size_t i = 0; i < vectorSize; ++i)
    {
        result += vec[i];
        if (i < vectorSize - 1)
            result += separator;
    }
    return result;
}

bool tryGetInt(const String &value, const int start, const int length, int *result)
{
    if (length == 0 || start + length > value.length())
        return false;

    *result = 0;
    int sign = 1;
    int i = start;

    if (value[i] == '-')
    {
        if (length == 1)
            return false; // Only a '-' sign is not valid.
        sign = -1;
        i++;
    }

    for (; i < start + length; i++)
    {
        char c = value[i];
        if (!isdigit(c))
            return false;

        *result = *result * 10 + (c - '0');
    }

    *result *= sign;
    return true;
}

bool tryGetInt(const String &value, int *result)
{
    return tryGetInt(value, 0, value.length(), result);
}

bool tryGetFloat(const String &value, float *result)
{
    if (value.isEmpty())
        return false;

    int decimalPoint = value.indexOf('.');
    // No decimal point, parse it as an int
    if (decimalPoint == -1)
    {
        int intResult;
        bool success = tryGetInt(value, &intResult);
        *result = static_cast<float>(intResult);
        return success;
    }

    // Check for more than decimal points
    if (value.indexOf('.', decimalPoint + 1) != -1)
    {
        return false;
    }

    // Read the integer part and the fractional part as ints
    int integerPart, fractionalPart;
    int fractionalPartLength = value.length() - decimalPoint - 1;
    bool success = tryGetInt(value, 0, decimalPoint, &integerPart) &&
                   tryGetInt(value, decimalPoint + 1, fractionalPartLength, &fractionalPart);

    // if the fraction is negative, it means the input has '-' sign in it.
    if (!success || fractionalPart < 0)
        return false;

    if (integerPart < 0)
    {
        fractionalPart = -fractionalPart;
    }

    *result = integerPart + static_cast<float>(fractionalPart) / pow(10.0f, fractionalPartLength);
    return true;
}
