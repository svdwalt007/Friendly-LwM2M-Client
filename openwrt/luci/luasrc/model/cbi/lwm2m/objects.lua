--[[
LwM2M Client - Objects Configuration

This page provides a view and management interface for LwM2M objects
registered with the client. It allows viewing object resources and
performing read/write operations on writable resources.

Copyright (C) 2024 Friendly Technologies
License: MIT
]]--

local ubus = require "ubus"
local json = require "luci.jsonc"
local sys = require "luci.sys"
local http = require "luci.http"

m = SimpleForm("lwm2m_objects",
	translate("LwM2M Objects"),
	translate("View and manage LwM2M objects registered with the client. " ..
		"Read-only view of objects, instances, and resources. " ..
		"Use the Status page for interactive resource operations."))

m.reset = false
m.submit = false

--------------------------------------------------------------------------------
-- Get objects from ubus
--------------------------------------------------------------------------------
local function get_objects()
	local conn = ubus.connect()
	if not conn then
		return nil, "Failed to connect to ubus"
	end

	local status, result = pcall(function()
		return conn:call("lwm2m", "list_objects", {})
	end)

	conn:close()

	if not status or not result then
		return nil, "Failed to get objects list"
	end

	return result, nil
end

local objects, err = get_objects()

--------------------------------------------------------------------------------
-- Display section
--------------------------------------------------------------------------------
s = m:section(Table)

if err then
	-- Show error message
	s.template = "cbi/nullsection"

	local msg = m:section(SimpleSection)
	msg.template = "lwm2m/error"
	msg.error = err
	msg.description = translate("Make sure the LwM2M client service is running.")

	return m
end

-- Object definitions for display names
local object_names = {
	[0] = "Security",
	[1] = "Server",
	[2] = "Access Control",
	[3] = "Device",
	[4] = "Connectivity Monitoring",
	[5] = "Firmware Update",
	[6] = "Location",
	[7] = "Connectivity Statistics",
	[12] = "WLAN Connectivity",
	[13] = "Bearer Selection",
	[3331] = "Memory",
	[3339] = "Audio Clip",
	[34601] = "Router Management",
	[34602] = "Ethernet Interface",
	[34603] = "GPIO Control",
	[34604] = "USB Management",
	[34605] = "Storage Management",
	[34606] = "System Monitor"
}

if objects and type(objects) == "table" then
	-- Create sections for each object
	for _, obj in ipairs(objects) do
		local obj_id = obj.object_id or obj.id
		local obj_name = object_names[obj_id] or ("Object " .. obj_id)

		local obj_section = m:section(SimpleSection)
		obj_section.title = string.format("Object %d - %s", obj_id, obj_name)
		obj_section.description = obj.description or ""

		-- Create info display
		local info = obj_section:option(DummyValue, "_info")
		info.rawhtml = true
		info.value = string.format([[
			<div class="cbi-section-node">
				<table class="table" style="width: 100%%;">
					<tr>
						<th style="width: 30%%;">Object ID</th>
						<td>%d</td>
					</tr>
					<tr>
						<th>Name</th>
						<td>%s</td>
					</tr>
					<tr>
						<th>Instances</th>
						<td>%s</td>
					</tr>
					<tr>
						<th>Status</th>
						<td>%s</td>
					</tr>
				</table>
			</div>
		]],
			obj_id,
			obj_name,
			obj.instances and table.concat(obj.instances, ", ") or "0",
			obj.enabled and "Enabled" or "Disabled"
		)
	end
else
	-- No objects found
	local msg = m:section(SimpleSection)
	msg.template = "cbi/nullsection"
	msg.title = translate("No Objects Registered")
	msg.description = translate("The LwM2M client has not registered any objects yet. " ..
		"Make sure the client is properly configured and connected to a server.")
end

--------------------------------------------------------------------------------
-- Firmware Update Object (5) Details
--------------------------------------------------------------------------------
if objects then
	for _, obj in ipairs(objects) do
		if obj.object_id == 5 or obj.id == 5 then
			local fw_section = m:section(SimpleSection)
			fw_section.title = translate("Firmware Update Status")
			fw_section.description = translate("Current status of firmware updates")

			-- Try to read firmware update resources
			local conn = ubus.connect()
			if conn then
				-- State resource (5/0/3)
				local state_result = conn:call("lwm2m", "resource_read", {
					object_id = 5,
					instance_id = 0,
					resource_id = 3
				})

				-- Update Result resource (5/0/5)
				local result_result = conn:call("lwm2m", "resource_read", {
					object_id = 5,
					instance_id = 0,
					resource_id = 5
				})

				conn:close()

				local state_names = {
					[0] = "Idle",
					[1] = "Downloading",
					[2] = "Downloaded",
					[3] = "Updating"
				}

				local result_names = {
					[0] = "Default",
					[1] = "Success",
					[2] = "Not enough storage",
					[3] = "Out of memory",
					[4] = "Connection lost",
					[5] = "CRC check failure",
					[6] = "Unsupported package type",
					[7] = "Invalid URI",
					[8] = "Update failed",
					[9] = "Unsupported protocol"
				}

				local state = state_result and state_result.value or 0
				local result = result_result and result_result.value or 0

				local fw_info = fw_section:option(DummyValue, "_fw_info")
				fw_info.rawhtml = true
				fw_info.value = string.format([[
					<div class="cbi-section-node">
						<table class="table" style="width: 100%%;">
							<tr>
								<th style="width: 30%%;">Update State</th>
								<td>%s</td>
							</tr>
							<tr>
								<th>Last Result</th>
								<td>%s</td>
							</tr>
						</table>
					</div>
				]],
					state_names[tonumber(state)] or "Unknown",
					result_names[tonumber(result)] or "Unknown"
				)
			end
			break
		end
	end
end

--------------------------------------------------------------------------------
-- Custom Objects Information
--------------------------------------------------------------------------------
local custom_section = m:section(SimpleSection)
custom_section.title = translate("Object Information")
custom_section.description = translate("Standard OMA LwM2M objects (0-32767) and custom objects (34601-34649)")

local obj_info = custom_section:option(DummyValue, "_obj_info")
obj_info.rawhtml = true
obj_info.value = [[
	<div class="cbi-section-node">
		<h4>Standard Objects (OMA Registered)</h4>
		<ul>
			<li><strong>Object 0:</strong> Security - Bootstrap and security credentials</li>
			<li><strong>Object 1:</strong> Server - Server configuration and lifecycle</li>
			<li><strong>Object 3:</strong> Device - Device information and control</li>
			<li><strong>Object 4:</strong> Connectivity Monitoring - Network status and statistics</li>
			<li><strong>Object 5:</strong> Firmware Update - OTA firmware management</li>
			<li><strong>Object 12:</strong> WLAN Connectivity - WiFi configuration and status</li>
			<li><strong>Object 13:</strong> Bearer Selection - Network bearer selection</li>
		</ul>
		<h4>Custom Objects (Walt Technologies)</h4>
		<ul>
			<li><strong>Object 34601:</strong> Router Management - LAN/WAN, DHCP, DNS, firewall</li>
			<li><strong>Object 34602:</strong> Ethernet Interface - Port statistics and link status</li>
			<li><strong>Object 34603:</strong> GPIO Control - LED, button, and GPIO management</li>
			<li><strong>Object 34604:</strong> USB Management - USB port monitoring and control</li>
			<li><strong>Object 34605:</strong> Storage Management - NAND, NVMe, USB storage</li>
			<li><strong>Object 34606:</strong> System Monitor - CPU, memory, thermal monitoring</li>
		</ul>
		<p><em>Use the Status page to read and write individual resource values.</em></p>
	</div>
]]

return m
