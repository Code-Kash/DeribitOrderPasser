# Fischer Framework Design Specification
## Deribit Order Processing System

---

## Executive Summary

The Fischer Framework is a high-performance order processing system designed for the Deribit cryptocurrency derivatives exchange. Named after the legendary chess Grandmaster Bobby Fischer, the framework embodies strategic thinking, precision, and optimal performance in every move. The system processes CSV order data and transforms it into Deribit JSON-RPC format with exceptional throughput and minimal latency.

---

## System Architecture

### Core Design Principles

#### 1. Policy-Traits Design Pattern
The system employs a sophisticated policy-traits architecture that enables compile-time customization and optimization:

```cpp
struct DeribitTraits
{
    using OrderIdType = int64_t;
    using MessageIdType = int64_t;
    using PriceType = double;
    using AmountType = double;
    using SizeType = size_t;

    static constexpr size_t InitialJsonBufferSize = 40960;
    static constexpr size_t MaxOrderCount = 10000;
    static constexpr MessageIdType InitialMessageId = 5275;
};
```

This design allows for:
- Protocol-specific customization without runtime overhead
- Easy adaptation to different exchanges or protocols
- Compile-time type safety and optimization

#### 2. One Class Per Header Philosophy
Each component is isolated in its own header/implementation pair:
- `FSHR_DERIBIT_OrderProcessor.h/hxx` - Main processing orchestration
- `FSHR_DERIBIT_CSVParser.h/hxx` - CSV parsing logic
- `FSHR_DERIBIT_JSONBuilder.h/hxx` - JSON construction
- `FSHR_DERIBIT_Logger.h/hxx` - Thread-safe logging
- `FSHR_DERIBIT_Order.h` - Order data structure
- `FSHR_DERIBIT_Constants.h` - System constants
- `FSHR_DERIBIT_Utils.h` - Utility functions

#### 3. Compile-Time Optimizations
- **Template Metaprogramming**: All major classes are templated on traits
- **Constexpr Everything**: Constants and utility functions leverage constexpr
- **RAII**: Automatic resource management throughout

#### 4. Memory Management
- **Smart Pointers**: std::unique_ptr for automatic memory management
- **Pre-allocation**: Buffers pre-allocated based on estimated sizes
- **RO5**: Rule of Five properly implemented
- **Zero-Copy Operations**: Minimal string copying during parsing

---

## Performance Metrics

### Current Performance (Release Build, -O3)
- **Throughput**: 239,894 orders/second
- **Latency**: 4.17 microseconds per order
- **Parse Time**: 1.31 microseconds per order
- **JSON Build**: 1.54 microseconds per order
- **File I/O**: 1.29 microseconds per order

### Memory Profile
- **Initial Buffer**: 40KB JSON buffer
- **Growth Factor**: 2x on overflow
- **Order Reservation**: 10,000 orders pre-allocated
- **Zero Dynamic Allocation**: In steady-state operation

---

## Future Optimization Considerations

### 1. CPU Affinity and NUMA Awareness
```cpp
// Future: Pin threads to specific cores
class ThreadAffinityManager
{
    void SetCoreAffinity(std::thread& thread, uint32_t coreMask);
    void OptimizeNUMAPlacement(void* buffer, size_t size);
};
```

### 2. Multi-Threading Architecture
```cpp
// Future: Pipeline architecture
class OrderPipeline
{
    std::thread m_ParseThread;    // CSV parsing
    std::thread m_BuildThread;    // JSON building
    std::thread m_WriteThread;    // File output

    lockfree::spsc_queue<Order> m_ParseQueue;
    lockfree::spsc_queue<std::string> m_OutputQueue;
};
```

### 3. FIX Protocol Support
```cpp
// Future: Alternative protocol support
template<typename Protocol>
class ProtocolAdapter
{
    // Specialize for FIX, JSON-RPC, Binary protocols
};

template<>
class ProtocolAdapter<FIXProtocol>
{
    void BuildFIXMessage(const Order& order);
};
```

### 4. Network Integration
```cpp
// Future: Direct exchange connectivity
class DeribitWebSocketClient
{
    void SendOrderAsync(const std::string& jsonPayload);
    void HandleOrderResponse(const std::string& response);
};
```

### 5. Scalability Enhancements
- **Memory Pools**: Custom allocators for hot-path allocations
- **SIMD Operations**: Vectorized parsing for numeric fields
- **Lock-Free Data Structures**: SPSC queues for communication between systems parts
- **Zero-Copy Networking**: Using io_uring or DPDK
- **Compression**: LZ4 for large order batches

### 6. Advanced Features
- **Order Book Reconstruction**: Build local order book from messages
- **Market Data Integration**: Real-time price feeds
- **Risk Management**: Position limits and exposure calculations
- **Latency Monitoring**: Microsecond-precision telemetry

---

## Coding Standards

### Language Requirements
- **English Only**: All code, comments, and documentation must be in English
- **No Hungarian Notation**: Variable names must not use Hungarian notation

### Naming Conventions

#### Classes
- **Format**: UpperCamelCase
- **Examples**: `OrderProcessor`, `JsonBuilder`, `CsvParser`
- **Interface Prefix**: Interface classes prefixed with 'I' (e.g., `IOrderHandler`)
- **No Abstract Prefix**: Abstract classes not prefixed with 'I'

#### Namespaces
- **Format**: lowercase
- **Primary**: `fischer`
- **Secondary**: `deribit`
- **Example**: `namespace fischer::deribit`

#### Variables
- **Member Variables**: Prefix with `m_` followed by CamelCase
  - Example: `m_ProcessedOrderCount`, `m_MessageIdCounter`
- **Global Variables**: Prefix with `g` followed by lowerCamelCase
  - Example: `gDefaultTimeout`, `gMaxRetries`
- **Constants**: UpperCamelCase without prefix
  - Example: `InitialBufferSize`, `MaxOrderCount`

#### Functions
- **Format**: UpperCamelCase
- **Verb-Noun Pattern**: Describe "what" not "how"
  - Good: `CalculatePosition()`, `ParseOrders()`
  - Bad: `DoCalculation()`, `HandleParsing()`
- **Accessors**: Use Get/Set/Is pattern
  - Examples: `GetOrderCount()`, `SetPrice()`, `IsValid()`

#### Enums
- **Use enum class**: Never plain enum
- **Format**: UpperCamelCase for type and values
```cpp
enum class OrderDirection : uint8_t
{
    Buy = 0,
    Sell = 1
};
```

### File Organization

#### File Naming
- **Headers**: `.h` extension
- **Implementation**: `.hxx` for templates, `.cpp` for regular source
- **Consistency**: All files prefixed with `FSHR_DERIBIT_`

#### Header Guards
```cpp
#pragma once  // Required at top of every header
```

#### Include Order
1. Project headers first
2. Third-party headers
3. Standard library headers
```cpp
#include "FSHR_DERIBIT_Order.h"        // Project
#include <boost/asio.hpp>              // Third-party
#include <string>                       // Standard
```

### Class Design

#### Declaration Order
```cpp
class OrderProcessor
{
public:
    // Public methods and types

protected:
    // Protected methods and types

private:
    // Private methods and types
    // Member variables at the end

private:
    int64_t m_MessageIdCounter;
    ProcessingStatus m_Status;
};
```

### Code Style

#### Formatting
- **Style**: Allman (BSD) style braces
```cpp
if (condition)
{
    // Code here
}
```

#### Whitespace
- **Control Flow**: Single space after keywords
```cpp
if (condition)  // Space after if
for (auto& item : container)  // Space after for
```
- **No Space**: Inside parentheses
```cpp
Function(param1, param2);  // No space after ( or before )
```

#### Comparisons
- **Yoda Style**: Constants on left (optional but recommended)
```cpp
if (nullptr == pointer)
if (42 == value)
if (true == flag)
```

### Best Practices

#### Functions
- **Line Limit**: 50-100 lines maximum
- **Parameter Count**: Minimize arguments (prefer structures for multiple related parameters)
- **Reentrant**: Avoid static variables
- **const Correctness**: Mark methods const when they don't modify state

#### Error Handling
- **Exceptions**: Use for exceptional conditions
- **No throw Specifications**: Don't use throw() or noexcept(false)
- **RAII**: Ensure exception safety through RAII

#### Magic Numbers
```cpp
// Bad
if (orders.size() > 10000)

// Good
constexpr size_t MaxOrderCount = 10000;
if (orders.size() > MaxOrderCount)
```

#### Namespace Usage
- **Never in Headers**: Never `using namespace` in headers
- **Avoid in Source**: Prefer explicit namespace qualification
```cpp
// Bad
using namespace std;
string name;

// Good
std::string name;
```

---

## Build Configuration

### Compiler Requirements
- **Minimum**: GCC 11.0 or Clang 13.0
- **Recommended**: GCC 14.2 or latest Clang
- **Standard**: C++20

### Optimization Flags
```makefile
DEBUG_FLAGS = -g -O0 -DDEBUG -fsanitize=address
RELEASE_FLAGS = -O3 -DNDEBUG -march=native -flto
PROFILE_FLAGS = -O2 -pg -fno-omit-frame-pointer
```

### Dependencies
- **Required**: Standard C++ library only

---

## Testing Strategy

### Unit Tests
- Each class should have corresponding test file
- Minimum 80% code coverage
- Edge cases and error conditions must be tested

### Performance Tests
- Benchmark against baseline metrics
- Profile for memory leaks and hotspots
- Stress test with maximum order volumes

### Integration Tests
- End-to-end order processing validation
- Protocol compliance verification
- Network connectivity tests (future)

---

## Version History

| Version | Date | Description |
|---------|------|-------------|
| 1.0.0   | 2024 | Initial release with CSV to JSON-RPC conversion |
| 1.1.0   | TBD  | Multi-threading support |
| 1.2.0   | TBD  | FIX protocol adapter |
| 2.0.0   | TBD  | Network integration and real-time processing |
