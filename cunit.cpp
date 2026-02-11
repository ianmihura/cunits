#include <exception>
#include <ratio>
#include <stdint.h>
#include <stdio.h>

class IncompatilbeDimException : public std::exception {
private:
  const char *message;

public:
  IncompatilbeDimException(const char *msg) : message(msg) {};
  const char *what() const noexcept { return message; };
};

struct Length {
  static uint8_t const ID = 1;
};
struct Temperature {
  static uint8_t const ID = 2;
};
struct Time {
  static uint8_t const ID = 3;
};

template <typename Dimension, typename Ratio> struct UnitSuffix {
  static constexpr const char *value = "<unit>";
};
template <> struct UnitSuffix<Length, std::ratio<1>> {
  static constexpr const char *value = "m";
};
template <> struct UnitSuffix<Length, std::ratio<1000>> {
  static constexpr const char *value = "km";
};
template <> struct UnitSuffix<Length, std::ratio<1, 100>> {
  static constexpr const char *value = "cm";
};
template <> struct UnitSuffix<Length, std::ratio<1, 1000>> {
  static constexpr const char *value = "mm";
};
template <> struct UnitSuffix<Length, std::ratio<1, 40>> {
  static constexpr const char *value = "in";
};
template <> struct UnitSuffix<Length, std::ratio<3>> {
  static constexpr const char *value = "ft";
};
template <> struct UnitSuffix<Length, std::ratio<1609>> {
  static constexpr const char *value = "mi";
};
template <> struct UnitSuffix<Temperature, std::ratio<1>> {
  static constexpr const char *value = "°C";
};
template <> struct UnitSuffix<Temperature, std::ratio<5, 9>> {
  static constexpr const char *value = "°F";
};
template <> struct UnitSuffix<Time, std::ratio<1>> {
  static constexpr const char *value = "min";
};
template <> struct UnitSuffix<Time, std::ratio<1, 60 * 1000>> {
  static constexpr const char *value = "ms";
};
template <> struct UnitSuffix<Time, std::ratio<1, 60 * 1000 * 1000>> {
  static constexpr const char *value = "µs";
};
template <> struct UnitSuffix<Time, std::ratio<1, 60>> {
  static constexpr const char *value = "s";
};
template <> struct UnitSuffix<Time, std::ratio<60>> {
  static constexpr const char *value = "hs";
};
template <> struct UnitSuffix<Time, std::ratio<60 * 24>> {
  static constexpr const char *value = "d";
};

template <typename Dimension_, typename Ratio = std::ratio<1>,
          int OffsetNum = 1, int OffsetDen = 1>
struct Quantity {
  double value;
  explicit Quantity(double val) : value(val) {}
  explicit Quantity(int val) : value(val) {}
  static constexpr const char *suffix = UnitSuffix<Dimension_, Ratio>::value;

  using Dimension = Dimension_;
  using Scale = Ratio;
  using Offset = std::ratio<OffsetNum, OffsetDen>;

  void PPrint() { printf("%.2f %s\n", value, suffix); };
  void PPrint(int i) {
    for (; i > 0; i--) {
      printf(" ");
    }
    PPrint();
  };
};

using Meters = Quantity<Length, std::ratio<1>>;
using Kilometers = Quantity<Length, std::ratio<1000>>;
using Centimeters = Quantity<Length, std::ratio<1, 100>>;
using Millimeters = Quantity<Length, std::ratio<1, 1000>>;
using Inches = Quantity<Length, std::ratio<1, 40>>;
using Feet = Quantity<Length, std::ratio<3>>;
using Miles = Quantity<Length, std::ratio<1609>>;
using Celsius = Quantity<Temperature, std::ratio<1>>;
using Fahrenheit = Quantity<Temperature, std::ratio<5, 9>, 32>;
using Minutes = Quantity<Time, std::ratio<1>>;
using Seconds = Quantity<Time, std::ratio<1, 60>>;
using Mili = Quantity<Time, std::ratio<1, 60 * 1000>>;
using Micro = Quantity<Time, std::ratio<1, 60 * 1000 * 1000>>;
using Hours = Quantity<Time, std::ratio<60>>;
using Days = Quantity<Time, std::ratio<60 * 24>>;

#define GENERATE_UNIT_LITERAL(Unit, Suffix)                                    \
  Unit operator""_##Suffix(long double val) {                                  \
    return Unit(static_cast<double>(val));                                     \
  }                                                                            \
  Unit operator""_##Suffix(unsigned long long val) {                           \
    return Unit(static_cast<double>(val));                                     \
  }

GENERATE_UNIT_LITERAL(Meters, m)
GENERATE_UNIT_LITERAL(Kilometers, km)
GENERATE_UNIT_LITERAL(Centimeters, cm)
GENERATE_UNIT_LITERAL(Millimeters, mm)
GENERATE_UNIT_LITERAL(Inches, in)
GENERATE_UNIT_LITERAL(Feet, ft)
GENERATE_UNIT_LITERAL(Miles, mi)
GENERATE_UNIT_LITERAL(Celsius, C)
GENERATE_UNIT_LITERAL(Fahrenheit, F)
GENERATE_UNIT_LITERAL(Minutes, min)
GENERATE_UNIT_LITERAL(Seconds, s)
GENERATE_UNIT_LITERAL(Mili, ms)
GENERATE_UNIT_LITERAL(Micro, mus)
GENERATE_UNIT_LITERAL(Hours, h)
GENERATE_UNIT_LITERAL(Days, day)

template <typename From, typename To> To unit_cast(From q) {
  if (To::Dimension::ID != From::Dimension::ID) {
    throw IncompatilbeDimException(
        "You can only convert units of the same Dimension");
  }

  using Factor = std::ratio_divide<typename From::Scale, typename To::Scale>;
  double result = q.value * Factor::num / Factor::den;

  if (From::Offset::num != To::Offset::num) {
    if (From::Offset::num > To::Offset::num) {
      result -= From::Offset::num;
    } else {
      result += To::Offset::num;
    }
  }

  return To(result);
}

template <typename LeftT, typename RightT>
auto operator+(LeftT left, RightT right) {
  auto converted_right = unit_cast<RightT, LeftT>(right);
  return LeftT(left.value + converted_right.value);
}
template <typename LeftT, typename RightT>
auto operator-(LeftT left, RightT right) {
  auto converted_right = unit_cast<RightT, LeftT>(right);
  return LeftT(left.value - converted_right.value);
}
// template <typename LeftT, typename RightT>
// auto operator*(LeftT left, RightT right) {
//   auto converted_right = unit_cast<RightT, LeftT>(right);
//   return LeftT(left.value * converted_right.value);
// }
// template <typename LeftT, typename RightT>
// auto operator/(LeftT left, RightT right) {
//   auto converted_right = unit_cast<RightT, LeftT>(right);
//   return LeftT(left.value / converted_right.value);
// }

int main() {
  printf("--- testing length assign and operation\n");
  auto dist1 = 5_km;
  auto dist2 = 500_m;
  auto dist3 = dist1 - dist2;
  auto dist4 = dist1 + dist2;

  dist1.PPrint(2);
  dist2.PPrint(2);
  dist3.PPrint(2);
  dist4.PPrint(2);
  printf("---\n");

  printf("--- testing length cast\n");
  unit_cast<Kilometers, Meters>(dist1).PPrint();
  unit_cast<Kilometers, Millimeters>(dist1).PPrint();
  unit_cast<Meters, Millimeters>(dist2).PPrint();
  unit_cast<Meters, Inches>(1_m).PPrint();
  unit_cast<Meters, Feet>(1_m).PPrint();
  unit_cast<Meters, Miles>(1_m).PPrint();
  printf("---\n");

  printf("--- testing temperature assign and operators\n");
  auto temp_c = 21.5_C;
  auto temp_f = 77_F;
  temp_c.PPrint();
  temp_f.PPrint();
  (21.5_C + 77_F).PPrint();
  (77_F + 21.5_C).PPrint();
  printf("---\n");

  printf("--- testing temperature cast\n");
  unit_cast<Celsius, Fahrenheit>(temp_c).PPrint();
  unit_cast<Fahrenheit, Celsius>(temp_f).PPrint();
  printf("---\n");

  printf("--- testing time assign and operators\n");
  auto time_1 = 48.5_min;
  auto time_2 = 120_s;
  auto time_3 = 3.5_h;
  auto time_4 = 0.1_day;
  time_1.PPrint();
  time_2.PPrint();
  time_3.PPrint();
  time_4.PPrint();
  printf("---\n");

  printf("--- testing time cast\n");
  unit_cast<Seconds, Minutes>(time_2).PPrint();
  unit_cast<Days, Minutes>(time_4).PPrint();
  printf("---\n");

  printf("--- testing cast fail\n");
  try {
    unit_cast<Fahrenheit, Meters>(temp_f);
  } catch (const IncompatilbeDimException &ex) {
    printf("ok\n");
  }
  try {
    temp_f + dist1;
  } catch (const IncompatilbeDimException &ex) {
    printf("ok\n");
  }
  printf("---\n");

  (3_day).PPrint();

  return 0;
}
