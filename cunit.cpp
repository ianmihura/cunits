#include <stdint.h>
#include <stdio.h>
#include <exception>
#include <ratio>

class IncompatilbeDimException : public std::exception {
private:
    const char* message;
public:
    IncompatilbeDimException(const char* msg) : message(msg) {};
    const char* what() const noexcept {
        return message;
    };
};

struct Length { static uint8_t const ID = 1; };
struct Temperature { static uint8_t const ID = 2; };
// struct Time { static uint8_t const ID = 3; };
// struct Weight { static uint8_t const ID = 4; };

template <typename Dimension, typename Ratio> struct UnitSuffix {
    static constexpr const char* value = "<unit>";
};
template <> struct UnitSuffix<Length, std::ratio<1>> {
    static constexpr const char* value = "m";
};
template <> struct UnitSuffix<Length, std::ratio<1000>> {
    static constexpr const char* value = "km";
};
template <> struct UnitSuffix<Length, std::ratio<1, 100>> {
    static constexpr const char* value = "cm";
};
template <> struct UnitSuffix<Length, std::ratio<1, 1000>> {
    static constexpr const char* value = "mm";
};
template <> struct UnitSuffix<Length, std::ratio<1, 40>> {
    static constexpr const char* value = "in";
};
template <> struct UnitSuffix<Length, std::ratio<3>> {
    static constexpr const char* value = "ft";
};
template <> struct UnitSuffix<Length, std::ratio<1609>> {
    static constexpr const char* value = "mi";
};
template <> struct UnitSuffix<Temperature, std::ratio<1>> {
    static constexpr const char* value = "°C";
};
template <> struct UnitSuffix<Temperature, std::ratio<5, 9>> {
    static constexpr const char* value = "°F";
};

template <typename Dimension_, typename Ratio = std::ratio<1>, int OffsetNum = 1, int OffsetDen = 1>
struct Quantity {
    double value;
    explicit Quantity(double val) : value(val) {}
    static constexpr const char* suffix = UnitSuffix<Dimension_, Ratio>::value;

    using Dimension = Dimension_;
    using Scale = Ratio;
    using Offset = std::ratio<OffsetNum, OffsetDen>;

    void PPrint() {
        printf("%.2f %s\n", value, suffix);
    };
    void PPrint(int i) {
        for (; i>0; i--) { printf(" "); }
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

Meters operator "" _m(long double val) {
    return Meters(static_cast<double>(val));
}
Kilometers operator "" _km(long double val) {
    return Kilometers(static_cast<double>(val));
}
Centimeters operator "" _cm(long double val) {
    return Centimeters(static_cast<double>(val));
}
Millimeters operator "" _mm(long double val) {
    return Millimeters(static_cast<double>(val));
}
Inches operator "" _in(long double val) {
    return Inches(static_cast<double>(val));
}
Feet operator "" _ft(long double val) {
    return Feet(static_cast<double>(val));
}
Miles operator "" _mi(long double val) {
    return Miles(static_cast<double>(val));
}
Celsius operator "" _C(long double val) {
    return Celsius(static_cast<double>(val));
}
Fahrenheit operator "" _F(long double val) {
    return Fahrenheit(static_cast<double>(val));
}

template <typename From, typename To>
To unit_cast(From q) {
    if (To::Dimension::ID != From::Dimension::ID) {
        throw IncompatilbeDimException("You can only convert units of the same Dimension");
    }

    using FromScale = From::Scale;
    using ToScale = To::Scale;
    using Factor = std::ratio_divide<FromScale, ToScale>;
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
template <typename LeftT, typename RightT>
auto operator*(LeftT left, RightT right) {
    auto converted_right = unit_cast<RightT, LeftT>(right);
    return LeftT(left.value * converted_right.value);
}
template <typename LeftT, typename RightT>
auto operator/(LeftT left, RightT right) {
    auto converted_right = unit_cast<RightT, LeftT>(right);
    return LeftT(left.value / converted_right.value);
}

// template <typename Dim, typename R1, typename R2>
// auto operator-(Quantity<Dim, R1> left, Quantity<Dim, R2> right) {
//     using Factor = std::ratio_divide<R2, R1>;
//     double converted_right = right.value * Factor::num / Factor::den;

//     return Quantity<Dim, R1>(left.value - converted_right);
// }

int main() {
    printf("--- testing length assign and operation\n");
    auto dist1 = 5.0_km;
    auto dist2 = 500.0_m;
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
    unit_cast<Meters, Inches>(1.0_m).PPrint();
    unit_cast<Meters, Feet>(1.0_m).PPrint();
    unit_cast<Meters, Miles>(1.0_m).PPrint();
    printf("---\n");
    
    printf("--- testing temperature assign and operators\n");
    auto temp_c = 21.5_C;
    auto temp_f = 77.0_F;
    temp_c.PPrint();
    temp_f.PPrint();
    (21.5_C + 77.0_F).PPrint();
    (77.0_F + 21.5_C).PPrint();
    printf("---\n");
    
    printf("--- testing temperature cast\n");
    unit_cast<Celsius, Fahrenheit>(temp_c).PPrint();
    unit_cast<Fahrenheit, Celsius>(temp_f).PPrint();
    printf("---\n");
    
    printf("--- testing cast fail\n");
    try {
        unit_cast<Fahrenheit, Meters>(temp_f);
    } catch (const IncompatilbeDimException& ex) {
        printf("ok\n");
    }
    try {
        temp_f + dist1;
    } catch (const IncompatilbeDimException& ex) {
        printf("ok\n");
    }
    printf("---\n");

    return 0;
}
