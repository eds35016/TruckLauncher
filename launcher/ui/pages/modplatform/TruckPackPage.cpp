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

#include "TruckPackPage.h"
#include "ui_TruckPackPage.h"

#include <QUrl>
#include <QFileInfo>

#include "Application.h"
#include "ui/dialogs/NewInstanceDialog.h"
#include "InstanceImportTask.h"

TruckPackPage::TruckPackPage(NewInstanceDialog* dialog, QWidget* parent) 
    : QWidget(parent), ui(new Ui::TruckPackPage), dialog(dialog)
{
    ui->setupUi(this);
    
    // Get the version manager from the application
    m_versionManager = APPLICATION->truckPackVersionManager();
    
    connect(ui->packComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &TruckPackPage::onPackSelectionChanged);
    connect(ui->ramComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &TruckPackPage::onRamSelectionChanged);
    
    // If version info is already loaded, populate immediately
    if (m_versionManager && m_versionManager->isLoaded()) {
        setupPackDropdown();
    } else if (m_versionManager) {
        // Otherwise wait for it to load
        connect(m_versionManager, &TruckPackVersionManager::versionInfoLoaded,
                this, &TruckPackPage::onVersionInfoLoaded);
    }
}

TruckPackPage::~TruckPackPage()
{
    delete ui;
}

QIcon TruckPackPage::icon() const
{
    return APPLICATION->getThemedIcon("truck");
}

bool TruckPackPage::shouldDisplay() const
{
    return true;
}

void TruckPackPage::retranslate()
{
    ui->retranslateUi(this);
}

void TruckPackPage::onVersionInfoLoaded()
{
    setupPackDropdown();
}

void TruckPackPage::setupPackDropdown()
{
    if (!m_versionManager || !m_versionManager->isLoaded()) {
        return;
    }

    ui->packComboBox->clear();
    m_indexToPackMap.clear();

    QList<TruckPackInfo> packs = m_versionManager->getAvailablePacks();
    
    for (int i = 0; i < packs.size(); ++i) {
        const TruckPackInfo& pack = packs[i];
        QString displayText = QString("%1 (v%2)").arg(pack.packName, pack.packVersion);
        ui->packComboBox->addItem(displayText);
        m_indexToPackMap[i] = pack;
    }

    // Enable the combo box if we have packs
    ui->packComboBox->setEnabled(!packs.isEmpty());
    
    // Select the default pack if specified, otherwise select the first pack
    if (!packs.isEmpty()) {
        int defaultIndex = 0;
        QString defaultPackName = m_versionManager->getDefaultPackName();
        
        if (!defaultPackName.isEmpty()) {
            // Find the index of the default pack
            for (int i = 0; i < packs.size(); ++i) {
                if (packs[i].packName == defaultPackName) {
                    defaultIndex = i;
                    break;
                }
            }
        }
        
        ui->packComboBox->setCurrentIndex(defaultIndex);
        onPackSelectionChanged(defaultIndex);
    }
}

void TruckPackPage::onPackSelectionChanged(int index)
{
    if (!m_indexToPackMap.contains(index)) {
        return;
    }

    const TruckPackInfo& selectedPack = m_indexToPackMap[index];
    
    // Update the description label
    QString description = tr("Selected: %1 version %2").arg(selectedPack.packName, selectedPack.packVersion);
    ui->descriptionLabel->setText(description);
    
    // Setup RAM dropdown with recommended value
    setupRamDropdown(selectedPack.recommendedRam);
}

void TruckPackPage::onRamSelectionChanged(int index)
{
    // Just store the selection, it will be used in triggerPackImport
    Q_UNUSED(index);
}

int TruckPackPage::ramStringToMB(const QString& ramStr) const
{
    // Convert strings like "8G" to megabytes (8192)
    if (ramStr.isEmpty()) {
        return 4096; // Default 4GB
    }
    
    QString str = ramStr.toUpper();
    int multiplier = 1024; // Default to GB
    
    if (str.endsWith('M')) {
        multiplier = 1;
        str.chop(1);
    } else if (str.endsWith('G')) {
        multiplier = 1024;
        str.chop(1);
    }
    
    bool ok;
    int value = str.toInt(&ok);
    if (!ok) {
        return 4096; // Default 4GB if parse fails
    }
    
    return value * multiplier;
}

QString TruckPackPage::ramMBToDisplayString(int mb, bool isRecommended) const
{
    int gb = mb / 1024;
    QString text = tr("%1 GB").arg(gb);
    if (isRecommended) {
        text += tr(" (Recommended)");
    }
    return text;
}

void TruckPackPage::setupRamDropdown(const QString& recommendedRam)
{
    ui->ramComboBox->clear();
    
    int recommendedMB = ramStringToMB(recommendedRam);
    int recommendedIndex = -1;
    
    // Populate with 2GB to 16GB options
    QList<int> ramOptions = { 2048, 4096, 6144, 8192, 10240, 12288, 14336, 16384 };
    
    for (int i = 0; i < ramOptions.size(); ++i) {
        int ramMB = ramOptions[i];
        bool isRecommended = (ramMB == recommendedMB);
        QString displayText = ramMBToDisplayString(ramMB, isRecommended);
        
        ui->ramComboBox->addItem(displayText, ramMB);
        
        if (isRecommended) {
            recommendedIndex = i;
        }
    }
    
    // Select the recommended option if found, otherwise select middle option (8GB)
    if (recommendedIndex >= 0) {
        ui->ramComboBox->setCurrentIndex(recommendedIndex);
    } else {
        // Default to 8GB (index 3)
        ui->ramComboBox->setCurrentIndex(3);
    }
}

void TruckPackPage::triggerPackImport()
{
    int currentIndex = ui->packComboBox->currentIndex();
    if (!m_indexToPackMap.contains(currentIndex)) {
        return;
    }

    const TruckPackInfo& selectedPack = m_indexToPackMap[currentIndex];
    
    QUrl packUrl(selectedPack.packUrl);
    QFileInfo fi(packUrl.fileName());
    
    // Get selected RAM
    int selectedRamMB = ui->ramComboBox->currentData().toInt();
    
    // Prepare extra info for truck pack metadata
    QMap<QString, QString> extraInfo;
    extraInfo["TruckPack"] = "true";
    extraInfo["TruckPackName"] = selectedPack.packName;
    extraInfo["TruckPackVersion"] = selectedPack.packVersion;
    extraInfo["MaxMemAlloc"] = QString::number(selectedRamMB);
    
    // Create an instance import task with the selected truck pack URL and metadata
    dialog->setSuggestedPack(selectedPack.packName, new InstanceImportTask(packUrl, this, std::move(extraInfo)));
    dialog->setSuggestedIcon("default");
}

void TruckPackPage::openedImpl()
{
    // Trigger the pack import when the page is opened
    triggerPackImport();
}
