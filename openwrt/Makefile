#
# Copyright (C) 2024-2025 Friendly Technologies
#
# This is free software, licensed under the MIT License.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=friendly-lwm2m-client
PKG_VERSION:=1.0.0
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/FriendlyTechnologies/Friendly-LwM2M-Client.git
PKG_SOURCE_VERSION:=v$(PKG_VERSION)
PKG_MIRROR_HASH:=skip

PKG_MAINTAINER:=Friendly Technologies <support@friendly-tech.com>
PKG_LICENSE:=MIT
PKG_LICENSE_FILES:=LICENSE

PKG_BUILD_PARALLEL:=1
CMAKE_INSTALL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/friendly-lwm2m-client
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Friendly LwM2M Client with Delta Firmware Updates
  URL:=https://github.com/FriendlyTechnologies/Friendly-LwM2M-Client
  DEPENDS:=+libstdcpp +libopenssl +zlib +libcurl +libbz2 +liblzma +libblkid +libpthread
endef

define Package/friendly-lwm2m-client/description
  Enhanced LwM2M client implementation supporting OMA LwM2M v1.2.1/v1.2.2
  specifications with advanced features including:
  - Delta firmware updates (BSDIFF, VCDIFF, Courgette)
  - Resumable block-wise transfers with BERT support
  - A/B partition rollback and recovery
  - Multi-component version management
  - OpenWRT MTD flash integration
endef

define Package/friendly-lwm2m-client/conffiles
/etc/config/lwm2m
/etc/friendly-lwm2m/config.json
endef

CMAKE_OPTIONS += \
	-DWITH_DTLS=ON \
	-DWITH_MBEDTLS=OFF \
	-DWITH_DELTA_FOTA=ON \
	-DWITH_BSDIFF=ON \
	-DWITH_VCDIFF=ON \
	-DWITH_COURGETTE=ON \
	-DWITH_COMPRESSION=ON \
	-DWITH_ROLLBACK=ON \
	-DBUILD_TESTS=OFF \
	-DBUILD_EXAMPLES=OFF \
	-DBUILD_SHARED_LIBS=OFF \
	-DCMAKE_BUILD_TYPE=Release

define Package/friendly-lwm2m-client/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/friendly_lwm2m_client $(1)/usr/bin/

	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libfriendly_lwm2m.* $(1)/usr/lib/

	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./files/lwm2m.config $(1)/etc/config/lwm2m

	$(INSTALL_DIR) $(1)/etc/friendly-lwm2m
	$(INSTALL_CONF) ./files/config.json $(1)/etc/friendly-lwm2m/config.json

	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/lwm2m.init $(1)/etc/init.d/lwm2m

	$(INSTALL_DIR) $(1)/usr/share/friendly-lwm2m
	$(INSTALL_DATA) ./files/objects/*.xml $(1)/usr/share/friendly-lwm2m/ 2>/dev/null || true
endef

define Package/friendly-lwm2m-client/postinst
#!/bin/sh
[ -n "$${IPKG_INSTROOT}" ] || {
	/etc/init.d/lwm2m enable
	/etc/init.d/lwm2m start
}
exit 0
endef

define Package/friendly-lwm2m-client/prerm
#!/bin/sh
[ -n "$${IPKG_INSTROOT}" ] || {
	/etc/init.d/lwm2m stop
	/etc/init.d/lwm2m disable
}
exit 0
endef

$(eval $(call BuildPackage,friendly-lwm2m-client))
