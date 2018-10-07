#include "mpvobject.h"
#include "mpvrenderer.h"

#include <QtGlobal>
#include <QDateTime>

static void wakeup(void *ctx)
{
    MpvObject *mpvhandler = (MpvObject*)ctx;
    QCoreApplication::postEvent(mpvhandler, new QEvent(QEvent::User));
}

MpvObject::MpvObject(QQuickItem * parent)
    : QQuickFramebufferObject(parent), mpv_gl(0)
{
    std::setlocale(LC_NUMERIC, "C");

    mpv = mpv::qt::Handle::FromRawHandle(mpv_create());
    if (!mpv)
        throw std::runtime_error("could not create mpv context");

#ifdef DEBUG_LIBMPV
    //Enable for debugging
    mpv_set_option_string(mpv, "terminal", "yes");
    mpv_set_option_string(mpv, "msg-level", "all=v");
#endif

    // Make use of the MPV_SUB_API_OPENGL_CB API.
    mpv::qt::set_option_variant(mpv, "gpu-context", "angle");
    mpv::qt::set_option_variant(mpv, "vo", "opengl-cb");
    //mpv::qt::set_option_variant(mpv, "input-cursor", "no");

    // Request hw decoding, just for testing.
    mpv::qt::set_option_variant(mpv, "hwdec", "auto");

    //Cache
    //mpv::qt::set_option_variant(mpv, "cache", 8192);

    if (mpv_initialize(mpv) < 0)
        throw std::runtime_error("could not initialize mpv context");

    // Setup the callback that will make QtQuick update and redraw if there
    // is a new video frame. Use a queued connection: this makes sure the
    // doUpdate() function is run on the GUI thread.
    mpv_gl = (mpv_opengl_cb_context *)mpv_get_sub_api(mpv, MPV_SUB_API_OPENGL_CB);

    if (!mpv_gl)
        throw std::runtime_error("OpenGL not compiled in");

    mpv_opengl_cb_set_update_callback(mpv_gl, MpvObject::on_update, (void *)this);
    connect(this, &MpvObject::onUpdate, this, &MpvObject::doUpdate,
            Qt::QueuedConnection);

    //Restore volume to 100
    //setProperty("volume", QVariant::fromValue(100));

    //Set observe properties
    mpv_observe_property(mpv, 0, "core-idle", MPV_FORMAT_FLAG);
    //mpv_observe_property(mpv, 0, "volume", MPV_FORMAT_DOUBLE);
    mpv_observe_property(mpv, 0, "cache-buffering-state", MPV_FORMAT_INT64);
    mpv_observe_property(mpv, 0, "playback-time", MPV_FORMAT_DOUBLE);

    // setup callback event handling
    mpv_set_wakeup_callback(mpv, wakeup, this);

    time = 0;
}

MpvObject::~MpvObject()
{
    if (mpv_gl)
        mpv_opengl_cb_set_update_callback(mpv_gl, NULL, NULL);
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

QQuickFramebufferObject::Renderer *MpvObject::createRenderer() const
{
    window()->setPersistentOpenGLContext(true);
    window()->setPersistentSceneGraph(true);
    return new MpvRenderer(this);
}

void MpvObject::pause()
{
    time = QDateTime::currentMSecsSinceEpoch();
    QStringList args = (QStringList() << "set" << "pause" << "yes");
    mpv::qt::command_variant(mpv, args);
}

void MpvObject::play(bool autoReload)
{
    if (autoReload && QDateTime::currentMSecsSinceEpoch() - time > 5000){
        qDebug() << "Waited too long, resetting playback" << mpv::qt::get_property_variant(mpv, "path");
        mpv::qt::command_variant(mpv, (QStringList() << "loadfile" << mpv::qt::get_property_variant(mpv, "path").toString()));
    }

    QStringList args = (QStringList() << "set" << "pause" << "no");
    mpv::qt::command_variant(mpv, args);
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
                mpv_event_property *prop = (mpv_event_property*)event->data;
                if(QString(prop->name) == "playback-time") // playback-time does the same thing as time-pos but works for streaming media
                {
                    if(prop->format == MPV_FORMAT_DOUBLE)
                    {
                        int pos = (int)*(double *)prop->data;
                        emit positionChanged(pos);
                    }
                }
                else if(QString(prop->name) == "volume")
                {
                   if(prop->format == MPV_FORMAT_DOUBLE)
                        emit volumeChanged(*(double*)prop->data);
                }
                else if(QString(prop->name) == "sid")
                {
//                    if(prop->format == MPV_FORMAT_INT64)
//                        setSid(*(int*)prop->data);
                }
                else if(QString(prop->name) == "aid")
                {
//                    if(prop->format == MPV_FORMAT_INT64)
//                        setAid(*(int*)prop->data);
                }
                else if(QString(prop->name) == "sub-visibility")
                {
//                    if(prop->format == MPV_FORMAT_FLAG)
//                        setSubtitleVisibility((bool)*(unsigned*)prop->data);
                }
                else if(QString(prop->name) == "mute")
                {
//                    if(prop->format == MPV_FORMAT_FLAG)
//                        setMute((bool)*(unsigned*)prop->data);
                }
                else if(QString(prop->name) == "core-idle")
                {
                    if(prop->format == MPV_FORMAT_FLAG)
                    {
                        if((bool)*(unsigned*)prop->data){
                            emit playingPaused();
                        } else {
                            emit playingResumed();
                        }
                    }
                }
                else if(QString(prop->name) == "cache-buffering-state")
                {
                    if(prop->format == MPV_FORMAT_INT64)
                    {
                        if ((int) *(int*)prop->data < 100)
                            emit bufferingStarted();
                    }
                }
                break;
            }
            case MPV_EVENT_IDLE:
                emit playingStopped();
                break;
                // these two look like they're reversed but they aren't. the names are misleading.
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
