#!/bin/bash

# Fix build14 errors - Execute handlers and validators
# This script fixes:
# 1. setExecuteHandler -> set<EXECUTE_T>
# 2. Validator type casts
# 3. Registry accessor names
# 4. Object ID constants

set -e

echo "Fixing build14 errors..."

# Function to fix execute handlers in a file
fix_execute_handlers() {
    local file=$1
    local class=$2

    echo "  Fixing execute handlers in $file for $class..."

    # This requires manual specification of handlers for each class
    # We'll do this file by file
}

# Fix WifiChannelOptimization
echo "Fixing WifiChannelOptimization..."
cat > /tmp/fix_wco.pl << 'EOF'
#!/usr/bin/perl
use strict;
use warnings;

my $file = $ARGV[0];
open(my $fh, '<', $file) or die $!;
my $content = do { local $/; <$fh> };
close($fh);

# Fix execute handlers
$content =~ s/resource\(SCAN_NOW_18\)->setExecuteHandler\(scanNow\);/resource(SCAN_NOW_18)->set<EXECUTE_T>([](Instance& inst, ID_T resId, const OPAQUE_T& data) {\n        return WifiChannelOptimization::scanNow(inst, resId, data);\n    });/g;

$content =~ s/resource\(OPTIMIZE_CHANNEL_19\)->setExecuteHandler\(optimizeChannel\);/resource(OPTIMIZE_CHANNEL_19)->set<EXECUTE_T>([](Instance& inst, ID_T resId, const OPAQUE_T& data) {\n        return WifiChannelOptimization::optimizeChannel(inst, resId, data);\n    });/g;

$content =~ s/resource\(SET_CHANNEL_20\)->setExecuteHandler\(setChannel\);/resource(SET_CHANNEL_20)->set<EXECUTE_T>([](Instance& inst, ID_T resId, const OPAQUE_T& data) {\n        return WifiChannelOptimization::setChannel(inst, resId, data);\n    });/g;

$content =~ s/resource\(ENABLE_DFS_21\)->setExecuteHandler\(enableDFS\);/resource(ENABLE_DFS_21)->set<EXECUTE_T>([](Instance& inst, ID_T resId, const OPAQUE_T& data) {\n        return WifiChannelOptimization::enableDFS(inst, resId, data);\n    });/g;

# Fix registry accessor
$content =~ s/wifiChannelOptimization\(\)/WifiChannelOptimization()/g;

# Fix getMultiple -> isMultiple
$content =~ s/getMultiple\(/isMultiple(/g;

open($fh, '>', $file) or die $!;
print $fh $content;
close($fh);
EOF

chmod +x /tmp/fix_wco.pl
perl /tmp/fix_wco.pl "wpp/registry/objects/w_10528_wifi_channel_optimization/WifiChannelOptimization.cpp"

# Fix MatterBridge
echo "Fixing MatterBridge..."
cat > /tmp/fix_mb.pl << 'EOF'
#!/usr/bin/perl
use strict;
use warnings;

my $file = $ARGV[0];
open(my $fh, '<', $file) or die $!;
my $content = do { local $/; <$fh> };
close($fh);

# Fix execute handlers
$content =~ s/resource\(COMMISSION_DEVICE_7\)->setExecuteHandler\(commissionDevice\);/resource(COMMISSION_DEVICE_7)->set<EXECUTE_T>([](Instance& inst, ID_T resId, const OPAQUE_T& data) {\n        return MatterBridge::commissionDevice(inst, resId, data);\n    });/g;

$content =~ s/resource\(REMOVE_DEVICE_8\)->setExecuteHandler\(removeDevice\);/resource(REMOVE_DEVICE_8)->set<EXECUTE_T>([](Instance& inst, ID_T resId, const OPAQUE_T& data) {\n        return MatterBridge::removeDevice(inst, resId, data);\n    });/g;

$content =~ s/resource\(DISCOVER_DEVICES_9\)->setExecuteHandler\(discoverDevices\);/resource(DISCOVER_DEVICES_9)->set<EXECUTE_T>([](Instance& inst, ID_T resId, const OPAQUE_T& data) {\n        return MatterBridge::discoverDevices(inst, resId, data);\n    });/g;

$content =~ s/resource\(ENABLE_THREAD_BR_10\)->setExecuteHandler\(enableThreadBR\);/resource(ENABLE_THREAD_BR_10)->set<EXECUTE_T>([](Instance& inst, ID_T resId, const OPAQUE_T& data) {\n        return MatterBridge::enableThreadBR(inst, resId, data);\n    });/g;

# Fix registry accessor
$content =~ s/matterBridge\(\)/MatterBridge()/g;

open($fh, '>', $file) or die $!;
print $fh $content;
close($fh);
EOF

chmod +x /tmp/fix_mb.pl
perl /tmp/fix_mb.pl "wpp/registry/objects/w_10529_matter_bridge/MatterBridge.cpp"

# Fix MatterDevice
echo "Fixing MatterDevice..."
cat > /tmp/fix_md.pl << 'EOF'
#!/usr/bin/perl
use strict;
use warnings;

my $file = $ARGV[0];
open(my $fh, '<', $file) or die $!;
my $content = do { local $/; <$fh> };
close($fh);

# Fix execute handlers
$content =~ s/resource\(INTERACT_13\)->setExecuteHandler\(interact\);/resource(INTERACT_13)->set<EXECUTE_T>([](Instance& inst, ID_T resId, const OPAQUE_T& data) {\n        return MatterDevice::interact(inst, resId, data);\n    });/g;

$content =~ s/resource\(READ_ATTRIBUTE_14\)->setExecuteHandler\(readAttribute\);/resource(READ_ATTRIBUTE_14)->set<EXECUTE_T>([](Instance& inst, ID_T resId, const OPAQUE_T& data) {\n        return MatterDevice::readAttribute(inst, resId, data);\n    });/g;

$content =~ s/resource\(WRITE_ATTRIBUTE_15\)->setExecuteHandler\(writeAttribute\);/resource(WRITE_ATTRIBUTE_15)->set<EXECUTE_T>([](Instance& inst, ID_T resId, const OPAQUE_T& data) {\n        return MatterDevice::writeAttribute(inst, resId, data);\n    });/g;

$content =~ s/resource\(SEND_COMMAND_16\)->setExecuteHandler\(sendCommand\);/resource(SEND_COMMAND_16)->set<EXECUTE_T>([](Instance& inst, ID_T resId, const OPAQUE_T& data) {\n        return MatterDevice::sendCommand(inst, resId, data);\n    });/g;

# Fix registry accessor
$content =~ s/matterDevice\(\)/MatterDevice()/g;

open($fh, '>', $file) or die $!;
print $fh $content;
close($fh);
EOF

chmod +x /tmp/fix_md.pl
perl /tmp/fix_md.pl "wpp/registry/objects/w_10530_matter_device/MatterDevice.cpp"

# Fix ThreadNetwork
echo "Fixing ThreadNetwork..."
cat > /tmp/fix_tn.pl << 'EOF'
#!/usr/bin/perl
use strict;
use warnings;

my $file = $ARGV[0];
open(my $fh, '<', $file) or die $!;
my $content = do { local $/; <$fh> };
close($fh);

# Fix execute handlers
$content =~ s/resource\(CREATE_NETWORK_9\)->setExecuteHandler\(createNetwork\);/resource(CREATE_NETWORK_9)->set<EXECUTE_T>([](Instance& inst, ID_T resId, const OPAQUE_T& data) {\n        return ThreadNetwork::createNetwork(inst, resId, data);\n    });/g;

$content =~ s/resource\(START_BORDER_ROUTER_10\)->setExecuteHandler\(startBorderRouter\);/resource(START_BORDER_ROUTER_10)->set<EXECUTE_T>([](Instance& inst, ID_T resId, const OPAQUE_T& data) {\n        return ThreadNetwork::startBorderRouter(inst, resId, data);\n    });/g;

$content =~ s/resource\(STOP_BORDER_ROUTER_11\)->setExecuteHandler\(stopBorderRouter\);/resource(STOP_BORDER_ROUTER_11)->set<EXECUTE_T>([](Instance& inst, ID_T resId, const OPAQUE_T& data) {\n        return ThreadNetwork::stopBorderRouter(inst, resId, data);\n    });/g;

# Fix registry accessor
$content =~ s/threadNetwork\(\)/ThreadNetwork()/g;

open($fh, '>', $file) or die $!;
print $fh $content;
close($fh);
EOF

chmod +x /tmp/fix_tn.pl
perl /tmp/fix_tn.pl "wpp/registry/objects/w_10531_thread_network/ThreadNetwork.cpp"

# Fix WanFailoverPolicy validators
echo "Fixing WanFailoverPolicy validators..."
perl -i -pe 's/->setDataVerifier\(\[\]\(const INT_T&/->setDataVerifier((VERIFY_INT_T)[](const INT_T&/g' \
    "wpp/registry/objects/w_10525_wan_failover_policy/WanFailoverPolicy.cpp"

# Fix WanFailoverPolicy execute handlers
cat > /tmp/fix_wfp.pl << 'EOF'
#!/usr/bin/perl
use strict;
use warnings;

my $file = $ARGV[0];
open(my $fh, '<', $file) or die $!;
my $content = do { local $/; <$fh> };
close($fh);

# Fix execute handlers
$content =~ s/resource\(APPLY_POLICY_17\)->setExecuteHandler\(applyPolicy\);/resource(APPLY_POLICY_17)->set<EXECUTE_T>([](Instance& inst, ID_T resId, const OPAQUE_T& data) {\n        return WanFailoverPolicy::applyPolicy(inst, resId, data);\n    });/g;

$content =~ s/resource\(FORCE_FAILOVER_18\)->setExecuteHandler\(forceFailover\);/resource(FORCE_FAILOVER_18)->set<EXECUTE_T>([](Instance& inst, ID_T resId, const OPAQUE_T& data) {\n        return WanFailoverPolicy::forceFailover(inst, resId, data);\n    });/g;

$content =~ s/resource\(FORCE_FAILBACK_19\)->setExecuteHandler\(forceFailback\);/resource(FORCE_FAILBACK_19)->set<EXECUTE_T>([](Instance& inst, ID_T resId, const OPAQUE_T& data) {\n        return WanFailoverPolicy::forceFailback(inst, resId, data);\n    });/g;

$content =~ s/resource\(RESET_COUNTERS_20\)->setExecuteHandler\(resetCounters\);/resource(RESET_COUNTERS_20)->set<EXECUTE_T>([](Instance& inst, ID_T resId, const OPAQUE_T& data) {\n        return WanFailoverPolicy::resetCounters(inst, resId, data);\n    });/g;

open($fh, '>', $file) or die $!;
print $fh $content;
close($fh);
EOF

chmod +x /tmp/fix_wfp.pl
perl /tmp/fix_wfp.pl "wpp/registry/objects/w_10525_wan_failover_policy/WanFailoverPolicy.cpp"

# Fix MultiWanHealthCheck execute handlers
cat > /tmp/fix_mwhc.pl << 'EOF'
#!/usr/bin/perl
use strict;
use warnings;

my $file = $ARGV[0];
open(my $fh, '<', $file) or die $!;
my $content = do { local $/; <$fh> };
close($fh);

# Fix execute handlers
$content =~ s/resource\(RUN_HEALTH_CHECK_15\)->setExecuteHandler\(runHealthCheck\);/resource(RUN_HEALTH_CHECK_15)->set<EXECUTE_T>([](Instance& inst, ID_T resId, const OPAQUE_T& data) {\n        return MultiWanHealthCheck::runHealthCheck(inst, resId, data);\n    });/g;

$content =~ s/resource\(RESET_STATS_16\)->setExecuteHandler\(resetStats\);/resource(RESET_STATS_16)->set<EXECUTE_T>([](Instance& inst, ID_T resId, const OPAQUE_T& data) {\n        return MultiWanHealthCheck::resetStats(inst, resId, data);\n    });/g;

$content =~ s/resource\(HTTP_PROBE_17\)->setExecuteHandler\(httpProbe\);/resource(HTTP_PROBE_17)->set<EXECUTE_T>([](Instance& inst, ID_T resId, const OPAQUE_T& data) {\n        return MultiWanHealthCheck::httpProbe(inst, resId, data);\n    });/g;

$content =~ s/resource\(TCP_PROBE_18\)->setExecuteHandler\(tcpProbe\);/resource(TCP_PROBE_18)->set<EXECUTE_T>([](Instance& inst, ID_T resId, const OPAQUE_T& data) {\n        return MultiWanHealthCheck::tcpProbe(inst, resId, data);\n    });/g;

open($fh, '>', $file) or die $!;
print $fh $content;
close($fh);
EOF

chmod +x /tmp/fix_mwhc.pl
perl /tmp/fix_mwhc.pl "wpp/registry/objects/w_10526_multiwan_health_check/MultiWanHealthCheck.cpp"

echo "Done fixing execute handlers and validators!"
