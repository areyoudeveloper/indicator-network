/*
 * Copyright © 2015 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 3,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *     Pete Woods <pete.woods@canonical.com>
 */

#include <connectivityqt/internal/dbus-property-cache.h>
#include <connectivityqt/vpn-connection.h>
#include <dbus-types.h>

#include <VpnConnectionInterface.h>

using namespace std;

namespace connectivityqt
{

class VpnConnection::Priv: public QObject
{
    Q_OBJECT

public:
    Priv(VpnConnection& parent) :
        p(parent)
    {
    }

public Q_SLOTS:
    void propertyChanged(const QString& name, const QVariant& value)
    {
        if (name == "id")
        {
            Q_EMIT p.idChanged(value.toString());
        }
        else if (name == "active")
        {
            Q_EMIT p.activeChanged(value.toBool());
        }
    }

public:
    VpnConnection& p;

    unique_ptr<ComUbuntuConnectivity1VpnVpnConnectionInterface> m_vpnInterface;

    internal::DBusPropertyCache::UPtr m_propertyCache;
};

VpnConnection::VpnConnection(const QDBusObjectPath& path, const QDBusConnection& connection) :
        d(new Priv(*this))
{
    d->m_vpnInterface = make_unique<
            ComUbuntuConnectivity1VpnVpnConnectionInterface>(
            DBusTypes::DBUS_NAME, path.path(), connection);

    d->m_propertyCache =
            make_unique<internal::DBusPropertyCache>(
                    DBusTypes::DBUS_NAME,
                    ComUbuntuConnectivity1VpnVpnConnectionInterface::staticInterfaceName(),
                    path.path(), connection);

    connect(d->m_propertyCache.get(),
                &internal::DBusPropertyCache::propertyChanged, d.get(),
                &Priv::propertyChanged);
}

VpnConnection::~VpnConnection()
{
}

QDBusObjectPath VpnConnection::path() const
{
    return QDBusObjectPath(d->m_vpnInterface->path());
}

QString VpnConnection::id() const
{
    return d->m_propertyCache->get("id").toString();
}

bool VpnConnection::active() const
{
    return d->m_propertyCache->get("active").toBool();
}

void VpnConnection::setId(const QString& id)
{
    d->m_propertyCache->set("id", id);
}

void VpnConnection::setActive(bool active)
{
    d->m_propertyCache->set("active", active);
}

void VpnConnection::updateSecrets()
{
    d->m_vpnInterface->UpdateSecrets();
}

}

#include "vpn-connection.moc"
