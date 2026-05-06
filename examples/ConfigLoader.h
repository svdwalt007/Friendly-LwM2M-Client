#ifndef CONFIG_LOADER_H
#define CONFIG_LOADER_H

#include <string>
#include <map>
#include <vector>
#include <cstdint>

namespace wpp {
    class Instance;
    class Object;
}

class ConfigLoader {
public:
    ConfigLoader();
    ~ConfigLoader();

    // Load configuration from JSON file
    bool loadFromFile(const std::string& filepath);

    // Apply configuration to LwM2M objects
    bool applyConfiguration();

    // Register an object to be configured
    void registerObject(int objectId, wpp::Object* object);

private:
    struct ResourceValue {
        enum Type {
            TYPE_STRING,
            TYPE_INT,
            TYPE_FLOAT,
            TYPE_BOOL,
            TYPE_OPAQUE,
            TYPE_OBJLNK,
            TYPE_NONE
        };

        Type type;
        std::string stringValue;
        int64_t intValue;
        double floatValue;
        bool boolValue;
        std::vector<uint8_t> opaqueValue;

        ResourceValue() : type(TYPE_NONE), intValue(0), floatValue(0.0), boolValue(false) {}
    };

    struct InstanceConfig {
        int instanceId;
        std::map<int, ResourceValue> resources;
    };

    struct ObjectConfig {
        std::string name;
        std::vector<InstanceConfig> instances;
    };

    std::map<int, ObjectConfig> m_config;
    std::map<int, wpp::Object*> m_objects;

    bool parseJsonFile(const std::string& filepath);
    bool applyObjectConfig(int objectId, const ObjectConfig& config);
    bool applyInstanceConfig(wpp::Object* object, const InstanceConfig& instanceConfig);
    bool setResourceValue(wpp::Instance* instance, int resourceId, const ResourceValue& value);
};

#endif // CONFIG_LOADER_H
