#include "mpvobject.h"
#include <stdexcept>
#include <clocale>
#include <algorithm>

#include <QObject>
#include <QtGlobal>
#include <QDateTime>
#include <QOpenGLContext>
#include <QGuiApplication>

#include <QtGui/QOpenGLFramebufferObject>

#include <QtQuick/QQuickWindow>
#include <QtQuick/QQuickView>
#include <QJSEngine>

namespace
{
static void wakeup(void *ctx)
{
    MpvObject *mpvhandler = (MpvObject*)ctx;
    QCoreApplication::postEvent(mpvhandler, new QEvent(QEvent::User));
}

void on_mpv_redraw(void *ctx)
{
    MpvObject::on_update(ctx);
}

static void *get_proc_address_mpv(void *ctx, const char *name)
{
    Q_UNUSED(ctx)

    QOpenGLContext *glctx = QOpenGLContext::currentContext();
    if (!glctx) return nullptr;

    return reinterpret_cast<void *>(glctx->getProcAddress(QByteArray(name)));
}

}

class MpvRenderer : public QQuickFramebufferObject::Renderer
{
    MpvObject *obj;

public:
    MpvRenderer(MpvObject *new_obj)
        : obj{new_obj}
    {

    }

    virtual ~MpvRenderer()
    {}

    // This function is called when a new FBO is needed.
    // This happens on the initial frame.
    QOpenGLFramebufferObject * createFramebufferObject(const QSize &size)
    {
        // init mpv_gl:
        if (!obj->mpv_gl)
        {
            mpv_opengl_init_params gl_init_params{get_proc_address_mpv, nullptr, nullptr};
            mpv_render_param params[]{
                {MPV_RENDER_PARAM_API_TYPE, const_cast<char *>(MPV_RENDER_API_TYPE_OPENGL)},
                {MPV_RENDER_PARAM_OPENGL_INIT_PARAMS, &gl_init_params},
                {MPV_RENDER_PARAM_INVALID, nullptr}
            };

            if (mpv_render_context_create(&obj->mpv_gl, obj->mpv, params) < 0)
                throw std::runtime_error("failed to initialize mpv GL context");
            mpv_render_context_set_update_callback(obj->mpv_gl, on_mpv_redraw, obj);
        }

        return QQuickFramebufferObject::Renderer::createFramebufferObject(size);
    }

    void render()
    {
        obj->window()->resetOpenGLState();

        QOpenGLFramebufferObject *fbo = framebufferObject();
        mpv_opengl_fbo mpfbo{static_cast<int>(fbo->handle()), fbo->width(), fbo->height(), 0};
        int flip_y{0};

        mpv_render_param params[] = {
            // Specify the default framebuffer (0) as target. This will
            // render onto the entire screen. If you want to show the video
            // in a smaller rectangle or apply fancy transformations, you'll
            // need to render into a separate FBO and draw it manually.
            {MPV_RENDER_PARAM_OPENGL_FBO, &mpfbo},
            // Flip rendering (needed due to flipped GL coordinate system).
            {MPV_RENDER_PARAM_FLIP_Y, &flip_y},
            {MPV_RENDER_PARAM_INVALID, nullptr}
        };
        // See render_gl.h on what OpenGL environment mpv expects, and
        // other API details.
        mpv_render_context_render(obj->mpv_gl, params);

        obj->window()->resetOpenGLState();
    }
};

MpvObject::MpvObject(QQuickItem * parent)
    : QQuickFramebufferObject(parent), mpv{mpv_create()}, mpv_gl(nullptr)
{
    if (!mpv)
        throw std::runtime_error("could not create mpv context");

#ifdef DEBUG_LIBMPV
    mpv_set_option_string(mpv, "terminal", "yes");
    mpv_set_option_string(mpv, "msg-level", "all=v");
#endif

    if (mpv_initialize(mpv) < 0)
        throw std::runtime_error("could not initialize mpv context");

    // Request hw decoding, just for testing.
    mpv::qt::set_option_variant(mpv, "hwdec", "auto");

    mpv_set_wakeup_callback(mpv, wakeup, this);

    connect(this, &MpvObject::onUpdate, this, &MpvObject::doUpdate, Qt::QueuedConnection);
}

MpvObject::~MpvObject()
{
    if (mpv_gl) // only initialized if something got drawn
    {
        mpv_render_context_free(mpv_gl);
    }

    mpv_terminate_destroy(mpv);
}

void MpvObject::on_update(void *ctx)
{
    MpvObject *self = (MpvObject *)ctx;
    emit self->onUpdate();
}

// connected to onUpdate(); signal makes sure it runs on the GUI thread
void MpvObject::doUpdate()
{
    update();
}

void MpvObject::command(const QVariant& params)
{
    mpv::qt::command_variant(mpv, params);
}

void MpvObject::setProperty(const QString& name, const QVariant& value)
{
    mpv::qt::set_property_variant(mpv, name, value);
}

void MpvObject::setOption(const QString &name, const QVariant &value)
{
    mpv::qt::set_option_variant(mpv, name, value);
}

bool MpvObject::observeProperty(const QString &name, const QJSValue &callback)
{
    if (!callback.isCallable()) return false;
    callbacks.emplace_back(std::make_unique<QJSValue>(callback));
    QJSValue *pCallback = callbacks[callbacks.size() - 1].get();
    if (mpv_observe_property(mpv, (uint64_t)(pCallback), name.toLatin1().data(), MPV_FORMAT_NODE) >= 0) {
        connect(callback.engine(), &QJSEngine::destroyed, this, [this, pCallback](QObject*){
            callbacks.erase(std::remove_if(callbacks.begin(), callbacks.end(), [&](auto const& cb){
                if (cb.get() == pCallback) {
                    mpv_unobserve_property(mpv, (uint64_t)pCallback);
                    return true;
                }
                return false;
            }));
        });
        return true;
    } else {
        callbacks.pop_back();
        return false;
    }
}

bool MpvObject::unobserveProperty(const QJSValue &callback)
{
    bool erased = false;
    callbacks.erase(std::remove_if(callbacks.begin(), callbacks.end(), [&](auto const& cb){
        if (cb->equals(callback)) {
            mpv_unobserve_property(mpv, (uint64_t)cb.get());
            erased = true;
            return true;
        }
        return false;
    }));
    return erased;
}

QQuickFramebufferObject::Renderer *MpvObject::createRenderer() const
{
    window()->setPersistentOpenGLContext(true);
    window()->setPersistentSceneGraph(true);
    return new MpvRenderer(const_cast<MpvObject *>(this));
}

QVariant MpvObject::getProperty(const QString &name)
{
    return mpv::qt::get_property_variant(mpv, name);
}

bool MpvObject::event(QEvent *event)
{
    if(event->type() == QEvent::User)
    {
        while(mpv)
        {
            mpv_event *event = mpv_wait_event(mpv, 0);
            if(event == nullptr ||
               event->event_id == MPV_EVENT_NONE)
            {
                break;
            }
            //HandleErrorCode(event->error);
            switch (event->event_id)
            {
            case MPV_EVENT_PROPERTY_CHANGE:
            {
                mpv_event_property *prop = reinterpret_cast<mpv_event_property*>(event->data);
                if (prop->format == MPV_FORMAT_NODE && event->reply_userdata) {
                    QJSValue& callback = *reinterpret_cast<QJSValue*>(event->reply_userdata);
                    mpv_node* node = reinterpret_cast<mpv_node*>(prop->data);
                    mpv::qt::node_autofree f(node);

                    if (callback.isCallable()) {
                        QJSEngine *engine = callback.engine();
                        callback.call({
                            engine->toScriptValue(mpv::qt::node_to_variant(node)),
                            engine->toScriptValue(QString(prop->name))
                        });
                    }
                }
                break;
            }
            case MPV_EVENT_IDLE:
                break;
            case MPV_EVENT_START_FILE:
                break;
            case MPV_EVENT_UNPAUSE:
                break;
            case MPV_EVENT_PAUSE:
                break;
            case MPV_EVENT_END_FILE:
                break;
            case MPV_EVENT_SHUTDOWN:
                QCoreApplication::quit();
                break;
            case MPV_EVENT_LOG_MESSAGE:
                break;

            default: // unhandled events
                break;
            }
        }
        return true;
    }
    return QObject::event(event);
}
