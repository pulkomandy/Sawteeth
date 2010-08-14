/* Copyright 2001, Arvid and Jonas Norberg
Distributed under the terms of the MIT Licence. */

// Currently not connected width part

#ifndef ST_PART_SQUARE_H
#define ST_PART_SQUARE_H

#include <InterfaceKit.h>
#include "song.h"

class stPartSquare : public BView
{
public:
	stPartSquare(Song* s,Channel *chan, uint16 index, int8 *transp,uint8 *part,uint8 *volume,BRect rect);
	~stPartSquare();

	void	Update();
	void	KeyDown(const char *bytes, int32 numBytes);
	void	MakeFocus(bool focused = true);
	void	MouseDown(BPoint point);
	void	Draw(BRect frame);
	void	MessageReceived(BMessage *message);

void	SetZoom(float PixelsPerPAL);
void	SetStartPAL(uint32 start);
uint32	StartPAL();
void	SetPALs(uint32 l);

void	Mark(bool mark = true);
bool	IsMark();

private:
	Channel * ch;
	Song *s;
	int8 *t;
	uint8 *p;
	uint8 *v;

	uint16 m_index;
	bool focus;
	bool marked;

	bigtime_t entering_started;
	bool entering;	

	uint32 PALs;		// #of PAL-screens occupied by current part
	uint32 startPAL;
	float Zoom;
	font_height fh;
	uint32 height;
	uint32 width;
	uint8 r,g,b;
	
};

#endif
