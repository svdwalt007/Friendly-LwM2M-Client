#
# Copyright (C) 2025 Walt Technologies
#
# This is free software, licensed under the GNU General Public License v2.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=friendly-lwm2m-client
PKG_VERSION:=1.0.0
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/Friendly-Technologies/Friendly-LwM2M-Client.git
PKG_SOURCE_VERSION:=HEAD
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_SOURCE_SUBDIR)

PKG_LICENSE:=Apache-2.0
PKG_LICENSE_FILES:=LICENSE
PKG_MAINTAINER:=Walt Technologies <contact@walttech.com>

PKG_BUILD_PARALLEL:=1
CMAKE_INSTALL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/friendly-lwm2m-client
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=IoT
  TITLE:=OMA LwM2M Client for OpenWRT One
  DEPENDS:=+libc +libstdcpp +libopenssl +libuci +libubox +libubus +libiwinfo
  URL:=https://github.com/Friendly-Technologies/Friendly-LwM2M-Client
endef

define Package/friendly-lwm2m-client/description
 Comprehensive OMA LwM2M client implementation for OpenWRT One router.
 Provides full device management capabilities including:
 - WiFi management (Object 12)
 - Network bearer selection (Object 13)
 - System resources monitoring (Object 34606)
 - GPIO/LED control (Object 34603)
 - Storage management (Object 34605)
 - Router configuration (Object 34601)
 - Ethernet interface management (Object 34602)
 - USB port management (Object 34604)

 Bootstrap Server: coap://demo-iot.friendly-tech.com:5680
 Endpoint Name: walttech888
endef

CMAKE_OPTIONS += \
	-DCMAKE_BUILD_TYPE=MinSizeRel \
	-DCMAKE_SYSTEM_NAME=Linux \
	-DCMAKE_C_COMPILER=$(TARGET_CC) \
	-DCMAKE_CXX_COMPILER=$(TARGET_CXX) \
	-DCMAKE_CXX_STANDARD=17 \
	-DOPENWRT_BUILD=ON \
	-DOPENWRT_VERSION="24.10.4" \
	-DLWM2M_BOOTSTRAP=ON

define Package/friendly-lwm2m-client/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/WppExample $(1)/usr/sbin/lwm2m-client

	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/lwm2m-client.init $(1)/etc/init.d/lwm2m-client

	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./files/lwm2m-client.config $(1)/etc/config/lwm2m-client

	$(INSTALL_DIR) $(1)/etc/lwm2m
	$(INSTALL_DATA) ./files/lwm2m-endpoint.conf $(1)/etc/lwm2m/endpoint.conf
endef

define Package/friendly-lwm2m-client/conffiles
/etc/config/lwm2m-client
/etc/lwm2m/endpoint.conf
endef

define Package/friendly-lwm2m-client/postinst
#!/bin/sh
if [ -z "$${IPKG_INSTROOT}" ]; then
	echo "Enabling lwm2m-client service..."
	/etc/init.d/lwm2m-client enable
	/etc/init.d/lwm2m-client start
fi
exit 0
endef

define Package/friendly-lwm2m-client/prerm
#!/bin/sh
if [ -z "$${IPKG_INSTROOT}" ]; then
	echo "Stopping lwm2m-client service..."
	/etc/init.d/lwm2m-client stop
	/etc/init.d/lwm2m-client disable
fi
exit 0
endef

$(eval $(call BuildPackage,friendly-lwm2m-client))
