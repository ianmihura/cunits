# Cunits: Compile-Time Unit Conversion

**Cunits** is a header-only C++ library for dimensional analysis. It uses template metaprogramming to ensure unit compatibility at compile time, eliminating runtime overhead and unit-mismatch errors.

---

## Features

* **Zero Runtime Overhead**: All conversions and checks occur during compilation.
* **Strong Typing**: Prevents nonsensical operations (e.g., adding meters to kilograms).
* **Expressive Syntax**: Supports User-Defined Literals (UDLs) like `5.0_m`.
* **Precision**: Uses `double` for exact conversion factors.

---

## Technical Architecture

The library relies on three C++ pillars:

1. **`std::ratio`**: Manages conversion factors as rational numbers.
2. **Template Metaprogramming**: Validates types using SFINAE and Type Traits.
3. **Strong Typing**: Wraps arithmetic types in unit-specific classes.

---

## Usage

### Basic Operations

Define measurements using literals. The compiler prevents incompatible operations.

```cpp
auto distance = 10.0_m;
auto time = 2.0_s;

// Valid: Results in a new unit type (m/s)
auto velocity = distance / time;

// Compilation Error: Cannot add meters to seconds
auto error = distance + time;
```

### Conversion

Convert between compatible units (e.g., Length) while blocking incompatible ones.

```cpp
Meter m = 1.0_m;
Foot f = m; // Automatic conversion via template operator

Celsius temp = 25.0_C;
Fahrenheit f_temp = temp; // Supported via specialized conversion traits
```

---

## Implementation Details

### Unit Class Template

The core `Quantity` class template takes a `Dimension` tag and a `Ratio` (relative to a base unit).

```cpp
template <typename Dimension, typename Ratio>
class Quantity {
    double value_;
public:
    explicit constexpr Quantity(double v) : value_(v) {}
    double value() const { return value_; }

    // Conversion constructor for compatible dimensions
    template <typename OtherRatio>
    constexpr Quantity(const Quantity<Dimension, OtherRatio>& other)
        : value_(other.value() * Ratio::num / Ratio::den) {}
};

```

### User-Defined Literals

UDLs provide a clean interface for initializing quantities.

```cpp
constexpr Quantity<Length, std::ratio<1>> operator"" _m(long double d) {
    return Quantity<Length, std::ratio<1>>(static_cast<double>(d));
}

constexpr Quantity<Length, std::ratio<3048, 10000>> operator"" _ft(long double d) {
    return Quantity<Length, std::ratio<3048, 10000>>(static_cast<double>(d));
}

```

---

## Installation

Copy the `Cunits.hpp` header into your project. Requires a C++17 compatible compiler.
