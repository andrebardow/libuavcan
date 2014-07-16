/*
 * Copyright (C) 2014 Pavel Kirienko <pavel.kirienko@gmail.com>
 */

#pragma once

#include <uavcan/build_config.hpp>
#include <uavcan/marshal/array.hpp>

#if !defined(UAVCAN_CPP_VERSION) || !defined(UAVCAN_CPP11)
# error UAVCAN_CPP_VERSION
#endif

#if UAVCAN_CPP_VERSION >= UAVCAN_CPP11
# include <type_traits>
#endif

namespace uavcan
{

#if UAVCAN_CPP_VERSION >= UAVCAN_CPP11

template <typename ArrayType_>
class UAVCAN_EXPORT CharArrayFormatter
{
    ArrayType_& array_;

    template <typename T>
    typename std::enable_if<std::is_floating_point<T>::value>::type
    writeValue(T value)
    {
        array_.template appendFormatted<double>("%f", double(value));
    }

    template <typename T>
    typename std::enable_if<std::is_integral<T>::value>::type
    writeValue(T value)
    {
        if (std::is_same<T, char>())
        {
            if (array_.size() != array_.capacity())
            {
                array_.push_back(value);
            }
        }
        else if (std::is_signed<T>())
        {
            array_.template appendFormatted<long long>("%lli", static_cast<long long>(value));
        }
        else
        {
            array_.template appendFormatted<unsigned long long>("%llu", static_cast<unsigned long long>(value));
        }
    }

    template <typename T>
    typename std::enable_if<std::is_pointer<T>::value && !std::is_same<T, const char*>::value>::type
    writeValue(T value)
    {
        array_.template appendFormatted<const void*>("%p", static_cast<const void*>(value));
    }

    void writeValue(const char* value)
    {
        array_.template appendFormatted<const char*>("%s", value);
    }

public:
    typedef ArrayType_ ArrayType;

    explicit CharArrayFormatter(ArrayType& array)
        : array_(array)
    { }

    ArrayType& getArray() { return array_; }
    const ArrayType& getArray() const { return array_; }

    void write(const char* text)
    {
        writeValue(text);
    }

    template <typename T, typename... Args>
    void write(const char* s, T value, Args... args)
    {
        while (s && *s)
        {
            if (*s == '%')
            {
                s += 1;
                if (*s != '%')
                {
                    writeValue(value);
                    write(++s, args...);
                    break;
                }
            }
            writeValue(*s++);
        }
    }
};

#else

template <typename ArrayType_>
class UAVCAN_EXPORT CharArrayFormatter
{
    ArrayType_& array_;

public:
    typedef ArrayType_ ArrayType;

    CharArrayFormatter(ArrayType& array)
        : array_(array)
    { }

    ArrayType& getArray() { return array_; }
    const ArrayType& getArray() const { return array_; }

    void write(const char* const text)
    {
        array_.template appendFormatted<const char*>("%s", text);
    }

    /**
     * This version does not support more than one formatted argument, though it can be improved.
     * And it is unsafe.
     * There is typesafe version for C++11 above.
     * TODO: make this version typesafe and add support for multiple args.
     */
    template <typename A>
    void write(const char* const format, const A value)
    {
        array_.template appendFormatted<A>(format, value);
    }
};

#endif

}
