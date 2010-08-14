/* Copyright 2001, Arvid and Jonas Norberg
Distributed under the terms of the MIT Licence. */

#ifndef _EXPORTAIFF_WINDOW_H
#define _EXPORTAIFF_WINDOW_H

#include <Window.h>

#include <File.h>
#include "song.h"
#include "stdefs.h"

class stAiffWindow : public BWindow
{
public:
	stAiffWindow(Song &copy_this, BFile &file);
	status_t InitCheck();
	~stAiffWindow();

	void Thread();
private:

	thread_id tid;
	status_t init;
	BFile f;
	Song *song;
	BStatusBar *pb;
};

#endif // _EXPORTAIFF_WINDOW_H
