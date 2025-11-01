#
# Copyright (C) 2024 Friendly Technologies
#
# This is free software, licensed under the MIT License.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=lwm2m-client
PKG_VERSION:=1.0.0
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/Friendly-Technologies/Friendly-LwM2M-Client.git
PKG_SOURCE_VERSION:=HEAD
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz

PKG_MAINTAINER:=Friendly Technologies
PKG_LICENSE:=MIT
PKG_LICENSE_FILES:=LICENSE

PKG_BUILD_PARALLEL:=1
CMAKE_INSTALL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/lwm2m-client
  SECTION:=net
  CATEGORY:=Network
  TITLE:=LwM2M Client for OpenWRT
  URL:=https://github.com/Friendly-Technologies/Friendly-LwM2M-Client
  DEPENDS:=+libuci +libubox +libubus +libstdcpp +libpthread
  USERID:=lwm2m=200:lwm2m=200
endef

define Package/lwm2m-client/description
  Lightweight M2M (LwM2M) client implementation for OpenWRT.
  Provides device management capabilities via LwM2M protocol.
  Features:
  - UCI configuration interface
  - ubus integration for local control
  - Hardware monitoring via sysfs
  - Support for multiple LwM2M servers
  - PSK, RPK, and no-security modes
endef

define Package/lwm2m-client/conffiles
/etc/config/lwm2m
endef

# CMake build options
CMAKE_OPTIONS += \
	-DCMAKE_BUILD_TYPE=MinSizeRel \
	-DBUILD_SHARED_LIBS=OFF \
	-DOPENWRT_BUILD=ON \
	-DOBJ_M_0_LWM2M_SECURITY=ON \
	-DOBJ_M_1_LWM2M_SERVER=ON \
	-DOBJ_M_3_DEVICE=ON \
	-DOBJ_O_4_CONNECTIVITY_MONITORING=ON \
	-DLWM2M_SUPPORT_TLV=ON \
	-DLWM2M_SUPPORT_JSON=ON \
	-DLWM2M_SUPPORT_SENML_JSON=ON

# Additional flags for size optimization
TARGET_CFLAGS += -Os -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti
TARGET_LDFLAGS += -Wl,--gc-sections

define Build/Prepare
	$(call Build/Prepare/Default)
	# Initialize git submodules if needed
	cd $(PKG_BUILD_DIR) && \
		if [ -d .git ]; then \
			git submodule update --init --recursive; \
		fi
endef

define Package/lwm2m-client/install
	# Install binary
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/lwm2m-client $(1)/usr/sbin/

	# Install init script
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/etc/init.d/lwm2m $(1)/etc/init.d/lwm2m

	# Install default configuration
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./files/etc/config/lwm2m $(1)/etc/config/lwm2m

	# Install ubus integration
	$(INSTALL_DIR) $(1)/usr/share/lwm2m
	# $(INSTALL_DATA) ./files/lwm2m-ubus.json $(1)/usr/share/lwm2m/
endef

define Package/lwm2m-client/postinst
#!/bin/sh
[ -z "$${IPKG_INSTROOT}" ] || exit 0

# Enable and start service
/etc/init.d/lwm2m enable
/etc/init.d/lwm2m start

echo "LwM2M client installed successfully"
echo "Configure via: /etc/config/lwm2m"
echo "Control via: ubus call lwm2m <command>"
exit 0
endef

define Package/lwm2m-client/prerm
#!/bin/sh
[ -z "$${IPKG_INSTROOT}" ] || exit 0

# Stop and disable service
/etc/init.d/lwm2m stop
/etc/init.d/lwm2m disable
exit 0
endef

$(eval $(call BuildPackage,lwm2m-client))
