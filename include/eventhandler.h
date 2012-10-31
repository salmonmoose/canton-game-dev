#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include <irrlicht.h>



class EventReceiver : public irr::IEventReceiver
{
private:
    enum  KEY_STATE_ENUM {UP, DOWN, PRESSED, RELEASED};
    enum PROCESS_STATE_ENUM {STARTED, ENDED};

    KEY_STATE_ENUM keyStates[irr::KEY_KEY_CODES_COUNT];
    
    PROCESS_STATE_ENUM processState;

    struct SMouseState
    {
        irr::core::position2di Position;
        float Wheel;
         KEY_STATE_ENUM Button[3];
    } mouseState;

public:
    enum MOUSE_BUTTON_ENUM {LEFT, MIDDLE, RIGHT};
    
    virtual bool OnEvent(const irr::SEvent& event)
    {
        bool eventprocessed = false;

        if(processState == STARTED)
        {
            if(event.EventType == irr::EET_KEY_INPUT_EVENT)
            {
                if(event.KeyInput.PressedDown == true)
                {
                    if(keyStates[event.KeyInput.Key] != DOWN)
                    {
                        keyStates[event.KeyInput.Key] == PRESSED;
                    }
                    else
                    {
                        keyStates[event.KeyInput.Key] == DOWN;
                    }
                }
                else
                {
                    if(keyStates[event.KeyInput.Key] != UP)
                    {
                        keyStates[event.KeyInput.Key] = RELEASED;
                    }
                }
            }

            if(event.EventType == irr::EET_MOUSE_INPUT_EVENT)
            {
                if(event.MouseInput.Event == irr::EMIE_MOUSE_MOVED)
                {
                    mouseState.Position.X = event.MouseInput.X;
                    mouseState.Position.Y = event.MouseInput.Y;
                }

                if(event.MouseInput.Event == irr::EMIE_MOUSE_WHEEL)
                {
                    mouseState.Wheel += event.MouseInput.Wheel;
                }

                if(event.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN)
                {
                    if(mouseState.Button[LEFT] == UP || mouseState.Button[LEFT] == RELEASED)
                    {
                        mouseState.Button[LEFT] == PRESSED;
                    }
                    else
                    {
                        mouseState.Button[LEFT] == DOWN;
                    }
                }

                if(event.MouseInput.Event == irr::EMIE_LMOUSE_LEFT_UP)
                {
                    if(mouseState.Button[LEFT] != UP)
                    {
                        mouseState.Button[LEFT] = RELEASED;
                    }
                }

                if(event.MouseInput.Event == irr::EMIE_MMOUSE_PRESSED_DOWN)
                {
                    if(mouseState.Button[MIDDLE] == UP || mouseState.Button[MIDDLE] == RELEASED)
                    {
                        mouseState.Button[MIDDLE] == PRESSED;
                    }
                    else
                    {
                        mouseState.Button[MIDDLE] == DOWN;
                    }
                }

                if(event.MouseInput.Event == irr::EMIE_MMOUSE_LEFT_UP)
                {
                    if(mouseState.Button[MIDDLE] != UP)
                    {
                        mouseState.Button[MIDDLE] = RELEASED;
                    }
                }

                if(event.MouseInput.Event == irr::EMIE_RMOUSE_PRESSED_DOWN)
                {
                    if(mouseState.Button[RIGHT] == UP || mouseState.Button[RIGHT] == RELEASED)
                    {
                        mouseState.Button[RIGHT] == PRESSED;
                    }
                    else
                    {
                        mouseState.Button[RIGHT] == DOWN;
                    }
                }

                if(event.MouseInput.Event == irr::EMIE_RMOUSE_LEFT_UP)
                {
                    if(mouseState.Button[RIGHT] != UP)
                    {
                        mouseState.Button[RIGHT] = RELEASED;
                    }
                }
            }
        }
        return false;
    }

    float Wheel()
    {
        return mouseState.Wheel;
    }

    int MouseX()
    {
        return mouseState.Position.X;
    }

    int MouseY()
    {
        return mouseState.Position.Y;
    }

    bool KeyDown(irr::EKEY_CODE keyCode) const
    {
        if(keyStates[keyCode] == DOWN || keyStates[keyCode] == PRESSED)
        {   
            return true;
        }
        else
        {
            return false;
        }
    }

    bool KeyPressed(irr::EKEY_CODE keyCode) const
    {
        if(keyStates[keyCode] == PRESSED)
        {
            return true;
        }
    }

    bool KeyUp(irr::EKEY_CODE keyCode) const
    {
        if(keyStates[keyCode] == UP || keyStates[keyCode] == RELEASED)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    bool KeyReleased(irr::EKEY_CODE keyCode) const
    {
        if(keyStates[keyCode] == RELEASED)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    bool ButtonDown(MOUSE_BUTTON_ENUM buttonCode) const
    {
        if(mouseState.Button[buttonCode] == DOWN || mouseState.Button[buttonCode] == PRESSED)
        {   
            return true;
        }
        else
        {
            return false;
        }
    }

    bool ButtonPressed(MOUSE_BUTTON_ENUM buttonCode) const
    {
        if(mouseState.Button[buttonCode] == PRESSED)
        {
            return true;
        }
    }

    bool ButtonUp(MOUSE_BUTTON_ENUM buttonCode) const
    {
        if(mouseState.Button[buttonCode] == UP || mouseState.Button[buttonCode] == RELEASED)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    bool ButtonReleased(MOUSE_BUTTON_ENUM buttonCode) const
    {
        if(mouseState.Button[buttonCode] == RELEASED)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    void endEventProcess()
    {
        processState = ENDED;
    }

    void startEventProcess()
    {
        processState = STARTED;

        for(int i = 0; i < irr::KEY_KEY_CODES_COUNT; i++)
        {
            if(keyStates[i] == RELEASED)
            {
                keyStates[i] = UP;
            }

            if(keyStates[i] == PRESSED)
            {
                keyStates[i] = DOWN;
            }
        }

        for(int i = 0; i < 3; i++)
        {
            if(mouseState.Button[i] == RELEASED)
            {
                mouseState.Button[i] = UP;
            }

            if(mouseState.Button[i] == PRESSED)
            {
                mouseState.Button[i] = DOWN;
            }
        }

        mouseState.Wheel = 0.f;
    }

    EventReceiver()
    {
            for (irr::u32 i = 0; i < irr::KEY_KEY_CODES_COUNT; ++i)
                    keyStates[i] = UP;
    }

    


};

#endif