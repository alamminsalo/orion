#include "mpvobject.h"
#include "mpvrenderer.h"


static void wakeup(void *ctx)
{
    MpvObject *mpvhandler = (MpvObject*)ctx;
    QCoreApplication::postEvent(mpvhandler, new QEvent(QEvent::User));
}

MpvObject::MpvObject(QQuickItem * parent)
    : QQuickFramebufferObject(parent), mpv_gl(0)
{
    mpv = mpv::qt::Handle::FromRawHandle(mpv_create());
    if (!mpv)
        throw std::runtime_error("could not create mpv context");

    mpv_set_option_string(mpv, "config", "yes");
    mpv_set_option_string(mpv, "config-dir", ".");

    //Enable for debugging
    mpv_set_option_string(mpv, "terminal", "yes");
    mpv_set_option_string(mpv, "msg-level", "all=v");

    if (mpv_initialize(mpv) < 0)
        throw std::runtime_error("could not initialize mpv context");

    // Make use of the MPV_SUB_API_OPENGL_CB API.
    mpv::qt::set_option_variant(mpv, "vo", "opengl-cb");
    mpv::qt::set_option_variant(mpv, "input-cursor", "no");

    // Request hw decoding, just for testing.
    //mpv::qt::set_option_variant(mpv, "hwdec", "auto");

    //Cache
    //mpv::qt::set_option_variant(mpv, "cache", 8192 * 1024);
    //mpv::qt::set_option_variant(mpv, "cache-pause", true);




    // Setup the callback that will make QtQuick update and redraw if there
    // is a new video frame. Use a queued connection: this makes sure the
    // doUpdate() function is run on the GUI thread.
    mpv_gl = (mpv_opengl_cb_context *)mpv_get_sub_api(mpv, MPV_SUB_API_OPENGL_CB);

    if (!mpv_gl)
        throw std::runtime_error("OpenGL not compiled in");

    mpv_opengl_cb_set_update_callback(mpv_gl, MpvObject::on_update, (void *)this);
    connect(this, &MpvObject::onUpdate, this, &MpvObject::doUpdate,
            Qt::QueuedConnection);


    //Set observe properties
    mpv_observe_property(mpv, 0, "core-idle", MPV_FORMAT_FLAG);
    //mpv_observe_property(mpv, 0, "paused-for-cache", MPV_FORMAT_FLAG);
    mpv_observe_property(mpv, 0, "cache-buffering-state", MPV_FORMAT_INT64);

    // setup callback event handling
    mpv_set_wakeup_callback(mpv, wakeup, this);
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

QQuickFramebufferObject::Renderer *MpvObject::createRenderer() const
{
    window()->setPersistentOpenGLContext(true);
    window()->setPersistentSceneGraph(true);
    return new MpvRenderer(this);
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
//                        setTime((int)*(double*)prop->data);
//                        lastTime = time;
                    }
                }
                else if(QString(prop->name) == "volume")
                {
//                    if(prop->format == MPV_FORMAT_DOUBLE)
//                        setVolume((int)*(double*)prop->data);
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
                        if((bool)*(unsigned*)prop->data)
                            emit playingPaused();
                        else
                            emit playingResumed();
                    }
                }
                else if(QString(prop->name) == "cache-buffering-state")
                {
                    if(prop->format == MPV_FORMAT_INT64)
                    {
                        //if((unsigned*)prop->data)
                            emit bufferingStarted();
//                        else
//                            ShowText(QString(), 0);
                    }
                }
                break;
            }
            case MPV_EVENT_IDLE:
                emit playingStopped();
//                fileInfo.length = 0;
//                setTime(0);
//                setPlayState(Mpv::Idle);
                break;
                // these two look like they're reversed but they aren't. the names are misleading.
            case MPV_EVENT_START_FILE:
                qDebug() << "AT FILE START";
                mpv::qt::set_option_variant(mpv, "cache", 8192 * 1024);
//                setPlayState(Mpv::Loaded);
                break;
            case MPV_EVENT_FILE_LOADED:
//                setPlayState(Mpv::Started);
//                LoadFileInfo();
//                SetProperties();
            case MPV_EVENT_UNPAUSE:
//                setPlayState(Mpv::Playing);
                break;
            case MPV_EVENT_PAUSE:
//                setPlayState(Mpv::Paused);
//                ShowText(QString(), 0);
                break;
            case MPV_EVENT_END_FILE:
//                if(playState == Mpv::Loaded)
//                    ShowText(tr("File couldn't be opened"));
//                setPlayState(Mpv::Stopped);
                break;
            case MPV_EVENT_SHUTDOWN:
                QCoreApplication::quit();
                break;
            case MPV_EVENT_LOG_MESSAGE:
            {
//                mpv_event_log_message *message = static_cast<mpv_event_log_message*>(event->data);
//                if(message != nullptr)
//                    emit messageSignal(message->text);
                break;
            }
            default: // unhandled events
                break;
            }
        }
        return true;
    }
    return QObject::event(event);
}
