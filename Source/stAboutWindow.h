/* Copyright 2001, Arvid and Jonas Norberg
Distributed under the terms of the MIT Licence. */

#ifndef _ABOUT_WINDOW_H
#define _ABOUT_WINDOW_H

#include <Window.h>
#include "stAboutView.h"

class stAboutWindow : public BWindow
{
public:
	stAboutWindow(int halfseconds = 4);
	~stAboutWindow();
private:
	stAboutView *v;
	BBitmap *bm;
};

#endif // _ABOUT_WINDOW_H
