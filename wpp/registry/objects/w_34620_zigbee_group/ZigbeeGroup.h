/*
 * ZigbeeGroup (Object 34620)
 * Walt Technologies - Zigbee Integration
 *
 * Manages Zigbee groups for collective device control
 */

#ifndef WPP_W_34620_ZIGBEE_GROUP_H
#define WPP_W_34620_ZIGBEE_GROUP_H

#include "ZigbeeGroupConfig.h"
#include "ZigbeeGroupInfo.h"
#include "Instance.h"
#include "InstSubject.h"

namespace wpp {

class WppClient;
class Object;

class ZigbeeGroup : public Instance {
public:
    enum ID: ID_T {
        GROUP_ID_0 = 0,            // Group ID (RW, Integer)
        GROUP_NAME_1 = 1,          // Group Name (RW, String)
        MEMBER_DEVICES_2 = 2,      // Member Devices (R, Multiple, String)
        MEMBER_COUNT_3 = 3,        // Member Count (R, Integer)
        SCENE_COUNT_4 = 4,         // Scene Count (R, Integer)
        CURRENT_SCENE_5 = 5,       // Current Scene (RW, Integer)
        ADD_MEMBER_6 = 6,          // Add Member (E, Execute)
        REMOVE_MEMBER_7 = 7,       // Remove Member (E, Execute)
        SCENE_RECALL_8 = 8,        // Scene Recall (E, Execute)
        SCENE_STORE_9 = 9,         // Scene Store (E, Execute)
        ON_10 = 10,                // On (E, Execute)
        OFF_11 = 11,               // Off (E, Execute)
        TOGGLE_12 = 12,            // Toggle (E, Execute)
        SET_LEVEL_13 = 13,         // Set Level (E, Execute)
        SET_COLOR_14 = 14          // Set Color (E, Execute)
    };

    /* Static object methods */
    static Object& object(WppClient& client);
    static Instance* createInst(WppClient& client, INST_T instId = ID_T_MAX_VAL);
    static Instance* instance(WppClient& client, INST_T instId);
    static bool remove(WppClient& client, INST_T instId);

    /* Instance lifecycle */
    ZigbeeGroup(Object& object, INST_T instId);
    ~ZigbeeGroup() override;

protected:
    /* ObjSubject override method */
    bool validate(ID_T resId, const void *data, size_t size) override;

private:
    /* Private methods */
    bool initResources(ItemOp *) override;

    /* Execute handlers */
    static bool addMember(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool removeMember(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool sceneRecall(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool sceneStore(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool turnOn(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool turnOff(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool toggle(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool setLevel(Instance& inst, ID_T resId, const OPAQUE_T& data);
    static bool setColor(Instance& inst, ID_T resId, const OPAQUE_T& data);

    /* Helper methods */
    void updateMemberList();
    bool sendGroupCommand(uint8_t commandId, const std::vector<uint8_t>& payload);
};

} // namespace wpp

#endif // WPP_W_34620_ZIGBEE_GROUP_H
