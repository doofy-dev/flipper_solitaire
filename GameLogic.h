#pragma once


#include <notification/notification.h>
#include "utils/Card.h"
#include "utils/List.h"
#include "utils/Input.h"
#include "utils/Vector.h"

enum GameState {
    Logo, Intro, Play, Solve, Finish
};

class GameLogic {

    List<Card> hand = List<Card>();
    RenderBuffer *buffer;

    List<Card> stock = List<Card>();
    List<Card> waste = List<Card>();
    List<Card> foundation[4] = {List<Card>(), List<Card>(), List<Card>(), List<Card>()};
    List<Card> tableau[7] = {List<Card>(), List<Card>(), List<Card>(), List<Card>(), List<Card>(), List<Card>(),
                             List<Card>()};
    int8_t selection[2] = {0, 0};
    int8_t selectedCard = 0;
    Card *tempCard;
    Vector tempPos = {0, 0};
    float tempTime = 0;
    int8_t target[2] = {0, -1};
    bool readyToRender = false;
    Vector velocity;
    NotificationApp *notification;
public:
    GameState state = Logo;
    bool dirty = true;
    double startTime;
    double end;

    GameLogic(RenderBuffer *buffer, InputEventHandler *inputHandler, NotificationApp *notification_app);

    ~GameLogic();

    void Update(float delta);

    void Input(int key, InputType type);

    void Reset();

    void GenerateDeck();

    bool CanSolve();

    void DoIntro(float delta);

    void DrawPlayScene();

    void HandleSolve(float delta);

    void QuickSolve();

    void FallingCard(float delta);

    bool isReady() const { return readyToRender; }

    void DrawColumn(uint8_t x, uint8_t y, uint8_t selected, int8_t column);

    void HandleNavigation(int key);

    void PickAndPlace();
    void PlayError();
    void PlayBounce();

    int8_t FirstNonFlipped(const List<Card> &deck);
};
