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

#include <connectivityqt/connectivity.h>
#include <connectivityqt/internal/dbus-property-cache.h>
#include <dbus-types.h>
#include <NetworkingStatusInterface.h>
#include <NetworkingStatusPrivateInterface.h>

#include <QDebug>

using namespace std;

namespace connectivityqt
{

class Connectivity::Priv: public QObject
{
    Q_OBJECT
public:
    Priv(Connectivity& parent, const QDBusConnection& sessionConnection) :
            p(parent), m_sessionConnection(sessionConnection)
    {
    }

    Connectivity& p;

    QDBusConnection m_sessionConnection;

    shared_ptr<QDBusServiceWatcher> m_serviceWatcher;

    shared_ptr<internal::DBusPropertyCache> m_propertyCache;

    shared_ptr<ComUbuntuConnectivity1NetworkingStatusInterface> m_readInterface;

    shared_ptr<ComUbuntuConnectivity1PrivateInterface> m_writeInterface;

public Q_SLOTS:
    void serviceRegistered()
    {
        m_readInterface = make_shared<
                ComUbuntuConnectivity1NetworkingStatusInterface>(
                DBusTypes::DBUS_NAME, DBusTypes::SERVICE_PATH,
                m_sessionConnection);

        m_writeInterface = make_shared<ComUbuntuConnectivity1PrivateInterface>(
                DBusTypes::DBUS_NAME, DBusTypes::PRIVATE_PATH,
                m_sessionConnection);
    }

    void serviceUnregistered()
    {
        m_readInterface.reset();
        m_writeInterface.reset();
    }

    void propertyChanged(const QString& name, const QVariant& value)
    {
        if (name == "FlightMode")
        {
            Q_EMIT p.flightModeUpdated(value.toBool());
        }
        else if (name == "FlightModeIsChanging")
        {
            Q_EMIT p.flightModeIsChangingUpdated(value.toBool());
        }
        else if (name == "WifiEnabled")
        {
            Q_EMIT p.wifiEnabledUpdated(value.toBool());
        }
        else if (name == "WifiEnabledIsChanging")
        {
            Q_EMIT p.wifiEnabledIsChangingUpdated(value.toBool());
        }
    }
};

Connectivity::Connectivity(const QDBusConnection& sessionConnection) :
        d(new Priv(*this, sessionConnection))
{
    d->m_serviceWatcher = make_shared<QDBusServiceWatcher>(DBusTypes::DBUS_NAME,
                                                           sessionConnection);
    connect(d->m_serviceWatcher.get(), &QDBusServiceWatcher::serviceRegistered,
            d.get(), &Priv::serviceRegistered);
    connect(d->m_serviceWatcher.get(),
            &QDBusServiceWatcher::serviceUnregistered, d.get(),
            &Priv::serviceUnregistered);

    d->m_propertyCache = make_shared<internal::DBusPropertyCache>(
            DBusTypes::DBUS_NAME, DBusTypes::SERVICE_INTERFACE,
            DBusTypes::SERVICE_PATH, sessionConnection);
    connect(d->m_propertyCache.get(),
            &internal::DBusPropertyCache::propertyChanged, d.get(),
            &Priv::propertyChanged);
}

Connectivity::~Connectivity()
{
}

bool Connectivity::flightMode() const
{
    return d->m_propertyCache->get("FlightMode").toBool();
}

bool Connectivity::flightModeIsChanging() const
{
    return d->m_propertyCache->get("FlightModeIsChanging").toBool();
}

bool Connectivity::wifiEnabled() const
{
    return d->m_propertyCache->get("WifiEnabled").toBool();
}

bool Connectivity::wifiEnabledIsChanging() const
{
    return d->m_propertyCache->get("WifiEnabledIsChanging").toBool();
}

}

#include "connectivity.moc"
