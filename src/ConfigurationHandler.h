#ifndef __H_CONFIGURATION_HANDLER__
#define __H_CONFIGURATION_HANDLER__
#include <HardwareSerial.h>
#include <array>
#include <optional>
#include <stdexcept>
#include <tuple>
#include "ConfigurationUtils.h"
#include "DataStructures.h"
#include "StorageMedium.h"
#include "internal/ParametersManager.h"

/**
 * @brief A mediator between configuration types and the StorageMedium.
 * It abstracts the complexity of interacting with the storage medium for configurations by providing functions for specific configuration operations,
 * streamlining the process of managing configurations.
 *
 */
class ConfigurationHandler
{
public:
    ConfigurationHandler(StorageMedium &storageMedium)
        : storageMedium(storageMedium) {}

    /**
     * @brief Checks if all the provided configurations have a configuration file in the storage medium.
     *
     * @tparam ConfigurationTypes - The type of the configurations you want to check.
     * @return true If all configurations have files in the storage medium,
     * @return false If at least one of the configuration types doesn't have a file.
     */
    template <typename... ConfigurationTypes>
    bool configsExist()
    {
        return (configurationExists<ConfigurationTypes>() && ...);
    }

    /**
     * @brief Checks if all the provided configurations have a configuration file in the storage medium and that it is complete.
     *
     * A complete configuration file, contains all the properties for that configuration (validation is not checked).
     *
     * @tparam ConfigurationTypes - The type of the configurations you want to check.
     * @return true If all configurations have complete files in the storage medium,
     * @return false If at least one of the configuration types doesn't have a complete file.
     */
    template <typename... ConfigurationTypes>
    bool configsAreComplete()
    {
        return (configurationIsComplete<ConfigurationTypes>() && ...);
    }

    /**
     * @brief Create a `FileHandler` object for the given configuration type, with the given file mode.
     *
     * @tparam ConfigurationType - The type of the configuration whose config-file you want to open.
     * @param fileMode -In which mode should the file be opened.
     * @return FileHandler - Object that provdes read/write functionality for the given file on this storage medium.
     */
    template <typename ConfigurationType>
    StorageMedium::FileHandler createFileHandler(const FileMode fileMode)
    {
        const String fileName = getConfigurationFileName<ConfigurationType>();
        return storageMedium.createFileHandler(fileName, fileMode);
    }

    /**
     * @brief Tries to load a configuration object from the storage medium.
     *
     * @tparam ConfigurationType - The type of configuration you want to load.
     * @return std::optional<ConfigurationType> - An optional object holding the configuration object in case of successful loading.
     */
    template <typename ConfigurationType>
    std::optional<ConfigurationType> loadConfiguration()
    {
        if (!configurationExists<ConfigurationType>())
            return std::nullopt;

        // Open file for read
        StorageMedium::FileHandler fileHandler = createFileHandler<ConfigurationType>(FileMode::READ);
        if (!fileHandler)
        {
            // Failed to open the file even though it exists.
            const String fileName = getConfigurationFileName<ConfigurationType>();
            Serial.printf("Error opening file: \"%s\"\n", fileName);
            throw std::runtime_error("Error opening file!");
        }
        // Read the data from the file.
        return ConfigurationFunctions<ConfigurationType>::loadAsObject(fileHandler);
    }

    /**
     * @brief Tries to load the configuration objects from the storage medium for all the given configuration types.
     *
     * Returns a tuple of optional values for each configuration type, in which every element is the result object for the corresponding configuration type.
     *
     * Example usage: `const auto [config1, config2] = confHandler.loadConfigurations<Config1, Config2>();`
     *
     * @tparam ConfigurationTypes - The types of configurations you want to load.
     * @return std::tuple<std::optional<ConfigurationTypes>...> - A tuple of optional objects holding the configuration object for each configuration type.
     */
    template <typename... ConfigurationTypes>
    std::tuple<std::optional<ConfigurationTypes>...> loadConfigurations()
    {
        return std::make_tuple(loadConfiguration<ConfigurationTypes>()...);
    }

    /**
     * @brief Deletes the configuration files for each configuration type from the storage medium.
     *
     * This method returns an array of booleans, each represent if the configuration was successfully deleted.
     * A `true` value means that either the file was deleted or it did not exist on the storage medium.
     *
     * @tparam ConfigurationTypes The configuration types you wish to delete from the storage medium.
     * @return A results array containing the operation result (success or failure) for each configuration type.
     */
    template <typename... ConfigurationTypes>
    std::array<bool, sizeof...(ConfigurationTypes)> deleteConfigurations()
    {
        return {deleteConfiguration<ConfigurationTypes>()...};
    }

    /**
     * @brief Load the values for each parameter in each of the configuration types.
     *
     * @tparam ConfigurationTypes - The configuration types you wish to load.
     * @return ParametersManager - An object containing the values for all the parameters.
     */
    template <typename... ConfigurationTypes>
    ParametersManager loadParameters()
    {
        ParametersManager paramsManager;
        (loadConfigParameters<ConfigurationTypes>(paramsManager), ...);
        return paramsManager;
    }

    /**
     * @brief Writes the values of each parameter into the appropriate config file in the storage medium.
     *
     * @tparam ConfigurationTypes
     * @param paramsManager - An object containing the values for all the parameters.
     */
    template <typename... ConfigurationTypes>
    void saveConfiguration(ParametersManager &paramsManager)
    {
        (saveConfig<ConfigurationTypes>(paramsManager), ...);
    }

private:
    StorageMedium &storageMedium;

    template <typename ConfigurationType>
    bool configurationExists()
    {
        return storageMedium.exists(getConfigurationFileName<ConfigurationType>());
    }

    template <typename ConfigurationType>
    bool configurationIsComplete()
    {
        ConfigInfo info = ConfigurationFunctions<ConfigurationType>::getConfigInfo();
        String fileName = getConfigurationFileName<ConfigurationType>();
        return storageMedium.isComplete(fileName, info.parameters);
    }

    template <typename ConfigurationType>
    String getConfigurationFileName()
    {
        return ConfigurationFunctions<ConfigurationType>::getConfigFileName();
    }

    template <typename ConfigurationType>
    void loadConfigParameters(ParametersManager &paramsManager)
    {
        ConfigInfo info = ConfigurationFunctions<ConfigurationType>::getConfigInfo();
        std::map<String, String> currentValues;
        if (configurationExists<ConfigurationType>())
        {
            StorageMedium::FileHandler fileHandler = createFileHandler<ConfigurationType>(FileMode::READ);
            // Failed to open the file even though it exists.
            if (!fileHandler)
            {
                const String fileName = getConfigurationFileName<ConfigurationType>();
                Serial.printf("Error opening file: \"%s\"\n", fileName);
                throw std::runtime_error("Error opening file!");
            }
            currentValues = ConfigurationFunctions<ConfigurationType>::loadAsMap(fileHandler);
        }

        const auto &getOptionsFunc = ConfigurationFunctions<ConfigurationType>::getOptionsFor;
        const auto getEmptyOptionsFunc = [](const String &_)
        { return std::vector<String>(); };

        for (const ParameterInfo &param : info.parameters)
        {
            String value = "";
            const auto &it = currentValues.find(param.name);
            if (it != currentValues.end())
                value = it->second;
            paramsManager.addParameter(info.title, param, value,
                                       // If the parameter is not a set of options, don't even try to get values simply return an empty vector.
                                       param.type == ParameterType::TYPE_OPTIONSET
                                           ? getOptionsFunc
                                           : getEmptyOptionsFunc);
        }
    }

    template <typename ConfigurationType>
    bool deleteConfiguration()
    {
        return storageMedium.deleteConfig(getConfigurationFileName<ConfigurationType>());
    }

    template <typename ConfigurationType>
    void saveConfig(ParametersManager &paramsManager)
    {
        auto fileHandler = createFileHandler<ConfigurationType>(FileMode::WRITE);
        if (!fileHandler)
        {
            const String fileName = getConfigurationFileName<ConfigurationType>();
            Serial.printf("Error opening file: \"%s\"\n", fileName);
            throw std::runtime_error("Error opening file!");
        }
        ConfigInfo config = ConfigurationFunctions<ConfigurationType>::getConfigInfo();
        ConfigurationFunctions<ConfigurationType>::save(paramsManager.getParametersValues(config.title), fileHandler);
    }
};

#endif // __H_CONFIGURATION_HANDLER__
