// Copyright 2016 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include <QtGui>
#include "citra_qt/configuration/configure_camera.h"
#include "core/settings.h"
#include "ui_configure_camera.h"

ConfigureCamera::ConfigureCamera(QWidget* parent)
    : QWidget(parent), ui(std::make_unique<Ui::ConfigureCamera>()) {
    ui->setupUi(this);

    ui->camera_outer_right_mode->clear();
    ui->camera_outer_right_mode->addItem("blank");
    ui->camera_outer_right_mode->addItem("image");
    ui->camera_outer_right_mode->addItem("qt");

    ui->camera_outer_left_mode->clear();
    ui->camera_outer_left_mode->addItem("blank");
    ui->camera_outer_left_mode->addItem("image");
    ui->camera_outer_left_mode->addItem("qt");

    ui->camera_inner_mode->clear();
    ui->camera_inner_mode->addItem("blank");
    ui->camera_inner_mode->addItem("image");
    ui->camera_inner_mode->addItem("qt");

    connect(ui->camera_outer_right_mode, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ConfigureCamera::OuterRightCameraModeChanged);
    connect(ui->camera_outer_left_mode, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ConfigureCamera::OuterLeftCameraModeChanged);
    connect(ui->camera_inner_mode, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ConfigureCamera::InnerCameraModeChanged);
 
    this->setConfiguration();
}

ConfigureCamera::~ConfigureCamera() {}

void ConfigureCamera::OuterRightCameraModeChanged(int index) {
     if (index) {
         ui->camera_outer_right_drop_area->show();
     } else {
         ui->camera_outer_right_drop_area->hide();
     }
 }
 
 void ConfigureCamera::OuterLeftCameraModeChanged(int index) {
     if (index) {
         ui->camera_outer_left_drop_area->show();
     } else {
         ui->camera_outer_left_drop_area->hide();
     }
 }
 
 void ConfigureCamera::InnerCameraModeChanged(int index) {
     if (index) {
         ui->camera_inner_drop_area->show();
     } else {
         ui->camera_inner_drop_area->hide();
     }
 }
 
void ConfigureCamera::setConfiguration() {
    using namespace Service::CAM;

    // outer right camera
    switch (ui->camera_outer_right_mode->currentIndex()) {
    case 0: { // blank
        ui->camera_outer_right_drop_area->hide();
        break;
    }
    case 1: { // image
        ui->camera_outer_right_drop_area->show();
        ui->camera_outer_right_drop_area->SetImage(QString::fromStdString(Settings::values.camera_config[OuterRightCamera]));
        break;
    }
    case 2: { // real
        ui->camera_outer_right_drop_area->hide();
        break;
    }
    }

    // outer left camera
    switch (ui->camera_outer_left_mode->currentIndex()) {
    case 0: { // blank
        ui->camera_outer_left_drop_area->hide();
        break;
    }
    case 1: { // image
        ui->camera_outer_left_drop_area->show();
        ui->camera_outer_left_drop_area->SetImage(QString::fromStdString(Settings::values.camera_config[OuterLeftCamera]));
        break;
    }
    case 2: { // real
        ui->camera_outer_left_drop_area->hide();
        break;
    }
    }

    // inner camera
    switch (ui->camera_outer_left_mode->currentIndex()) {
    case 0: { // blank
        ui->camera_inner_drop_area->hide();
        break;
    }
    case 1: { // image
        ui->camera_inner_drop_area->show();
        ui->camera_inner_drop_area->SetImage(QString::fromStdString(Settings::values.camera_config[InnerCamera]));
        break;
    }
    case 2: { // real
        ui->camera_inner_drop_area->hide();
        break;
    }
    }
}

void ConfigureCamera::applyConfiguration() {
    using namespace Service::CAM;

    // outer right camera
    Settings::values.camera_name[OuterRightCamera] =
            ui->camera_outer_right_mode->currentText().toStdString();
    Settings::values.camera_config[OuterRightCamera] =
            ui->camera_outer_right_drop_area->GetImageFilepath().toStdString();
	 
    // outer left camera
    Settings::values.camera_name[OuterLeftCamera] =
            ui->camera_outer_left_mode->currentText().toStdString();
    Settings::values.camera_config[OuterLeftCamera] =
            ui->camera_outer_left_drop_area->GetImageFilepath().toStdString();
		
    // inner camera
    Settings::values.camera_name[InnerCamera] =
        ui->camera_inner_mode->currentText().toStdString();
    Settings::values.camera_config[InnerCamera] =
        ui->camera_inner_drop_area->GetImageFilepath().toStdString();
 
    Settings::Apply();
}

void ConfigureCamera::retranslateUi() {
    ui->retranslateUi(this);
}
