#include <dolphin/dolphin.h>
#include <notification/notification_messages.h>
#include "GameLoop.h"

GameLoop::GameLoop() {
    inputHandler = InputEventHandler();
    dolphin_deed(DolphinDeedPluginGameStart);
    input = (FuriPubSub *) furi_record_open(RECORD_INPUT_EVENTS);
    gui = (Gui *) furi_record_open(RECORD_GUI);
    canvas = gui_direct_draw_acquire(gui);
    input_subscription = furi_pubsub_subscribe(input, &input_callback, this);
    buffer = new RenderBuffer(128, 64);
    render_mutex = (FuriMutex *) furi_mutex_alloc(FuriMutexTypeNormal);
    notification_app = (NotificationApp *) furi_record_open(RECORD_NOTIFICATION);
    notification_message_block(notification_app, &sequence_display_backlight_enforce_on);
    logic = new GameLogic(buffer, &inputHandler, notification_app);
    if (!render_mutex) {
        return;
    }
    buffer_thread_ptr = furi_thread_alloc_ex(
        "BackBufferThread", 3 * 1024, render_thread, this);
}

void GameLoop::input_callback(const void *value, void *ctx) {
    auto *inst = (GameLoop *) ctx;
    const auto *event = (const InputEvent *) value;
    inst->inputHandler.Set(event->key, event->type);
    inst->logic->dirty = event->type != InputTypePress;
    if (event->type == InputTypeLong && event->key == InputKeyBack) {
        inst->processing = false;
    }
}

int32_t GameLoop::render_thread(void *ctx) {
    auto *inst = (GameLoop *) ctx;
    uint32_t last_tick = 0;
    furi_thread_set_current_priority(FuriThreadPriorityIdle);

    while (inst->renderRunning) {
        uint32_t tick = furi_get_tick();
        if (inst->logic->dirty) {
            if(furi_mutex_acquire(inst->render_mutex, 20) == FuriStatusOk) {
                inst->logic->dirty = false;
                inst->logic->Update((tick - last_tick) / 1000.0f);
                last_tick = tick;
                furi_mutex_release(inst->render_mutex);
            }
        }
        furi_thread_yield();
    }

    return 0;
}
char timeString[24];

void GameLoop::Start() {
    if (!render_mutex) {
        return;
    }

    furi_thread_start(buffer_thread_ptr);
    furi_thread_set_current_priority(FuriThreadPriorityIdle);
    while (processing) {
        if (logic->isReady()) {
            furi_mutex_acquire(render_mutex, FuriWaitForever);
            buffer->render(canvas);

            if(logic->end>0 &&logic->state == Logo){
                int diff = logic->end - logic->startTime;
                if(diff>0) {
                    int hours = diff / 3600000;
                    int minutes = (diff % 3600000) / 60000;
                    int seconds = (diff % 60000) / 1000;
                    snprintf(timeString, sizeof(timeString), "Completed: %02d:%02d:%02d", hours, minutes, seconds);
                    canvas_set_font(canvas, FontSecondary);
                    canvas_draw_str_aligned(canvas, 60, 5, AlignCenter, AlignTop, timeString);
                }
            }


            furi_mutex_release(render_mutex);
            canvas_commit(canvas);
        }
        furi_thread_yield();
    }
}

GameLoop::~GameLoop() {
    notification_message_block(notification_app, &sequence_display_backlight_enforce_auto);
    furi_pubsub_unsubscribe(input, input_subscription);

    renderRunning = false;
    furi_thread_join(buffer_thread_ptr);
    furi_thread_free(buffer_thread_ptr);

    canvas = NULL;
    gui_direct_draw_release(gui);
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_INPUT_EVENTS);
    furi_record_close(RECORD_NOTIFICATION);
    furi_mutex_free(render_mutex);

    delete logic;
    delete buffer;
}
