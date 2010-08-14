/* Copyright 2001, Arvid and Jonas Norberg
Distributed under the terms of the MIT Licence. */

#ifndef _ST_PART_WINDOW_H_
#define _ST_PART_WINDOW_H_

#include <Window.h>
#include "stTrackerControl.h"
#include "stChannelView.h"
#include "song.h"

class stTrackerView;
class stMainWindow;

class stPartWindow:public BWindow
{
public:
	stPartWindow(stMainWindow *win, BPoint point,Part *part_to_edit, Song *song);
	~stPartWindow();
//	virtual bool QuitRequested();
	virtual void MessageReceived(BMessage *message);
private:
	void FullUpdate(int8 low, uint8 high);
	void UpdateTitle();
	stMainWindow *main_win;
	stTrackerControl *tracker_control;
	Song *s;
	Part *part;
	BTextControl *name_string;
};

#endif // _ST_PART_WINDOW_H_
