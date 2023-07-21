#include <furi.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include "Game.h"

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent *input;
} AppEvent;

static FuriMutex *mutex;

static void input_callback(InputEvent *input_event, FuriMessageQueue *event_queue) {
    furi_assert(event_queue);
    AppEvent event = {.type = EventTypeKey, .input = input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void update_timer_callback(FuriMessageQueue *event_queue) {
    furi_assert(event_queue);
    AppEvent event = {.type = EventTypeTick, .input=nullptr};
    furi_message_queue_put(event_queue, &event, 0);
}

static void render_callback(Canvas *const canvas, void *ctx) {
    Game *g = (Game *) ctx;
    auto status = furi_mutex_acquire(mutex, 150);
    if (g == nullptr || status != FuriStatusOk) return;
    g->Render(canvas);
    UNUSED(canvas);
    UNUSED(g);
    furi_mutex_release(mutex);
}

#ifdef __cplusplus
extern "C"
{
#endif
int32_t solitaire_app(void *p) {
    UNUSED(p);
    FURI_LOG_D("MEMORY", "Free %i", memmgr_get_free_heap());
    int32_t return_code = 0;
    size_t start = memmgr_get_free_heap();
    Game *game = new Game;
    FuriMessageQueue *event_queue = furi_message_queue_alloc(8, sizeof(AppEvent));
    mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    if (mutex) {
        bool processing = true;
        auto *notification = static_cast<NotificationApp *>(furi_record_open(RECORD_NOTIFICATION));

        notification_message_block(notification, &sequence_display_backlight_enforce_on);

        ViewPort *view_port = view_port_alloc();
        view_port_draw_callback_set(view_port, render_callback, game);
        view_port_input_callback_set(view_port, input_callback, event_queue);

        FuriTimer *timer = furi_timer_alloc(update_timer_callback, FuriTimerTypePeriodic, event_queue);
        furi_timer_start(timer, furi_kernel_get_tick_frequency() / 20);

        auto gui = static_cast<Gui *>(furi_record_open("gui"));
        gui_add_view_port(gui, view_port, GuiLayerFullscreen);

        AppEvent event;
        game->Reset();

        while (processing) {
            FuriStatus event_status = furi_message_queue_get(event_queue, &event, FuriWaitForever);
            furi_mutex_acquire(mutex, FuriWaitForever);

            if (event_status == FuriStatusOk) {
                if (event.type == EventTypeKey) {
                    if (event.input->type == InputTypeLong) {
                        switch (event.input->key) {
                            case InputKeyUp:
                            case InputKeyDown:
                            case InputKeyRight:
                            case InputKeyLeft:
                            case InputKeyOk:
                                game->LongPress(event.input->key);
                                break;
                            case InputKeyBack:
                                processing = false;
                                return_code = 1;
                            default:
                                break;
                        }
                    } else if (event.input->type == InputTypePress) {
                        game->Press(event.input->key);
                    }
                } else if (event.type == EventTypeTick) {
                    game->Update();
                }
            }

            view_port_update(view_port);
            furi_mutex_release(mutex);
        }


        notification_message_block(notification, &sequence_display_backlight_enforce_auto);
        furi_timer_free(timer);
        view_port_enabled_set(view_port, false);
        gui_remove_view_port(gui, view_port);
        furi_record_close(RECORD_GUI);
        furi_record_close(RECORD_NOTIFICATION);
        view_port_free(view_port);
    } else {
        FURI_LOG_E("APP", "cannot create mutex\r\n");
        return_code = 255;
    }


    delete game;
    furi_mutex_free(mutex);
    furi_message_queue_free(event_queue);
    start = memmgr_get_free_heap()-start;
    if(start!=0)
        FURI_LOG_E("MEMORY", "Leak detected %i", start);
    return return_code;
}

#ifdef __cplusplus
}
#endif