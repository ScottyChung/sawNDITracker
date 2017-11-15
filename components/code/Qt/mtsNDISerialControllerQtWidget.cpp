/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2014-07-21

  (C) Copyright 2014-2017 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

// system include
#include <iostream>

// Qt include
#include <QString>
#include <QtGui>
#include <QMessageBox>
#include <QPushButton>

// cisst
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstParameterTypes/prmForceCartesianSet.h>
#include <sawNDITracker/mtsNDISerialControllerQtWidget.h>

CMN_IMPLEMENT_SERVICES_DERIVED_ONEARG(mtsNDISerialControllerQtWidget, mtsComponent, std::string);

mtsNDISerialControllerQtWidget::mtsNDISerialControllerQtWidget(const std::string & componentName, double periodInSeconds):
    mtsComponent(componentName),
    TimerPeriodInMilliseconds(periodInSeconds)
{
    QMMessage = new mtsMessageQtWidget();

    // Setup CISST Interface
    mtsInterfaceRequired * interfaceRequired = AddInterfaceRequired("Controller");
    if (interfaceRequired) {
        QMMessage->SetInterfaceRequired(interfaceRequired);
        interfaceRequired->AddFunction("GetPeriodStatistics", Tracker.GetPeriodStatistics);
        interfaceRequired->AddFunction("Connect", Tracker.Connect);
    }
    setupUi();
    startTimer(TimerPeriodInMilliseconds); // ms
}

void mtsNDISerialControllerQtWidget::Configure(const std::string &filename)
{
    CMN_LOG_CLASS_INIT_VERBOSE << "Configure: " << filename << std::endl;
}

void mtsNDISerialControllerQtWidget::Startup(void)
{
    CMN_LOG_CLASS_INIT_VERBOSE << "mtsNDISerialControllerQtWidget::Startup" << std::endl;
    if (!parent()) {
        show();
    }
}

void mtsNDISerialControllerQtWidget::Cleanup(void)
{
    this->hide();
    CMN_LOG_CLASS_INIT_VERBOSE << "mtsNDISerialControllerQtWidget::Cleanup" << std::endl;
}

void mtsNDISerialControllerQtWidget::closeEvent(QCloseEvent * event)
{
    int answer = QMessageBox::warning(this, tr("mtsNDISerialControllerQtWidget"),
                                      tr("Do you really want to quit this application?"),
                                      QMessageBox::No | QMessageBox::Yes);
    if (answer == QMessageBox::Yes) {
        event->accept();
        QCoreApplication::exit();
    } else {
        event->ignore();
    }
}

void mtsNDISerialControllerQtWidget::timerEvent(QTimerEvent * CMN_UNUSED(event))
{
    // make sure we should update the display
    if (this->isHidden()) {
        return;
    }

    mtsExecutionResult executionResult;

    Tracker.GetPeriodStatistics(IntervalStatistics);
    QMIntervalStatistics->SetValue(IntervalStatistics);
}

void mtsNDISerialControllerQtWidget::setupUi(void)
{
    QHBoxLayout * mainLayout = new QHBoxLayout;

    // Side by side for 3D position, gripper...
    QVBoxLayout * controlLayout = new QVBoxLayout;
    mainLayout->addLayout(controlLayout);

    QPushButton * connectButton = new QPushButton("Connect");
    controlLayout->addWidget(connectButton);
    connect(connectButton, SIGNAL(clicked()),
            this, SLOT(SlotConnect()));

    controlLayout->addStretch();

    // System
    QVBoxLayout * systemLayout = new QVBoxLayout();
    mainLayout->addLayout(systemLayout);

    // Timing
    QMIntervalStatistics = new mtsIntervalStatisticsQtWidget();
    systemLayout->addWidget(QMIntervalStatistics);

    // Messages
    QMMessage->setupUi();
    systemLayout->addWidget(QMMessage);

    setLayout(mainLayout);
    setWindowTitle("sawNDITracker");
    resize(sizeHint());
}

void mtsNDISerialControllerQtWidget::SlotConnect(void)
{
    Tracker.Connect();
}