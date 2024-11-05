#ifndef __H_INPUT_INTERFACE__
#define __H_INPUT_INTERFACE__
#include <WString.h>
#include <map>
#include <vector>
#include "DataStructures.h"
#include "internal/ParametersManager.h"
#include "internal/ValidationResult.h"

/**
 * @brief A simple interface that allows you to get values for configuration types and save them.
 * Abstract class designed to standardize the process of reading and editing configurations.
 */
class InputInterface
{
public:
    /**
     * @brief Sets the validation and save functions, and the parameters manager for following sessions.
     * 
     * @param parametersManager The `ParametersManager` instance to use for the next input sessions.
     * @param validateCallback Callback function to validate the current state of `parametersManager` instance.
     * @param saveCallback Callback function to save the state the current state of `parametersManager` instance.
     */
    void initialize(ParametersManager *parametersManager, const std::function<ChainedValidationResults()> validateCallback, const std::function<void()> saveCallback)
    {
        this->parameters = parametersManager;
        this->validate = validateCallback;
        this->save = saveCallback;
    }

    /**
     * @brief Add the configuration's parameters to this input interface.
     * 
     * @param info The metadata for configuration to be added.
     */
    void registerConfiguration(const ConfigInfo &info)
    {
        init(info, parameters->getParametersValues(info.title));
    }

    /**
     * @brief Starts this input interface and blocks until the input is validated or the session in canceled.
     * 
     */
    void start()
    {
        currentState = SessionState::GETTING_INPUT;
        startImpl();

        while (currentState == SessionState::GETTING_INPUT)
        {
            update();
        }

        if (currentState == SessionState::INPUT_VALIDATED)
            save();
        cleanup();
    }

protected:
    ParametersManager &getParametersManger()
    {
        return *parameters;
    }

    void cancelSession() { currentState = SessionState::ABORTED; }

    ChainedValidationResults validateInput()
    {
        auto validationResult = validate();
        currentState = validationResult.match<SessionState>([]()
                                                            { return SessionState::INPUT_VALIDATED; },
                                                            [](const std::vector<String> &_)
                                                            { return SessionState::GETTING_INPUT; });
        return validationResult;
    }

    virtual void init(const ConfigInfo &configInfo, const std::map<String, String> &currentValues) = 0;

    virtual void startImpl() = 0;
    virtual void update() = 0;
    virtual void cleanup() = 0;

private:
    enum class SessionState
    {
        /// @brief indicates that the input is still being read.
        GETTING_INPUT,
        /// @brief Signifies that the input has been validated successfully and is ready for saving.
        INPUT_VALIDATED,
        /// @brief Indicates the input session was canceled.
        ABORTED
    };
    ParametersManager *parameters;
    std::function<ChainedValidationResults()> validate;
    std::function<void()> save;
    SessionState currentState;
};

#endif  // __H_INPUT_INTERFACE__
