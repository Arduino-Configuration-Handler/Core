#ifndef __H_INPUT_INTERFACE__
#define __H_INPUT_INTERFACE__
#include <WString.h>
#include <map>
#include <vector>
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
        startImpl();
    }

protected:
    std::function<ChainedValidationResults()> validate;
    std::function<void()> save;

    ParametersManager &getParametersManger()
    {
        return *parameters;
    }

    virtual void init(const ConfigInfo &configInfo, const std::map<String, String> &currentValues) = 0;

    virtual void startImpl() = 0;

private:
    ParametersManager *parameters;
};

#endif  // __H_INPUT_INTERFACE__
