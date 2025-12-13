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

#include <QDialog>
#include <QString>

class TruckPackUpdateDialog : public QDialog {
    Q_OBJECT

   public:
    explicit TruckPackUpdateDialog(const QString& packName,
                                    const QString& currentVersion,
                                    const QString& latestVersion,
                                    QWidget* parent = nullptr);

    enum Result {
        LaunchAnyway,
        UpdateNow,
        Cancel
    };

    Result getResult() const { return m_result; }

   private slots:
    void onUpdateClicked();
    void onLaunchAnywayClicked();
    void onCancelClicked();

   private:
    Result m_result = Cancel;
};
