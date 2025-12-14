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

#include "TruckPackUpdateDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>

TruckPackUpdateDialog::TruckPackUpdateDialog(const QString& packName,
                                               const QString& currentVersion,
                                               const QString& latestVersion,
                                               QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Truck Pack Update Available"));
    setMinimumWidth(400);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Icon and message
    QLabel* messageLabel = new QLabel(this);
    messageLabel->setText(
        tr("<html><body>"
           "<p><b>A newer version of %1 is available!</b></p>"
           "<p>Current version: <b>%2</b></p>"
           "<p>Latest version: <b>%3</b></p>"
           "<p>Would you like to update before launching?</p>"
           "<p><b>WARNING: Updating will delete any single player world saves! Make sure to back them up before proceeding.</b></p>"
           "</body></html>")
            .arg(packName, currentVersion, latestVersion));
    messageLabel->setWordWrap(true);
    mainLayout->addWidget(messageLabel);

    mainLayout->addSpacing(20);

    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    QPushButton* updateButton = new QPushButton(tr("Update Now"), this);
    updateButton->setDefault(true);
    connect(updateButton, &QPushButton::clicked, this, &TruckPackUpdateDialog::onUpdateClicked);
    buttonLayout->addWidget(updateButton);

    QPushButton* launchButton = new QPushButton(tr("Launch Anyway"), this);
    connect(launchButton, &QPushButton::clicked, this, &TruckPackUpdateDialog::onLaunchAnywayClicked);
    buttonLayout->addWidget(launchButton);

    QPushButton* cancelButton = new QPushButton(tr("Cancel"), this);
    connect(cancelButton, &QPushButton::clicked, this, &TruckPackUpdateDialog::onCancelClicked);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}

void TruckPackUpdateDialog::onUpdateClicked()
{
    m_result = UpdateNow;
    accept();
}

void TruckPackUpdateDialog::onLaunchAnywayClicked()
{
    m_result = LaunchAnyway;
    accept();
}

void TruckPackUpdateDialog::onCancelClicked()
{
    m_result = Cancel;
    reject();
}
