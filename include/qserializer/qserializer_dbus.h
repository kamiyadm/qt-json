#pragma once

#include <QDBusMetaType>

#include "qserializer.h"

namespace qserializer
{

template <typename T>
QDBusArgument &move_in(QDBusArgument &args, const T &x)
{
        QVariant v = QVariant::fromValue<T>(x);
        if (!v.template canConvert<QVariantMap>()) {
                Q_ASSERT(false);
                qCritical() << "Failed to write QDBusArgument as a map";
        }
        args << v.toMap();
        return args;
}

template <typename T>
const QDBusArgument &move_out(const QDBusArgument &args, T &x)
{
        QVariantMap map;
        args >> map;
        QVariant v = map;
        if (!v.canConvert<T>()) {
                Q_ASSERT(false);
                qCritical() << "Failed to write QDBusArgument as a map";
        }
        x = v.value<T>();
        return args;
}

}
#define QSERIALIZER_DECLARE_DBUS(T)                                       \
        [[maybe_unused]] inline const QDBusArgument &operator<<(          \
                QDBusArgument &args, const QSharedPointer<T> &x)          \
        {                                                                 \
                return qserializer::move_in<QSharedPointer<T>>(args, x);  \
        }                                                                 \
        [[maybe_unused]] inline const QDBusArgument &operator>>(          \
                const QDBusArgument &args, QSharedPointer<T> &x)          \
        {                                                                 \
                return qserializer::move_out<QSharedPointer<T>>(args, x); \
        }                                                                 \
        QSERIALIZER_DECLARE(T)

#define QSERIALIZER_IMPL_DBUS(T, ...)                                      \
        QSERIALIZER_IMPL(T, {                                              \
                qDBusRegisterMetaType<QSharedPointer<T>>();                \
                qDBusRegisterMetaType<QList<QSharedPointer<T>>>();         \
                qDBusRegisterMetaType<QMap<QString, QSharedPointer<T>>>(); \
        } __VA_OPT__(, ) __VA_ARGS__)
