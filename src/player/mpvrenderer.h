#ifndef MPVRENDERER_H_
#define MPVRENDERER_H_


#include "mpvobject.h"
#include <stdexcept>
#include <clocale>

#include <QObject>
#include <QtGlobal>
#include <QOpenGLFunctions>
#include <QGuiApplication>

#ifdef Q_OS_WIN
    #include <Windows.h>
#endif

#include <QtGui/QOpenGLFramebufferObject>

class MpvRenderer : public QQuickFramebufferObject::Renderer, protected QOpenGLFunctions
{
    static void *get_proc_address(void *ctx, const char *name) {
        (void)ctx;
        QOpenGLContext *glctx = QOpenGLContext::currentContext();
        if (!glctx)
        return NULL;

        void *res = (void *)glctx->getProcAddress(QByteArray(name));
#ifdef Q_OS_WIN
        if (!res)
        {
            HMODULE oglmod = GetModuleHandleA("opengl32.dll");
            return (void *) GetProcAddress(oglmod, name);
        }
#endif
        return res;
    }

    mpv::qt::Handle mpv;
    QQuickWindow *window;
    mpv_opengl_cb_context *mpv_gl;
public:
    MpvRenderer(const MpvObject *obj)
        : mpv(obj->mpv), window(obj->window()), mpv_gl(obj->mpv_gl)
    {
        int r = mpv_opengl_cb_init_gl(mpv_gl, NULL, get_proc_address, NULL);
        if (r < 0)
            throw std::runtime_error("could not initialize OpenGL");
    }

    virtual ~MpvRenderer()
    {
        // Until this call is done, we need to make sure the player remains
        // alive. This is done implicitly with the mpv::qt::Handle instance
        // in this class.
        mpv_opengl_cb_uninit_gl(mpv_gl);
    }

    void render()
    {
        QOpenGLFramebufferObject *fbo = framebufferObject();
        window->resetOpenGLState();
        mpv_opengl_cb_draw(mpv_gl, fbo->handle(), fbo->width(), fbo->height());
        window->resetOpenGLState();
    }
};


#endif
