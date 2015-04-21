/*
 * Copyright (C) 2015 Canonical, Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *     Pete Woods <pete.woods@canonical.com>
 */

#pragma once

#include <QDBusConnection>
#include <QObject>
#include <QString>
#include <memory>

namespace connectivityqt
{
namespace internal
{

class DBusPropertyCache: public QObject
{
    Q_OBJECT

public:
    DBusPropertyCache(const QString &service, const QString& interface,
                      const QString &path, const QDBusConnection &connection);

    ~DBusPropertyCache();

    QVariant get(const QString& name);


Q_SIGNALS:
    void propertyChanged(const QString& name, const QVariant& value);

protected:
    class Priv;
    std::shared_ptr<Priv> d;
};

}
}
