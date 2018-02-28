// Copyright 2018 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include <QCamera>
#include <QCameraInfo>
#include <QByteArray>
#include "citra_qt/camera/real_camera.h"
#include "common/math_util.h"
#include "core/settings.h"

namespace Camera {

RealCamera::RealCamera(int camera_id_) : camera_id(camera_id_) {
    camera_ptr.reset(new QCamera(QCameraInfo::defaultCamera()));
    imagecapture_ptr.reset(new QCameraImageCapture(camera_ptr.data()));
    connect(imagecapture_ptr.data(), &QCameraImageCapture::imageAvailable, this, &RealCamera::UpdateFrame);
}

void RealCamera::StartCapture() {
    camera_ptr->start();
}

void RealCamera::StopCapture() {
    camera_ptr->stop();
}

void RealCamera::SetFormat(Service::CAM::OutputFormat output_format) {
    output_rgb = output_format == Service::CAM::OutputFormat::RGB565;
}

void RealCamera::SetResolution(const Service::CAM::Resolution& resolution) {
    width = resolution.width;
    height = resolution.height;
}

void RealCamera::SetFlip(Service::CAM::Flip flip) {
    using namespace Service::CAM;
    flip_horizontal = (flip == Flip::Horizontal) || (flip == Flip::Reverse);
    flip_vertical = (flip == Flip::Vertical) || (flip == Flip::Reverse);
}

void RealCamera::SetEffect(Service::CAM::Effect effect) {
    if (effect != Service::CAM::Effect::None) {
        LOG_ERROR(Service_CAM, "Unimplemented effect %d", static_cast<int>(effect));
    }
}

void RealCamera::UpdateFrame(int id, const QVideoFrame &image) {
    Q_UNUSED(id);
    frame = QVideoFrame(image);
}

std::vector<u16> RealCamera::ReceiveFrame() const {
    frame.map(QAbstractVideoBuffer::ReadWrite);
    QImage image;
    QImage::Format format = QVideoFrame::imageFormatFromPixelFormat(frame.pixelFormat());
    if (format != QImage::Format_Invalid)
        image = QImage(frame.bits(), frame.width(), frame.height(), format);
    else
        image = QImage::fromData(QByteArray(reinterpret_cast<char*>(frame.bits()), 10));
    frame.unmap();
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
}

std::unique_ptr<CameraInterface> RealCameraFactory::Create(int _camera_id) const {
    return std::make_unique<RealCamera>(_camera_id);
}

} // namespace Camera
