# CUnit: C++ Unit Conversion Library

A lightweight, header-only C++ library for dimensional analysis and unit conversion. Using C++ templates and user-defined literals to provide type-safe operations on physical quantities.

## Features

- **Dimensional Safety**: Prevents mixing incompatible units (e.g., adding Temperature to Length) at runtime via exceptions.
- **User-Defined Literals**: Intuitive syntax for defining quantities: `10.0_km`, `21.5_C`, etc.
- **Automatic Conversion**: Perform arithmetic operations between different units of the same dimension seamlessly.
- **Unit Casting**: Explicit conversion between units using `unit_cast<From, To>`.
- **Supported Dimensions**:
  - **Length**: `m`, `km`, `cm`, `mm`, `in`, `ft`, `mi`.
  - **Temperature**: `Celsius` (°C), `Fahrenheit` (°F).

## Usage Examples

### Basic Length Operations
```cpp
auto dist1 = 5.0_km;
auto dist2 = 500.0_m;

auto sum = dist1 + dist2;       // Result is in Kilometers (5.50 km)
auto diff = dist1 - dist2;      // Result is in Kilometers (4.50 km)

dist1.PPrint();                 // Output: 5.00 km
sum.PPrint();                   // Output: 5.50 km
```

### Temperature Conversion
```cpp
auto temp_c = 21.5_C;
auto temp_f = 77.0_F;

// Automatic conversion in operations
auto total = temp_c + temp_f;   // Converts Fahrenheit to Celsius then adds
total.PPrint();                 // Output: 46.50 °C

// Explicit casting
auto converted = unit_cast<Celsius, Fahrenheit>(temp_c);
converted.PPrint();             // Output: 70.70 °F
```

### Type Safety and Exceptions
The library throws `IncompatilbeDimException` if you attempt to operate on or cast between different dimensions.

```cpp
try {
    auto result = 10.0_m + 32.0_F; // Throws IncompatilbeDimException
} catch (const IncompatilbeDimException& ex) {
    printf("Error: %s\n", ex.what());
}
```

## How to use
Simply include `cunit.cpp` (or rename to `.hpp` for header-only usage) in your project. It requires C++11 or later for `std::ratio` and user-defined literals.
