#include "EventReceiver.h"

bool EventReceiver::OnEvent(const irr::SEvent & event)
{
	bool eventprocessed = false;

        if(event.EventType == irr::EET_KEY_INPUT_EVENT)
        {   
            if(processState == STARTED)
            {
            
                if(event.KeyInput.PressedDown == true)
                {
                    if(keyState[event.KeyInput.Key] != DOWN)
                    {
                        keyState[event.KeyInput.Key] = PRESSED;
                    }
                    else
                    {
                        keyState[event.KeyInput.Key] = DOWN;
                    }
                }
                else
                {
                    if(keyState[event.KeyInput.Key] != UP)
                    {
                        keyState[event.KeyInput.Key] = RELEASED;
                    }
                }
            }

            eventprocessed = true;
        }

        if(event.EventType == irr::EET_MOUSE_INPUT_EVENT)
        {
            if(processState == STARTED)
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
                    if(mouseState.Button[MOUSE_BUTTON_LEFT] == UP || mouseState.Button[MOUSE_BUTTON_LEFT] == RELEASED)
                    {
                        mouseState.Button[MOUSE_BUTTON_LEFT] = PRESSED;
                    }
                    else
                    {
                        mouseState.Button[MOUSE_BUTTON_LEFT] = DOWN;
                    }
                }

                if(event.MouseInput.Event == irr::EMIE_LMOUSE_LEFT_UP)
                {
                    if(mouseState.Button[MOUSE_BUTTON_LEFT] != UP)
                    {
                        mouseState.Button[MOUSE_BUTTON_LEFT] = RELEASED;
                    }
                }

                if(event.MouseInput.Event == irr::EMIE_MMOUSE_PRESSED_DOWN)
                {
                    if(mouseState.Button[MOUSE_BUTTON_MIDDLE] == UP || mouseState.Button[MOUSE_BUTTON_MIDDLE] == RELEASED)
                    {
                        mouseState.Button[MOUSE_BUTTON_MIDDLE] = PRESSED;
                    }
                    else
                    {
                        mouseState.Button[MOUSE_BUTTON_MIDDLE] = DOWN;
                    }
                }

                if(event.MouseInput.Event == irr::EMIE_MMOUSE_LEFT_UP)
                {
                    if(mouseState.Button[MOUSE_BUTTON_MIDDLE] != UP)
                    {
                        mouseState.Button[MOUSE_BUTTON_MIDDLE] = RELEASED;
                    }
                }

                if(event.MouseInput.Event == irr::EMIE_RMOUSE_PRESSED_DOWN)
                {
                    if(mouseState.Button[MOUSE_BUTTON_RIGHT] == UP || mouseState.Button[MOUSE_BUTTON_RIGHT] == RELEASED)
                    {
                        mouseState.Button[MOUSE_BUTTON_RIGHT] = PRESSED;
                    }
                    else
                    {
                        mouseState.Button[MOUSE_BUTTON_RIGHT] = DOWN;
                    }
                }

                if(event.MouseInput.Event == irr::EMIE_RMOUSE_LEFT_UP)
                {
                    if(mouseState.Button[MOUSE_BUTTON_RIGHT] != UP)
                    {
                        mouseState.Button[MOUSE_BUTTON_RIGHT] = RELEASED;
                    }
                }
            }

            eventprocessed = true;
        }
        return eventprocessed;
}

float EventReceiver::Wheel() { return mouseState.Wheel; }

int EventReceiver::MouseX() { return mouseState.Position.X; }

int EventReceiver::MouseY() { return mouseState.Position.Y; }

irr::core::position2di EventReceiver::MousePosition() { return mouseState.Position; }

bool EventReceiver::KeyDown(irr::EKEY_CODE keyCode)
{
    if(keyState[keyCode] == DOWN || keyState[keyCode] == PRESSED) 
    	{ return true; }
    else 
    	{ return false; }
}

bool EventReceiver::KeyPressed(irr::EKEY_CODE keyCode)
{
    if(keyState[keyCode] == PRESSED)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool EventReceiver::KeyUp(irr::EKEY_CODE keyCode)
{
    if(keyState[keyCode] == UP || keyState[keyCode] == RELEASED)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool EventReceiver::KeyReleased(irr::EKEY_CODE keyCode)
{
    if(keyState[keyCode] == RELEASED)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool EventReceiver::ButtonDown(MOUSE_BUTTON_ENUM buttonCode)
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

bool EventReceiver::ButtonPressed(MOUSE_BUTTON_ENUM buttonCode)
{
    if(mouseState.Button[buttonCode] == PRESSED)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool EventReceiver::ButtonUp(MOUSE_BUTTON_ENUM buttonCode)
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

bool EventReceiver::ButtonReleased(MOUSE_BUTTON_ENUM buttonCode)
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

void EventReceiver::EndEventProcess()
{
    processState = ENDED;
}

void EventReceiver::StartEventProcess()
{
    processState = STARTED;

    for(int i = 0; i < irr::KEY_KEY_CODES_COUNT; i++)
    {
        if(keyState[i] == RELEASED)
        {
            keyState[i] = UP;
        }

        if(keyState[i] == PRESSED)
        {
            keyState[i] = DOWN;
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

EventReceiver::EventReceiver()
{
    for(irr::u32 i = 0; i < irr::KEY_KEY_CODES_COUNT; ++i)
    {
        keyState[i] = UP;
    }

    for(irr::u32 i = 0; i < 3; ++i)
    {
        mouseState.Button[i] = UP;
    }

    mouseState.Position.X = 0;
    mouseState.Position.Y = 0;
    mouseState.Wheel = 0.f;
}