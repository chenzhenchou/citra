// Copyright 2018 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include <QImage>
#include <QString>
#include <QCameraInfo>
#include <QImageEncoderSettings>
#include "citra_qt/camera/qt_camera.h"
#include "common/file_util.h"
#include "common/math_util.h"
#include "core/settings.h"

namespace Camera {

QtCamera::QtCamera(int camera_id_) : camera_id(camera_id_) {
    cam = new QCamera(QCameraInfo::defaultCamera());
    cap = new QCameraImageCapture(cam);
    cam->setCaptureMode(QCamera::CaptureStillImage);
    cap->setCaptureDestination(QCameraImageCapture::CaptureToFile);
    QImageEncoderSettings encoder_settings = cap->encodingSettings();
    encoder_settings.setCodec("image/png");
    cap->setEncodingSettings(encoder_settings);
    SetResolution(Service::CAM::Resolution{400, 240, 0, 0, 0, 0});
    path = QString::fromStdString(FileUtil::GetUserPath(D_USER_IDX)) +
                                  QString("camera_image.png");
}

void QtCamera::StartCapture() {
    cam->start();
}

void QtCamera::StopCapture() {
    cam->stop();
    if (FileUtil::Exists(path.toStdString()))
        FileUtil::Delete(path.toStdString());
}

void QtCamera::SetFormat(Service::CAM::OutputFormat output_format) {
    output_rgb = output_format == Service::CAM::OutputFormat::RGB565;
}

void QtCamera::SetResolution(const Service::CAM::Resolution& resolution) {
    width = resolution.width;
    height = resolution.height;

    QImageEncoderSettings encoder_settings = cap->encodingSettings();
    encoder_settings.setResolution(width, height);
    cap->setEncodingSettings(encoder_settings);
}

void QtCamera::SetFlip(Service::CAM::Flip flip) {
    using namespace Service::CAM;
    flip_horizontal = (flip == Flip::Horizontal) || (flip == Flip::Reverse);
    flip_vertical = (flip == Flip::Vertical) || (flip == Flip::Reverse);
}

void QtCamera::SetEffect(Service::CAM::Effect effect) {
    if (effect != Service::CAM::Effect::None) {
        LOG_ERROR(Service_CAM, "Unimplemented effect %d", static_cast<int>(effect));
    }
}

std::vector<u16> QtCamera::ReceiveFrame() const {
    cam->searchAndLock();
    cap->capture(path);
    cam->unlock();
    QImage image(path);
    if (!image.isNull()) {
        std::vector<u16> buffer(width * height);
        QImage scaled =
            image.scaled(width, height, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        QImage transformed =
            scaled.copy((scaled.width() - width) / 2, (scaled.height() - height) / 2, width, height)
                .mirrored(flip_horizontal, flip_vertical);
        if (output_rgb) {
            QImage converted = transformed.convertToFormat(QImage::Format_RGB16);
            std::memcpy(buffer.data(), converted.bits(), width * height * sizeof(u16));
        } else {
            auto dest = buffer.begin();
            bool write = false;
            int py, pu, pv;
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    QRgb rgb = transformed.pixel(x, y);
                    int r = qRed(rgb), g = qGreen(rgb), b = qBlue(rgb);

                    // The following transformation is a reverse of the one in Y2R using ITU_Rec601
                    r = (r << 5) - 0x18 + 0x166F;
                    g = (g << 5) - 0x18 - 0x10EE;
                    b = (b << 5) - 0x18 + 0x1C5B;
                    int y, u, v;
                    y = 0.00933073 * r + 0.0183538 * g + 0.00356543 * b;
                    u = -0.00527299 * r - 0.0103722 * g + 0.0156451 * b;
                    v = 0.0156741 * r - 0.0131245 * g - 0.00254958 * b;

                    if (write) {
                        pu = (pu + u) / 2;
                        pv = (pv + v) / 2;
                        using MathUtil::Clamp;
                        *(dest++) = (u16)Clamp(py, 0, 0xFF) | ((u16)Clamp(pu, 0, 0xFF) << 8);
                        *(dest++) = (u16)Clamp(y, 0, 0xFF) | ((u16)Clamp(pv, 0, 0xFF) << 8);
                    } else {
                        py = y;
                        pu = u;
                        pv = v;
                    }
                   write = !write;
                }
            }
        }
        return buffer;
    } else {
        return std::vector<u16>(width * height, output_rgb ? 0 : 0x8000);
    }
}

std::unique_ptr<CameraInterface> QtCameraFactory::Create(int _camera_id) const {
    return std::make_unique<QtCamera>(_camera_id);
}

} // namespace Camera
