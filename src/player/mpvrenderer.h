#ifndef MPVRENDERER_H_
#define MPVRENDERER_H_


#include "mpvobject.h"
#include <stdexcept>
#include <clocale>

#include <QObject>
#include <QtGlobal>
#include <QOpenGLFunctions>
#include <QGuiApplication>
#include <QDebug>

#ifdef Q_OS_WIN
    #include <Windows.h>
#endif

#include <QtGui/QOpenGLFramebufferObject>

class MpvRenderer : public QQuickFramebufferObject::Renderer, protected QOpenGLFunctions
{
    static void *get_proc_address(void *ctx, const char *name);

    mpv::qt::Handle mpv;
    QQuickWindow *window;
    mpv_opengl_cb_context *mpv_gl;

public:
    MpvRenderer(const MpvObject *obj);

    virtual ~MpvRenderer();

    void render();
};


#endif
