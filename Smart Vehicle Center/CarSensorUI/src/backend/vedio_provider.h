#ifndef VEDIO_PROVIDER_H
#define VEDIO_PROVIDER_H

#include <QObject>
#include <QCamera>
#include <QAbstractVideoSurface>

class VedioProvider : public QObject {
    Q_OBJECT
    Q_PROPERTY(QAbstractVideoSurface* videoSurface READ videoSurface WRITE setVideoSurface NOTIFY surfaceChanged)

public:
    // 修改这里：删掉后面的 { ... }，只留分号
    explicit VedioProvider(QObject *parent = nullptr); 
    
    QAbstractVideoSurface* videoSurface() const { return m_surface; }
    void setVideoSurface(QAbstractVideoSurface* surface);

signals:
    void surfaceChanged();

private:
    void setupCamera();
    QCamera *m_camera;
    QAbstractVideoSurface *m_surface;
};

#endif