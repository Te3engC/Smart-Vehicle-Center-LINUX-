#include "vedio_provider.h"
#include <QCamera>

VedioProvider::VedioProvider(QObject *parent) : QObject(parent), m_camera(nullptr), m_surface(nullptr) {}

void VedioProvider::setVideoSurface(QAbstractVideoSurface* surface) {
    if (m_surface != surface) {
        m_surface = surface;
        emit surfaceChanged();
        if (m_surface) setupCamera();
    }
}

void VedioProvider::setupCamera() {
    if (m_camera) { m_camera->stop(); delete m_camera; }
    
    // 直接调用硬件节点，不做格式干预
    m_camera = new QCamera("/dev/video10", this);
    m_camera->setViewfinder(m_surface);
    m_camera->start(); 
}