#ifndef MPVOBJECT_H
#define MPVOBJECT_H

#include <QtQuick/QQuickFramebufferObject>

#include <mpv/client.h>
#include <mpv/opengl_cb.h>
#include <mpv/qthelper.hpp>
#include <QEvent>
#include <QtGui/QOpenGLFramebufferObject>
#include <QtQuick/QQuickWindow>
#include <QtQuick/QQuickView>

class MpvObject : public QQuickFramebufferObject
{
    Q_OBJECT

    mpv::qt::Handle mpv;
    mpv_opengl_cb_context *mpv_gl;

    friend class MpvRenderer;

public:
    MpvObject(QQuickItem * parent = 0);
    virtual ~MpvObject();
    virtual Renderer *createRenderer() const;

public slots:
    void command(const QVariant& params);
    void setProperty(const QString& name, const QVariant& value);

signals:
    void onUpdate();
    void playingPaused();
    void playingStopped();
    void playingResumed();
    void bufferingStarted();
    //void bufferingEnded();

private slots:
    void doUpdate();

private:
    static void on_update(void *ctx);
    bool event(QEvent *event);
};


#endif // MPVOBJECT_H
