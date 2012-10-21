#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include <irrlicht.h>

class EventReceiver : public irr::IEventReceiver
{
public:
        virtual bool OnEvent(const irr::SEvent& event)
        {
                if (event.EventType == irr::EET_KEY_INPUT_EVENT)
                        KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;

                return false;
        }

        virtual bool IsKeyDown(irr::EKEY_CODE keyCode) const
        {
                return KeyIsDown[keyCode];
        }
        
        EventReceiver()
        {
                for (irr::u32 i=0; i<irr::KEY_KEY_CODES_COUNT; ++i)
                        KeyIsDown[i] = false;
        }

private:
        bool KeyIsDown[irr::KEY_KEY_CODES_COUNT];
};

#endif