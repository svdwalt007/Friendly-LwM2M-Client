/*
 * UsbManagement Info (Object 10516)
 * Walt Technologies - OpenWRT One Router
 */

#ifndef WPP_USB_MANAGEMENT_INFO_H
#define WPP_USB_MANAGEMENT_INFO_H

#include "Lwm2mObjectBase.h"

#ifdef OBJ_W_10516_USB_MANAGEMENT

#define USB_MANAGEMENT_OBJECT_ID 10516

static const Lwm2mObjectMetaInfo USB_MANAGEMENT_META_INFO = {
    .uri = "urn:waltech:lwm2m:10516",
    .id = USB_MANAGEMENT_OBJECT_ID,
    .version = {1, 0},
    .multi = true  // Multiple instances - one per USB port
};

#endif // OBJ_W_10516_USB_MANAGEMENT
#endif // WPP_USB_MANAGEMENT_INFO_H
