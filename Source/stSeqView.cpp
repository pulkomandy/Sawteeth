/* Copyright 2001, Arvid and Jonas Norberg
Distributed under the terms of the MIT Licence. */

#include <stdio.h>
#include <Message.h>

#include "stSeqView.h"
#include "stMainWindow.h"
#include "song.h"

stSeqView::stSeqView(Song *song, BRect frame) : 
	BView(frame, "Channel",  B_FOLLOW_ALL_SIDES, B_FRAME_EVENTS | B_PULSE_NEEDED | B_WILL_DRAW | B_DRAW_ON_CHILDREN)
{
//	fprintf(stderr,"stSeqView::stSeqView()\n");
	semi_channel = 0;
	s = song;
	SetViewColor(168,168,168);
	m_z = 1;
	font_height fh;
	be_plain_font->GetHeight(&fh);
//	printf("%f, %f, %f\n", fh.ascent, fh.descent, fh.leading);
	height = (int)(3 * (fh.ascent + fh.descent));

	bpv = new stBPView(BRect(0,0,frame.Width(),11), s);
	AddChild(bpv);


	for (int cnt=0 ; cnt<song->ChannelCount() ; cnt++){
		ps[cnt] = new stChannelView(s, s->GetChannel(cnt),
			BRect(0,12+cnt*height,300,11+(cnt+1)*height));
		AddChild(ps[cnt]);
	}
}

int stSeqView::SemiFocus()
{
	return semi_channel;
}

void stSeqView::FrameResized(float , float )
{
	Update();
}

void stSeqView::AttachedToWindow(void)
{
	Update(true);
}

void stSeqView::Update(bool all, float l , float r)
{
	width = 0;
	for (int cnt=0 ; cnt<s->ChannelCount() ; cnt++){
		if (all) ps[cnt]->Update();
		if (ps[cnt]->Width() > width){
			width = ps[cnt]->Width();
		}
	}

	int totalheight = (int)(height * s->ChannelCount());
	int range = (int)(totalheight - Frame().Height());
	if (range < 0 ) range = 0;
	BScrollBar *sb;
	if ((sb=ScrollBar(B_VERTICAL))!=NULL)
	{
		sb->SetProportion(Frame().Height()/totalheight);
		sb->SetRange(0,range);
		sb->SetSteps(height,Frame().Height());
	}

	range = (int)(width - Frame().Width());

	if (range < 0 ) range = 0;
	if ((sb=ScrollBar(B_HORIZONTAL))!=NULL)
	{
		sb->SetProportion(Frame().Width()/width);
		sb->SetRange(0,range);
		sb->SetSteps(16,Frame().Width());
	}
	
	// här borde tester ske
	if (l > -.5 && r > -.5){
		// these ( left & right ) are in parents coordinate-system
		float left = ConvertToParent(BPoint(l,0)).x;
		float right = ConvertToParent(BPoint(r,0)).x;

//		printf("%f - %f\n",left,right);

		if (left < 0.0){
			ScrollTo(l,0);
		}
		else if (right > Parent()->Bounds().right)
		{
			ScrollTo(r - Parent()->Bounds().right,0);
		}
//		if (ConvertToParent(entry_list[i+1]->ConvertToParent(BPoint(0,entry_height))).y>Parent()->Frame().Height())
//			ScrollTo(entry_list[i+1]->ConvertToParent(BPoint(0,entry_height)-BPoint(0,Parent()->Frame().Height()-4)));
	}

}

void stSeqView::Pulse()
{
	SetPos(s->GetPos());
}

stSeqView::~stSeqView()
{
//	fprintf(stderr,"stSeqView::~stSeqView()\n");
}

void stSeqView::MessageReceived(BMessage *message)
{
//puts("stSeqView::MessageReceived");
	uint32 startPAL;
	switch ( message->what ) {
		case ST_SEMI_FOCUS:{
			semi_channel = message->FindInt16("focus");
//			printf("semi_channel = %d\n", semi_channel);
		}break;
		
		case 'unmk':
			for (int c = 0 ; c < s->ChannelCount() ; c++) ps[c]->UnMark();
		break;
		
		case 'mv_u': {//	puts("seqview-mv-up");
			int c = semi_channel;
			if ( c < 1 || c >= s->ChannelCount() ) return;
			ps[c]->UnMark();
			if (B_OK != message->FindInt32("startPAL",(int32 *)&startPAL)) return;
			ps[c-1]->MakeFocusPAL(startPAL);
		} break; 

		case 'mv_d': {//	puts("seqview-mv-down");
			int c = semi_channel;
			if ( (c+1) >= s->ChannelCount()) return;
			ps[c]->UnMark();
			if (B_OK != message->FindInt32("startPAL",(int32 *)&startPAL)) return;
			ps[c+1]->MakeFocusPAL(startPAL);
		} break;

		default:
			BView::MessageReceived(message);
		break; 
	}

}


void stSeqView::AddBreakPoint(uint32 PAL, uint32 cmd)
{
	if ( semi_channel < 0 || semi_channel >= s->ChannelCount() ){
		printf("AddBreakPoint.. error semifocus=%d\n",semi_channel);
		return;
	}
	//printf("stSeqView cmd = %d'%c' PAL = %d\n",cmd,cmd,PAL);

	int basePAL = ps[semi_channel]->GetFocusPosition();
	s->NewBreakPoint(basePAL + PAL, cmd);
	bpv->Invalidate();
};

void stSeqView::DelBreakPoint(uint32 PAL)
{
	if ( semi_channel < 0 || semi_channel >= s->ChannelCount() ){
		printf("DelBreakPoint.. error semifocus=%d\n",semi_channel);
		return;
	}
	//printf("stSeqView cmd = %d'%c' PAL = %d\n",cmd,cmd,PAL);

	int basePAL = ps[semi_channel]->GetFocusPosition();
	s->RemoveBreakPoint(basePAL + PAL);
	bpv->Invalidate();
};


void stSeqView::SetZoom(float z)
{
	m_z = z;
	bpv->SetZoom(z);

	for (int c=0 ; c < s->ChannelCount() ; c++){
//fprintf(stderr,"SeqView.SetZoom(%d)\n",c);
		ps[c]->SetZoom(z);
	}

	Update();
}


void stSeqView::NewChannel()
{
	int tmp = s->NewChannel();
	if (tmp > 0){
		ps[tmp] = new stChannelView(s,s->GetChannel(tmp),BRect(0,6+tmp*height,300,6+((tmp+1)*height) -1));
		ps[tmp]->SetZoom(m_z);
		AddChild(ps[tmp]);
		Update();
	}
}


int	stSeqView::GetFocusPosition()
{
	int c = semi_channel;
	if (c < 0 || c >= s->ChannelCount()) return 0;
	return ps[c]->GetFocusPosition();
}



void stSeqView::Cut() // kör copy och tar bort...
{
	int c = semi_channel;
	if (c < 0 || c >= s->ChannelCount()) return;
	ps[c]->Cut();

	Update(true);
}

void stSeqView::Copy() // kopiera till clipboard
{
	int c = semi_channel;
	if (c < 0 || c >= s->ChannelCount()) return;
	ps[c]->Copy();
}

void stSeqView::Paste() // töm clipboard och insert i 
{
	int c = semi_channel;
	if (c < 0 || c >= s->ChannelCount()) return;
	ps[c]->Paste();
	
	Update(true);
}

// Draw after childen so the position marker is above them and thus, visible.
void stSeqView::DrawAfterChildren(BRect r)
{
	float start = pos*m_z;
	//if (r.left > start || r.right < start) return;
	SetHighColor(255,200,40, 128);
	SetDrawingMode(B_OP_ALPHA);
	FillRect( BRect( start, 0, start + 2 , Bounds().Height() ) );
}

void stSeqView::SetPos(uint32 p)
{
	if (pos == p) return;
	// TODO : only invalidate the needed area
	pos = p;
	Invalidate();
}
