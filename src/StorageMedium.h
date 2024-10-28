#ifndef __H_STORAGE_MEDIUM__
#define __H_STORAGE_MEDIUM__
#include <WString.h>
#include <stdexcept>
#include <stdint.h>
#include <vector>
#include "DataStructures.h"

enum class FileMode : uint8_t
{
  READ,
  WRITE,
  APPEND
};

/**
 * @brief Abstract class that serves as a base interface for interacting with various file storage systems.
 * It defines a set of methods that enable derived classes to implement specific storage functionalities,
 * allowing for access and manipulation of files within a specified medium.
 *
 */
class StorageMedium
{
public:
  /**
   * @brief Instantiated by the StorageMedium, it encapsulates a unified interface for reading from and writing to a file in the associated storage medium.
   * This abstraction simplifies file operations, allowing managing file I/O without needing to handle the underlying storage implementation.
   *
   */
  class FileHandler
  {
    // Make sure that only StorageMedium is allowed to construct a FileHandler
    friend class StorageMedium;

  public:
    virtual ~FileHandler()
    {
      if (!disposed && isOpen)
        disposeFunc();
    }

    template <typename T>
    T read(const String &key, const T defaultValue = T()) const
    {
      if (!*this)
        throw std::runtime_error("Trying to read from a disposed/unopen file!");
      return storageMedium.read<T>(key, defaultValue);
    }
    template <typename T>
    void write(const String &key, const T value) const
    {
      if (!*this)
        throw std::runtime_error("Trying to write to a disposed/unopen file!");
      storageMedium.write<T>(key, value);
    }

    void dispose()
    {
      if (disposed || !isOpen)
        return;
      disposeFunc();
      disposed = true;
    }

    operator bool() const
    {
      return isOpen && !disposed;
    }

  private:
    FileHandler(StorageMedium &storageMedium, const bool isOpen, const std::function<void()> disposeFunc)
        : storageMedium(storageMedium), isOpen(isOpen), disposeFunc(disposeFunc), disposed(false) {}

    StorageMedium &storageMedium;
    const std::function<void()> disposeFunc;
    const bool isOpen;
    bool disposed;
  };

  /**
   * @brief Checks if a file named `fileName` exists in this stroage medium.
   *
   * @param fileName The name of the file whose existence you want to check.
   * @return true - If the file exists,
   * @return false - Otherwise.
   */
  bool exists(const String &fileName);

  /**
   * @brief Checks if the file contains all the specified parameters.
   *
   * This method verifies whether the specified file includes all the parameters provided in the list.
   *
   * @param fileName The name of the file to check for completeness.
   * @param parameters A list of parameters that the file must contain to be considered complete.
   * @return true if the file contains all specified parameters; false otherwise.
   */
  bool isComplete(const String &fileName, const std::vector<ParameterInfo> &parameters);

  /**
   * @brief Deletes the configuration file from this storage medium.
   *
   * This method returns true if the file was successfully deleted or if it did not exist on the storage medium.
   * It returns false if the deletion failed for any reason.
   *
   * @param fileName The name of the file to delete.
   * @return true if the file was deleted or did not exist; false if the deletion failed.
   */
  bool deleteConfig(const String &fileName);

  /**
   * @brief Create a `FileHandler` object for the given file name, with the given file mode.
   *
   * @param fileName The name of the file that you wish to open.
   * @param fileMode In which mode should the file be opened.
   * @return FileHandler - Object that provdes read/write functinallity for the given file on this storage medium.
   */
  FileHandler createFileHandler(const String &fileName, const FileMode fileMode);

protected:
  virtual bool openFile(const String &fileName, const FileMode fileMode) = 0;
  virtual void closeFile() = 0;

  virtual bool existsImpl(const String &fileName) = 0;
  virtual bool isCompleteImpl(const String &fileName, const std::vector<ParameterInfo> &parameters) = 0;
  virtual bool deleteImpl(const String &fileName) = 0;

#pragma region Read and Write abstract functions
  virtual int8_t readChar(const String &key, const int8_t defaultValue) = 0;
  virtual uint8_t readUChar(const String &key, const uint8_t defaultValue) = 0;
  virtual int16_t readShort(const String &key, const int16_t defaultValue) = 0;
  virtual uint16_t readUShort(const String &key, const uint16_t defaultValue) = 0;
  virtual int32_t readInt(const String &key, const int32_t defaultValue) = 0;
  virtual uint32_t readUInt(const String &key, const uint32_t defaultValue) = 0;
  virtual int64_t readLong(const String &key, const int64_t defaultValue) = 0;
  virtual uint64_t readULong(const String &key, const uint64_t defaultValue) = 0;
  virtual float readFloat(const String &key, const float defaultValue) = 0;
  virtual double readDouble(const String &key, const double defaultValue) = 0;
  virtual bool readBool(const String &key, const bool defaultValue) = 0;
  virtual String readString(const String &key, const String defaultValue) = 0;

  virtual void writeChar(const String &key, const int8_t value) = 0;
  virtual void writeUChar(const String &key, const uint8_t value) = 0;
  virtual void writeShort(const String &key, const int16_t value) = 0;
  virtual void writeUShort(const String &key, const uint16_t value) = 0;
  virtual void writeInt(const String &key, const int32_t value) = 0;
  virtual void writeUInt(const String &key, const uint32_t value) = 0;
  virtual void writeLong(const String &key, const int64_t value) = 0;
  virtual void writeULong(const String &key, const uint64_t value) = 0;
  virtual void writeFloat(const String &key, const float value) = 0;
  virtual void writeDouble(const String &key, const double value) = 0;
  virtual void writeBool(const String &key, const bool value) = 0;
  virtual void writeString(const String &key, const String value) = 0;
#pragma endregion

private:
  template <typename T>
  T read(const String &key, const T defaultValue = T());

  template <typename T>
  void write(const String &key, const T value);
};

#endif // __H_STORAGE_MEDIUM__
