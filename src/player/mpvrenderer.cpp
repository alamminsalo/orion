#include "mpvrenderer.h"


void *MpvRenderer::get_proc_address(void *ctx, const char *name) {
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

MpvRenderer::MpvRenderer(const MpvObject *obj)
    : mpv(obj->mpv), window(obj->window()), mpv_gl(obj->mpv_gl)
{
    int r = mpv_opengl_cb_init_gl(mpv_gl, NULL, get_proc_address, NULL);
    if (r < 0)
        throw std::runtime_error("could not initialize OpenGL");
}


MpvRenderer::~MpvRenderer()
{
    // Until this call is done, we need to make sure the player remains
    // alive. This is done implicitly with the mpv::qt::Handle instance
    // in this class.
    mpv_opengl_cb_uninit_gl(mpv_gl);
}

void MpvRenderer::render()
{
    QOpenGLFramebufferObject *fbo = framebufferObject();
    window->resetOpenGLState();
    mpv_opengl_cb_draw(mpv_gl, fbo->handle(), fbo->width(), fbo->height());
    window->resetOpenGLState();
}
