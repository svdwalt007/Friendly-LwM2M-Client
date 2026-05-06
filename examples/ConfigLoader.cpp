#include "ConfigLoader.h"
#include "WppRegistry.h"
#include "Object.h"
#include "Instance.h"
#include "Resource.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>

// Simple JSON parser for our specific use case
class SimpleJsonParser {
public:
    struct JsonValue {
        enum Type { OBJECT, ARRAY, STRING, NUMBER, BOOLEAN, NULL_TYPE };
        Type type;
        std::string stringValue;
        double numberValue;
        bool boolValue;
        std::map<std::string, JsonValue> objectValue;
        std::vector<JsonValue> arrayValue;

        JsonValue() : type(NULL_TYPE), numberValue(0), boolValue(false) {}
    };

    static bool parse(const std::string& json, JsonValue& result) {
        size_t pos = 0;
        return parseValue(json, pos, result);
    }

private:
    static void skipWhitespace(const std::string& str, size_t& pos) {
        while (pos < str.length() && std::isspace(str[pos])) {
            pos++;
        }
    }

    static bool parseValue(const std::string& str, size_t& pos, JsonValue& result) {
        skipWhitespace(str, pos);
        if (pos >= str.length()) return false;

        char ch = str[pos];
        if (ch == '{') return parseObject(str, pos, result);
        if (ch == '[') return parseArray(str, pos, result);
        if (ch == '"') return parseString(str, pos, result);
        if (ch == 't' || ch == 'f') return parseBoolean(str, pos, result);
        if (ch == 'n') return parseNull(str, pos, result);
        if (ch == '-' || std::isdigit(ch)) return parseNumber(str, pos, result);

        return false;
    }

    static bool parseObject(const std::string& str, size_t& pos, JsonValue& result) {
        result.type = JsonValue::OBJECT;
        pos++; // skip '{'
        skipWhitespace(str, pos);

        if (pos < str.length() && str[pos] == '}') {
            pos++;
            return true;
        }

        while (pos < str.length()) {
            skipWhitespace(str, pos);

            // Parse key
            JsonValue key;
            if (!parseString(str, pos, key)) return false;

            skipWhitespace(str, pos);
            if (pos >= str.length() || str[pos] != ':') return false;
            pos++; // skip ':'

            // Parse value
            JsonValue value;
            if (!parseValue(str, pos, value)) return false;

            result.objectValue[key.stringValue] = value;

            skipWhitespace(str, pos);
            if (pos >= str.length()) return false;

            if (str[pos] == '}') {
                pos++;
                return true;
            }

            if (str[pos] != ',') return false;
            pos++; // skip ','
        }

        return false;
    }

    static bool parseArray(const std::string& str, size_t& pos, JsonValue& result) {
        result.type = JsonValue::ARRAY;
        pos++; // skip '['
        skipWhitespace(str, pos);

        if (pos < str.length() && str[pos] == ']') {
            pos++;
            return true;
        }

        while (pos < str.length()) {
            JsonValue value;
            if (!parseValue(str, pos, value)) return false;
            result.arrayValue.push_back(value);

            skipWhitespace(str, pos);
            if (pos >= str.length()) return false;

            if (str[pos] == ']') {
                pos++;
                return true;
            }

            if (str[pos] != ',') return false;
            pos++; // skip ','
        }

        return false;
    }

    static bool parseString(const std::string& str, size_t& pos, JsonValue& result) {
        result.type = JsonValue::STRING;
        pos++; // skip opening '"'

        std::string value;
        while (pos < str.length()) {
            char ch = str[pos];
            if (ch == '"') {
                pos++;
                result.stringValue = value;
                return true;
            }
            if (ch == '\\' && pos + 1 < str.length()) {
                pos++;
                ch = str[pos];
                if (ch == 'n') value += '\n';
                else if (ch == 't') value += '\t';
                else if (ch == 'r') value += '\r';
                else value += ch;
            } else {
                value += ch;
            }
            pos++;
        }

        return false;
    }

    static bool parseNumber(const std::string& str, size_t& pos, JsonValue& result) {
        result.type = JsonValue::NUMBER;
        size_t start = pos;

        if (str[pos] == '-') pos++;

        while (pos < str.length() && std::isdigit(str[pos])) pos++;

        if (pos < str.length() && str[pos] == '.') {
            pos++;
            while (pos < str.length() && std::isdigit(str[pos])) pos++;
        }

        if (pos < str.length() && (str[pos] == 'e' || str[pos] == 'E')) {
            pos++;
            if (pos < str.length() && (str[pos] == '+' || str[pos] == '-')) pos++;
            while (pos < str.length() && std::isdigit(str[pos])) pos++;
        }

        std::string numStr = str.substr(start, pos - start);
        result.numberValue = std::stod(numStr);
        return true;
    }

    static bool parseBoolean(const std::string& str, size_t& pos, JsonValue& result) {
        result.type = JsonValue::BOOLEAN;
        if (str.substr(pos, 4) == "true") {
            result.boolValue = true;
            pos += 4;
            return true;
        }
        if (str.substr(pos, 5) == "false") {
            result.boolValue = false;
            pos += 5;
            return true;
        }
        return false;
    }

    static bool parseNull(const std::string& str, size_t& pos, JsonValue& result) {
        if (str.substr(pos, 4) == "null") {
            result.type = JsonValue::NULL_TYPE;
            pos += 4;
            return true;
        }
        return false;
    }
};

ConfigLoader::ConfigLoader() {
}

ConfigLoader::~ConfigLoader() {
}

void ConfigLoader::registerObject(int objectId, wpp::Object* object) {
    m_objects[objectId] = object;
}

bool ConfigLoader::loadFromFile(const std::string& filepath) {
    return parseJsonFile(filepath);
}

bool ConfigLoader::parseJsonFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open configuration file: " << filepath << std::endl;
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string jsonContent = buffer.str();
    file.close();

    SimpleJsonParser::JsonValue root;
    if (!SimpleJsonParser::parse(jsonContent, root)) {
        std::cerr << "Failed to parse JSON configuration" << std::endl;
        return false;
    }

    if (root.type != SimpleJsonParser::JsonValue::OBJECT) {
        std::cerr << "Root JSON element must be an object" << std::endl;
        return false;
    }

    auto lwm2mObjectsIt = root.objectValue.find("lwm2m_objects");
    if (lwm2mObjectsIt == root.objectValue.end()) {
        std::cerr << "Missing 'lwm2m_objects' key in configuration" << std::endl;
        return false;
    }

    const auto& lwm2mObjects = lwm2mObjectsIt->second;
    if (lwm2mObjects.type != SimpleJsonParser::JsonValue::OBJECT) {
        std::cerr << "'lwm2m_objects' must be an object" << std::endl;
        return false;
    }

    // Parse each object configuration
    for (const auto& objEntry : lwm2mObjects.objectValue) {
        int objectId = std::stoi(objEntry.first);
        const auto& objValue = objEntry.second;

        if (objValue.type != SimpleJsonParser::JsonValue::OBJECT) {
            std::cerr << "Object " << objectId << " configuration must be an object" << std::endl;
            continue;
        }

        ObjectConfig objConfig;

        auto nameIt = objValue.objectValue.find("name");
        if (nameIt != objValue.objectValue.end() && nameIt->second.type == SimpleJsonParser::JsonValue::STRING) {
            objConfig.name = nameIt->second.stringValue;
        }

        auto instancesIt = objValue.objectValue.find("instances");
        if (instancesIt == objValue.objectValue.end() || instancesIt->second.type != SimpleJsonParser::JsonValue::ARRAY) {
            std::cerr << "Object " << objectId << " missing 'instances' array" << std::endl;
            continue;
        }

        // Parse instances
        for (const auto& instValue : instancesIt->second.arrayValue) {
            if (instValue.type != SimpleJsonParser::JsonValue::OBJECT) continue;

            InstanceConfig instConfig;

            auto idIt = instValue.objectValue.find("id");
            if (idIt != instValue.objectValue.end() && idIt->second.type == SimpleJsonParser::JsonValue::NUMBER) {
                instConfig.instanceId = static_cast<int>(idIt->second.numberValue);
            } else {
                continue;
            }

            auto resourcesIt = instValue.objectValue.find("resources");
            if (resourcesIt == instValue.objectValue.end() || resourcesIt->second.type != SimpleJsonParser::JsonValue::OBJECT) {
                continue;
            }

            // Parse resources
            for (const auto& resEntry : resourcesIt->second.objectValue) {
                int resourceId = std::stoi(resEntry.first);
                const auto& resValue = resEntry.second;

                ResourceValue rv;
                switch (resValue.type) {
                    case SimpleJsonParser::JsonValue::STRING:
                        rv.type = ResourceValue::TYPE_STRING;
                        rv.stringValue = resValue.stringValue;
                        break;
                    case SimpleJsonParser::JsonValue::NUMBER:
                        // Check if the number is an integer by comparing with truncated value
                        {
                            int64_t intPart = static_cast<int64_t>(resValue.numberValue);
                            double fracPart = resValue.numberValue - static_cast<double>(intPart);
                            if (fracPart < 0.0001 && fracPart > -0.0001) {
                                rv.type = ResourceValue::TYPE_INT;
                                rv.intValue = intPart;
                            } else {
                                rv.type = ResourceValue::TYPE_FLOAT;
                                rv.floatValue = resValue.numberValue;
                            }
                        }
                        break;
                    case SimpleJsonParser::JsonValue::BOOLEAN:
                        rv.type = ResourceValue::TYPE_BOOL;
                        rv.boolValue = resValue.boolValue;
                        break;
                    case SimpleJsonParser::JsonValue::OBJECT:
                    case SimpleJsonParser::JsonValue::ARRAY:
                        // For complex types, we'll skip for now
                        // These would need special handling for multiple resources
                        rv.type = ResourceValue::TYPE_NONE;
                        break;
                    default:
                        rv.type = ResourceValue::TYPE_NONE;
                        break;
                }

                if (rv.type != ResourceValue::TYPE_NONE) {
                    instConfig.resources[resourceId] = rv;
                }
            }

            objConfig.instances.push_back(instConfig);
        }

        m_config[objectId] = objConfig;
    }

    std::cout << "Loaded configuration for " << m_config.size() << " objects" << std::endl;
    return true;
}

bool ConfigLoader::applyConfiguration() {
    int successCount = 0;
    int failCount = 0;

    for (const auto& entry : m_config) {
        int objectId = entry.first;
        const ObjectConfig& config = entry.second;

        if (applyObjectConfig(objectId, config)) {
            successCount++;
        } else {
            failCount++;
        }
    }

    std::cout << "Configuration applied: " << successCount << " objects configured, "
              << failCount << " failed" << std::endl;

    return failCount == 0;
}

bool ConfigLoader::applyObjectConfig(int objectId, const ObjectConfig& config) {
    auto objIt = m_objects.find(objectId);
    if (objIt == m_objects.end()) {
        std::cerr << "Object " << objectId << " (" << config.name << ") not registered" << std::endl;
        return false;
    }

    wpp::Object* object = objIt->second;
    std::cout << "Configuring Object " << objectId << " (" << config.name << ")" << std::endl;

    for (const auto& instanceConfig : config.instances) {
        if (!applyInstanceConfig(object, instanceConfig)) {
            std::cerr << "Failed to configure instance " << instanceConfig.instanceId
                      << " of object " << objectId << std::endl;
            return false;
        }
    }

    return true;
}

bool ConfigLoader::applyInstanceConfig(wpp::Object* object, const InstanceConfig& instanceConfig) {
    wpp::Instance* instance = object->instance(instanceConfig.instanceId);
    if (!instance) {
        std::cerr << "Instance " << instanceConfig.instanceId << " not found in object" << std::endl;
        return false;
    }

    int resourcesSet = 0;
    for (const auto& resEntry : instanceConfig.resources) {
        int resourceId = resEntry.first;
        const ResourceValue& value = resEntry.second;

        if (setResourceValue(instance, resourceId, value)) {
            resourcesSet++;
        }
    }

    std::cout << "  Instance " << instanceConfig.instanceId << ": "
              << resourcesSet << " resources configured" << std::endl;

    return true;
}

bool ConfigLoader::setResourceValue(wpp::Instance* instance, int resourceId, const ResourceValue& value) {
    // Check if resource exists first
    if (!instance->isExist(resourceId)) {
        // Resource might not exist, which is okay - not all resources are implemented
        return false;
    }

    bool success = false;
    switch (value.type) {
        case ResourceValue::TYPE_STRING:
            success = instance->set(resourceId, value.stringValue);
            break;
        case ResourceValue::TYPE_INT:
            success = instance->set(resourceId, static_cast<int>(value.intValue));
            break;
        case ResourceValue::TYPE_FLOAT:
            success = instance->set(resourceId, value.floatValue);
            break;
        case ResourceValue::TYPE_BOOL:
            success = instance->set(resourceId, value.boolValue);
            break;
        default:
            return false;
    }

    if (!success) {
        // Set operation failed - resource might be read-only or type mismatch
        return false;
    }

    return true;
}
