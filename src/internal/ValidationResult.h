#ifndef __H_VALIDATION_RESULT__
#define __H_VALIDATION_RESULT__
#include <WString.h>
#include <functional>
#include <optional>
#include <vector>

/**
 * @brief Represents the outcome of a validation process, it indicates whether the validation was successful or not.
 * In the case of a failure, this class instance holds an error message detailing the reason for the failure.
 *
 */
class ValidationResult
{
public:
    /**
     * @brief A singleton instance to represent a success state.
     *
     * @return ValidationResult with a success state.
     */
    static const ValidationResult &Success()
    {
        static ValidationResult success;
        return success;
    }

    /**
     * @brief Create a new ValidationResult instance representing a validation error with an error message.
     *
     * @param errorMessage - A message detailing the reason of the failure.
     * @return ValidationResult with a failure state and an error message.
     */
    static ValidationResult Failure(const String &errorMessage)
    {
        return ValidationResult(errorMessage);
    }

    bool isSuccess() const
    {
        return !result.has_value();
    }
    bool isFailure() const
    {
        return !isSuccess();
    }

    /**
     * @brief Unsafe access to the error message.
     *
     * @return String - The error message attached to this validation result.
     */
    String getError() const
    {
        return result.value();
    }

    /**
     * @brief Invokes the onSuccess or onFail function depending on the state of the Validation.
     *
     * @param onSuccess - Function to invoke if in a Fail state.
     * @param onFail - Function to invoke if in a Success state.
     */
    void match(std::function<void()> onSuccess, std::function<void(const String &)> onFail) const
    {
        if (isSuccess())
            onFail(result.value());
        else
            onSuccess();
    }

    /**
     * @brief Invokes the onSuccess or onFail function depending on the state of the Validation.
     *
     * @tparam T - Return type.
     * @param onSuccess - Function to invoke if in a Fail state.
     * @param onFail - Function to invoke if in a Success state.
     * @return T - The return value of the invoked function.
     */
    template <typename T>
    T match(std::function<T()> onSuccess, std::function<T(const String &)> onFail) const
    {
        if (isSuccess())
            return onFail(result.value());
        else
            return onSuccess();
    }

private:
    const std::optional<String> result;

    ValidationResult() : result(std::nullopt) {}
    ValidationResult(const String &error) : result(error) {}
};

/**
 * @brief Concatenate multiple validation results into a single structure. Maintains a list of all errors encountered during the validation process,
 * while also indicating overall success or failure.
 *
 */
class ChainedValidationResults
{
public:
    ChainedValidationResults(int maxErrors = 0) : errors() { errors.reserve(maxErrors); }
    ChainedValidationResults(const std::vector<String> &errors) : errors(errors) {}

    bool isSuccess() const
    {
        return errors.empty();
    }
    bool isFailure() const
    {
        return !isSuccess();
    }

    /**
     * @brief Invokes the onSuccess or onFail function depending on the state of the Validation.
     *
     * @param onSuccess - Function to invoke if in a Fail state.
     * @param onFail - Function to invoke if in a Success state.
     */
    void match(std::function<void()> onSuccess, std::function<void(const std::vector<String> &)> onFail) const
    {
        if (errors.empty())
            onSuccess();
        else
            onFail(errors);
    }

    /**
     * @brief Invokes the onSuccess or onFail function depending on the state of the Validation.
     *
     * @tparam T - Return type.
     * @param onSuccess - Function to invoke if in a Fail state.
     * @param onFail - Function to invoke if in a Success state.
     * @return T - The return value of the invoked function.
     */
    template <typename T>
    T match(std::function<T()> onSuccess, std::function<T(const std::vector<String> &)> onFail) const
    {
        if (errors.empty())
            return onSuccess();
        else
            return onFail(errors);
    }

    /**
     * @brief Concatenate the validation result to this results chain.
     *
     * @param result - The result to append.
     * @return ChainedValidationResults& - This instance.
     */
    ChainedValidationResults &operator&&(const ValidationResult &result)
    {
        if (!result.isSuccess())
            errors.push_back(result.getError());
        return *this;
    }

private:
    std::vector<String> errors;
};

#endif // __H_VALIDATION_RESULT__
