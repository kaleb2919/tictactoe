#include "Game.h"

#include <iostream>
#include <random>
#include <thread>
using namespace std::literals;

Game::Game()
{
    menu.push_back(new Button(L"New game", &Game::start));
    menu.push_back(new Select(L"Type", &Game::getGameTypeValue, &Game::changeGameTypeLeft, &Game::changeGameTypeRight));
    menu.push_back(new Select(L"Field", &Game::getFieldSizeValue, &Game::changeFieldSizeLeft, &Game::changeFieldSizeRight));
    menu.push_back(new Select(L"Win", &Game::getWinRuleValue, &Game::changeWinRuleLeft, &Game::changeWinRuleRight));
    menu.push_back(new Button(L"Quit", &Game::quit));

    createKeyEvent(VK_UP, &Game::moveFieldCursorUp);
    createKeyEvent(VK_DOWN, &Game::moveFieldCursorDown);
    createKeyEvent(VK_LEFT, &Game::moveFieldCursorLeft);
    createKeyEvent(VK_RIGHT, &Game::moveFieldCursorRight);
    createKeyEvent(VK_SPACE, &Game::turnPlayer);
    createKeyEvent(VK_ESCAPE, &Game::breakGame);

    createKeyEvent(VK_UP, &Game::selectUpMenuOption);
    createKeyEvent(VK_DOWN, &Game::selectDownMenuOption);
    createKeyEvent(VK_LEFT, &Game::changeLeftMenuOption);
    createKeyEvent(VK_RIGHT, &Game::changeRightMenuOption);
    createKeyEvent(VK_SPACE, &Game::applyMenuOption);

    initField();
    update();
}

Game::~Game()
{
    for (auto event : events)
    {
        delete event;
    }

    for (auto element : menu)
    {
        delete element;
    }
}

void Game::createKeyEvent(int key, void(Game::* p_function)())
{
    events.push_back(new Event { key, p_function });
}

void Game::fireKeyEvent(int key)
{
    for (auto event : events)
    {
        if (event->key == key)
        {
            (this->*event->p_function)();
        }
    }
}

void Game::selectUpMenuOption()
{
    if (is_menu)
    {
        int size = menu.size();
        menu_cursor_position = (size + --menu_cursor_position % size) % size;
    }
}

void Game::selectDownMenuOption()
{
    if (is_menu)
    {
        int size = menu.size();
        menu_cursor_position = (size + ++menu_cursor_position % size) % size;
    }
}

void Game::changeLeftMenuOption()
{
    if (is_menu)
    {
        auto select = dynamic_cast<Select*>(menu[menu_cursor_position]);

        if (select)
        {
            (this->*select->action_left)();
        }
    }
}

void Game::changeRightMenuOption()
{
    if (is_menu)
    {
        auto select = dynamic_cast<Select*>(menu[menu_cursor_position]);

        if (select)
        {
            (this->*select->action_right)();
        }
    }
}

void Game::applyMenuOption()
{
    if (is_menu)
    {
        auto button = dynamic_cast<Button*>(menu[menu_cursor_position]);

        if (button)
        {
            (this->*button->action)();
        }
    }
}

void Game::moveFieldCursorUp()
{
    if (is_game_started && (turn == PLAYER || turn == PLAYER_2))
    {
        game_cursor_position.y = (field_size + (game_cursor_position.y - 1) % field_size) % field_size;
    }
}

void Game::moveFieldCursorDown()
{
    if (is_game_started && (turn == PLAYER || turn == PLAYER_2))
    {
        game_cursor_position.y = (field_size + (game_cursor_position.y + 1) % field_size) % field_size;
    }
}

void Game::moveFieldCursorLeft()
{
    if (is_game_started && (turn == PLAYER || turn == PLAYER_2))
    {
        game_cursor_position.x = (field_size + (game_cursor_position.x - 1) % field_size) % field_size;
    }
}

void Game::moveFieldCursorRight()
{
    if (is_game_started && (turn == PLAYER || turn == PLAYER_2))
    {
        game_cursor_position.x = (field_size + (game_cursor_position.x + 1) % field_size) % field_size;
    }
}

void Game::turnPlayer()
{
    int turn_position = game_cursor_position.x + game_cursor_position.y * field_size;

    if (is_game_started && (turn == PLAYER || turn == PLAYER_2))
    {
        if (field[turn_position] == EMPTY)
        {
            last_turn = game_cursor_position;
            possible_turns.erase(std::find(possible_turns.begin(), possible_turns.end(), turn_position));
            field[turn_position] = getCurrentTurnMarker();
            checkGameState();
            if (turn == PLAYER)
            {
                turn = game_type == 0 ? PLAYER_2 : AI;
            }
            else
            {
                turn = PLAYER;
            }
        }
    }
}

void Game::turnAI()
{
    if (is_game_started && (turn == AI || turn == AI_2))
    {
        int possible_turn = popPossibleTurn(rand() % possible_turns.size());
        last_turn.x = possible_turn % field_size;
        last_turn.y = (possible_turn - possible_turn % field_size) / field_size;
        field[possible_turn] = getCurrentTurnMarker();
        checkGameState();
        if (turn == AI)
        {
            turn = game_type == 1 ? PLAYER : AI_2;
        }
        else
        {
            turn = AI;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

Mark Game::getCurrentTurnMarker()
{
    if (turn == first_turn)
    {
        return first_marker == X ? X : O;
    }

    return first_marker == X ? O : X;
}

int Game::popPossibleTurn(int position)
{
    std::swap(possible_turns[position], possible_turns.back());
    int turn_position = possible_turns.back();
    possible_turns.pop_back();
    return turn_position;
}


void Game::changeGameTypeLeft()
{
    game_type = (3 + --game_type % 3) % 3;
}

void Game::changeGameTypeRight()
{
    game_type = (3 + ++game_type % 3) % 3;
}

void Game::changeFieldSizeLeft()
{
    field_size = (std::max)(3, --field_size);
    win_sequence_count = (std::min)(win_sequence_count, field_size);
    initField();
}

void Game::changeFieldSizeRight()
{
    field_size = (std::min)(++field_size, 16);
    initField();
}

void Game::changeWinRuleLeft()
{
    win_sequence_count = (std::max)(3, --win_sequence_count);
}

void Game::changeWinRuleRight()
{
    win_sequence_count = (std::min)(++win_sequence_count, field_size);
}

std::wstring Game::getGameTypeValue()
{
    switch (game_type)
    {
    case 0:
        return L"PvP";
    case 1:
        return L"PvAI";
    case 2:
        return L"AIvAI";
    default:
        return L"Unknown";
    }
}

std::wstring Game::getFieldSizeValue()
{
    return std::to_wstring(field_size);
}

std::wstring Game::getWinRuleValue()
{
    return std::to_wstring(win_sequence_count);
}

void Game::initField()
{
    if (field)
    {
        delete [] field;
    }

    possible_turns.clear();
    field = new short[field_size * field_size];
    for (int i = 0; i < field_size * field_size; i++)
    {
        field[i] = 0;
        possible_turns.push_back(i);
    }
}

void Game::start()
{
    switch (game_type)
    {
    case 0:
        first_turn = turn = rand() % 2 ? PLAYER : PLAYER_2;
        break;
    case 1:
        first_turn = turn = rand() % 2 ? PLAYER : AI;
        break;
    case 2:
        first_turn = turn = rand() % 2 ? AI : AI_2;
        break;
    default:
        first_turn = PLAYER;
    }

    first_marker = rand() % 2 ? X : O;
    is_menu = false;
    is_game_started = true;
}

void Game::breakGame()
{
    initField();
    menu_cursor_position = 0;
    game_cursor_position = {0, 0};
    is_menu = true;
    is_game_started = false;
    is_win = false;
    first_turn = PLAYER;
}

void Game::quit()
{
    is_running = false;
}

void Game::renderField()
{
    for (int y = 0; y < field_size; y++)
    {
        int start_position = field_offset + (y + 1) * screen_width;
        for (int x = 0; x < field_size; x++)
        {
            wchar_t field_value;
            switch (field[x + y * field_size])
            {
            case EMPTY:
                field_value = L' ';
                break;
            case X:
                field_value = L'X';
                break;
            case O:
                field_value = L'O';
                break;
            default:
                field_value = L'E';
            }

            if (is_game_started && (turn == PLAYER || turn == PLAYER_2) && game_cursor_position.x == x && game_cursor_position.y == y)
            {
                wsprintf(&screen_buffer[start_position + x * 3], L"[%lc]", field_value);
            }
            else
            {
                wsprintf(&screen_buffer[start_position + x * 3], L"|%lc|", field_value);
            }
        }
    }
}

void Game::renderMenu()
{
    int i = 1;
    for (auto element : menu)
    {
        if (menu_cursor_position == i - 1)
        {
            wsprintf(&screen_buffer[menu_offset - 1 + screen_width * i], L">");
        }

        auto select = dynamic_cast<Select*>(element);

        if (select)
        {
            wsprintf(&screen_buffer[menu_offset + screen_width * i++], L"%ws: %ws", select->label.c_str(), (this->*select->action_value)().c_str());
        }
        else
        {
            wsprintf(&screen_buffer[menu_offset + screen_width * i++], L"%ws", element->label.c_str());
        }
    }
}

void Game::renderGameInfo()
{
    std::wstring who_turn;

    switch (turn)
    {
    case PLAYER:
        who_turn = L"Player";
        break;
    case AI:
        who_turn = L"AI";
        break;
    case AI_2:
        who_turn = L"AI 2";
        break;
    case PLAYER_2:
        who_turn = L"Player 2";
        break;
    }

    wsprintf(&screen_buffer[game_info_offset + screen_width], L"Turn: %ws (%lc)", who_turn.c_str(), getCurrentTurnMarker() == X ? L'X' : L'O');

    if (is_win)
    {
        wsprintf(&screen_buffer[game_info_offset + screen_width * 3], L"Wins: %lc", wins == X ? L'X' : L'O');
    }

    if (is_draw)
    {
        wsprintf(&screen_buffer[game_info_offset + screen_width * 3], L"Draw");
    }
}

void Game::update()
{
    clear();
    if (is_menu)
    {
        renderMenu();
    }
    else
    {
        renderGameInfo();
    }

    renderField();
    refresh();
}

bool Game::checkCrossLeft()
{
    int sequence = 0;
    int cursor = last_turn.x;
    int offset = 0;
    while (cursor < field_size &&
        last_turn.y + offset < field_size &&
        field[cursor + (last_turn.y + offset++) * field_size] == getCurrentTurnMarker())
    {
        sequence++;
        cursor++;
    }

    offset = 1;
    cursor = last_turn.x - 1;
    while (cursor >= 0 &&
        last_turn.y - offset >= 0 &&
        field[cursor + (last_turn.y - offset++) * field_size] == getCurrentTurnMarker())
    {
        sequence++;
        cursor--;
    }

    return sequence >= win_sequence_count;
}

bool Game::checkCrossRight()
{
    int sequence = 0;
    int cursor = last_turn.y;
    int offset = 0;
    while (cursor < field_size &&
        last_turn.x - offset >= 0 &&
        field[last_turn.x - offset++ + cursor * field_size] == getCurrentTurnMarker())
    {
        sequence++;
        cursor++;
    }

    offset = 1;
    cursor = last_turn.y - 1;
    while (cursor >= 0 &&
        last_turn.x + offset < field_size &&
        field[last_turn.x + offset++ + cursor * field_size] == getCurrentTurnMarker())
    {
        sequence++;
        cursor--;
    }

    return sequence >= win_sequence_count;
}

bool Game::checkHorizontal()
{
    int sequence = 0;
    int cursor = last_turn.x;
    while (cursor < field_size && field[cursor + last_turn.y * field_size] == getCurrentTurnMarker())
    {
        sequence++;
        cursor++;
    }

    cursor = last_turn.x - 1;
    while (cursor >= 0 && field[cursor + last_turn.y * field_size] == getCurrentTurnMarker())
    {
        sequence++;
        cursor--;
    }

    return sequence >= win_sequence_count;
}

bool Game::checkVertical()
{
    int sequence = 0;
    int cursor = last_turn.y;
    while (cursor < field_size && field[last_turn.x + cursor * field_size] == getCurrentTurnMarker())
    {
        sequence++;
        cursor++;
    }

    cursor = last_turn.y - 1;
    while (cursor >= 0 && field[last_turn.x + cursor * field_size] == getCurrentTurnMarker())
    {
        sequence++;
        cursor--;
    }

    return sequence >= win_sequence_count;
}

void Game::checkGameState()
{
    if (checkCrossLeft() || checkCrossRight() || checkHorizontal() || checkVertical())
    {
        wins = getCurrentTurnMarker();
        is_win = true;
        is_game_started = false;
    }

    if (possible_turns.size() == 0)
    {
        is_draw = true;
        is_game_started = false;
    }
}

void Game::init()
{
    Game game;

    while(game.is_running)
    {
        if (game.game_type != 0 && game.turn == AI || game.turn == AI_2)
        {
            game.turnAI();
        }

        game.fireKeyEvent(game.getLastPressedKey());
        game.update();
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }
}
