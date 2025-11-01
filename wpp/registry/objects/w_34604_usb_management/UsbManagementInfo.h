/*
 * UsbManagement Info (Object 34604)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_USB_MANAGEMENT_INFO_H
#define WPP_USB_MANAGEMENT_INFO_H

#include "Lwm2mObjectBase.h"

#ifdef OBJ_W_34604_USB_MANAGEMENT

#define USB_MANAGEMENT_OBJECT_ID 34604

static const Lwm2mObjectMetaInfo USB_MANAGEMENT_META_INFO = {
    .uri = "urn:waltech:lwm2m:34604",
    .id = USB_MANAGEMENT_OBJECT_ID,
    .version = {1, 0},
    .multi = true  // Multiple instances - one per USB port
};

#endif // OBJ_W_34604_USB_MANAGEMENT
#endif // WPP_USB_MANAGEMENT_INFO_H
