--[[
LwM2M Client LUCI Controller

This module provides web interface for the Friendly LwM2M Client on OpenWRT.
It integrates with UCI configuration and ubus for real-time status monitoring.

Copyright (C) 2024 Friendly Technologies
License: MIT
]]--

module("luci.controller.lwm2m", package.seeall)

function index()
	-- Only show menu if package is installed
	if not nixio.fs.access("/etc/config/lwm2m") then
		return
	end

	-- Main menu entry under "Services"
	local page = entry({"admin", "services", "lwm2m"},
		alias("admin", "services", "lwm2m", "status"),
		_("LwM2M Client"), 60)
	page.dependent = true
	page.acl_depends = { "luci-app-lwm2m" }

	-- Status page (default)
	entry({"admin", "services", "lwm2m", "status"},
		template("lwm2m/status"),
		_("Status"), 1)

	-- Main configuration page
	entry({"admin", "services", "lwm2m", "main"},
		cbi("lwm2m/main"),
		_("General Settings"), 2)

	-- Objects configuration page
	entry({"admin", "services", "lwm2m", "objects"},
		cbi("lwm2m/objects"),
		_("LwM2M Objects"), 3)

	-- Security configuration page
	entry({"admin", "services", "lwm2m", "security"},
		cbi("lwm2m/security"),
		_("Security"), 4)

	-- AJAX endpoints for status updates
	entry({"admin", "services", "lwm2m", "status_json"},
		call("action_status"),
		nil).leaf = true

	entry({"admin", "services", "lwm2m", "objects_json"},
		call("action_objects"),
		nil).leaf = true

	entry({"admin", "services", "lwm2m", "resource_read"},
		call("action_resource_read"),
		nil).leaf = true

	entry({"admin", "services", "lwm2m", "resource_write"},
		call("action_resource_write"),
		nil).leaf = true

	entry({"admin", "services", "lwm2m", "action"},
		call("action_client"),
		nil).leaf = true
end

-- Get client status via ubus
function action_status()
	local ubus = require "ubus"
	local json = require "luci.jsonc"

	local conn = ubus.connect()
	if not conn then
		luci.http.prepare_content("application/json")
		luci.http.write_json({
			success = false,
			error = "Failed to connect to ubus"
		})
		return
	end

	-- Get LwM2M client status
	local status = conn:call("lwm2m", "status", {})

	-- Get service status
	local service_running = false
	local init_status = luci.sys.exec("/etc/init.d/lwm2m status")
	if init_status and init_status:match("running") then
		service_running = true
	end

	-- Get uptime
	local uptime = luci.sys.exec("cat /proc/uptime | awk '{print $1}'")

	conn:close()

	local result = {
		success = true,
		service_running = service_running,
		system_uptime = tonumber(uptime) or 0,
		client_status = status or {
			registered = false,
			endpoint = "unknown",
			state = "disconnected"
		}
	}

	luci.http.prepare_content("application/json")
	luci.http.write_json(result)
end

-- Get registered objects list
function action_objects()
	local ubus = require "ubus"
	local json = require "luci.jsonc"

	local conn = ubus.connect()
	if not conn then
		luci.http.prepare_content("application/json")
		luci.http.write_json({
			success = false,
			error = "Failed to connect to ubus"
		})
		return
	end

	local objects = conn:call("lwm2m", "list_objects", {})
	conn:close()

	local result = {
		success = true,
		objects = objects or {}
	}

	luci.http.prepare_content("application/json")
	luci.http.write_json(result)
end

-- Read resource value
function action_resource_read()
	local json = require "luci.jsonc"
	local ubus = require "ubus"

	local object_id = tonumber(luci.http.formvalue("object_id"))
	local instance_id = tonumber(luci.http.formvalue("instance_id"))
	local resource_id = tonumber(luci.http.formvalue("resource_id"))

	if not object_id or not instance_id or not resource_id then
		luci.http.prepare_content("application/json")
		luci.http.write_json({
			success = false,
			error = "Missing parameters"
		})
		return
	end

	local conn = ubus.connect()
	if not conn then
		luci.http.prepare_content("application/json")
		luci.http.write_json({
			success = false,
			error = "Failed to connect to ubus"
		})
		return
	end

	local result = conn:call("lwm2m", "resource_read", {
		object_id = object_id,
		instance_id = instance_id,
		resource_id = resource_id
	})

	conn:close()

	luci.http.prepare_content("application/json")
	luci.http.write_json({
		success = true,
		result = result
	})
end

-- Write resource value
function action_resource_write()
	local json = require "luci.jsonc"
	local ubus = require "ubus"

	local object_id = tonumber(luci.http.formvalue("object_id"))
	local instance_id = tonumber(luci.http.formvalue("instance_id"))
	local resource_id = tonumber(luci.http.formvalue("resource_id"))
	local value = luci.http.formvalue("value")

	if not object_id or not instance_id or not resource_id or not value then
		luci.http.prepare_content("application/json")
		luci.http.write_json({
			success = false,
			error = "Missing parameters"
		})
		return
	end

	local conn = ubus.connect()
	if not conn then
		luci.http.prepare_content("application/json")
		luci.http.write_json({
			success = false,
			error = "Failed to connect to ubus"
		})
		return
	end

	local result = conn:call("lwm2m", "resource_write", {
		object_id = object_id,
		instance_id = instance_id,
		resource_id = resource_id,
		value = value
	})

	conn:close()

	luci.http.prepare_content("application/json")
	luci.http.write_json({
		success = true,
		result = result
	})
end

-- Client actions (register, update, deregister, reload)
function action_client()
	local json = require "luci.jsonc"
	local ubus = require "ubus"

	local action = luci.http.formvalue("action")

	if not action then
		luci.http.prepare_content("application/json")
		luci.http.write_json({
			success = false,
			error = "Missing action parameter"
		})
		return
	end

	local conn = ubus.connect()
	if not conn then
		luci.http.prepare_content("application/json")
		luci.http.write_json({
			success = false,
			error = "Failed to connect to ubus"
		})
		return
	end

	local result
	if action == "register" then
		result = conn:call("lwm2m", "register", {})
	elseif action == "update" then
		result = conn:call("lwm2m", "update", {})
	elseif action == "deregister" then
		result = conn:call("lwm2m", "deregister", {})
	elseif action == "reload" then
		result = conn:call("lwm2m", "reload_config", {})
	elseif action == "restart" then
		luci.sys.call("/etc/init.d/lwm2m restart")
		result = { message = "Service restarting" }
	elseif action == "start" then
		luci.sys.call("/etc/init.d/lwm2m start")
		result = { message = "Service started" }
	elseif action == "stop" then
		luci.sys.call("/etc/init.d/lwm2m stop")
		result = { message = "Service stopped" }
	else
		conn:close()
		luci.http.prepare_content("application/json")
		luci.http.write_json({
			success = false,
			error = "Unknown action"
		})
		return
	end

	conn:close()

	luci.http.prepare_content("application/json")
	luci.http.write_json({
		success = true,
		result = result
	})
end
