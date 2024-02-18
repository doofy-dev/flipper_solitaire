#pragma once

#include <notification/notification.h>
#include "utils/Input.h"
#include "utils/RenderBuffer.h"
#include "utils/Card.h"
#include "utils/Vector.h"
#include "GameLogic.h"


class GameLoop {
    Gui *gui;
    Canvas *canvas;
    InputEventHandler inputHandler;
    FuriMutex *render_mutex;
    FuriThread *buffer_thread_ptr;
    RenderBuffer *buffer;
    bool renderRunning = true;
    FuriPubSub *input;
    FuriPubSubSubscription *input_subscription;
    NotificationApp *notification_app;
    bool processing = true;

    GameLogic *logic;

    static void input_callback(const void *value, void *ctx);

    static int32_t render_thread(void *ctx);

public:
    GameLoop();

    ~GameLoop();

    void Start();
};
