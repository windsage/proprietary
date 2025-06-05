/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#ifndef DATABASE_TYPES_H
#define DATABASE_TYPES_H

#include <utility>
#include <vector>
#include <optional>
#include <cassert>
class QtiRilPreparedStatement;

template <class T>
class QtiRilParameterValue;

template <typename T>
struct to_pointer {
        using type = T;
            static inline constexpr T *convert(T &t) { return &t; }
            static inline constexpr T *convert(T *t) { return t; }
};

template <typename T>
struct to_ref{
        using type = T;
            static inline T &convert(T *t) { return *t; }
            static inline T &convert(T &t) { return t; }
};

template <typename T>
struct QtiRilParamTypeOf {};

class QtiRilParameter;
class QtiRilValueBase {
    public:
        enum class Type {
            UNKNOWN,
            BLOB,
            BLOB64,
            DOUBLE,
            INT,
            INT64,
            NULLT,
            TEXT,
            TEXT16,
            TEXT64,
            VALUE,
            POINTER,
            ZEROBLOB,
            ZEROBLOB64,
            INVALID,
        };
        virtual ~QtiRilValueBase() {}
    private:
        Type type;
    protected:
        friend class QtiRilParameter;
        QtiRilValueBase(Type t):
            type(t) {}
        virtual void *doGetPtr() = 0;
        Type getType() const {
            return type;
        }
        virtual QtiRilValueBase *clone() = 0;
};

class QtiRilParameter {
    private:
        QtiRilValueBase::Type type;
        QtiRilValueBase *value;
    public:
        using Type = QtiRilValueBase::Type;
        template <typename T>
        using valid = std::optional<T>;

        QtiRilParameter(const QtiRilParameter &o):
            type(o.type),
            value(o.value ? o.value->clone() : nullptr) {}
        template <class T>
        QtiRilParameter(T value):
            type(QtiRilParamTypeOf<T>::type_id),
            value(new typename QtiRilParamTypeOf<T>::type(value))
        { }
        constexpr QtiRilParameter():
            type(QtiRilValueBase::Type::INVALID),
            value(nullptr) {}

        static const QtiRilParameter &getInvalidParam() {
            static const QtiRilParameter invalid{};
            return invalid;
        }
        ~QtiRilParameter() {
            delete value;
            value = nullptr;
            type = QtiRilValueBase::Type::NULLT;
        }
        QtiRilParameter &operator= (QtiRilParameter &&o) {
            type = std::move(o.type);
            value = std::move(o.value);
            return *this;
        }

        QtiRilParameter(QtiRilValueBase *value):
            type(value ? value->getType() : Type::NULLT),
            value(value) {};

        template <typename T>
        QtiRilParameter(const QtiRilParameter &o):
            type(o.value ? o.value->getType() : Type::NULLT),
            value(o.value ? o.value->clone() : nullptr)
        { }

        QtiRilParameter(QtiRilParameter &&o):
            type(std::move(o.type)),
            value(std::move(o.value)) { }


    protected:
        friend class QtiRilValueBase;
        QtiRilParameter(Type t):
            type(t),
            value(nullptr)
        {}

        void *doGetPtr() {
            void *ret = nullptr;
            if (value) {
                ret = value->doGetPtr();
            }
            return ret;
        }

        const void *doGetPtr() const {
            const void *ret = nullptr;
            if (value) {
                ret = value->doGetPtr();
            }
            return ret;
        }

    public:
        bool isBlob() { return type == Type::BLOB; };
        bool isBlob64() { return type == Type::BLOB64; };
        bool isDouble() { return type == Type::DOUBLE; };
        bool isInt() { return type == Type::INT; };
        bool isInt64() { return type == Type::INT64; };
        bool isNull() { return type == Type::NULLT; };
        bool isText() { return type == Type::TEXT; };
        bool isText16() { return type == Type::TEXT16; };
        bool isText64() { return type == Type::TEXT64; };
        bool isValue() { return type == Type::VALUE; };
        bool isPointer() { return type == Type::POINTER; };
        bool isZeroBlob() { return type == Type::ZEROBLOB; };
        bool isZeroBlob64() { return type == Type::ZEROBLOB64; };
        template <typename T>
        bool isOfType() {
            return type == QtiRilParamTypeOf<T>::type_id;
        }
        template <typename T>
        T &get() {
            assert(type == QtiRilParamTypeOf<T>::type_id);
            return to_ref<T>::convert(
                    static_cast<typename to_ref<T>::type *>(doGetPtr()));
        }
        template <typename T>
        const T &get() const {
            return to_ref<const T>::convert(
                    reinterpret_cast<typename to_ref<const T>::type *>(doGetPtr()));
        }
        Type getType() const {
            return type;
        }
};

template <>
struct QtiRilParamTypeOf<double> {
    static const QtiRilParameter::Type type_id = QtiRilParameter::Type::DOUBLE;
    using type = QtiRilParameterValue<double>;
};

template <>
struct QtiRilParamTypeOf<int32_t> {
    static const QtiRilParameter::Type type_id = QtiRilParameter::Type::INT;
    using type = QtiRilParameterValue<int32_t>;
};

template <>
struct QtiRilParamTypeOf<int64_t> {
    static const QtiRilParameter::Type type_id = QtiRilParameter::Type::INT64;
    using type = QtiRilParameterValue<int64_t>;
};

template <>
struct QtiRilParamTypeOf<std::nullptr_t> {
    static const QtiRilParameter::Type type_id = QtiRilParameter::Type::NULLT;
    using type = QtiRilParameterValue<std::nullptr_t>;
};

template <>
struct QtiRilParamTypeOf<std::string> {
    static const QtiRilParameter::Type type_id = QtiRilParameter::Type::TEXT;
    using type = QtiRilParameterValue<std::string>;
};

template <>
struct QtiRilParamTypeOf<std::basic_string<uint8_t>> {
    static const QtiRilParameter::Type type_id = QtiRilParameter::Type::BLOB;
    using type = QtiRilParameterValue<std::basic_string<uint8_t>>;
};

template <>
struct QtiRilParamTypeOf<const char *> {
    static const QtiRilParameter::Type type_id = QtiRilParameter::Type::TEXT;
    using type = QtiRilParameterValue<std::string>;
};

template <QtiRilValueBase::Type VT>
struct MapType{};

template <>
struct MapType<QtiRilValueBase::Type::BLOB> {
    using type = std::basic_string<uint8_t>;
    using paramType = QtiRilParamTypeOf<type>;
};
template <>
struct MapType<QtiRilValueBase::Type::DOUBLE> {
    using type = double;
    using paramType = QtiRilParamTypeOf<type>;
};
template <>
struct MapType<QtiRilValueBase::Type::INT> {
    using type = int;
    using paramType = QtiRilParamTypeOf<int>;
};
template <>
struct MapType<QtiRilValueBase::Type::INT64> {
    using type = int64_t;
    using paramType = QtiRilParameterValue<type>;
};
template <>
struct MapType<QtiRilValueBase::Type::NULLT> {
    using type = std::nullptr_t;
    using paramType = QtiRilParameterValue<type>;
};
template <>
struct MapType<QtiRilValueBase::Type::TEXT> {
    using type = std::string;
    using paramType = QtiRilParameterValue<type>;
};
template <>
struct MapType<QtiRilValueBase::Type::TEXT16> {
    using type = std::basic_string<uint8_t>;
    using paramType = QtiRilParameterValue<type>;
};


template <typename T>
class QtiRilParameterValue: public QtiRilValueBase {
    private:
        T value;
    protected:
        virtual void *doGetPtr() {
            return const_cast<void *>(static_cast<const void *>(to_pointer<T>::convert(value)));
        }
    public:
        QtiRilParameterValue(const QtiRilParameterValue &o):
                QtiRilValueBase(o.QtiRilValueBase::getType()),
                value(o.value) {}
        QtiRilParameterValue(QtiRilParameterValue<T> &&o):
                QtiRilValueBase(std::move(o.QtiRilValueBase::getType())),
                value(std::move(o.value)) {}
        QtiRilParameterValue(T value):
                QtiRilValueBase(QtiRilParamTypeOf<T>::type_id),
                value(value) {}
        virtual QtiRilValueBase *clone() {
            return new QtiRilParameterValue(*this);
        }
};

class QtiRilParameterList {
    public:
        using container = std::vector<QtiRilParameter>;
    private:
        std::vector<QtiRilParameter> params;
    public:
        using size_type = typename container::size_type;
        using reference = container::reference;
        using const_reference = container::const_reference;
        using iterator = container::iterator;
        using const_iterator = container::const_iterator;
        template <typename V>
        void addValue(V val) {
            params.emplace_back(val);
        }
        QtiRilParameter &get(size_t idx) {
            return params[idx];
        }
        size_type size() const noexcept { return params.size(); }
        iterator begin() noexcept { return params.begin(); }
        const_iterator begin() const noexcept { return params.begin(); }
        const_iterator cbegin() const noexcept { return params.cbegin(); }
        iterator end() noexcept { return params.end(); }
        const_iterator end() const noexcept { return params.end(); }
        const_iterator cend() const noexcept { return params.cend(); }
        ~QtiRilParameterList() {
        }
};

class QtiRilDbRow {
    public:
        enum class Result {
            OK,
            DONE,
            BUSY,
            ERROR,
            UNKNOWN,
        };
        virtual const std::string &errorMessage() const = 0;
        virtual Result getResult() const = 0;
        virtual QtiRilParameter get(size_t column) const = 0;
        QtiRilParameter operator[] (size_t column) const {
            return get(column);
        }
        virtual size_t getColumnCount() const = 0;
        virtual ~QtiRilDbRow() {}
    private:
};

#endif
