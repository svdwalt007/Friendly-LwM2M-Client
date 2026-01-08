--[[
LwM2M Client - Security Configuration

This CBI model provides security configuration for LwM2M servers,
including PSK, RPK, Certificate, and NoSec modes.

Copyright (C) 2024 Friendly Technologies
License: MIT
]]--

local uci = require "luci.model.uci".cursor()
local sys = require "luci.sys"
local util = require "luci.util"

-- Main map
m = Map("lwm2m",
	translate("LwM2M Client - Security Settings"),
	translate("Configure security credentials for LwM2M servers. " ..
		"Each server configuration should have a corresponding security configuration. " ..
		"Security modes: NoSec (no security), PSK (Pre-Shared Key), " ..
		"RPK (Raw Public Key), Certificate (X.509 certificates)."))

-- Apply/Save hooks
function m.on_after_commit(self)
	-- Reload the client configuration after saving
	sys.call("/etc/init.d/lwm2m reload 2>/dev/null")
end

--------------------------------------------------------------------------------
-- Security Configuration Section
--------------------------------------------------------------------------------
s = m:section(TypedSection, "security",
	translate("Security Configuration"),
	translate("Configure security credentials for each LwM2M server."))
s.anonymous = false
s.addremove = true
s.template = "cbi/tblsection"

function s.create(self, section)
	local new_section = TypedSection.create(self, section)
	-- Set default values for new security config
	uci:set("lwm2m", new_section, "mode", "3") -- NoSec by default
	uci:save("lwm2m")
	return new_section
end

-- Security name (section name)
name = s:option(DummyValue, ".name", translate("Security Name"))
name.width = "20%"

-- Security Mode
mode = s:option(ListValue, "mode", translate("Security Mode"))
mode:value("3", translate("NoSec (No Security)"))
mode:value("0", translate("PSK (Pre-Shared Key)"))
mode:value("1", translate("RPK (Raw Public Key)"))
mode:value("2", translate("Certificate (X.509)"))
mode.default = "3"
mode.rmempty = false
mode.width = "30%"

-- Quick status display
status = s:option(DummyValue, "_status", translate("Status"))
status.rawhtml = true
function status.cfgvalue(self, section)
	local sec_mode = uci:get("lwm2m", section, "mode")
	local mode_name = {
		["0"] = "PSK",
		["1"] = "RPK",
		["2"] = "Certificate",
		["3"] = "NoSec"
	}
	local color = {
		["0"] = "green",
		["1"] = "green",
		["2"] = "green",
		["3"] = "orange"
	}
	return string.format('<span style="color: %s; font-weight: bold;">%s</span>',
		color[sec_mode] or "gray",
		mode_name[sec_mode] or "Unknown")
end

--------------------------------------------------------------------------------
-- Detailed Security Configuration (Edit Page)
--------------------------------------------------------------------------------
-- We need to show different fields based on security mode

-- PSK Identity (for PSK mode)
psk_id = s:option(Value, "psk_identity",
	translate("PSK Identity"),
	translate("Pre-Shared Key identity string (plain text or hex)"))
psk_id.placeholder = "myidentity"
psk_id.password = false
psk_id.rmempty = true
psk_id:depends("mode", "0")

function psk_id.validate(self, value, section)
	local sec_mode = m:formvalue("cbid.lwm2m." .. section .. ".mode")
	if sec_mode == "0" then
		if not value or #value == 0 then
			return nil, translate("PSK identity is required for PSK mode")
		end
	end
	return value
end

-- PSK Key (for PSK mode)
psk_key = s:option(Value, "psk_key",
	translate("PSK Key"),
	translate("Pre-Shared Key in hexadecimal format (e.g., 73656372657470736b6b6579313233)"))
psk_key.placeholder = "hex string (e.g., 73656372657470736b6b6579313233)"
psk_key.password = true
psk_key.rmempty = true
psk_key:depends("mode", "0")

function psk_key.validate(self, value, section)
	local sec_mode = m:formvalue("cbid.lwm2m." .. section .. ".mode")
	if sec_mode == "0" then
		if not value or #value == 0 then
			return nil, translate("PSK key is required for PSK mode")
		end
		-- Check if valid hex string
		if not value:match("^[0-9a-fA-F]+$") then
			return nil, translate("PSK key must be a valid hexadecimal string")
		end
		if #value % 2 ~= 0 then
			return nil, translate("PSK key must have even number of hex digits")
		end
		if #value < 16 then
			return nil, translate("PSK key too short (minimum 8 bytes / 16 hex digits)")
		end
		if #value > 64 then
			return nil, translate("PSK key too long (maximum 32 bytes / 64 hex digits)")
		end
	end
	return value
end

-- Public Key (for RPK mode)
public_key = s:option(TextValue, "public_key",
	translate("Public Key"),
	translate("Raw public key in PEM or DER format (base64 encoded)"))
public_key.rows = 10
public_key.rmempty = true
public_key:depends("mode", "1")
public_key:depends("mode", "2")

-- Private Key (for RPK and Certificate modes)
private_key = s:option(TextValue, "private_key",
	translate("Private Key"),
	translate("Private key in PEM format"))
private_key.rows = 10
private_key.password = true
private_key.rmempty = true
private_key:depends("mode", "1")
private_key:depends("mode", "2")

-- Server Public Key (for RPK mode)
server_public_key = s:option(TextValue, "server_public_key",
	translate("Server Public Key"),
	translate("Server's raw public key in PEM or DER format (base64 encoded)"))
server_public_key.rows = 10
server_public_key.rmempty = true
server_public_key:depends("mode", "1")

-- Certificate Chain (for Certificate mode)
certificate = s:option(TextValue, "certificate",
	translate("Certificate Chain"),
	translate("X.509 certificate chain in PEM format"))
certificate.rows = 15
certificate.rmempty = true
certificate:depends("mode", "2")

-- CA Certificate (for Certificate mode)
ca_cert = s:option(TextValue, "ca_certificate",
	translate("CA Certificate"),
	translate("Certificate Authority (CA) certificate in PEM format"))
ca_cert.rows = 10
ca_cert.rmempty = true
ca_cert:depends("mode", "2")

--------------------------------------------------------------------------------
-- Helper Information Section
--------------------------------------------------------------------------------
info = m:section(SimpleSection)
info.title = translate("Security Configuration Help")
info.description = translate("Guidelines for configuring security modes")

helper = info:option(DummyValue, "_helper")
helper.rawhtml = true
helper.value = [[
	<div class="cbi-section-node">
		<h4>Security Modes</h4>
		<ul>
			<li><strong>NoSec (Mode 3):</strong> No security, plain CoAP. Only use for testing!</li>
			<li><strong>PSK (Mode 0):</strong> Pre-Shared Key, symmetric cryptography.
				<ul>
					<li>Requires: PSK Identity (string) and PSK Key (hex)</li>
					<li>Example PSK Key: 73656372657470736b6b6579313233 (hex for "secretpskkey123")</li>
					<li>Most common mode for IoT devices</li>
				</ul>
			</li>
			<li><strong>RPK (Mode 1):</strong> Raw Public Key, asymmetric cryptography.
				<ul>
					<li>Requires: Public Key, Private Key, Server Public Key</li>
					<li>More secure than PSK, less overhead than certificates</li>
				</ul>
			</li>
			<li><strong>Certificate (Mode 2):</strong> X.509 certificates.
				<ul>
					<li>Requires: Certificate Chain, Private Key, CA Certificate</li>
					<li>Most secure, requires PKI infrastructure</li>
					<li>Use PEM format for all certificates</li>
				</ul>
			</li>
		</ul>

		<h4>Generating PSK Key</h4>
		<p>To generate a random PSK key:</p>
		<pre>openssl rand -hex 16</pre>
		<p>This generates a 16-byte (128-bit) key in hexadecimal format.</p>

		<h4>Generating Certificates (for RPK/Certificate modes)</h4>
		<p>Generate a self-signed certificate for testing:</p>
		<pre>
# Generate private key
openssl ecparam -genkey -name prime256v1 -out client-key.pem

# Generate self-signed certificate
openssl req -new -x509 -key client-key.pem -out client-cert.pem -days 365
		</pre>

		<h4>Security Best Practices</h4>
		<ul>
			<li>Never use NoSec mode in production</li>
			<li>Use PSK mode for resource-constrained devices</li>
			<li>Use Certificate mode when PKI infrastructure is available</li>
			<li>Rotate PSK keys regularly</li>
			<li>Keep private keys secure and never share them</li>
			<li>Use strong random values for PSK keys (minimum 128 bits)</li>
		</ul>

		<h4>Matching Security with Server</h4>
		<p>Each server configuration (in General Settings) should reference a security
		configuration via the "Security Instance ID". Make sure the security mode matches
		what the server expects.</p>
	</div>
]]

--------------------------------------------------------------------------------
-- Testing Section
--------------------------------------------------------------------------------
test_section = m:section(SimpleSection)
test_section.title = translate("Security Testing")
test_section.description = translate("Test security configuration")

test_info = test_section:option(DummyValue, "_test_info")
test_info.rawhtml = true
test_info.value = [[
	<div class="cbi-section-node">
		<p>To test your security configuration:</p>
		<ol>
			<li>Save your security settings</li>
			<li>Ensure the server configuration references the correct security instance</li>
			<li>Restart the LwM2M client: <code>/etc/init.d/lwm2m restart</code></li>
			<li>Check logs: <code>logread | grep lwm2m</code></li>
			<li>Verify connection on the Status page</li>
		</ol>
		<p><strong>Note:</strong> Make sure your LwM2M server is configured with matching
		security credentials. PSK identity and key must match exactly on both client and server.</p>
	</div>
]]

return m
