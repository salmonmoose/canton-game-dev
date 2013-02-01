#ifndef ICON_H
#define ICON_H

#include "irrlicht.h"
#include "engine.h"

class Icon
{
public:
	Icon();

	~Icon(){};

	virtual void Initialize();
	virtual void Update();

	Ability * mAbility;

	irr::EKEY_CODE * binding;

    irr::gui::IGUIImage * icon;
    irr::gui::IGUIStaticText * key;
    irr::gui::IGUIStaticText * coolDown;
};

#endif