#
# Copyright (C) 2024 Friendly Technologies
#
# This is free software, licensed under the MIT License.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

LUCI_TITLE:=LwM2M Client Web Interface
LUCI_DEPENDS:=+friendly-lwm2m-client +luci-base +luci-lib-json +luci-lib-jsonc
LUCI_PKGARCH:=all

PKG_NAME:=luci-app-lwm2m
PKG_VERSION:=1.0.0
PKG_RELEASE:=1
PKG_LICENSE:=MIT
PKG_MAINTAINER:=Friendly Technologies <support@friendly-tech.com>

define Package/luci-app-lwm2m
  SECTION:=luci
  CATEGORY:=LuCI
  SUBMENU:=3. Applications
  TITLE:=$(LUCI_TITLE)
  DEPENDS:=$(LUCI_DEPENDS)
  PKGARCH:=$(LUCI_PKGARCH)
endef

define Package/luci-app-lwm2m/description
  LuCI web interface for the Friendly LwM2M Client.
  Provides configuration and status monitoring for LwM2M device management.

  Features:
  - Real-time status monitoring
  - Server and security configuration
  - LwM2M objects management
  - Service control (start/stop/restart)
  - Interactive resource operations
endef

define Build/Compile
	# Nothing to compile for LuCI modules
endef

define Package/luci-app-lwm2m/install
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/controller
	$(INSTALL_DATA) ./luasrc/controller/lwm2m.lua \
		$(1)/usr/lib/lua/luci/controller/

	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/model/cbi/lwm2m
	$(INSTALL_DATA) ./luasrc/model/cbi/lwm2m/main.lua \
		$(1)/usr/lib/lua/luci/model/cbi/lwm2m/
	$(INSTALL_DATA) ./luasrc/model/cbi/lwm2m/objects.lua \
		$(1)/usr/lib/lua/luci/model/cbi/lwm2m/
	$(INSTALL_DATA) ./luasrc/model/cbi/lwm2m/security.lua \
		$(1)/usr/lib/lua/luci/model/cbi/lwm2m/

	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/view/lwm2m
	$(INSTALL_DATA) ./luasrc/view/lwm2m/status.htm \
		$(1)/usr/lib/lua/luci/view/lwm2m/

	# Install i18n translations (if available)
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/i18n
	# Add translation files here when available
	# $(INSTALL_DATA) ./po/*/lwm2m.po $(1)/usr/lib/lua/luci/i18n/

	# Root menu entry
	$(INSTALL_DIR) $(1)/usr/share/luci/menu.d
	echo '{ "admin/services/lwm2m": { "title": "LwM2M Client", "order": 60, "action": { "type": "alias", "path": "admin/services/lwm2m/status" }, "depends": { "acl": [ "luci-app-lwm2m" ] } } }' \
		> $(1)/usr/share/luci/menu.d/luci-app-lwm2m.json

	# ACL configuration
	$(INSTALL_DIR) $(1)/usr/share/rpcd/acl.d
	$(INSTALL_DATA) ./root/usr/share/rpcd/acl.d/luci-app-lwm2m.json \
		$(1)/usr/share/rpcd/acl.d/ 2>/dev/null || \
	echo '{ "luci-app-lwm2m": { "description": "Grant access to LwM2M client interface", "read": { "ubus": { "lwm2m": [ "*" ] }, "uci": [ "lwm2m" ] }, "write": { "ubus": { "lwm2m": [ "*" ] }, "uci": [ "lwm2m" ] } } }' \
		> $(1)/usr/share/rpcd/acl.d/luci-app-lwm2m.json
endef

define Package/luci-app-lwm2m/postinst
#!/bin/sh
[ -n "$${IPKG_INSTROOT}" ] || {
	# Clear luci cache
	rm -rf /tmp/luci-indexcache /tmp/luci-modulecache
	# Restart rpcd to reload ACLs
	/etc/init.d/rpcd restart
}
exit 0
endef

define Package/luci-app-lwm2m/postrm
#!/bin/sh
[ -n "$${IPKG_INSTROOT}" ] || {
	# Clear luci cache
	rm -rf /tmp/luci-indexcache /tmp/luci-modulecache
}
exit 0
endef

$(eval $(call BuildPackage,luci-app-lwm2m))

# For LuCI build system compatibility
include ../../luci.mk

# Fallback if luci.mk is not available
ifeq ($(wildcard ../../luci.mk),)

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

define Build/Configure
endef

$(eval $(call BuildPackage,luci-app-lwm2m))

endif
