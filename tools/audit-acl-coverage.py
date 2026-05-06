#!/usr/bin/env python3
"""
ACL Coverage Audit Tool for Friendly LwM2M Client

This tool parses the examples/objects.cpp file to verify that every object
instance created has a corresponding Access Control (AC) instance.

The LwM2M Access Control Object (Object 2) is required to grant servers
permission to access other object instances. Without a corresponding AC
instance, servers will receive 4.01 Unauthorized responses.

Usage:
    python tools/audit-acl-coverage.py [--verbose] [--strict]

Exit codes:
    0 - All object instances have AC coverage
    1 - Coverage gaps detected
    2 - Parse error or file not found

Author: Claude Code
Date: 2026-04-30
"""

import argparse
import re
import sys
from dataclasses import dataclass, field
from pathlib import Path
from typing import Dict, List, Optional, Set, Tuple


@dataclass
class ObjectInstance:
    """Represents a declared object instance."""
    object_name: str
    object_id: int
    instance_variable: Optional[str]  # Variable name holding the instance
    line_number: int
    has_object_ac: bool = False  # AC for the object (Object-level)
    has_instance_ac: bool = False  # AC for this specific instance

    @property
    def needs_instance_ac(self) -> bool:
        """Instance-level AC is needed when we have a variable reference."""
        return self.instance_variable is not None


@dataclass
class AuditResult:
    """Results of the ACL coverage audit."""
    objects_found: List[ObjectInstance] = field(default_factory=list)
    coverage_gaps: List[str] = field(default_factory=list)
    warnings: List[str] = field(default_factory=list)

    @property
    def success(self) -> bool:
        return len(self.coverage_gaps) == 0


# Mapping of object names to their Object IDs
# Based on OMA LwM2M Registry and custom objects
OBJECT_ID_MAP = {
    # Core OMA Objects
    "Lwm2mSecurity": 0,
    "Lwm2mServer": 1,
    "Lwm2mAccessControl": 2,
    "Device": 3,
    "ConnectivityMonitoring": 4,
    "FirmwareUpdate": 5,
    "Location": 6,
    "WlanConnectivity": 12,
    "BearerSelection": 13,

    # IPSO Objects
    "AudioClip": 3339,

    # Custom Objects - OMA registered range
    "StarlinkTerminal": 10512,
    "RouterManagement": 10513,
    "EthernetInterface": 10514,
    "GpioControl": 10515,
    "UsbManagement": 10516,
    "StorageManagement": 10517,
    "SystemMonitor": 10518,
    "HardwareWatchdog": 10519,
    "Mikrobus": 10520,
    "FirewallConfig": 10521,
    "PoeManagement": 10522,
}

# Objects that don't need instance-level AC (AC Object itself, Security)
AC_EXEMPT_OBJECTS = {"Lwm2mAccessControl", "Lwm2mSecurity"}


def parse_objects_cpp(filepath: Path, verbose: bool = False) -> AuditResult:
    """
    Parse objects.cpp to find object instance creations and AC grants.

    Returns an AuditResult with all findings.
    """
    result = AuditResult()

    if not filepath.exists():
        result.coverage_gaps.append(f"File not found: {filepath}")
        return result

    content = filepath.read_text(encoding='utf-8')
    lines = content.split('\n')

    # Track current context (which init function we're in)
    current_function = None
    current_object_name = None
    instances_in_function: Dict[str, ObjectInstance] = {}

    # Patterns
    func_pattern = re.compile(r'void\s+(\w+)Init\s*\(')

    # Instance creation patterns:
    # Instance *device = Device::createInst(client);
    # Device::createInst(client);
    # Instance *wlan0 = WlanConnectivity::createInst(client, 0);
    create_inst_pattern = re.compile(
        r'(?:(?:Instance\s*\*|auto\s+|(\w+)\s*[*&]\s*)?(\w+)\s*=\s*)?'
        r'(\w+)::createInst\s*\('
    )

    # AC creation for object:
    # Lwm2mAccessControl::create(Device::object(client), ...)
    ac_object_pattern = re.compile(
        r'Lwm2mAccessControl::create\s*\(\s*(\w+)::object\s*\('
    )

    # AC creation for instance:
    # Lwm2mAccessControl::create(*device, TEST_SERVER_SHORT_ID);
    # Lwm2mAccessControl::create(sysMon, TEST_SERVER_SHORT_ID);
    ac_instance_pattern = re.compile(
        r'Lwm2mAccessControl::create\s*\(\s*\*?\s*(\w+)\s*,\s*TEST_SERVER_SHORT_ID'
    )

    # Alternative instance AC pattern (using getter):
    # Lwm2mAccessControl::create(*FirmwareUpdate::instance(client), TEST_SERVER_SHORT_ID);
    ac_instance_getter_pattern = re.compile(
        r'Lwm2mAccessControl::create\s*\(\s*\*\s*(\w+)::instance\s*\('
    )

    for line_num, line in enumerate(lines, 1):
        # Skip commented lines
        stripped = line.strip()
        if stripped.startswith('//') or stripped.startswith('/*') or stripped.startswith('*'):
            continue

        # Track function entry
        func_match = func_pattern.search(line)
        if func_match:
            # Save previous function's instances
            if current_function and instances_in_function:
                result.objects_found.extend(instances_in_function.values())

            current_function = func_match.group(1)
            current_object_name = current_function
            instances_in_function = {}
            if verbose:
                print(f"[{line_num}] Entering function: {current_function}Init")

        # Track instance creation
        create_match = create_inst_pattern.search(line)
        if create_match:
            type_hint = create_match.group(1)  # Type name if using typed pointer
            var_name = create_match.group(2)   # Variable name
            obj_name = create_match.group(3)   # Object class name

            if obj_name in OBJECT_ID_MAP:
                obj_id = OBJECT_ID_MAP[obj_name]

                # If no variable name, it's a direct call like Device::createInst(client)
                # We might capture it from a later getInstance call
                inst = ObjectInstance(
                    object_name=obj_name,
                    object_id=obj_id,
                    instance_variable=var_name,
                    line_number=line_num
                )

                if var_name:
                    instances_in_function[var_name] = inst
                else:
                    # No variable - use object name as key
                    instances_in_function[f"_anon_{obj_name}_{line_num}"] = inst

                if verbose:
                    var_info = f" -> {var_name}" if var_name else " (no variable)"
                    print(f"[{line_num}] Instance created: {obj_name}::createInst{var_info}")

        # Track AC object-level grant
        ac_obj_match = ac_object_pattern.search(line)
        if ac_obj_match:
            obj_name = ac_obj_match.group(1)
            # Mark all instances of this object as having object-level AC
            for inst in instances_in_function.values():
                if inst.object_name == obj_name:
                    inst.has_object_ac = True
            if verbose:
                print(f"[{line_num}] AC object-level grant: {obj_name}")

        # Track AC instance-level grant (by variable name)
        ac_inst_match = ac_instance_pattern.search(line)
        if ac_inst_match:
            var_name = ac_inst_match.group(1)
            if var_name in instances_in_function:
                instances_in_function[var_name].has_instance_ac = True
                if verbose:
                    print(f"[{line_num}] AC instance-level grant: {var_name}")

        # Track AC instance-level grant (by getter)
        ac_getter_match = ac_instance_getter_pattern.search(line)
        if ac_getter_match:
            obj_name = ac_getter_match.group(1)
            # Find anonymous instance for this object
            for key, inst in instances_in_function.items():
                if inst.object_name == obj_name and not inst.has_instance_ac:
                    inst.has_instance_ac = True
                    if verbose:
                        print(f"[{line_num}] AC instance-level grant via getter: {obj_name}")
                    break

    # Don't forget the last function
    if current_function and instances_in_function:
        result.objects_found.extend(instances_in_function.values())

    # Analyze coverage gaps
    for inst in result.objects_found:
        if inst.object_name in AC_EXEMPT_OBJECTS:
            continue

        if not inst.has_object_ac:
            result.coverage_gaps.append(
                f"/{inst.object_id} ({inst.object_name}) - Missing object-level AC "
                f"(line {inst.line_number})"
            )

        if inst.needs_instance_ac and not inst.has_instance_ac:
            result.coverage_gaps.append(
                f"/{inst.object_id}/0 ({inst.object_name}) - Missing instance-level AC "
                f"(line {inst.line_number}, var: {inst.instance_variable})"
            )

    return result


def print_summary(result: AuditResult, verbose: bool = False) -> None:
    """Print audit summary."""
    print("\n" + "=" * 60)
    print("ACL Coverage Audit Summary")
    print("=" * 60)

    print(f"\nObjects analyzed: {len(result.objects_found)}")

    # Group by object name
    by_object: Dict[str, List[ObjectInstance]] = {}
    for inst in result.objects_found:
        if inst.object_name not in by_object:
            by_object[inst.object_name] = []
        by_object[inst.object_name].append(inst)

    if verbose:
        print("\nObject instances found:")
        for obj_name, instances in sorted(by_object.items(),
                                          key=lambda x: OBJECT_ID_MAP.get(x[0], 99999)):
            obj_id = OBJECT_ID_MAP.get(obj_name, "?")
            exempt = " (AC exempt)" if obj_name in AC_EXEMPT_OBJECTS else ""
            print(f"  /{obj_id} {obj_name}{exempt}: {len(instances)} instance(s)")
            for inst in instances:
                obj_ac = "Y" if inst.has_object_ac else "N"
                inst_ac = "Y" if inst.has_instance_ac else "N"
                var = inst.instance_variable or "(anonymous)"
                print(f"      - {var}: ObjectAC={obj_ac}, InstanceAC={inst_ac}")

    if result.coverage_gaps:
        print(f"\n[FAIL] Coverage gaps found: {len(result.coverage_gaps)}")
        for gap in result.coverage_gaps:
            print(f"  - {gap}")
    else:
        print("\n[PASS] All object instances have AC coverage!")

    if result.warnings:
        print(f"\nWarnings: {len(result.warnings)}")
        for warning in result.warnings:
            print(f"  - {warning}")

    print()


def main() -> int:
    """Main entry point."""
    parser = argparse.ArgumentParser(
        description="Audit ACL coverage for LwM2M object instances",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__
    )
    parser.add_argument(
        "-v", "--verbose",
        action="store_true",
        help="Show detailed parsing output"
    )
    parser.add_argument(
        "-s", "--strict",
        action="store_true",
        help="Treat warnings as errors"
    )
    parser.add_argument(
        "-f", "--file",
        type=Path,
        default=None,
        help="Path to objects.cpp (default: auto-detect)"
    )

    args = parser.parse_args()

    # Find objects.cpp
    if args.file:
        objects_cpp = args.file
    else:
        # Try relative paths from script location
        script_dir = Path(__file__).parent
        candidates = [
            script_dir.parent / "examples" / "objects.cpp",
            Path("examples/objects.cpp"),
            Path("objects.cpp"),
        ]
        objects_cpp = None
        for candidate in candidates:
            if candidate.exists():
                objects_cpp = candidate
                break

        if objects_cpp is None:
            print("ERROR: Could not find examples/objects.cpp", file=sys.stderr)
            print("Try: python tools/audit-acl-coverage.py -f /path/to/objects.cpp",
                  file=sys.stderr)
            return 2

    print(f"Auditing: {objects_cpp.resolve()}")

    result = parse_objects_cpp(objects_cpp, verbose=args.verbose)
    print_summary(result, verbose=args.verbose)

    if result.coverage_gaps:
        return 1

    if args.strict and result.warnings:
        return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
