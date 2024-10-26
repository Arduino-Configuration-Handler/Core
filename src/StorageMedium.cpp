#include "StorageMedium.h"

#pragma region Template implementations for read and write functions.
template <>
int8_t StorageMedium::read<int8_t>(const String &key, const int8_t defaultValue)
{
  return readChar(key, defaultValue);
}
template <>
char StorageMedium::read<char>(const String &key, const char defaultValue)
{
  return readChar(key, defaultValue);
}

template <>
uint8_t StorageMedium::read<uint8_t>(const String &key, const uint8_t defaultValue)
{
  return readUChar(key, defaultValue);
}

template <>
int16_t StorageMedium::read<int16_t>(const String &key, const int16_t defaultValue)
{
  return readShort(key, defaultValue);
}

template <>
uint16_t StorageMedium::read<uint16_t>(const String &key, const uint16_t defaultValue)
{
  return readUShort(key, defaultValue);
}

template <>
int32_t StorageMedium::read<int32_t>(const String &key, const int32_t defaultValue)
{
  return readInt(key, defaultValue);
}

template <>
int StorageMedium::read<int>(const String &key, const int defaultValue)
{
  return readInt(key, defaultValue);
}

template <>
uint32_t StorageMedium::read<uint32_t>(const String &key, const uint32_t defaultValue)
{
  return readUInt(key, defaultValue);
}

template <>
uint StorageMedium::read<uint>(const String &key, const uint defaultValue)
{
  return readUInt(key, defaultValue);
}

template <>
int64_t StorageMedium::read<int64_t>(const String &key, const int64_t defaultValue)
{
  return readLong(key, defaultValue);
}

template <>
uint64_t StorageMedium::read<uint64_t>(const String &key, const uint64_t defaultValue)
{
  return readULong(key, defaultValue);
}

template <>
float StorageMedium::read<float>(const String &key, const float defaultValue)
{
  return readFloat(key, defaultValue);
}

template <>
double StorageMedium::read<double>(const String &key, const double defaultValue)
{
  return readDouble(key, defaultValue);
}

template <>
bool StorageMedium::read<bool>(const String &key, const bool defaultValue)
{
  return readBool(key, defaultValue);
}

template <>
String StorageMedium::read<String>(const String &key, const String defaultValue)
{
  return readString(key, defaultValue);
}

template <>
void StorageMedium::write<int8_t>(const String &key, const int8_t value)
{
  writeChar(key, value);
}

template <>
void StorageMedium::write<uint8_t>(const String &key, const uint8_t value)
{
  writeUChar(key, value);
}

template <>
void StorageMedium::write<int16_t>(const String &key, const int16_t value)
{
  writeShort(key, value);
}

template <>
void StorageMedium::write<uint16_t>(const String &key, const uint16_t value)
{
  writeUShort(key, value);
}

template <>
void StorageMedium::write<int32_t>(const String &key, const int32_t value)
{
  writeInt(key, value);
}

template <>
void StorageMedium::write<uint32_t>(const String &key, const uint32_t value)
{
  writeUInt(key, value);
}

template <>
void StorageMedium::write<int64_t>(const String &key, const int64_t value)
{
  writeLong(key, value);
}

template <>
void StorageMedium::write<uint64_t>(const String &key, const uint64_t value)
{
  writeULong(key, value);
}

template <>
void StorageMedium::write<float>(const String &key, const float value)
{
  writeFloat(key, value);
}

template <>
void StorageMedium::write<double>(const String &key, const double value)
{
  writeDouble(key, value);
}

template <>
void StorageMedium::write<bool>(const String &key, const bool value)
{
  writeBool(key, value);
}

template <>
void StorageMedium::write<String>(const String &key, const String value)
{
  writeString(key, value);
}
#pragma endregion

bool StorageMedium::exists(const String &fileName)
{
  if (fileName.isEmpty())
    return false;
  return existsImpl(fileName);
}

bool StorageMedium::isComplete(const String &fileName, const std::vector<ParameterInfo> &parameters)
{
  if (fileName.isEmpty() || !exists(fileName))
    return false;
  return isCompleteImpl(fileName, parameters);
}

bool StorageMedium::deleteConfig(const String &fileName)
{
  if (fileName.isEmpty() || !exists(fileName))
    return false;
  return deleteImpl(fileName);
}

StorageMedium::FileHandler StorageMedium::createFileHandler(const String &fileName, const FileMode fileMode)
{
  bool isOpen = openFile(fileName, fileMode);
  FileHandler handler(*this, isOpen, [this]()
                      { closeFile(); });
  return handler;
}
