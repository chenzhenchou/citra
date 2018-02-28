// Copyright 2018 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include <QCamera>
#include <QString>
#include <QObject>
#include <QCameraImageCapture>
#include "core/frontend/camera/factory.h"
#include "core/frontend/camera/interface.h"

namespace Camera {

class QtCamera final : public CameraInterface, public QObject {
public:
    QtCamera(int camera_id);
    void StartCapture() override;
    void StopCapture() override;
    void SetResolution(const Service::CAM::Resolution&) override;
    void SetFlip(Service::CAM::Flip) override;
    void SetEffect(Service::CAM::Effect) override;
    void SetFormat(Service::CAM::OutputFormat) override;
    std::vector<u16> ReceiveFrame() const override;

private:
    int camera_id;
    int width, height;
    bool output_rgb;
    bool flip_horizontal, flip_vertical;
    QString path;
    QCamera* cam;
    QCameraImageCapture* cap;
};

class QtCameraFactory final : public CameraFactory {
public:
    std::unique_ptr<CameraInterface> Create(int _camera_id) const override;
};

} // namespace Camera
