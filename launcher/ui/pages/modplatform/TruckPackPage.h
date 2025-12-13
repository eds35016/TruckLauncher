// SPDX-License-Identifier: GPL-3.0-only
/*
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (c) 2022 Jamie Mansfield <jmansfield@cadixdev.org>
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

#include <QWidget>
#include <QMap>

#include "tasks/Task.h"
#include "ui/pages/BasePage.h"
#include "TruckPackVersionManager.h"

namespace Ui {
class TruckPackPage;
}

class NewInstanceDialog;

class TruckPackPage : public QWidget, public BasePage {
    Q_OBJECT

   public:
    explicit TruckPackPage(NewInstanceDialog* dialog, QWidget* parent = 0);
    virtual ~TruckPackPage();
    virtual QString displayName() const override { return tr("Add Truck Pack"); }
    virtual QIcon icon() const override;
    virtual QString id() const override { return "truckpack"; }
    virtual QString helpPage() const override { return "Truck-pack"; }
    virtual bool shouldDisplay() const override;
    void retranslate() override;

    void openedImpl() override;

   private slots:
    void onVersionInfoLoaded();
    void onPackSelectionChanged(int index);
    void onRamSelectionChanged(int index);

   private:
    void setupPackDropdown();
    void setupRamDropdown(const QString& recommendedRam);
    int ramStringToMB(const QString& ramStr) const;
    QString ramMBToDisplayString(int mb, bool isRecommended) const;
    void triggerPackImport();

    Ui::TruckPackPage* ui = nullptr;
    NewInstanceDialog* dialog = nullptr;
    TruckPackVersionManager* m_versionManager = nullptr;
    
    QMap<int, TruckPackInfo> m_indexToPackMap;
};
