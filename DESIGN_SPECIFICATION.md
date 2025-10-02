# Fischer Framework Design Specification

---

The Fischer Framework, named after chess grandmaster Bobby Fischer, is a high-performance order processing system designed for the Deribit cryptocurrency derivatives exchange. The system processes CSV order data and transforms it into Deribit JSON-RPC format.

---

## System Architecture

### Core Design Principles

#### 1. Traits Design Pattern
The framework employs a traits-based architecture that enables compile-time configuration and optimization:

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
    static constexpr size_t MaxFieldCount = 32;
    static constexpr MessageIdType InitialMessageId = 5275;

    // Performance tuning flags
    static constexpr bool EnableVectorReserve = true;
    static constexpr bool EnableBufferPreallocation = true;
};
```

This traits pattern allows:
- **Compile-time Polymorphism**: Different exchange protocols can be supported by swapping traits (i.e. BitsoTraits, HyperliquidTraits, etc.)
- **Zero-cost Abstractions**: All trait lookups resolved at compile time
- **Type Safety**: Strong typing through trait typedefs prevents errors

#### 2. Compile-Time Optimizations

##### Template Metaprogramming
All core components are templated on the traits parameter, enabling:
- **Inlining**: Compiler can inline all trait-dependent code
- **Dead Code Elimination**: Unused code paths removed at compile time
- **Constant Propagation**: Trait constants baked into machine code

##### Constexpr
Extensive use of constexpr for compile-time computation:
- **String Literals**: All protocol strings are constexpr std::string_view
- **Utility Functions**: ParseBool, field lookups resolved at compile time
- **Size Calculations**: Buffer sizes computed during compilation

##### Cache-Line Optimization
Data structures aligned to 64-byte cache lines:
```cpp
struct alignas(64) Order
{
    // Hot data grouped together
    OrderIdType m_Id;
    AmountType m_Amount;
    AmountType m_Contracts;
    std::string m_InstrumentName;
    // Cold data at the end
    std::optional<std::string> m_TriggerFillCondition;
};
```

#### 3. RAII and Memory Management
- **Automatic Resource Management**: All resources managed through RAII
- **Pre-allocation**: Vectors and strings reserve capacity upfront
- **Move Semantics**: Efficient transfer of ownership without copying
- **Zero Dynamic Allocation**: In steady-state operation after initialization

---

## How to Build

### Quick Start
```bash
make release

./bin/deribit_order_passer deribit_orders.txt output.txt

make debug

make clean
```

### Requirements
- C++20 compatible compiler (GCC 11+ or Clang 14+)

---

## Performance Metrics

### Current Performance (Release Build, -O3, with optimizations)
- **Throughput**: ~293,000 orders/second
- **Latency**: 2.82 - 3.85 microseconds per order (average: 3.41 μs)
- **Parse Time**: ~1.23 microseconds per order
- **JSON Build**: ~1.43 microseconds per order
- **File I/O**: ~1.15 microseconds per order

*Performance measured using std::chrono::high_resolution_clock for microsecond-precision timing of 10,000 order processing cycles.*

---

## Future Optimization

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
Lock-free SPSC (Single Producer Single Consumer) queues would enable high-performance pipelined processing with dedicated threads for parsing, JSON building, and I/O operations, eliminating contention and maximizing throughput. In even further cases, these queues can optimize communication of data from the market data feed handlers -> execution system -> order passer.

```cpp
// Future: Pipeline architecture using lock-free data structures
class OrderPipeline
{
    std::thread m_ParseThread;    // CSV parsing
    std::thread m_BuildThread;    // JSON building
    std::thread m_WriteThread;    // File output

    boost::lockfree::spsc_queue<Order> m_ParseQueue;
    boost::lockfree::spsc_queue<std::string> m_OutputQueue;
};
```

### 3. FIX Protocol Support
FIX protocol would be needed for production trading systems as it's the industry standard for order routing and execution with guaranteed message delivery and recovery.

### 4. Network Integration
Direct exchange connectivity would utilize WebSocket or binary protocols with kernel bypass networking (DPDK/io_uring/taskset) for ultra-low latency order submission and market data reception.

### 5. Market Data Feeder
A dedicated market data feeder component would consume real-time price feeds, maintaining local order books and providing tick-by-tick updates for trading decisions and risk calculations.

### 6. Latency Measurement & Testing
Replace std::chrono with hardware timestamping solutions like Corvil for nanosecond-precision latency measurement, enabling comprehensive performance analysis across the entire trading infrastructure. Also, develop automated testing infrastructure including unit tests, integration tests, and latency benchmarks to validate correctness and performance under various market conditions and order volumes.

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