#pragma once

#include <type_traits>

template<class En>
requires std::is_enum_v<En>
constexpr uint8_t enum_size();

template<class En>
requires std::is_enum_v<En>
constexpr inline auto toEn(int i) {
    assert(i < enum_size<En>());
    return static_cast<En>(i);
}

template<class En>
requires std::is_enum_v<En>
constexpr inline auto toNum(En en) {
    return static_cast<int>(en);
}

/// @brief simple define for enum class creation with compiler-time evaluated size
#define ENUM_DEFINE(EnumName, ...)\
    \
    enum class EnumName : int { __VA_ARGS__ };\
    \
    template<>\
    [[maybe_unused]] constexpr uint8_t enum_size<EnumName>() {\
        enum EnumName { __VA_ARGS__ }; \
        EnumName enumArray[]{ __VA_ARGS__ }; \
        return sizeof(enumArray) / sizeof(enumArray[0]); \
    }\
    \

#define ENUM_DEFINE_NS(NS, EnumName, ...)\
    namespace NS {                       \
        enum class EnumName : int { __VA_ARGS__ };       \
    }                                    \
        template<>\
        [[maybe_unused]] constexpr uint8_t enum_size<NS::EnumName>() {\
            enum EnumName { __VA_ARGS__ }; \
            EnumName enumArray[]{ __VA_ARGS__ }; \
            return sizeof(enumArray) / sizeof(enumArray[0]); \
        }\


