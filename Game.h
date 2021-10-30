#pragma once
#include <string>
#include <vector>

#include "Console.h"

class Game;
struct MenuElement;
struct Button;
struct Select;

struct Event
{
    int key = 0;
    void (Game::*p_function) ();
};

struct Position
{
    int x;
    int y;
};

enum Turn
{
    PLAYER = 0,
    AI = 1,
    AI_2 = 2,
    PLAYER_2 = 3
};

enum Mark
{
    EMPTY = 0,
    X = 1,
    O = 2,
};

class Game final : public Console
{
    std::vector<Event*> events;
    std::vector<MenuElement*> menu;

    bool is_running = true;
    bool is_game_started = false;
    bool is_menu = true;
    bool is_win = false;
    bool is_draw = false;

    int win_sequence_count = 5;
    int field_size = 8;
    int game_type = 0;

    int menu_offset = 2;
    int field_offset = 21;
    int game_info_offset = 2;

    Position last_turn = { 0, 0};

    int menu_cursor_position = 0;
    Position game_cursor_position = {0, 0};

    Mark wins = EMPTY;
    Turn turn = PLAYER;
    std::vector<int> possible_turns;
    short* field;
    Turn first_turn = PLAYER;
    Mark first_marker = X;

    Game();
    ~Game() override;

public:
    void createKeyEvent(int key, void (Game::*p_function) ());
    void fireKeyEvent(int key);

    void selectUpMenuOption();
    void selectDownMenuOption();
    void changeLeftMenuOption();
    void changeRightMenuOption();
    void applyMenuOption();

    void moveFieldCursorUp();
    void moveFieldCursorDown();
    void moveFieldCursorLeft();
    void moveFieldCursorRight();

    void turnPlayer();
    void turnAI();
    Mark getCurrentTurnMarker();

    int popPossibleTurn(int position);

    void changeGameTypeLeft();
    void changeGameTypeRight();

    void changeFieldSizeLeft();
    void changeFieldSizeRight();

    void changeWinRuleLeft();
    void changeWinRuleRight();

    std::wstring getGameTypeValue();
    std::wstring getFieldSizeValue();
    std::wstring getWinRuleValue();

    void initField();
    void start();
    void breakGame();
    void quit();

    void renderField();
    void renderMenu();
    void renderGameInfo();

    void update();

    bool checkCrossLeft();
    bool checkCrossRight();
    bool checkHorizontal();
    bool checkVertical();
    void checkGameState();

    static void init();
};

struct MenuElement
{
    MenuElement(const std::wstring& label) :
        label(label)
    {}

    virtual ~MenuElement() = default;

    std::wstring label;
};

struct Button : MenuElement
{
    Button(const std::wstring& label, void (Game::* action)()): MenuElement(label), action(action) {}

    void (Game::*action) ();
};

struct Select : MenuElement
{
    Select(const std::wstring& label, std::wstring(Game::* action_value)(), void(Game::* action_left)(), void(Game::* action_right)()):
        MenuElement(label),
        action_value(action_value),
        action_left(action_left),
        action_right(action_right)
    {}

    std::wstring (Game::*action_value) ();
    void (Game::*action_left) ();
    void (Game::*action_right) ();
};
