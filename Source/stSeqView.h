/* Copyright 2001, Arvid and Jonas Norberg
Distributed under the terms of the MIT Licence. */

#ifndef ST_SEQ_VIEW_H
#define ST_SEQ_VIEW_H

#include "song.h"
#include "stChannelView.h"
#include "stBpView.h"
#include "stdefs.h"

class stSeqView : public BView
{
public:
	stSeqView(Song *s, BRect frame);
	~stSeqView();

void	Pulse();

void	MessageReceived(BMessage *message); 
void	DrawAfterChildren(BRect r);
void	SetZoom(float PixelsPerPAL);
void	AttachedToWindow(void);
void	FrameResized(float w, float h);

void	Cut();
void	Copy();
void	Paste();

int		GetFocusPosition();

int		SemiFocus();

void	Update( bool all = false, float l = -1 , float r = -1);

void	NewChannel();
void	DeleteChannel();

void	AddBreakPoint(uint32 PAL, uint32 cmd);
void	DelBreakPoint(uint32 PAL);

void SetPos(uint32 p);

int ChannelHeight() {return height;}

private:
	stBPView *bpv;

	int semi_channel;
	int width;
	int height;
	Song *s;
	stChannelView *ps[CHN];
	float m_z;
	
	// Position marker
	uint32 pos;
};

#endif
