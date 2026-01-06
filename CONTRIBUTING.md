# Contributing to Friendly LwM2M Client

Thank you for your interest in contributing to the Friendly LwM2M Client! This document provides guidelines and information for contributors.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Setup](#development-setup)
- [Making Changes](#making-changes)
- [Coding Standards](#coding-standards)
- [Testing](#testing)
- [Documentation](#documentation)
- [Submitting Changes](#submitting-changes)
- [Review Process](#review-process)

---

## Code of Conduct

This project adheres to a Code of Conduct that all contributors are expected to follow. Please read [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md) before contributing.

**Key principles:**
- Be respectful and inclusive
- Focus on constructive feedback
- Prioritize the project's best interests
- Help maintain a welcoming environment

---

## Getting Started

### Prerequisites

Before contributing, ensure you have:

1. **Git** installed and configured
2. **C++17 compatible compiler** (GCC 8+, Clang 7+)
3. **CMake 3.16+**
4. **Development dependencies**:
   ```bash
   # Ubuntu/Debian
   sudo apt-get install build-essential cmake libssl-dev libcurl4-openssl-dev \
       zlib1g-dev clang-format cppcheck doxygen lcov
   ```

### Fork and Clone

1. Fork the repository on GitHub
2. Clone your fork:
   ```bash
   git clone https://github.com/YOUR_USERNAME/Friendly-LwM2M-Client.git
   cd Friendly-LwM2M-Client
   ```
3. Add upstream remote:
   ```bash
   git remote add upstream https://github.com/FriendlyTechnologies/Friendly-LwM2M-Client.git
   ```

---

## Development Setup

### Build Configuration

```bash
# Create build directory
mkdir build && cd build

# Configure with all development options
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_TESTS=ON \
    -DENABLE_COVERAGE=ON \
    -DENABLE_SANITIZERS=ON \
    -DWITH_MQTT=ON \
    -DWITH_EDGE_AI=ON

# Build
make -j$(nproc)
```

### IDE Setup

#### VS Code
```json
// .vscode/settings.json
{
    "cmake.configureArgs": [
        "-DCMAKE_BUILD_TYPE=Debug",
        "-DBUILD_TESTS=ON"
    ],
    "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools"
}
```

#### CLion
- Open the project directory
- CMake options are auto-detected
- Set build type to Debug for development

---

## Making Changes

### Branch Naming

Use descriptive branch names:

| Type | Pattern | Example |
|------|---------|---------|
| Feature | `feature/description` | `feature/mqtt-qos2-support` |
| Bug fix | `fix/description` | `fix/blockwise-timeout` |
| Documentation | `docs/description` | `docs/edge-ai-guide` |
| Refactor | `refactor/description` | `refactor/transport-layer` |
| Test | `test/description` | `test/mqtt-stress-tests` |

### Workflow

1. **Create a branch**:
   ```bash
   git checkout -b feature/your-feature
   ```

2. **Make changes** following coding standards

3. **Commit with meaningful messages**:
   ```bash
   git commit -m "feat: add MQTT QoS 2 support

   - Implement exactly-once delivery
   - Add PUBREC/PUBREL/PUBCOMP handling
   - Update tests for QoS 2

   Closes #123"
   ```

4. **Keep up to date**:
   ```bash
   git fetch upstream
   git rebase upstream/main
   ```

---

## Coding Standards

### C++ Style Guide

We follow a modified Google C++ Style Guide with these specifics:

#### Formatting

```cpp
// Use 4-space indentation
void function() {
    if (condition) {
        doSomething();
    }
}

// Opening brace on same line
class MyClass {
public:
    void method();
};

// Pointer/reference alignment
void function(const std::string& str, int* ptr);
```

#### Naming Conventions

```cpp
// Classes: PascalCase
class MqttTransport;
class EdgeAIInferenceObject;

// Functions/Methods: camelCase
void processMessages();
bool isConnected() const;

// Variables: camelCase
int messageCount;
std::string endpointName_;  // Member variables with trailing underscore

// Constants: UPPER_SNAKE_CASE
constexpr int MAX_BLOCK_SIZE = 1024;
static const char* DEFAULT_PREFIX = "lwm2m";

// Enums: PascalCase with PascalCase values
enum class MqttQoS {
    AtMostOnce = 0,
    AtLeastOnce = 1,
    ExactlyOnce = 2
};

// Namespaces: lowercase
namespace lwm2m {
namespace transport {
```

#### Headers

```cpp
// Include guards
#ifndef FRIENDLY_LWM2M_MQTT_TRANSPORT_H
#define FRIENDLY_LWM2M_MQTT_TRANSPORT_H

// Include order:
// 1. Corresponding header
// 2. C system headers
// 3. C++ standard library
// 4. Other libraries
// 5. Project headers

#include "mqtt_transport.h"

#include <cstdint>
#include <cstring>

#include <string>
#include <vector>
#include <memory>

#include <openssl/ssl.h>

#include "lwm2m_core.h"
#include "transport/base_transport.h"

#endif // FRIENDLY_LWM2M_MQTT_TRANSPORT_H
```

### Code Formatting Tool

```bash
# Format all source files
find src include -name "*.cpp" -o -name "*.h" | xargs clang-format -i

# Check formatting without modifying
find src include -name "*.cpp" -o -name "*.h" | xargs clang-format --dry-run -Werror
```

### Static Analysis

```bash
# Run cppcheck
cppcheck --enable=all --std=c++17 src/ include/

# Run clang-tidy
clang-tidy src/*.cpp -- -std=c++17 -I include/
```

---

## Testing

### Writing Tests

All new features must include tests:

```cpp
#include <gtest/gtest.h>
#include "transport/mqtt_transport.h"

class MqttTransportTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code
    }

    void TearDown() override {
        // Cleanup code
    }
};

TEST_F(MqttTransportTest, ConnectToValidBroker) {
    // Arrange
    MqttTransport::Config config;
    config.server.brokerUri = "localhost";
    MqttTransport transport(config);

    // Act
    auto result = transport.connect();

    // Assert
    EXPECT_EQ(result, MqttResult::SUCCESS);
}
```

### Running Tests

```bash
# Run all tests
make run_tests

# Run specific test
./test_mqtt_transport

# Run with verbose output
./test_mqtt_transport --gtest_output=xml:results.xml

# Run specific test case
./test_mqtt_transport --gtest_filter=MqttTransportTest.ConnectToValidBroker
```

### Coverage Requirements

- **Minimum coverage**: 80% for new code
- **Target coverage**: 90%+

```bash
# Generate coverage report
make coverage

# View report
open coverage/index.html
```

---

## Documentation

### Code Documentation

Use Doxygen-style comments:

```cpp
/**
 * @file mqtt_transport.h
 * @brief LwM2M MQTT Transport Binding Implementation
 * 
 * Implements OMA LwM2M v1.2.1 Section 8 MQTT transport binding.
 * 
 * @copyright Copyright (c) 2024-2026 Friendly Technologies
 * @license MIT License
 */

/**
 * @brief Connect to MQTT broker
 * 
 * Establishes a connection to the configured MQTT broker using
 * the security settings specified in the configuration.
 * 
 * @return MqttResult::SUCCESS on successful connection
 * @return MqttResult::ERROR_TIMEOUT if connection times out
 * @return MqttResult::ERROR_TLS if TLS handshake fails
 * 
 * @note This method blocks until connection completes or times out.
 * @see disconnect()
 * 
 * @code
 * MqttTransport transport(config);
 * if (transport.connect() == MqttResult::SUCCESS) {
 *     // Connected successfully
 * }
 * @endcode
 */
MqttResult connect();
```

### User Documentation

Update relevant docs in `docs/`:

- `GETTING_STARTED.md` - Quick start guide
- `API_REFERENCE.md` - API documentation
- `BUILD_GUIDE.md` - Build instructions
- Feature-specific guides

---

## Submitting Changes

### Commit Messages

Follow [Conventional Commits](https://www.conventionalcommits.org/):

```
<type>(<scope>): <subject>

<body>

<footer>
```

**Types:**
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation
- `style`: Formatting (no code change)
- `refactor`: Code restructuring
- `perf`: Performance improvement
- `test`: Adding tests
- `chore`: Maintenance tasks

**Examples:**
```
feat(mqtt): add QoS 2 support for exactly-once delivery

Implement the full QoS 2 handshake with PUBREC, PUBREL, and PUBCOMP
messages. This ensures exactly-once delivery semantics for critical
LwM2M operations.

- Add message state tracking
- Implement timeout and retry logic
- Add comprehensive unit tests

Closes #123
```

### Pull Request Process

1. **Ensure all tests pass**:
   ```bash
   make run_tests
   ```

2. **Update documentation** as needed

3. **Create pull request** with:
   - Clear title describing the change
   - Description of what and why
   - Link to related issues
   - Screenshots/examples if applicable

4. **Address review feedback** promptly

### Pull Request Template

```markdown
## Description
Brief description of the changes.

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Documentation update
- [ ] Refactoring
- [ ] Performance improvement

## Testing
- [ ] Unit tests added/updated
- [ ] Integration tests added/updated
- [ ] Manual testing performed

## Checklist
- [ ] Code follows project style guidelines
- [ ] Self-review completed
- [ ] Documentation updated
- [ ] Tests pass locally
- [ ] No new warnings

## Related Issues
Fixes #(issue number)
```

---

## Review Process

### What Reviewers Look For

1. **Correctness**: Does the code work as intended?
2. **Tests**: Are there adequate tests?
3. **Style**: Does it follow coding standards?
4. **Documentation**: Is it well-documented?
5. **Performance**: Any performance concerns?
6. **Security**: Any security implications?

### Review Timeline

- Initial response: Within 2 business days
- Full review: Within 5 business days
- Re-review after changes: Within 2 business days

### Approval Requirements

- At least 1 maintainer approval
- All CI checks passing
- No unresolved conversations

---

## Questions?

- Open a [Discussion](https://github.com/FriendlyTechnologies/Friendly-LwM2M-Client/discussions)
- Email: dev@friendly-tech.com
- Join our Slack: #lwm2m-client

---

Thank you for contributing to Friendly LwM2M Client! 🙏
