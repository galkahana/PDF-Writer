#pragma once

#include "IOBasicTypes.h"
#include <string>
#include <vector>

namespace ByteListImpl {
    // using basic_string as ByteList, with fallback on vector when basic_string is NOT available for Byte (unsigned char). (see this issue for why: https://github.com/galkahana/PDF-Writer/issues/324)
    // basic_string is preferred as it performs better. (even if the custom implementation for things like append uses basic_string instead of vector it performs better...so it's not about
    // having custom implementation)
    // SFINAE detection is used for determining the availability of std::char_traits<Byte>.
    
    template <typename T>
    struct HasCharTraits {
        // This will only compile if std::char_traits<T> is valid
        template <typename U>
        static char test(typename std::char_traits<U>::char_type*);

        // fallback overload
        template <typename U>
        static long test(...);
        
        enum { value = sizeof(test<T>(0)) == sizeof(char) };
    };
    
    // General purpose Selector template (maybe extract if still here later)
    template <bool Condition, typename TrueType, typename FalseType>
    struct Select {
        typedef TrueType type;
    };
    
    template <typename TrueType, typename FalseType>
    struct Select<false, TrueType, FalseType> {
        typedef FalseType type;
    };
}

// and here we get to the purpose of this - Select basic_string if char_traits<Byte> exists, otherwise vector
typedef ByteListImpl::Select<
    ByteListImpl::HasCharTraits<IOBasicTypes::Byte>::value,
    std::basic_string<IOBasicTypes::Byte>,
    std::vector<IOBasicTypes::Byte>
>::type ByteList;

