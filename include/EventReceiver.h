#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include <irrlicht.h>
#include <iostream>


class EventReceiver : public irr::IEventReceiver
{
private:
    enum  KEY_STATE_ENUM {UP, DOWN, PRESSED, RELEASED};
    enum PROCESS_STATE_ENUM {STARTED, ENDED};

    KEY_STATE_ENUM keyState[irr::KEY_KEY_CODES_COUNT];
    
    PROCESS_STATE_ENUM processState;

    struct SMouseState
    {
        irr::core::position2di Position;
        float Wheel;
        KEY_STATE_ENUM Button[3];
    } mouseState;

public:
    enum MOUSE_BUTTON_ENUM {MOUSE_BUTTON_LEFT, MOUSE_BUTTON_MIDDLE, MOUSE_BUTTON_RIGHT};
    
    bool OnEvent(const irr::SEvent&);

    float Wheel();

    int MouseX();

    int MouseY();

    irr::core::position2di MousePosition();

    bool KeyDown(irr::EKEY_CODE keyCode);

    bool KeyPressed(irr::EKEY_CODE keyCode);

    bool KeyUp(irr::EKEY_CODE keyCode);

    bool KeyReleased(irr::EKEY_CODE keyCode);

    bool ButtonDown(MOUSE_BUTTON_ENUM buttonCode);

    bool ButtonPressed(MOUSE_BUTTON_ENUM buttonCode);

    bool ButtonUp(MOUSE_BUTTON_ENUM buttonCode);

    bool ButtonReleased(MOUSE_BUTTON_ENUM buttonCode);

    void EndEventProcess();

    void StartEventProcess();

    EventReceiver();
};
#endif