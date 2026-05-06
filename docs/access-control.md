# LwM2M Access Control

This document describes how Access Control works in the Friendly LwM2M Client and the critical rule that every object instance must have corresponding Access Control coverage.

## Overview

The LwM2M Access Control Object (Object ID: 2) determines which LwM2M Servers have permission to access other object instances. Without proper Access Control configuration, servers will receive `4.01 Unauthorized` responses when attempting to read, write, or execute operations on objects.

## The AC Coverage Rule

**Every object instance MUST have a corresponding Access Control instance that grants the LwM2M Server access.**

This rule is enforced by the `tools/audit-acl-coverage.py` tool, which should be run in CI to gate merges.

### Why This Matters

When a server (e.g., Leshan, Friendly Technologies IoT DM) sends a CoAP GET request like `GET /6/0` (Location instance 0), the client performs an ACL check:

1. Look up the AC instance that covers Object 6, Instance 0
2. Check if the requesting server's Short Server ID (SSID) matches the Owner
3. If Owner, grant full access. If not Owner, check the ACL multi-resource.
4. If no AC instance exists, or ACL check fails: return `4.01 Unauthorized`

### Symptoms of Missing AC Coverage

- Server receives `4.01 Unauthorized` when reading object data
- Object appears in registration but is not readable
- PCAP analysis shows successful Register but failed GET requests

## How AC Instances Work

### AC for Object-Level Operations

An AC instance covering object-level operations (Create, Delete) uses:

```cpp
// Object-level AC: covers operations on Object 6 as a whole
Lwm2mAccessControl::create(Location::object(client), Lwm2mAccessControl::ALL_OBJ_RIGHTS);
```

This creates an AC instance where:
- `Object ID (Resource 0)` = 6 (Location)
- `Object Instance ID (Resource 1)` = 65535 (AC_OBJ_INST_NOT_SET)
- `Owner (Resource 3)` = 65535 (AC_CLIENT_OWNER - client-owned)

### AC for Instance-Level Operations

An AC instance covering instance-level operations (Read, Write, Execute) uses:

```cpp
// Instance-level AC: grants SSID 123 access to /6/0
Instance *location = Location::createInst(client);
Lwm2mAccessControl::create(*location, TEST_SERVER_SHORT_ID);
```

This creates an AC instance where:
- `Object ID (Resource 0)` = 6 (Location)
- `Object Instance ID (Resource 1)` = 0 (the instance ID)
- `Owner (Resource 3)` = 123 (the LwM2M Server's SSID)

### Owner Semantics

When `Owner (Resource 3)` = Server SSID:
- That server has **all rights** to the covered object instance
- The ACL multi-resource (Resource 2) can grant other servers specific rights
- If ACL is empty, only the Owner has access

### The TEST_SERVER_SHORT_ID

The constant `TEST_SERVER_SHORT_ID` is defined in `examples/objects.h`:

```cpp
#define TEST_SERVER_SHORT_ID 123
```

This must match:
1. `Short Server ID (Resource 0)` in the LwM2M Server Object (`/1/0/0`)
2. `Short Server ID (Resource 10)` in the LwM2M Security Object (`/0/0/10`)

## Adding AC for New Objects

When adding a new object, follow this pattern in `examples/objects.cpp`:

```cpp
#ifdef OBJ_O_NEW_OBJECT
void newObjectInit(WppClient &client) {
    // 1. Register the object
    client.registry().registerObj(NewObject::object(client));

    // 2. Create instance(s) - SAVE THE POINTER
    Instance *inst0 = NewObject::createInst(client, 0);

    // 3. Configure instance resources
    inst0->set<STRING_T>(NewObject::NAME_0, "My Instance");

    // 4. Create AC for BOTH object AND instance
    #if OBJ_O_2_LWM2M_ACCESS_CONTROL
    // Object-level AC (for Create/Delete operations on the object)
    Lwm2mAccessControl::create(NewObject::object(client), Lwm2mAccessControl::ALL_OBJ_RIGHTS);

    // Instance-level AC (for Read/Write/Execute on this instance)
    Lwm2mAccessControl::create(*inst0, TEST_SERVER_SHORT_ID);
    #endif
}
#endif
```

### Common Mistakes

**Mistake 1: Missing instance-level AC**

```cpp
// WRONG: Only creates object-level AC
Lwm2mAccessControl::create(Location::object(client), Lwm2mAccessControl::ALL_OBJ_RIGHTS);
// Result: GET /6/0 returns 4.01 Unauthorized
```

**Mistake 2: Not saving the instance pointer**

```cpp
// WRONG: Can't create AC without a reference to the instance
Location::createInst(client);  // No variable to reference
```

**Mistake 3: Creating multiple instances without AC for all**

```cpp
// WRONG: Only first instance has AC
Instance *eth0 = EthernetInterface::createInst(client, 0);
Instance *eth1 = EthernetInterface::createInst(client, 1);
Lwm2mAccessControl::create(*eth0, TEST_SERVER_SHORT_ID);
// Result: GET /10514/1 returns 4.01 Unauthorized

// CORRECT: AC for all instances
Lwm2mAccessControl::create(*eth0, TEST_SERVER_SHORT_ID);
Lwm2mAccessControl::create(*eth1, TEST_SERVER_SHORT_ID);
```

## AC Exempt Objects

Some objects do not need AC instances:

| Object | ID | Reason |
|--------|-----|--------|
| LwM2M Security | 0 | Server access controlled differently |
| LwM2M Access Control | 2 | Would be circular dependency |

## The ACL Multi-Resource

Resource 2 (ACL) is a multi-instance resource that grants specific rights to non-Owner servers:

```cpp
// Grant server SSID 456 read access to instance
Lwm2mAccessControl::addAcl(*location, 456, Lwm2mAccessControl::R_O_W_ATTR);
```

ACL values (from `Lwm2mAccessControl::ACL`):
- `NO_ACCESS` = 0
- `R_O_W_ATTR` = 1 (Read, Observe, Write-Attributes)
- `WRITE` = 2
- `EXECUTE` = 4
- `DELETE` = 8
- `CREATE` = 16 (object-level only)

## Verification

### Manual Verification

Run the audit tool:

```bash
python tools/audit-acl-coverage.py -v
```

Expected output when everything is correct:
```
[PASS] All object instances have AC coverage!
```

### CI Integration

Add to your CI workflow (GitHub Actions example):

```yaml
- name: Audit ACL Coverage
  run: python tools/audit-acl-coverage.py --strict
```

The tool exits with code 1 if any coverage gaps are found.

### Testing with a Server

1. Start Leshan or another LwM2M server
2. Register the client
3. Attempt to read all objects in registration
4. Check for `4.01 Unauthorized` responses
5. Missing AC instances will cause specific endpoints to fail

## Troubleshooting

### "4.01 Unauthorized" on Specific Object

1. Identify the object ID from the CoAP request (e.g., `GET /6/0` = Object 6, Instance 0)
2. Run `python tools/audit-acl-coverage.py -v` to check coverage
3. Add the missing AC instance in the object's init function
4. Rebuild and re-register

### Audit Tool Reports Gap But Code Looks Correct

1. Check that the `createInst` result is saved to a variable
2. Check that the AC creation uses `*variable` (dereference, not pointer)
3. Check that the code is not inside a comment or disabled `#ifdef`

## References

- [OMA LwM2M Access Control Object (Object 2) Specification](http://www.openmobilealliance.org/tech/profiles/LWM2M_Access_Control-v1_1.xml)
- [RFC 7252 - CoAP 4.01 Unauthorized](https://www.rfc-editor.org/rfc/rfc7252#section-5.9.2)
- `Issues/pcap-combo.md` - P1-7 (Location AC instance missing)
- `docs/PCAP_ANALYSIS_AND_REMEDIATION_SUMMARY.md` - P1-7 section

---

**Author**: Claude Code
**Date**: 2026-04-30
**Issue**: P1-7 - Location AC instance missing
