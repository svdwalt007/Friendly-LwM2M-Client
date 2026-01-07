/*
 * GpioControl Info (Object 34603)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_GPIO_CONTROL_INFO_H
#define WPP_GPIO_CONTROL_INFO_H

#include "Lwm2mObjectBase.h"

#ifdef OBJ_W_34603_GPIO_CONTROL

#define GPIO_CONTROL_OBJECT_ID 34603

static const Lwm2mObjectMetaInfo GPIO_CONTROL_META_INFO = {
    .uri = "urn:waltech:lwm2m:34603",
    .id = GPIO_CONTROL_OBJECT_ID,
    .version = {1, 0},
    .multi = true  // Multiple instances - one per GPIO/LED/Button
};

#endif // OBJ_W_34603_GPIO_CONTROL
#endif // WPP_GPIO_CONTROL_INFO_H
