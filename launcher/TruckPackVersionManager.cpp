// SPDX-License-Identifier: GPL-3.0-only
/*
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (c) 2024
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "TruckPackVersionManager.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>

#include "Application.h"
#include "net/ApiDownload.h"

TruckPackVersionManager::TruckPackVersionManager(QObject* parent) : QObject(parent)
{
    m_downloadedData = std::make_shared<QByteArray>();
}

void TruckPackVersionManager::fetchVersionInfo()
{
    qDebug() << "TruckPackVersionManager: Fetching truck pack version info from" << VERSION_URL;

    m_downloadJob.reset(new NetJob("TruckPackVersionFetch", APPLICATION->network()));
    auto download = Net::ApiDownload::makeByteArray(QUrl(VERSION_URL), m_downloadedData);
    m_downloadJob->addNetAction(download);

    connect(m_downloadJob.get(), &NetJob::succeeded, this, &TruckPackVersionManager::onDownloadSucceeded);
    connect(m_downloadJob.get(), &NetJob::failed, this, &TruckPackVersionManager::onDownloadFailed);

    m_downloadJob->start();
}

void TruckPackVersionManager::onDownloadSucceeded()
{
    qDebug() << "TruckPackVersionManager: Download succeeded";
    parseVersionJson(*m_downloadedData);
    m_loaded = true;
    emit versionInfoLoaded();
}

void TruckPackVersionManager::onDownloadFailed(QString reason)
{
    qWarning() << "TruckPackVersionManager: Failed to download version info:" << reason;
    m_loaded = false;
    emit versionInfoFailed(reason);
}

void TruckPackVersionManager::parseVersionJson(const QByteArray& data)
{
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "TruckPackVersionManager: JSON parse error:" << parseError.errorString();
        return;
    }

    if (!doc.isObject()) {
        qWarning() << "TruckPackVersionManager: JSON root is not an object";
        return;
    }

    QJsonObject root = doc.object();
    QJsonArray packsArray = root["packs"].toArray();

    // Read default pack name
    m_defaultPackName = root["default_pack"].toString();
    if (!m_defaultPackName.isEmpty()) {
        qDebug() << "TruckPackVersionManager: Default pack:" << m_defaultPackName;
    }

    m_packVersions.clear();

    for (const QJsonValue& packValue : packsArray) {
        QJsonObject packObj = packValue.toObject();

        TruckPackInfo info;
        info.packName = packObj["pack_name"].toString();
        info.packVersion = packObj["pack_version"].toString();
        info.packUrl = packObj["pack_url"].toString();
        info.recommendedRam = packObj["recommended_ram"].toString();

        if (!info.packName.isEmpty() && !info.packVersion.isEmpty() && !info.packUrl.isEmpty()) {
            m_packVersions[info.packName] = info;
            qDebug() << "TruckPackVersionManager: Loaded pack:" << info.packName << "version:" << info.packVersion << "recommended RAM:" << info.recommendedRam;
        } else {
            qWarning() << "TruckPackVersionManager: Skipping incomplete pack entry";
        }
    }

    qDebug() << "TruckPackVersionManager: Loaded" << m_packVersions.size() << "truck packs";
}

bool TruckPackVersionManager::isPackOutdated(const QString& instanceName, const QString& currentVersion) const
{
    // Try to find a matching pack by checking if the instance name contains the pack name
    for (auto it = m_packVersions.constBegin(); it != m_packVersions.constEnd(); ++it) {
        const TruckPackInfo& pack = it.value();
        
        // Check if the instance name contains this pack's name
        if (instanceName.contains(pack.packName, Qt::CaseInsensitive)) {
            // Compare versions
            return currentVersion != pack.packVersion;
        }
    }

    return false;
}

QString TruckPackVersionManager::getLatestVersion(const QString& packName) const
{
    auto it = m_packVersions.find(packName);
    if (it != m_packVersions.end()) {
        return it.value().packVersion;
    }
    return QString();
}

QList<TruckPackInfo> TruckPackVersionManager::getAvailablePacks() const
{
    return m_packVersions.values();
}

TruckPackInfo TruckPackVersionManager::getPackInfo(const QString& packName) const
{
    return m_packVersions.value(packName, TruckPackInfo());
}
