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

#pragma once

#include <QObject>
#include <QString>
#include <QMap>
#include <QList>
#include <QJsonObject>
#include <memory>

#include "net/NetJob.h"

struct TruckPackInfo {
    QString packName;
    QString packVersion;
    QString packUrl;
    QString recommendedRam;  // e.g., "8G"
};

class TruckPackVersionManager : public QObject {
    Q_OBJECT

   public:
    explicit TruckPackVersionManager(QObject* parent = nullptr);
    ~TruckPackVersionManager() = default;

    // Fetch the latest version information from the server
    void fetchVersionInfo();

    // Check if an instance has an outdated truck pack
    bool isPackOutdated(const QString& instanceName, const QString& currentVersion) const;

    // Get the latest version for a specific pack
    QString getLatestVersion(const QString& packName) const;

    // Get all available truck packs
    QList<TruckPackInfo> getAvailablePacks() const;

    // Get specific pack info by name
    TruckPackInfo getPackInfo(const QString& packName) const;

    // Get the default pack name
    QString getDefaultPackName() const { return m_defaultPackName; }

    // Check if version info has been loaded
    bool isLoaded() const { return m_loaded; }

   signals:
    void versionInfoLoaded();
    void versionInfoFailed(const QString& error);

   private slots:
    void onDownloadSucceeded();
    void onDownloadFailed(QString reason);

   private:
    void parseVersionJson(const QByteArray& data);

    QMap<QString, TruckPackInfo> m_packVersions;
    QString m_defaultPackName;
    NetJob::Ptr m_downloadJob;
    std::shared_ptr<QByteArray> m_downloadedData;
    bool m_loaded = false;

    static constexpr const char* VERSION_URL = "https://philoop.net/mc/pack_version.txt";
};
