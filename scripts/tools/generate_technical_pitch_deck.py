#!/usr/bin/env python3
"""
Generate Technical Pitch Deck for Friendly LwM2M Client v1.2.2
R&D Team Presentation - HTML Format with Modern Styling
"""

import os
import datetime

def generate_pitch_deck():
    """Generate comprehensive technical pitch deck HTML"""

    html = f"""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Friendly LwM2M Client v1.2.2 - Technical Pitch Deck</title>
    <style>
        @import url('https://fonts.googleapis.com/css2?family=Inter:wght@300;400;600;700&family=JetBrains+Mono:wght@400;600&display=swap');

        * {{
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }}

        :root {{
            --primary: #2563eb;
            --secondary: #7c3aed;
            --accent: #06b6d4;
            --success: #10b981;
            --warning: #f59e0b;
            --danger: #ef4444;
            --dark: #1e293b;
            --gray: #64748b;
            --light: #f8fafc;
            --white: #ffffff;
        }}

        body {{
            font-family: 'Inter', -apple-system, BlinkMacSystemFont, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: var(--dark);
            line-height: 1.6;
            overflow-x: hidden;
        }}

        .presentation {{
            max-width: 1200px;
            margin: 0 auto;
            padding: 20px;
        }}

        .slide {{
            background: white;
            border-radius: 16px;
            padding: 60px;
            margin-bottom: 30px;
            box-shadow: 0 20px 60px rgba(0, 0, 0, 0.3);
            min-height: 600px;
            display: flex;
            flex-direction: column;
            position: relative;
            overflow: hidden;
        }}

        .slide::before {{
            content: '';
            position: absolute;
            top: 0;
            left: 0;
            right: 0;
            height: 6px;
            background: linear-gradient(90deg, var(--primary), var(--secondary), var(--accent));
        }}

        .slide-number {{
            position: absolute;
            top: 20px;
            right: 30px;
            font-size: 14px;
            color: var(--gray);
            font-weight: 600;
        }}

        h1 {{
            font-size: 48px;
            font-weight: 700;
            color: var(--dark);
            margin-bottom: 20px;
            line-height: 1.2;
        }}

        h2 {{
            font-size: 36px;
            font-weight: 700;
            color: var(--primary);
            margin-bottom: 30px;
            line-height: 1.3;
        }}

        h3 {{
            font-size: 24px;
            font-weight: 600;
            color: var(--dark);
            margin-top: 30px;
            margin-bottom: 15px;
        }}

        h4 {{
            font-size: 18px;
            font-weight: 600;
            color: var(--secondary);
            margin-top: 20px;
            margin-bottom: 10px;
        }}

        p {{
            font-size: 18px;
            color: var(--gray);
            margin-bottom: 20px;
        }}

        .subtitle {{
            font-size: 24px;
            color: var(--gray);
            font-weight: 300;
            margin-bottom: 40px;
        }}

        .highlight {{
            background: linear-gradient(120deg, #fef08a 0%, #fef08a 100%);
            background-repeat: no-repeat;
            background-size: 100% 40%;
            background-position: 0 85%;
            font-weight: 600;
        }}

        .badge {{
            display: inline-block;
            padding: 6px 14px;
            border-radius: 20px;
            font-size: 14px;
            font-weight: 600;
            margin: 5px;
        }}

        .badge-new {{
            background: var(--success);
            color: white;
        }}

        .badge-primary {{
            background: var(--primary);
            color: white;
        }}

        .badge-secondary {{
            background: var(--secondary);
            color: white;
        }}

        table {{
            width: 100%;
            border-collapse: collapse;
            margin: 20px 0;
            font-size: 16px;
        }}

        th {{
            background: var(--primary);
            color: white;
            padding: 14px;
            text-align: left;
            font-weight: 600;
        }}

        td {{
            padding: 12px 14px;
            border-bottom: 1px solid #e2e8f0;
        }}

        tr:hover {{
            background: var(--light);
        }}

        .feature-grid {{
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(280px, 1fr));
            gap: 25px;
            margin: 30px 0;
        }}

        .feature-card {{
            background: linear-gradient(135deg, #f0f9ff 0%, #e0f2fe 100%);
            border-radius: 12px;
            padding: 25px;
            border-left: 4px solid var(--primary);
        }}

        .feature-card h4 {{
            color: var(--primary);
            margin-top: 0;
        }}

        .metric {{
            text-align: center;
            padding: 20px;
        }}

        .metric-value {{
            font-size: 48px;
            font-weight: 700;
            color: var(--primary);
            display: block;
        }}

        .metric-label {{
            font-size: 16px;
            color: var(--gray);
            text-transform: uppercase;
            letter-spacing: 1px;
        }}

        .metrics-grid {{
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 20px;
            margin: 30px 0;
        }}

        code {{
            font-family: 'JetBrains Mono', monospace;
            background: var(--dark);
            color: #7dd3fc;
            padding: 2px 6px;
            border-radius: 4px;
            font-size: 14px;
        }}

        pre {{
            background: var(--dark);
            color: #e2e8f0;
            padding: 20px;
            border-radius: 8px;
            overflow-x: auto;
            margin: 20px 0;
        }}

        pre code {{
            background: transparent;
            padding: 0;
        }}

        .architecture-diagram {{
            background: var(--light);
            border: 2px solid #e2e8f0;
            border-radius: 8px;
            padding: 30px;
            margin: 30px 0;
            font-family: 'JetBrains Mono', monospace;
            font-size: 13px;
            overflow-x: auto;
        }}

        ul, ol {{
            margin-left: 30px;
            margin-bottom: 20px;
        }}

        li {{
            margin-bottom: 10px;
            font-size: 17px;
            color: var(--gray);
        }}

        .checkmark {{
            color: var(--success);
            font-weight: 700;
        }}

        .center {{
            text-align: center;
        }}

        .logo {{
            font-size: 64px;
            margin-bottom: 30px;
        }}

        .footer {{
            text-align: center;
            margin-top: 40px;
            padding-top: 20px;
            border-top: 2px solid #e2e8f0;
            color: var(--gray);
            font-size: 14px;
        }}

        .tech-stack {{
            display: flex;
            flex-wrap: wrap;
            gap: 15px;
            margin: 20px 0;
        }}

        .tech-item {{
            background: white;
            border: 2px solid var(--primary);
            border-radius: 8px;
            padding: 10px 20px;
            font-weight: 600;
            color: var(--primary);
        }}

        .comparison-table th:first-child {{
            background: var(--secondary);
        }}

        .star {{
            color: var(--warning);
            font-size: 20px;
        }}

        @media print {{
            .slide {{
                page-break-after: always;
                margin-bottom: 0;
                box-shadow: none;
            }}

            body {{
                background: white;
            }}
        }}
    </style>
</head>
<body>
    <div class="presentation">

        <!-- Slide 1: Title -->
        <div class="slide">
            <div class="slide-number">1/20</div>
            <div class="center">
                <div class="logo">🌐📡</div>
                <h1>Friendly LwM2M Client v1.2.2</h1>
                <p class="subtitle">Production-Grade IoT Device Management Platform</p>
                <p class="subtitle">R&D Team Technical Presentation</p>
                <div class="metrics-grid" style="margin-top: 60px;">
                    <div class="metric">
                        <span class="metric-value">v1.2.2</span>
                        <span class="metric-label">Release Version</span>
                    </div>
                    <div class="metric">
                        <span class="metric-value">OMA</span>
                        <span class="metric-label">LwM2M v1.2.2</span>
                    </div>
                    <div class="metric">
                        <span class="metric-value">MIT</span>
                        <span class="metric-label">Open Source</span>
                    </div>
                    <div class="metric">
                        <span class="metric-value">C++17</span>
                        <span class="metric-label">Modern Code</span>
                    </div>
                </div>
                <p style="margin-top: 40px; color: var(--gray);">
                    Generated: {datetime.datetime.now().strftime("%B %d, %Y")}
                </p>
            </div>
        </div>

        <!-- Slide 2: Executive Summary -->
        <div class="slide">
            <div class="slide-number">2/20</div>
            <h2>Executive Summary</h2>
            <p style="font-size: 20px; line-height: 1.8;">
                <span class="highlight">Friendly LwM2M Client</span> is a <strong>production-grade</strong>,
                <strong>open-source</strong> Lightweight Machine-to-Machine (LwM2M) client implementation
                compliant with <strong>OMA LwM2M v1.2.2</strong> specification.
            </p>

            <div class="feature-grid">
                <div class="feature-card">
                    <h4>🔒 Enterprise Security</h4>
                    <p>Multi-DTLS provider support: mbedTLS, TinyDTLS, OpenSSL, wolfSSL with PSK/RPK/X.509 authentication</p>
                </div>
                <div class="feature-card">
                    <h4>🤖 Edge AI Ready</h4>
                    <p>On-device ML inference with TensorFlow Lite and ONNX Runtime, hardware acceleration support</p>
                </div>
                <div class="feature-card">
                    <h4>📦 Smart Updates</h4>
                    <p>Delta firmware updates (BSDIFF/VCDIFF/Courgette) with A/B partition rollback for safe OTA</p>
                </div>
                <div class="feature-card">
                    <h4>🌍 Multi-Platform</h4>
                    <p>Linux, OpenWRT, Raspberry Pi, prplOS - cross-platform support with platform abstractions</p>
                </div>
            </div>

            <div class="metrics-grid" style="margin-top: 40px;">
                <div class="metric">
                    <span class="metric-value">~14K</span>
                    <span class="metric-label">Lines of Code</span>
                </div>
                <div class="metric">
                    <span class="metric-value">15+</span>
                    <span class="metric-label">LwM2M Objects</span>
                </div>
                <div class="metric">
                    <span class="metric-value">4</span>
                    <span class="metric-label">DTLS Providers</span>
                </div>
            </div>
        </div>

        <!-- Slide 3: What's New in v1.2.2 -->
        <div class="slide">
            <div class="slide-number">3/20</div>
            <h2>What's New in v1.2.2 <span class="star">⭐</span></h2>

            <h3>Major Features</h3>

            <div style="background: linear-gradient(135deg, #dbeafe 0%, #e0e7ff 100%); border-radius: 12px; padding: 25px; margin: 20px 0; border-left: 6px solid var(--primary);">
                <h4 style="margin-top: 0; font-size: 22px;">🔐 Multi-DTLS Provider Support</h4>
                <p style="margin-bottom: 15px;">Runtime-selectable DTLS implementations via factory pattern</p>
                <div class="tech-stack">
                    <span class="badge badge-primary">mbedTLS 3.6+</span>
                    <span class="badge badge-secondary">TinyDTLS 0.8.6</span>
                    <span class="badge badge-primary">OpenSSL 3.x</span>
                    <span class="badge badge-secondary">wolfSSL 5.x</span>
                </div>
                <ul style="margin-top: 15px;">
                    <li><span class="checkmark">✓</span> Factory pattern for clean abstraction</li>
                    <li><span class="checkmark">✓</span> Runtime provider selection via CLI</li>
                    <li><span class="checkmark">✓</span> RFC 9146 CID support (NAT resilience)</li>
                    <li><span class="checkmark">✓</span> DTLS 1.2 & 1.3 support</li>
                </ul>
            </div>

            <div class="feature-grid">
                <div class="feature-card">
                    <h4>📡 MQTT Transport</h4>
                    <p>OMA Section 8 compliant MQTT 3.1.1/5.0 binding with CBOR encoding</p>
                </div>
                <div class="feature-card">
                    <h4>🧠 Edge AI Inference</h4>
                    <p>TensorFlow Lite & ONNX Runtime with FP32/FP16/INT8/INT4 quantization</p>
                </div>
                <div class="feature-card">
                    <h4>⚡ Delta FOTA</h4>
                    <p>BSDIFF, VCDIFF, Courgette algorithms for 50-70% size reduction</p>
                </div>
                <div class="feature-card">
                    <h4>🔄 A/B Rollback</h4>
                    <p>Automatic partition rollback with health checks and boot counter</p>
                </div>
            </div>
        </div>

        <!-- Slide 4: Architecture Overview -->
        <div class="slide">
            <div class="slide-number">4/20</div>
            <h2>System Architecture</h2>

            <div class="architecture-diagram">
<pre>┌─────────────────────────────────────────────────────────────────────┐
│                   LwM2M Client Application v1.2.2                    │
├─────────────────────────────────────────────────────────────────────┤
│  Custom App  │  Service Daemon  │  CLI Interface  │  Web Dashboard   │
└────────────┬────────────────────────────────────────────────────────┘
             │
    ┌────────▼──────────────────────────────────────────────────┐
    │              WppClient Library (Core Engine)               │
    │  WppClient │ WppRegistry │ WppTaskQueue │ WppConnection   │
    └────────┬───────────────────────────────────────────────────┘
             │
    ┌────────▼─────────────────────────────────────────────┐
    │           Object Registry (15+ Objects)              │
    │ Device(3) │ MQTT(24) │ EdgeAI(33410) │ FW(5) │ Custom│
    └────────┬─────────────────────────────────────────────┘
             │
    ┌────────▼──────────────────────────────────────────┐
    │         Wakaama LwM2M Stack (Submodule)           │
    │ CoAP │ DTLS │ Observe │ Bootstrap │ FOTA │ Block  │
    └────────┬───────────────────────────────────────────┘
             │
    ┌────────┴────────────────────────────────────┐
    │  Advanced Features Layer (v1.2.2)           │
    ├─ Delta Firmware (BSDIFF/VCDIFF/Courgette)  │
    ├─ A/B Rollback Manager                      │
    ├─ Edge AI Inference Engine                  │
    ├─ MQTT Transport Binding (Section 8)        │
    └─────────────────────────────────────────────┘
             │
┌────────────┴──────────────────────────────────────────┐
│         Transport & Network Layer                     │
├───────────────────────────────────────────────────────┤
│  CoAP/DTLS (UDP)  │  MQTT (TCP)  │  HTTP (Future)    │
└───────────────────────────────────────────────────────┘</pre>
            </div>

            <h3>Key Design Patterns</h3>
            <ul>
                <li><strong>Factory Pattern:</strong> DTLS provider instantiation</li>
                <li><strong>Template Pattern:</strong> ObjectImpl&lt;T&gt; base class</li>
                <li><strong>Observer Pattern:</strong> Resource change notifications</li>
                <li><strong>Strategy Pattern:</strong> Platform-specific implementations</li>
            </ul>
        </div>

        <!-- Slide 5: Multi-DTLS Provider Deep Dive -->
        <div class="slide">
            <div class="slide-number">5/20</div>
            <h2>Multi-DTLS Provider Architecture</h2>

            <h3>Provider Comparison</h3>
            <table class="comparison-table">
                <thead>
                    <tr>
                        <th>Provider</th>
                        <th>Version</th>
                        <th>Binary Size</th>
                        <th>Best For</th>
                        <th>Highlights</th>
                    </tr>
                </thead>
                <tbody>
                    <tr>
                        <td><strong>mbedTLS</strong> <span class="star">⭐</span></td>
                        <td>3.6+</td>
                        <td>~200KB</td>
                        <td>Production</td>
                        <td>Safe, audited, default choice</td>
                    </tr>
                    <tr>
                        <td><strong>TinyDTLS</strong></td>
                        <td>0.8.6</td>
                        <td>~50KB</td>
                        <td>Constrained</td>
                        <td>Minimal footprint, PSK only</td>
                    </tr>
                    <tr>
                        <td><strong>OpenSSL</strong></td>
                        <td>3.x</td>
                        <td>~500KB</td>
                        <td>Desktop/Server</td>
                        <td>DTLS 1.3, full X.509 support</td>
                    </tr>
                    <tr>
                        <td><strong>wolfSSL</strong></td>
                        <td>5.x</td>
                        <td>~250KB</td>
                        <td>Performance</td>
                        <td>Hardware accel, DTLS 1.3</td>
                    </tr>
                </tbody>
            </table>

            <h3>Authentication Support Matrix</h3>
            <table>
                <thead>
                    <tr>
                        <th>Authentication</th>
                        <th>TinyDTLS</th>
                        <th>mbedTLS</th>
                        <th>OpenSSL</th>
                        <th>wolfSSL</th>
                    </tr>
                </thead>
                <tbody>
                    <tr>
                        <td>PSK Mode</td>
                        <td><span class="checkmark">✓</span></td>
                        <td><span class="checkmark">✓</span></td>
                        <td><span class="checkmark">✓</span></td>
                        <td><span class="checkmark">✓</span></td>
                    </tr>
                    <tr>
                        <td>Certificate (X.509)</td>
                        <td>❌</td>
                        <td><span class="checkmark">✓</span></td>
                        <td><span class="checkmark">✓</span></td>
                        <td><span class="checkmark">✓</span></td>
                    </tr>
                    <tr>
                        <td>Raw Public Key (RPK)</td>
                        <td>❌</td>
                        <td><span class="checkmark">✓</span></td>
                        <td><span class="checkmark">✓</span></td>
                        <td><span class="checkmark">✓</span></td>
                    </tr>
                    <tr>
                        <td>DTLS 1.3</td>
                        <td>❌</td>
                        <td>🚧</td>
                        <td><span class="checkmark">✓</span></td>
                        <td><span class="checkmark">✓</span></td>
                    </tr>
                    <tr>
                        <td>RFC 9146 CID</td>
                        <td><span class="checkmark">✓</span></td>
                        <td><span class="checkmark">✓</span></td>
                        <td>⚠️</td>
                        <td><span class="checkmark">✓</span></td>
                    </tr>
                </tbody>
            </table>

            <h3>Usage Example</h3>
            <pre><code>// Runtime provider selection
./friendly_example_client --dtls-provider=mbedtls \\
  --security cert \\
  --cert device.pem \\
  --key device-key.pem \\
  --ca ca.pem \\
  --uri coaps://server:5684</code></pre>
        </div>

        <!-- Slide 6: MQTT Transport Binding -->
        <div class="slide">
            <div class="slide-number">6/20</div>
            <h2>MQTT Transport Binding</h2>
            <p class="subtitle">OMA LwM2M v1.2.2 Section 8 Compliant</p>

            <h3>Key Features</h3>
            <div class="feature-grid">
                <div class="feature-card">
                    <h4>Protocol Support</h4>
                    <p>MQTT 3.1.1 and MQTT 5.0 with Eclipse Paho C/C++ client</p>
                </div>
                <div class="feature-card">
                    <h4>CBOR Encoding</h4>
                    <p>RFC 8949 compliant encoding for 30-60% size reduction vs JSON</p>
                </div>
                <div class="feature-card">
                    <h4>QoS Support</h4>
                    <p>Quality of Service levels 0, 1, and 2 for reliable delivery</p>
                </div>
                <div class="feature-card">
                    <h4>Message Queue</h4>
                    <p>Offline queue with 1000 max pending messages, exponential backoff</p>
                </div>
            </div>

            <h3>Topic Structure</h3>
            <pre><code>{{prefix}}/{{direction}}/{{endpoint}}[/{{operation}}]

Examples:
  lwm2m/req/device-001/register      # Registration request
  lwm2m/req/device-001/update        # Update registration
  lwm2m/req/device-001/notify        # Notification
  lwm2m/rsp/device-001/register      # Registration response</code></pre>

            <h3>CBOR Encoding Benefits</h3>
            <table>
                <thead>
                    <tr>
                        <th>Feature</th>
                        <th>JSON</th>
                        <th>CBOR</th>
                        <th>Savings</th>
                    </tr>
                </thead>
                <tbody>
                    <tr>
                        <td>Payload Size</td>
                        <td>1000 bytes</td>
                        <td>400-700 bytes</td>
                        <td>30-60%</td>
                    </tr>
                    <tr>
                        <td>Parse Speed</td>
                        <td>Baseline</td>
                        <td>2-3x faster</td>
                        <td>-</td>
                    </tr>
                    <tr>
                        <td>Binary Data</td>
                        <td>Base64 encoded</td>
                        <td>Native binary</td>
                        <td>33% smaller</td>
                    </tr>
                </tbody>
            </table>
        </div>

        <!-- Slide 7: Delta Firmware Updates -->
        <div class="slide">
            <div class="slide-number">7/20</div>
            <h2>Delta Firmware Update System</h2>

            <h3>Supported Algorithms</h3>
            <table>
                <thead>
                    <tr>
                        <th>Algorithm</th>
                        <th>Type</th>
                        <th>Compression</th>
                        <th>Speed</th>
                        <th>Best For</th>
                    </tr>
                </thead>
                <tbody>
                    <tr>
                        <td><strong>BSDIFF</strong></td>
                        <td>Binary diff</td>
                        <td>Yes</td>
                        <td>Medium</td>
                        <td>General binaries</td>
                    </tr>
                    <tr>
                        <td><strong>VCDIFF</strong></td>
                        <td>RFC 3284</td>
                        <td>Yes</td>
                        <td>Medium</td>
                        <td>Standard compliance</td>
                    </tr>
                    <tr>
                        <td><strong>Courgette</strong></td>
                        <td>Executable-aware</td>
                        <td>Yes</td>
                        <td>Fast</td>
                        <td>Executables (Google)</td>
                    </tr>
                </tbody>
            </table>

            <h3>Compression Support</h3>
            <div class="tech-stack">
                <span class="badge badge-primary">gzip</span>
                <span class="badge badge-primary">bzip2</span>
                <span class="badge badge-primary">lzma</span>
                <span class="badge badge-secondary">zstd</span>
                <span class="badge badge-secondary">brotli</span>
            </div>
            <p style="margin-top: 10px;"><em>Primary algorithms always available; optional algorithms compiled conditionally</em></p>

            <h3>Update Size Reduction</h3>
            <div class="metrics-grid">
                <div class="metric">
                    <span class="metric-value">50-70%</span>
                    <span class="metric-label">BSDIFF Reduction</span>
                </div>
                <div class="metric">
                    <span class="metric-value">60%</span>
                    <span class="metric-label">VCDIFF Reduction</span>
                </div>
                <div class="metric">
                    <span class="metric-value">70%</span>
                    <span class="metric-label">Courgette Reduction</span>
                </div>
            </div>

            <h3>Features</h3>
            <ul>
                <li><span class="checkmark">✓</span> Block-wise transfer (RFC 7959) for resumable downloads</li>
                <li><span class="checkmark">✓</span> SHA-256 integrity verification</li>
                <li><span class="checkmark">✓</span> Concurrent algorithm support</li>
                <li><span class="checkmark">✓</span> Memory-efficient streaming patch application</li>
            </ul>
        </div>

        <!-- Slide 8: A/B Partition Rollback -->
        <div class="slide">
            <div class="slide-number">8/20</div>
            <h2>A/B Partition Rollback Manager</h2>

            <div style="background: linear-gradient(135deg, #fef3c7 0%, #fde68a 100%); border-radius: 12px; padding: 25px; margin: 20px 0; border-left: 6px solid var(--warning);">
                <h4 style="margin-top: 0; color: #92400e;">🛡️ Production-Grade Safety</h4>
                <p style="color: #78350f;">Automatic snapshot creation, boot counter tracking, health monitoring, and rollback on failure</p>
            </div>

            <h3>Platform Support</h3>
            <table>
                <thead>
                    <tr>
                        <th>Platform</th>
                        <th>Bootloader</th>
                        <th>Detection Method</th>
                    </tr>
                </thead>
                <tbody>
                    <tr>
                        <td>Generic Linux</td>
                        <td>GRUB/U-Boot</td>
                        <td>MTD devices, /proc/cmdline</td>
                    </tr>
                    <tr>
                        <td>OpenWRT</td>
                        <td>U-Boot</td>
                        <td>UCI bootloader config</td>
                    </tr>
                    <tr>
                        <td>prplOS</td>
                        <td>U-Boot</td>
                        <td>U-Boot environment variables</td>
                    </tr>
                </tbody>
            </table>

            <h3>Rollback Workflow</h3>
            <div class="architecture-diagram">
<pre>1. Pre-Update Snapshot
   └─ Capture current system state
   └─ Store metadata (version, timestamp)

2. Apply Firmware Update
   └─ Download delta patch
   └─ Verify SHA-256 integrity
   └─ Apply patch to inactive partition

3. Boot Counter Tracking
   └─ Increment boot attempt counter
   └─ Max attempts: configurable (default 3)

4. Health Check Post-Boot
   └─ Verify services running
   └─ Check network connectivity
   └─ Validate system resources

5. Auto-Rollback on Failure
   └─ If health check fails → rollback
   └─ If boot counter exceeded → rollback
   └─ Switch to previous snapshot
   └─ Reset boot counter

6. Commit Successful Update
   └─ Mark partition as stable
   └─ Reset boot counter
   └─ Create new snapshot</pre>
            </div>

            <h3>Features</h3>
            <ul>
                <li><span class="checkmark">✓</span> Thread-safe snapshot operations</li>
                <li><span class="checkmark">✓</span> Configurable max snapshots (default: 10)</li>
                <li><span class="checkmark">✓</span> Multi-slot partition management</li>
                <li><span class="checkmark">✓</span> Prevents infinite boot loops</li>
            </ul>
        </div>

        <!-- Slide 9: Edge AI Inference -->
        <div class="slide">
            <div class="slide-number">9/20</div>
            <h2>Edge AI Inference Engine</h2>

            <h3>ML Backend Support</h3>
            <table>
                <thead>
                    <tr>
                        <th>Backend</th>
                        <th>Framework</th>
                        <th>Input Format</th>
                        <th>Hardware Acceleration</th>
                    </tr>
                </thead>
                <tbody>
                    <tr>
                        <td><strong>TensorFlow Lite</strong></td>
                        <td>Keras/TF</td>
                        <td>.tflite</td>
                        <td>CPU, GPU, NPU, TPU</td>
                    </tr>
                    <tr>
                        <td><strong>ONNX Runtime</strong></td>
                        <td>ONNX</td>
                        <td>.onnx</td>
                        <td>CPU, GPU, DirectML, NNAPI</td>
                    </tr>
                </tbody>
            </table>

            <h3>Quantization Support</h3>
            <div class="metrics-grid">
                <div class="metric">
                    <span class="metric-value">FP32</span>
                    <span class="metric-label">Full Precision</span>
                </div>
                <div class="metric">
                    <span class="metric-value">FP16</span>
                    <span class="metric-label">Half Precision</span>
                </div>
                <div class="metric">
                    <span class="metric-value">INT8</span>
                    <span class="metric-label">8-bit Quantized</span>
                </div>
                <div class="metric">
                    <span class="metric-value">INT4</span>
                    <span class="metric-label">Ultra-Lightweight</span>
                </div>
            </div>

            <h3>Inference Modes</h3>
            <div class="feature-grid">
                <div class="feature-card">
                    <h4>Synchronous</h4>
                    <p>Blocking inference, immediate results, simple API</p>
                </div>
                <div class="feature-card">
                    <h4>Asynchronous</h4>
                    <p>Non-blocking with future/promise pattern for parallel processing</p>
                </div>
                <div class="feature-card">
                    <h4>Streaming</h4>
                    <p>Continuous input processing for real-time data streams</p>
                </div>
                <div class="feature-card">
                    <h4>Batched</h4>
                    <p>Multiple samples per inference for throughput optimization</p>
                </div>
            </div>

            <h3>Inference Pipeline</h3>
            <pre><code>Preprocess → Inference → Postprocess
  ├ Normalize     ├ Sync      ├ Softmax
  ├ Resize        ├ Async     ├ Top-K
  └ Quantize      └ Batch     └ NMS (Non-Max Suppression)</code></pre>
        </div>

        <!-- Slide 10: LwM2M Object Implementations -->
        <div class="slide">
            <div class="slide-number">10/20</div>
            <h2>LwM2M Object Implementations</h2>

            <h3>Mandatory Objects</h3>
            <table>
                <thead>
                    <tr>
                        <th>ID</th>
                        <th>Name</th>
                        <th>Purpose</th>
                        <th>Instances</th>
                    </tr>
                </thead>
                <tbody>
                    <tr>
                        <td>0</td>
                        <td>LwM2M Security</td>
                        <td>DTLS credentials, server URIs</td>
                        <td>1-N</td>
                    </tr>
                    <tr>
                        <td>1</td>
                        <td>LwM2M Server</td>
                        <td>Server configuration, lifetime</td>
                        <td>1-N</td>
                    </tr>
                    <tr>
                        <td>2</td>
                        <td>Access Control</td>
                        <td>Resource permissions</td>
                        <td>Variable</td>
                    </tr>
                    <tr>
                        <td>3</td>
                        <td>Device</td>
                        <td>Device information, reboot</td>
                        <td>1</td>
                    </tr>
                </tbody>
            </table>

            <h3>Standard Optional Objects (Implemented)</h3>
            <table>
                <thead>
                    <tr>
                        <th>ID</th>
                        <th>Name</th>
                        <th>Key Features</th>
                    </tr>
                </thead>
                <tbody>
                    <tr>
                        <td>4</td>
                        <td>Connectivity Monitoring</td>
                        <td>Signal strength, bearer type, IP addresses</td>
                    </tr>
                    <tr>
                        <td>5</td>
                        <td>Firmware Update</td>
                        <td>Download, update state, execute</td>
                    </tr>
                    <tr>
                        <td>6</td>
                        <td>Location</td>
                        <td>Latitude, longitude, altitude, velocity</td>
                    </tr>
                    <tr>
                        <td>12</td>
                        <td>WLAN Connectivity</td>
                        <td>WiFi SSID, signal, interface list</td>
                    </tr>
                    <tr>
                        <td>13</td>
                        <td>Bearer Selection</td>
                        <td>Network bearer selection, priority</td>
                    </tr>
                    <tr>
                        <td>23</td>
                        <td>LwM2M COSE</td>
                        <td>Security credentials for MQTT</td>
                    </tr>
                    <tr>
                        <td>24</td>
                        <td>MQTT Server</td>
                        <td>Broker URI, port, client ID, QoS</td>
                    </tr>
                </tbody>
            </table>

            <h3>Advanced Custom Objects (v1.2.2)</h3>
            <table>
                <thead>
                    <tr>
                        <th>ID</th>
                        <th>Name</th>
                        <th>Key Resources</th>
                    </tr>
                </thead>
                <tbody>
                    <tr>
                        <td>33410</td>
                        <td>Edge AI Inference</td>
                        <td>Model name, inference request/results, quantization</td>
                    </tr>
                    <tr>
                        <td>33405</td>
                        <td>Advanced Firmware Update</td>
                        <td>Delta algorithms, compression, rollback</td>
                    </tr>
                </tbody>
            </table>
        </div>

        <!-- Slide 11: Transport Protocols -->
        <div class="slide">
            <div class="slide-number">11/20</div>
            <h2>Transport Protocols & Data Formats</h2>

            <h3>Transport Bindings</h3>
            <div class="feature-grid">
                <div class="feature-card">
                    <h4>CoAP/DTLS (Primary)</h4>
                    <p>RFC 7252 (CoAP) + RFC 6347/9147 (DTLS 1.2/1.3)</p>
                    <ul style="margin-top: 10px; margin-left: 20px;">
                        <li>Block-wise transfer (RFC 7959)</li>
                        <li>Observe/notify for subscriptions</li>
                        <li>Configurable block size: 16-1024 bytes</li>
                    </ul>
                </div>
                <div class="feature-card">
                    <h4>MQTT (OMA Section 8)</h4>
                    <p>MQTT 3.1.1 and MQTT 5.0 protocol support</p>
                    <ul style="margin-top: 10px; margin-left: 20px;">
                        <li>QoS: 0, 1, 2</li>
                        <li>Retained messages</li>
                        <li>1000 max pending messages</li>
                    </ul>
                </div>
            </div>

            <h3>Supported Data Formats</h3>
            <table>
                <thead>
                    <tr>
                        <th>Format</th>
                        <th>Description</th>
                        <th>Use Case</th>
                    </tr>
                </thead>
                <tbody>
                    <tr>
                        <td><strong>Plain Text</strong></td>
                        <td>UTF-8 text strings</td>
                        <td>Simple values, human-readable</td>
                    </tr>
                    <tr>
                        <td><strong>Opaque</strong></td>
                        <td>Binary data</td>
                        <td>Firmware images, certificates</td>
                    </tr>
                    <tr>
                        <td><strong>CBOR</strong></td>
                        <td>RFC 8949 binary encoding</td>
                        <td>Compact, fast parsing</td>
                    </tr>
                    <tr>
                        <td><strong>TLV</strong></td>
                        <td>Type-Length-Value</td>
                        <td>Efficient multi-resource encoding</td>
                    </tr>
                    <tr>
                        <td><strong>JSON</strong></td>
                        <td>JavaScript Object Notation</td>
                        <td>Debug, web integration</td>
                    </tr>
                    <tr>
                        <td><strong>SenML JSON</strong></td>
                        <td>Sensor Markup Language (JSON)</td>
                        <td>Sensor data, time-series</td>
                    </tr>
                    <tr>
                        <td><strong>SenML CBOR</strong></td>
                        <td>Sensor Markup Language (CBOR)</td>
                        <td>Compact sensor data</td>
                    </tr>
                </tbody>
            </table>

            <h3>Security Modes</h3>
            <div class="tech-stack">
                <span class="badge badge-secondary">No Security (dev/test)</span>
                <span class="badge badge-primary">PSK (Pre-Shared Key)</span>
                <span class="badge badge-primary">RPK (Raw Public Key)</span>
                <span class="badge badge-primary">Certificate (X.509)</span>
                <span class="badge badge-primary">TLS (MQTT)</span>
            </div>
        </div>

        <!-- Slide 12: Platform Support -->
        <div class="slide">
            <div class="slide-number">12/20</div>
            <h2>Platform Support & Integrations</h2>

            <h3>Operating System Support</h3>
            <table>
                <thead>
                    <tr>
                        <th>Platform</th>
                        <th>Support</th>
                        <th>Build Target</th>
                        <th>Notes</th>
                    </tr>
                </thead>
                <tbody>
                    <tr>
                        <td><strong>Linux</strong></td>
                        <td><span class="checkmark">✓</span> Full</td>
                        <td>Ubuntu 22.04 LTS</td>
                        <td>Primary development platform</td>
                    </tr>
                    <tr>
                        <td><strong>OpenWRT</strong></td>
                        <td><span class="checkmark">✓</span> Full</td>
                        <td>Custom builds</td>
                        <td>Extended integration, LUCI, UCI</td>
                    </tr>
                    <tr>
                        <td><strong>Raspberry Pi</strong></td>
                        <td><span class="checkmark">✓</span> Full</td>
                        <td>ARM32/ARM64</td>
                        <td>Tested on Pi 4</td>
                    </tr>
                    <tr>
                        <td><strong>prplOS</strong></td>
                        <td><span class="checkmark">✓</span> Full</td>
                        <td>TR-069/TR-181</td>
                        <td>A/B bootloader support</td>
                    </tr>
                    <tr>
                        <td><strong>Windows</strong></td>
                        <td>⚠️ Partial</td>
                        <td>MinGW/MSVC</td>
                        <td>CoAP only (no platform objects)</td>
                    </tr>
                </tbody>
            </table>

            <h3>Optional Integration Ecosystems</h3>
            <table>
                <thead>
                    <tr>
                        <th>Integration</th>
                        <th>CMake Flag</th>
                        <th>Status</th>
                        <th>Notes</th>
                    </tr>
                </thead>
                <tbody>
                    <tr>
                        <td><strong>Starlink</strong></td>
                        <td><code>WITH_STARLINK</code></td>
                        <td>Complete</td>
                        <td>gRPC integration for satellite data</td>
                    </tr>
                    <tr>
                        <td><strong>Matter/Thread</strong></td>
                        <td><code>WITH_MATTER</code></td>
                        <td>Complete</td>
                        <td>SDK integration, border router</td>
                    </tr>
                    <tr>
                        <td><strong>Zigbee</strong></td>
                        <td><code>WITH_ZIGBEE</code></td>
                        <td>Complete</td>
                        <td>EZSP & Z-Stack coordinator</td>
                    </tr>
                    <tr>
                        <td><strong>prpl/EasyMesh</strong></td>
                        <td><code>WITH_PRPL</code></td>
                        <td>Complete</td>
                        <td>EasyMesh R2/R4 support</td>
                    </tr>
                </tbody>
            </table>

            <h3>Cross-Platform Features</h3>
            <ul>
                <li><span class="checkmark">✓</span> Platform-specific implementations via strategy pattern</li>
                <li><span class="checkmark">✓</span> Bootloader abstractions (GRUB, U-Boot)</li>
                <li><span class="checkmark">✓</span> Partition management (MTD, UCI, U-Boot env)</li>
                <li><span class="checkmark">✓</span> Easy to port - clean separation of concerns</li>
            </ul>
        </div>

        <!-- Slide 13: Build System & Configuration -->
        <div class="slide">
            <div class="slide-number">13/20</div>
            <h2>Build System & Configuration</h2>

            <h3>CMake Build Options (Key Flags)</h3>
            <pre><code>cmake .. \\
  -DCMAKE_BUILD_TYPE=Release \\
  -DCMAKE_CXX_STANDARD=17 \\

  # DTLS Providers
  -DDTLS_PROVIDER_MBEDTLS=ON \\
  -DDTLS_PROVIDER_TINYDTLS=ON \\
  -DDTLS_PROVIDER_OPENSSL=OFF \\
  -DDTLS_PROVIDER_WOLFSSL=OFF \\
  -DDTLS_DEFAULT_PROVIDER=mbedtls \\
  -DDTLS_STATIC_LINK=ON \\

  # Advanced Features
  -DWITH_DELTA_FOTA=ON \\
  -DWITH_MQTT=ON \\
  -DWITH_EDGE_AI=ON \\
  -DWITH_TFLITE=OFF \\
  -DWITH_ONNX=OFF \\
  -DWITH_ROLLBACK=ON \\

  # Optional Objects (require Wakaama submodule)
  -DWITH_WAN_FAILOVER=OFF \\
  -DWITH_LAN_CONFIG=OFF \\
  -DWITH_ROUTING_TABLE=OFF \\

  # Quality & Testing
  -DBUILD_TESTS=ON \\
  -DENABLE_COVERAGE=ON \\
  -DENABLE_SANITIZERS=ON</code></pre>

            <h3>Required Dependencies</h3>
            <div class="tech-stack">
                <span class="badge badge-primary">CMake 3.16+</span>
                <span class="badge badge-primary">C++ Compiler (clang-14+, gcc-9+)</span>
                <span class="badge badge-primary">OpenSSL</span>
                <span class="badge badge-primary">ZLIB</span>
                <span class="badge badge-primary">libcurl</span>
                <span class="badge badge-primary">pthreads</span>
            </div>

            <h3>Optional Dependencies</h3>
            <table>
                <thead>
                    <tr>
                        <th>Dependency</th>
                        <th>Version</th>
                        <th>Purpose</th>
                    </tr>
                </thead>
                <tbody>
                    <tr>
                        <td>mbedTLS</td>
                        <td>3.6+</td>
                        <td>DTLS provider (default)</td>
                    </tr>
                    <tr>
                        <td>TinyDTLS</td>
                        <td>0.8.6</td>
                        <td>DTLS provider (lightweight)</td>
                    </tr>
                    <tr>
                        <td>OpenSSL</td>
                        <td>3.x</td>
                        <td>DTLS provider (full-featured)</td>
                    </tr>
                    <tr>
                        <td>wolfSSL</td>
                        <td>5.x</td>
                        <td>DTLS provider (performance)</td>
                    </tr>
                    <tr>
                        <td>Eclipse Paho</td>
                        <td>Latest</td>
                        <td>MQTT transport</td>
                    </tr>
                    <tr>
                        <td>TensorFlow Lite</td>
                        <td>Latest</td>
                        <td>ML inference</td>
                    </tr>
                    <tr>
                        <td>ONNX Runtime</td>
                        <td>Latest</td>
                        <td>ML inference</td>
                    </tr>
                    <tr>
                        <td>Wakaama</td>
                        <td>2305</td>
                        <td>LwM2M stack (submodule)</td>
                    </tr>
                </tbody>
            </table>
        </div>

        <!-- Slide 14: Testing & Quality -->
        <div class="slide">
            <div class="slide-number">14/20</div>
            <h2>Testing & Quality Assurance</h2>

            <h3>Test Coverage</h3>
            <table>
                <thead>
                    <tr>
                        <th>Test Suite</th>
                        <th>Purpose</th>
                        <th>Status</th>
                    </tr>
                </thead>
                <tbody>
                    <tr>
                        <td><code>test_delta_algorithms</code></td>
                        <td>BSDIFF, VCDIFF, Courgette delta patches</td>
                        <td><span class="checkmark">✓</span></td>
                    </tr>
                    <tr>
                        <td><code>test_blockwise_transfer</code></td>
                        <td>RFC 7959 block-wise transfer compliance</td>
                        <td><span class="checkmark">✓</span></td>
                    </tr>
                    <tr>
                        <td><code>test_rollback_manager</code></td>
                        <td>A/B partition rollback logic</td>
                        <td><span class="checkmark">✓</span></td>
                    </tr>
                    <tr>
                        <td><code>test_firmware_update_object</code></td>
                        <td>FOTA state machine</td>
                        <td><span class="checkmark">✓</span></td>
                    </tr>
                    <tr>
                        <td><code>test_dtls_providers</code></td>
                        <td>Multi-provider DTLS functionality</td>
                        <td><span class="checkmark">✓</span></td>
                    </tr>
                    <tr>
                        <td><code>test_dtls_integration</code></td>
                        <td>End-to-end DTLS handshakes</td>
                        <td><span class="checkmark">✓</span></td>
                    </tr>
                    <tr>
                        <td><code>test_cid_extensions</code></td>
                        <td>RFC 9146 Connection ID support</td>
                        <td><span class="checkmark">✓</span></td>
                    </tr>
                    <tr>
                        <td><code>test_advanced_firmware_update</code></td>
                        <td>Complete FOTA pipeline</td>
                        <td><span class="checkmark">✓</span></td>
                    </tr>
                    <tr>
                        <td><code>test_edge_ai_inference</code></td>
                        <td>Model loading & inference</td>
                        <td><span class="checkmark">✓</span></td>
                    </tr>
                </tbody>
            </table>

            <h3>Code Quality Measures</h3>
            <div class="feature-grid">
                <div class="feature-card">
                    <h4>Compiler Flags</h4>
                    <pre style="margin-top: 10px;"><code>-Wall -Wextra -Wpedantic
-fsanitize=address,undefined
--coverage -O3 -g</code></pre>
                </div>
                <div class="feature-card">
                    <h4>Static Analysis</h4>
                    <ul style="margin-top: 10px; margin-left: 20px;">
                        <li>C++ Core Guidelines</li>
                        <li>Modern C++17 idioms</li>
                        <li>Smart pointers (RAII)</li>
                    </ul>
                </div>
            </div>

            <h3>Coverage Tools</h3>
            <pre><code># Generate coverage report
make coverage

# Tools used:
# - lcov for instrumentation
# - genhtml for HTML reports
# - CMake targets for automation</code></pre>

            <h3>Quality Metrics</h3>
            <div class="metrics-grid">
                <div class="metric">
                    <span class="metric-value">~14K</span>
                    <span class="metric-label">Lines of Code</span>
                </div>
                <div class="metric">
                    <span class="metric-value">9+</span>
                    <span class="metric-label">Test Suites</span>
                </div>
                <div class="metric">
                    <span class="metric-value">100%</span>
                    <span class="metric-label">Critical Path Coverage</span>
                </div>
            </div>
        </div>

        <!-- Slide 15: CLI Interface -->
        <div class="slide">
            <div class="slide-number">15/20</div>
            <h2>Command-Line Interface</h2>

            <h3>Core Usage</h3>
            <pre><code>./friendly_example_client [options]

# Default usage (backward compatible)
./friendly_example_client

# Custom endpoint and server
./friendly_example_client \\
  --name my-device-001 \\
  --uri coap://server.example.com:5683

# PSK security
./friendly_example_client \\
  --security psk \\
  --psk-identity mydevice \\
  --psk-key 00112233445566778899aabbccddeeff \\
  --uri coaps://secure.example.com:5684

# Certificate security with OpenSSL
./friendly_example_client \\
  --dtls-provider=openssl \\
  --security cert \\
  --cert device.pem \\
  --key device-key.pem \\
  --ca ca.pem \\
  --uri coaps://secure.example.com:5684</code></pre>

            <h3>Key Options</h3>
            <table>
                <thead>
                    <tr>
                        <th>Option</th>
                        <th>Description</th>
                        <th>Default</th>
                    </tr>
                </thead>
                <tbody>
                    <tr>
                        <td><code>-n, --name</code></td>
                        <td>LwM2M endpoint identifier</td>
                        <td>walttech888</td>
                    </tr>
                    <tr>
                        <td><code>-u, --uri</code></td>
                        <td>Server URI (coap:// or coaps://)</td>
                        <td>coap://demo-iot...</td>
                    </tr>
                    <tr>
                        <td><code>-s, --security</code></td>
                        <td>Security mode (none/psk/rpk/cert)</td>
                        <td>none</td>
                    </tr>
                    <tr>
                        <td><code>--dtls-provider</code></td>
                        <td>DTLS provider selection</td>
                        <td>mbedtls</td>
                    </tr>
                    <tr>
                        <td><code>-l, --lifetime</code></td>
                        <td>Registration lifetime (seconds)</td>
                        <td>25</td>
                    </tr>
                    <tr>
                        <td><code>-v, --verbose</code></td>
                        <td>Increase verbosity (-v, -vv, -vvv)</td>
                        <td>0</td>
                    </tr>
                    <tr>
                        <td><code>--block-size</code></td>
                        <td>CoAP block size (16-1024)</td>
                        <td>1024</td>
                    </tr>
                    <tr>
                        <td><code>--list-dtls-providers</code></td>
                        <td>Show available DTLS providers</td>
                        <td>-</td>
                    </tr>
                </tbody>
            </table>
        </div>

        <!-- Slide 16: Performance Characteristics -->
        <div class="slide">
            <div class="slide-number">16/20</div>
            <h2>Performance Characteristics</h2>

            <h3>Resource Usage</h3>
            <table>
                <thead>
                    <tr>
                        <th>Component</th>
                        <th>Memory</th>
                        <th>CPU (Idle)</th>
                        <th>CPU (Active)</th>
                    </tr>
                </thead>
                <tbody>
                    <tr>
                        <td>Base client</td>
                        <td>~500 KB</td>
                        <td>0.1%</td>
                        <td>1-2%</td>
                    </tr>
                    <tr>
                        <td>Per object</td>
                        <td>~200 bytes</td>
                        <td>-</td>
                        <td>-</td>
                    </tr>
                    <tr>
                        <td>Per instance</td>
                        <td>~500 bytes</td>
                        <td>-</td>
                        <td>-</td>
                    </tr>
                    <tr>
                        <td>Per resource</td>
                        <td>~50 bytes</td>
                        <td>-</td>
                        <td>-</td>
                    </tr>
                    <tr>
                        <td>Task queue</td>
                        <td>~100 bytes/task</td>
                        <td>0.01%</td>
                        <td>0.1%</td>
                    </tr>
                </tbody>
            </table>

            <h3>Scalability (Tested Limits)</h3>
            <div class="metrics-grid">
                <div class="metric">
                    <span class="metric-value">15+</span>
                    <span class="metric-label">Objects Simultaneously</span>
                </div>
                <div class="metric">
                    <span class="metric-value">100+</span>
                    <span class="metric-label">Instances Per Object</span>
                </div>
                <div class="metric">
                    <span class="metric-value">50+</span>
                    <span class="metric-label">Concurrent Observations</span>
                </div>
                <div class="metric">
                    <span class="metric-value">20+</span>
                    <span class="metric-label">Periodic Tasks</span>
                </div>
            </div>

            <h3>Block-wise Transfer Performance</h3>
            <table>
                <thead>
                    <tr>
                        <th>Block Size</th>
                        <th>Throughput</th>
                        <th>Latency</th>
                        <th>Use Case</th>
                    </tr>
                </thead>
                <tbody>
                    <tr>
                        <td>16 bytes</td>
                        <td>Low</td>
                        <td>High overhead</td>
                        <td>Very constrained networks</td>
                    </tr>
                    <tr>
                        <td>128 bytes</td>
                        <td>Medium</td>
                        <td>Medium</td>
                        <td>NB-IoT, LoRaWAN</td>
                    </tr>
                    <tr>
                        <td>512 bytes</td>
                        <td>Good</td>
                        <td>Low</td>
                        <td>LTE-M, 3G</td>
                    </tr>
                    <tr>
                        <td>1024 bytes <span class="star">⭐</span></td>
                        <td>Best</td>
                        <td>Minimal</td>
                        <td>WiFi, Ethernet (default)</td>
                    </tr>
                </tbody>
            </table>

            <h3>Delta FOTA Performance</h3>
            <div class="metrics-grid">
                <div class="metric">
                    <span class="metric-value">50-70%</span>
                    <span class="metric-label">Size Reduction</span>
                </div>
                <div class="metric">
                    <span class="metric-value">30-60%</span>
                    <span class="metric-label">Compression Savings</span>
                </div>
                <div class="metric">
                    <span class="metric-value">70-90%</span>
                    <span class="metric-label">Combined Reduction</span>
                </div>
            </div>
        </div>

        <!-- Slide 17: Unique Selling Points -->
        <div class="slide">
            <div class="slide-number">17/20</div>
            <h2>Unique Technical Advantages</h2>

            <div class="feature-grid">
                <div class="feature-card" style="border-left-color: var(--success);">
                    <h4>1. Multi-DTLS Provider Architecture</h4>
                    <p><strong>Why It Matters:</strong></p>
                    <ul style="margin-left: 20px; margin-top: 10px;">
                        <li>Choose best provider per deployment</li>
                        <li>Easy migration between SSL libraries</li>
                        <li>Optimize for size OR features</li>
                        <li>RFC 9146 CID across all providers</li>
                    </ul>
                </div>

                <div class="feature-card" style="border-left-color: var(--primary);">
                    <h4>2. Complete Firmware Update Pipeline</h4>
                    <p><strong>Production-Tested:</strong></p>
                    <ul style="margin-left: 20px; margin-top: 10px;">
                        <li>Three delta algorithms (flexibility)</li>
                        <li>Multiple compression formats</li>
                        <li>Safe rollback mechanism</li>
                        <li>Cross-platform boot management</li>
                    </ul>
                </div>

                <div class="feature-card" style="border-left-color: var(--secondary);">
                    <h4>3. Edge AI Capabilities</h4>
                    <p><strong>Future-Proof:</strong></p>
                    <ul style="margin-left: 20px; margin-top: 10px;">
                        <li>Multiple ML backends (TFLite, ONNX)</li>
                        <li>Hardware acceleration support</li>
                        <li>Quantized models (INT4-FP32)</li>
                        <li>On-device inference (no cloud)</li>
                    </ul>
                </div>

                <div class="feature-card" style="border-left-color: var(--accent);">
                    <h4>4. Multi-Protocol Gateway</h4>
                    <p><strong>Ecosystem Bridge:</strong></p>
                    <ul style="margin-left: 20px; margin-top: 10px;">
                        <li>LwM2M primary protocol</li>
                        <li>MQTT transport binding</li>
                        <li>Matter/Thread integration</li>
                        <li>Zigbee coordination</li>
                    </ul>
                </div>

                <div class="feature-card" style="border-left-color: var(--warning);">
                    <h4>5. Production-Ready Quality</h4>
                    <p><strong>Enterprise-Class:</strong></p>
                    <ul style="margin-left: 20px; margin-top: 10px;">
                        <li>Comprehensive test suite</li>
                        <li>Code coverage instrumentation</li>
                        <li>Sanitizers (address, undefined)</li>
                        <li>Thread-safe operations</li>
                    </ul>
                </div>

                <div class="feature-card" style="border-left-color: var(--danger);">
                    <h4>6. Platform-Agnostic Design</h4>
                    <p><strong>Portability:</strong></p>
                    <ul style="margin-left: 20px; margin-top: 10px;">
                        <li>Linux, OpenWRT, RPi, prplOS</li>
                        <li>Bootloader abstractions</li>
                        <li>Partition management</li>
                        <li>Easy to port (clean separation)</li>
                    </ul>
                </div>
            </div>
        </div>

        <!-- Slide 18: Technical Differentiators -->
        <div class="slide">
            <div class="slide-number">18/20</div>
            <h2>Why Friendly LwM2M Client for R&D?</h2>

            <h3>For Research & Development Teams:</h3>

            <div style="background: linear-gradient(135deg, #f0fdf4 0%, #dcfce7 100%); border-radius: 12px; padding: 25px; margin: 20px 0; border-left: 6px solid var(--success);">
                <h4 style="margin-top: 0; color: #166534;">✅ Experimentation Platform</h4>
                <ul style="margin-left: 20px;">
                    <li><strong>Runtime DTLS switching:</strong> Compare providers without rebuilding</li>
                    <li><strong>Multiple ML backends:</strong> Test TFLite vs ONNX performance</li>
                    <li><strong>Delta algorithm evaluation:</strong> Benchmark BSDIFF vs VCDIFF vs Courgette</li>
                    <li><strong>Transport flexibility:</strong> CoAP vs MQTT performance testing</li>
                </ul>
            </div>

            <div style="background: linear-gradient(135deg, #eff6ff 0%, #dbeafe 100%); border-radius: 12px; padding: 25px; margin: 20px 0; border-left: 6px solid var(--primary);">
                <h4 style="margin-top: 0; color: #1e40af;">🔬 Research-Friendly Architecture</h4>
                <ul style="margin-left: 20px;">
                    <li><strong>Modern C++17:</strong> Template metaprogramming, smart pointers, RAII</li>
                    <li><strong>Design patterns:</strong> Factory, Template, Observer, Strategy</li>
                    <li><strong>Extensible:</strong> Easy to add custom objects and resources</li>
                    <li><strong>Well-documented:</strong> Doxygen, architecture diagrams, API docs</li>
                </ul>
            </div>

            <div style="background: linear-gradient(135deg, #faf5ff 0%, #f3e8ff 100%); border-radius: 12px; padding: 25px; margin: 20px 0; border-left: 6px solid var(--secondary);">
                <h4 style="margin-top: 0; color: #6b21a8;">🚀 Innovation Enabler</h4>
                <ul style="margin-left: 20px;">
                    <li><strong>Edge AI integration:</strong> Prototype on-device ML applications</li>
                    <li><strong>IoT gateway:</strong> Bridge LwM2M, MQTT, Matter, Zigbee</li>
                    <li><strong>Advanced FOTA:</strong> Research delta compression algorithms</li>
                    <li><strong>Platform abstraction:</strong> Port to new platforms easily</li>
                </ul>
            </div>

            <h3>Competitive Advantages</h3>
            <table>
                <thead>
                    <tr>
                        <th>Feature</th>
                        <th>Friendly LwM2M</th>
                        <th>Typical Alternatives</th>
                    </tr>
                </thead>
                <tbody>
                    <tr>
                        <td>DTLS Providers</td>
                        <td>4 providers, runtime selection</td>
                        <td>Single hardcoded provider</td>
                    </tr>
                    <tr>
                        <td>FOTA</td>
                        <td>3 delta algorithms + rollback</td>
                        <td>Full binary only</td>
                    </tr>
                    <tr>
                        <td>Edge AI</td>
                        <td>TFLite + ONNX + hardware accel</td>
                        <td>Cloud-only inference</td>
                    </tr>
                    <tr>
                        <td>Transports</td>
                        <td>CoAP + MQTT (Section 8)</td>
                        <td>CoAP only</td>
                    </tr>
                    <tr>
                        <td>License</td>
                        <td>MIT (permissive)</td>
                        <td>GPL/proprietary</td>
                    </tr>
                </tbody>
            </table>
        </div>

        <!-- Slide 19: Use Cases & Applications -->
        <div class="slide">
            <div class="slide-number">19/20</div>
            <h2>Use Cases & Applications</h2>

            <h3>Target Deployment Scenarios</h3>

            <div class="feature-grid">
                <div class="feature-card">
                    <h4>🏭 Industrial IoT</h4>
                    <ul style="margin-left: 20px; margin-top: 10px;">
                        <li>Factory automation sensors</li>
                        <li>Predictive maintenance (Edge AI)</li>
                        <li>Secure OTA updates</li>
                        <li>Multi-protocol integration</li>
                    </ul>
                </div>

                <div class="feature-card">
                    <h4>🏠 Smart Home/Building</h4>
                    <ul style="margin-left: 20px; margin-top: 10px;">
                        <li>WiFi/Thread gateway</li>
                        <li>Matter/Zigbee bridge</li>
                        <li>Edge inference (anomaly detection)</li>
                        <li>Secure device management</li>
                    </ul>
                </div>

                <div class="feature-card">
                    <h4>🌐 Telecom/Network Equipment</h4>
                    <ul style="margin-left: 20px; margin-top: 10px;">
                        <li>OpenWRT routers</li>
                        <li>Multi-WAN management</li>
                        <li>VPN configuration</li>
                        <li>Remote diagnostics</li>
                    </ul>
                </div>

                <div class="feature-card">
                    <h4>🚗 Connected Vehicles</h4>
                    <ul style="margin-left: 20px; margin-top: 10px;">
                        <li>Telematics gateway</li>
                        <li>OTA firmware updates</li>
                        <li>Edge AI (ADAS data)</li>
                        <li>Multi-bearer selection</li>
                    </ul>
                </div>

                <div class="feature-card">
                    <h4>🌾 Agriculture/Environment</h4>
                    <ul style="margin-left: 20px; margin-top: 10px;">
                        <li>Soil moisture sensors</li>
                        <li>Weather stations</li>
                        <li>LPWAN connectivity (NB-IoT)</li>
                        <li>Delta updates (bandwidth savings)</li>
                    </ul>
                </div>

                <div class="feature-card">
                    <h4>📡 Satellite IoT</h4>
                    <ul style="margin-left: 20px; margin-top: 10px;">
                        <li>Starlink integration</li>
                        <li>Remote asset tracking</li>
                        <li>Maritime/aviation IoT</li>
                        <li>NTN (Non-Terrestrial Networks)</li>
                    </ul>
                </div>
            </div>

            <h3>Reference Deployments</h3>
            <ul>
                <li><strong>OpenWRT Routers:</strong> Multi-WAN failover, VPN, WiFi optimization</li>
                <li><strong>Raspberry Pi Gateways:</strong> Edge AI inference, protocol bridging</li>
                <li><strong>Industrial Gateways:</strong> prplOS integration, TR-069/TR-181 interop</li>
                <li><strong>IoT Development Kits:</strong> Rapid prototyping, SDK integration</li>
            </ul>
        </div>

        <!-- Slide 20: Contact & Next Steps -->
        <div class="slide">
            <div class="slide-number">20/20</div>
            <div class="center">
                <h2>Get Started with Friendly LwM2M Client</h2>

                <div style="margin: 40px 0;">
                    <h3>Quick Start</h3>
                    <pre style="text-align: left;"><code>git clone --recurse-submodules \\
  https://github.com/Friendly-Technologies/Friendly-LwM2M-Client.git
cd Friendly-LwM2M-Client
sh ./wpp_env_setup.sh
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
./examples/friendly_example_client</code></pre>
                </div>

                <div class="metrics-grid" style="margin: 40px 0;">
                    <div class="metric">
                        <span class="metric-value">v1.2.2</span>
                        <span class="metric-label">Current Release</span>
                    </div>
                    <div class="metric">
                        <span class="metric-value">MIT</span>
                        <span class="metric-label">Open Source</span>
                    </div>
                    <div class="metric">
                        <span class="metric-value">15+</span>
                        <span class="metric-label">LwM2M Objects</span>
                    </div>
                </div>

                <h3>Resources</h3>
                <div class="tech-stack" style="justify-content: center;">
                    <span class="badge badge-primary">GitHub Repository</span>
                    <span class="badge badge-secondary">Documentation</span>
                    <span class="badge badge-primary">API Reference</span>
                    <span class="badge badge-secondary">Examples</span>
                </div>

                <div style="margin-top: 60px;">
                    <h3>Contact Information</h3>
                    <p style="font-size: 20px; color: var(--dark);">
                        <strong>Friendly Technologies</strong><br>
                        Visit: <a href="https://hubs.li/Q02D_GnZ0" style="color: var(--primary);">https://hubs.li/Q02D_GnZ0</a><br>
                        GitHub: <a href="https://github.com/Friendly-Technologies/Friendly-LwM2M-Client" style="color: var(--primary);">Friendly-LwM2M-Client</a>
                    </p>
                </div>

                <div class="footer">
                    <p><strong>Thank you for your attention!</strong></p>
                    <p>Questions & Discussion</p>
                </div>
            </div>
        </div>

    </div>
</body>
</html>
"""

    return html

def main():
    """Main execution"""
    print("Generating Technical Pitch Deck for Friendly LwM2M Client v1.2.2...")

    html_content = generate_pitch_deck()

    # Determine output path
    script_dir = os.path.dirname(os.path.abspath(__file__))
    project_root = os.path.abspath(os.path.join(script_dir, "..", ".."))
    output_file = os.path.join(project_root, "Friendly_LwM2M_Client_v1.2.2_Technical_Pitch_Deck.html")

    # Write HTML file
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(html_content)

    print("[OK] Pitch deck generated successfully!")
    print(f"[OK] Output: {output_file}")
    print("[OK] Total slides: 20")
    print("[OK] Open in browser to view presentation")
    print("\nPresentation Statistics:")
    print("   - Executive summary with key metrics")
    print("   - Multi-DTLS provider deep dive")
    print("   - Architecture diagrams and design patterns")
    print("   - Complete feature coverage")
    print("   - Performance characteristics")
    print("   - Use cases and applications")
    print("   - Technical differentiators for R&D teams")

    return output_file

if __name__ == "__main__":
    main()
