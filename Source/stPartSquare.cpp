/* Copyright 2001, Arvid and Jonas Norberg
Distributed under the terms of the MIT Licence. */

#include <stdio.h> // remove this line
#include "stPartSquare.h"
#include "stChannelView.h"
#include "song.h"
#include "stdefs.h"

stPartSquare::stPartSquare(Song* song,Channel * chan, uint16 index ,int8 *transp,uint8 *part,uint8 *volume, BRect rect):
	BView(rect,"partsquare", B_FOLLOW_NONE, B_WILL_DRAW )
{
	ch = chan;
	s = song;
	p = part;
	t =transp;
	v = volume;
	
	m_index = index;
	
	be_plain_font->GetHeight(&fh);
//	printf("%f, %f, %f\n", fh.ascent, fh.descent, fh.leading);

//	fprintf(stderr,"stPartSquare::stPartSquare()%p, %p, %p\n",s,p,t);
	SetFont(be_plain_font);
	
	r = 197;
	g = 197;
	b = 197;

	SetViewColor(r,g,b);
	SetLowColor(r,g,b);
		

	SetZoom(0.5);
	height = (int)(rect.bottom - rect.top);
	
	PALs = 1;
	startPAL = 0;

	width = (int)(Frame().right - Frame().left);


	focus = false;
	marked = false;
	
}

stPartSquare::~stPartSquare()
{
//	puts("stPartSquare::~stPartSquare()");
}
	
void	stPartSquare::SetZoom(float PixelsPerPAL)
{
	Zoom = PixelsPerPAL;
//	puts("stPartSquare::SetZoom()");

	width = (int)(Frame().right - Frame().left);
	
	Update();

}

void	stPartSquare::Update()
{
	int tmp = height - *t + 1;
	if ( tmp < 0 ) tmp = 0;   
	if ( tmp > (int)(2 * height+1)) tmp = (2 * height +1);

	SetStartPAL(startPAL);
	SetPALs(PALs);
	
	Invalidate();
}

void	stPartSquare::SetStartPAL(uint32 start)
{
	int tmp = height - *t + 1;
	if ( tmp < 0 ) tmp = 0;
	if ( tmp > (int)(2 * height) ) tmp = 2 * height;

	startPAL = start;
	MoveTo(		(int)(Zoom * startPAL), tmp);
}

uint32	stPartSquare::StartPAL()
{
	return startPAL;
}

void	stPartSquare::SetPALs(uint32 l)
{
	PALs = l;
	ResizeTo(	(int)(Zoom * (startPAL + PALs)) - (int)(Zoom * startPAL) - 1, height);
	width = (int)(Frame().right - Frame().left);
}

#define DIFF 25
void	stPartSquare::Draw(BRect )
{
//	puts("stPartSquare::Draw()");
	if (marked){
			SetHighColor(0,0,0);
			FillRect(BRect(0,0,width,height));
	
			SetHighColor(255,255,255);
			StrokeRect(BRect(0,0,width,height));
			SetLowColor(0,0,0);
	}else
		if (focus){
			SetHighColor(255,255,255);
			FillRect(BRect(0,0,width,height));
	
			SetHighColor(0,0,0);
			StrokeRect(BRect(0,0,width,height));
			SetLowColor(255,255,255);
		}else{
			SetHighColor(r,g,b);
			FillRect(BRect(0,0,width,height));

			SetHighColor(r+DIFF,g+DIFF,b+DIFF);
			StrokeLine(BPoint(width-1,0), BPoint(0,0));
			StrokeLine(BPoint(0,height-1));
			SetHighColor(r-DIFF,g-DIFF,b-DIFF);
			StrokeLine(BPoint(1,height), BPoint(width-1,height));

			SetLowColor(r,g,b);
			SetHighColor(0,0,0);
		}
		
	MovePenTo(2,height - fh.descent);
	
	char temp[5]={0,0,0,0,0};
	sprintf(temp,"%d",*p);
	DrawString(temp);
	DrawString(":");
	sprintf(temp,"%d",*t);
	DrawString(temp);
	DrawString(":-");
	sprintf(temp,"%d",*v);
	DrawString(temp);
	DrawString(":");
	DrawString(s->GetPart(*p)->name);
	
	if (ch->lloop == m_index){
		SetHighColor( 255, 100, 40 );
		FillRect(BRect(0,0,3,3));
	}

	if (ch->rloop == m_index){
		SetHighColor( 40, 200, 100 );
		FillRect(BRect(width-3,height-3,width,height));
	}

}

void	stPartSquare::MakeFocus(bool focused)
{
	entering = false;
	BView::MakeFocus(focused);
	focus = focused;
	if ( focused )((stChannelView *)Parent())->MakeSemiFocus();
	Invalidate();
}

void	stPartSquare::MouseDown(BPoint)
{
	Window()->PostMessage('unmk',Parent()->Parent());
	MakeFocus();
	((stChannelView*)Parent())->FindSquare();
/*
	BMessage *msg = Window()->CurrentMessage();
	uint32 tmp = msg->FindInt32("clicks");
	if (tmp>1){
		BMessage *message=new BMessage(ST_EDIT_PART);
		message->AddInt32("index",*p);
		Window()->PostMessage(message);
	}
*/
}

void stPartSquare::MessageReceived(BMessage *message)
{
	Window()->DetachCurrentMessage();
	Window()->PostMessage(message);
}

void	stPartSquare::KeyDown(const char *bytes, int32 numBytes) 
{
	int32 mod;
//	Window()->CurrentMessage()->PrintToStream();
	Window()->CurrentMessage()->FindInt32("modifiers",&mod);
//	printf("mod == %x\n",mod);
	
	if ( numBytes == 1 ) {
		// kolla om det är en siffra
		if (bytes[0] >= '0' && bytes[0] <='9'){
			BMessage * mess = new BMessage('setp');

			if (!entering || (system_time() > entering_started + 1000000)){
				entering_started = system_time();
				mess->AddInt16("part",bytes[0]-'0');
			}else{
				mess->AddInt16("part",(bytes[0]-'0') + (*p * 10));
			}
			Window()->PostMessage(mess,Parent());
			entering = true;
			return;
		}
		
		entering = false;

		switch ( bytes[0] ) { 
			case B_ENTER:{
				BMessage *message=new BMessage(ST_EDIT_PART);
				message->AddInt32("index",*p);
				Window()->PostMessage(message);
			} break;
			
			case 'l':{
				ch->lloop = m_index;
				((stChannelView*)Parent())->Update();
			} break;

			case 'r':{
				ch->rloop = m_index;
				((stChannelView*)Parent())->Update();
			} break;
			
			case B_RIGHT_ARROW:
				if (mod & 0x1004)
					Window()->PostMessage('ch_r',Parent());
				else
					if (mod & 0x101){
						Window()->PostMessage('mk_r',Parent());
					}else
					Window()->PostMessage('mv_r',Parent());
			break; 
			
			case B_LEFT_ARROW: 
				if (mod & 0x1004)
					Window()->PostMessage('ch_l',Parent());
				else
					if (mod & 0x101)
						Window()->PostMessage('mk_l',Parent());
					else
						Window()->PostMessage('mv_l',Parent());
			break; 
			
			case B_UP_ARROW:
				if (mod & 0x1004) Window()->PostMessage('tr_u',Parent());
				else{
					BMessage *mess = new BMessage('mv_u');
					mess->AddInt32("startPAL",startPAL);
					Window()->PostMessage(mess,Parent()->Parent());
				}
			break; 

			case B_DOWN_ARROW: 
				if (mod & 0x1004) Window()->PostMessage('tr_d',Parent());
				else{
					BMessage *mess = new BMessage('mv_d');
					mess->AddInt32("startPAL",startPAL);
					Window()->PostMessage(mess,Parent()->Parent());
				}
			break; 
		} 
	} 
}

void	stPartSquare::Mark(bool mark)
{
	marked = mark;
}

bool	stPartSquare::IsMark()
{
	return marked;
}
