// Copyright 2018 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include <QImage>
#include <QCamera>
#include <QVideoFrame>
#include <QScopedPointer>
#include <QCameraImageCapture>
#include "core/frontend/camera/factory.h"
#include "core/frontend/camera/interface.h"

namespace Camera {

class RealCamera final : public CameraInterface, public QObject {
public:
    RealCamera(int camera_id);
    void StartCapture() override;
    void StopCapture() override;
    void SetResolution(const Service::CAM::Resolution&) override;
    void SetFlip(Service::CAM::Flip) override;
    void SetEffect(Service::CAM::Effect) override;
    void SetFormat(Service::CAM::OutputFormat) override;
    void UpdateFrame(int id, const QVideoFrame &image);
    std::vector<u16> ReceiveFrame() const override;

private:
    int camera_id;
    int width, height;
    bool output_rgb;
    bool flip_horizontal, flip_vertical;
    mutable QVideoFrame frame;
    QScopedPointer<QCamera> camera_ptr;
    QScopedPointer<QCameraImageCapture> imagecapture_ptr;
};

class RealCameraFactory final : public CameraFactory {
public:
    std::unique_ptr<CameraInterface> Create(int _camera_id) const override;
};

} // namespace Camera
