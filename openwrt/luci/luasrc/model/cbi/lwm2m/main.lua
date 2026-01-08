--[[
LwM2M Client - Main Configuration

This CBI model provides the main configuration interface for the LwM2M client,
including general settings, network configuration, and device information.

Copyright (C) 2024 Friendly Technologies
License: MIT
]]--

local uci = require "luci.model.uci".cursor()
local sys = require "luci.sys"
local http = require "luci.http"

-- Main map
m = Map("lwm2m",
	translate("LwM2M Client - General Settings"),
	translate("Configure general settings, network interface, and device information for the LwM2M client."))

-- Apply/Save hooks
function m.on_after_commit(self)
	-- Reload the client configuration after saving
	sys.call("/etc/init.d/lwm2m reload 2>/dev/null")
end

--------------------------------------------------------------------------------
-- General Settings Section
--------------------------------------------------------------------------------
s = m:section(TypedSection, "general", translate("General Settings"))
s.anonymous = true
s.addremove = false

-- Endpoint Name
endpoint = s:option(Value, "endpoint_name",
	translate("Endpoint Name"),
	translate("Unique identifier for this LwM2M client device. Must be unique per server."))
endpoint.placeholder = "openwrt-device"
endpoint.rmempty = false
endpoint.datatype = "string"

function endpoint.validate(self, value, section)
	if not value or #value == 0 then
		return nil, translate("Endpoint name cannot be empty")
	end
	if #value > 64 then
		return nil, translate("Endpoint name too long (max 64 characters)")
	end
	-- Check for valid characters (alphanumeric, dash, underscore)
	if not value:match("^[a-zA-Z0-9_-]+$") then
		return nil, translate("Endpoint name can only contain alphanumeric characters, dash and underscore")
	end
	return value
end

-- Bootstrap mode
bootstrap = s:option(Flag, "enable_bootstrap",
	translate("Enable Bootstrap"),
	translate("Enable bootstrap mode for automatic server configuration"))
bootstrap.rmempty = false
bootstrap.default = "0"

-- Log Level
loglevel = s:option(ListValue, "log_level",
	translate("Log Level"),
	translate("Verbosity level for system logs"))
loglevel:value("0", translate("Emergency"))
loglevel:value("1", translate("Alert"))
loglevel:value("2", translate("Critical"))
loglevel:value("3", translate("Error"))
loglevel:value("4", translate("Warning"))
loglevel:value("5", translate("Notice"))
loglevel:value("6", translate("Info"))
loglevel:value("7", translate("Debug"))
loglevel.default = "2"
loglevel.rmempty = false

-- Objects Config Path
objconfig = s:option(Value, "objects_config",
	translate("Objects Configuration"),
	translate("Path to custom objects configuration file (optional)"))
objconfig.placeholder = "/etc/lwm2m/objects.conf"
objconfig.rmempty = true

--------------------------------------------------------------------------------
-- Network Settings Section
--------------------------------------------------------------------------------
n = m:section(TypedSection, "network", translate("Network Settings"))
n.anonymous = true
n.addremove = false

-- Network Interface
iface = n:option(ListValue, "interface",
	translate("Network Interface"),
	translate("Network interface to use for LwM2M communication"))
iface:value("", translate("Auto"))
iface:value("lan", translate("LAN"))
iface:value("wan", translate("WAN"))
iface:value("wan6", translate("WAN6"))
-- Add all available network interfaces
uci:foreach("network", "interface", function(s)
	if s[".name"] and s[".name"] ~= "loopback" then
		iface:value(s[".name"])
	end
end)
iface.default = "wan"
iface.rmempty = false

-- Local Port
port = n:option(Value, "port",
	translate("Local Port"),
	translate("Local UDP port for CoAP communication (0 for automatic)"))
port.placeholder = "5683"
port.default = "5683"
port.datatype = "port"
port.rmempty = false

-- IPv4 Support
ipv4 = n:option(Flag, "ipv4",
	translate("Enable IPv4"),
	translate("Enable IPv4 support"))
ipv4.default = "1"
ipv4.rmempty = false

-- IPv6 Support
ipv6 = n:option(Flag, "ipv6",
	translate("Enable IPv6"),
	translate("Enable IPv6 support"))
ipv6.default = "1"
ipv6.rmempty = false

--------------------------------------------------------------------------------
-- Device Information Section
--------------------------------------------------------------------------------
d = m:section(TypedSection, "device", translate("Device Information"))
d.anonymous = true
d.addremove = false
d.description = translate("These values will be exposed via LwM2M Object 3 (Device)")

-- Manufacturer
manufacturer = d:option(Value, "manufacturer",
	translate("Manufacturer"),
	translate("Device manufacturer name"))
manufacturer.placeholder = "Friendly Technologies"
manufacturer.default = "Friendly Technologies"
manufacturer.maxlength = 64
manufacturer.rmempty = false

-- Model
model = d:option(Value, "model",
	translate("Model Number"),
	translate("Device model identifier"))
model.placeholder = "OpenWRT-LwM2M-Client"
model.default = "OpenWRT-LwM2M-Client"
model.maxlength = 64
model.rmempty = false

-- Serial Number
serial = d:option(Value, "serial",
	translate("Serial Number"),
	translate("Device serial number (leave empty for auto-generated)"))
serial.placeholder = "Auto-generated"
serial.maxlength = 64
serial.rmempty = true

-- Firmware Version
fwversion = d:option(Value, "firmware_version",
	translate("Firmware Version"),
	translate("Current firmware version string"))
fwversion.placeholder = "1.0.0"
fwversion.default = "1.0.0"
fwversion.maxlength = 32
fwversion.rmempty = false

-- Hardware Version
hwversion = d:option(Value, "hardware_version",
	translate("Hardware Version"),
	translate("Hardware version string"))
hwversion.placeholder = "1.0"
hwversion.default = "1.0"
hwversion.maxlength = 32
hwversion.rmempty = false

-- Device Type
devtype = d:option(Value, "device_type",
	translate("Device Type"),
	translate("Device type classification"))
devtype.placeholder = "Router"
devtype.default = "Router"
devtype.maxlength = 32
devtype.rmempty = false

--------------------------------------------------------------------------------
-- Server Configuration Section
--------------------------------------------------------------------------------
sv = m:section(TypedSection, "server",
	translate("LwM2M Server Configuration"),
	translate("Configure LwM2M servers. Multiple servers can be configured."))
sv.anonymous = false
sv.addremove = true
sv.template = "cbi/tblsection"
sv.extedit = subnode and subnode .. "/%s"

function sv.create(self, section)
	local new_section = TypedSection.create(self, section)
	-- Set default values for new server
	uci:set("lwm2m", new_section, "lifetime", "300")
	uci:set("lwm2m", new_section, "binding", "U")
	uci:set("lwm2m", new_section, "bootstrap", "0")
	uci:save("lwm2m")
	return new_section
end

-- Server name (section name) displayed in table
name = sv:option(DummyValue, ".name", translate("Server Name"))

-- Server URI
uri = sv:option(Value, "uri", translate("Server URI"))
uri.placeholder = "coap://leshan.eclipseprojects.io:5683"
uri.rmempty = false
uri.datatype = "string"

function uri.validate(self, value, section)
	if not value or #value == 0 then
		return nil, translate("URI cannot be empty")
	end
	-- Basic URI validation
	if not value:match("^coaps?://") then
		return nil, translate("URI must start with coap:// or coaps://")
	end
	return value
end

-- Server ID
serverid = sv:option(Value, "server_id", translate("Server ID"))
serverid.placeholder = "1"
serverid.default = "1"
serverid.datatype = "uinteger"
serverid.rmempty = false

-- Lifetime
lifetime = sv:option(Value, "lifetime", translate("Lifetime (seconds)"))
lifetime.placeholder = "300"
lifetime.default = "300"
lifetime.datatype = "range(60,86400)"
lifetime.rmempty = false

-- Binding Mode
binding = sv:option(ListValue, "binding", translate("Binding Mode"))
binding:value("U", translate("UDP (U)"))
binding:value("UQ", translate("UDP with Queue (UQ)"))
binding:value("S", translate("SMS (S)"))
binding:value("SQ", translate("SMS with Queue (SQ)"))
binding:value("US", translate("UDP + SMS (US)"))
binding:value("UQS", translate("UDP Queue + SMS (UQS)"))
binding.default = "U"
binding.rmempty = false

-- Bootstrap flag
isbootstrap = sv:option(Flag, "bootstrap", translate("Bootstrap Server"))
isbootstrap.default = "0"
isbootstrap.rmempty = false

-- Security Instance
secinstance = sv:option(Value, "security_instance", translate("Security Instance ID"))
secinstance.placeholder = "0"
secinstance.default = "0"
secinstance.datatype = "uinteger"
secinstance.rmempty = false
secinstance.description = translate("Reference to the security configuration for this server")

return m
