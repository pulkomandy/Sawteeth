/* Copyright 2001, Arvid and Jonas Norberg
Distributed under the terms of the MIT Licence. */

#include <Application.h>
#include <AppFileInfo.h>
#include <File.h>
#include <Roster.h>
#include <Screen.h>
#include <Window.h>
#include <stdio.h>

#include "stAboutView.h"

stBmView::stBmView(BRect rect, BBitmap *bm):
	BView(rect,"name", B_FOLLOW_NONE , B_WILL_DRAW )
{
	m_bm = bm;
	SetViewColor(B_TRANSPARENT_COLOR);
	SetLowColor(0,0,0);
	SetHighColor(255,255,255);

	app_info info;
	be_app->GetAppInfo(&info);
	BFile file(&info.ref, B_READ_ONLY);
	BAppFileInfo appMime(&file);
	version_info versionInfo;
	if (appMime.GetVersionInfo(&versionInfo, B_APP_VERSION_KIND) == B_OK) {
		if (versionInfo.major == 0 && versionInfo.middle == 0
			&& versionInfo.minor == 0) {
			return;
		}
	
		sprintf(verstr,"%" B_PRId32 ".%" B_PRId32 ".%" B_PRId32,
			versionInfo.major, versionInfo.middle, versionInfo.minor);
	}

}
void stBmView::Draw(BRect r)
{
	DrawBitmap(m_bm, r, r);
	DrawString(verstr,	BPoint(1, 10));
}
void stBmView::MouseDown(BPoint)
{
	Window()->PostMessage(B_QUIT_REQUESTED);
}

stAboutView::stAboutView(BRect rect, int halfseconds):
	BView(rect,"name", B_FOLLOW_NONE , B_WILL_DRAW | B_PULSE_NEEDED ),
	stop(halfseconds),
	count(0)
{
	if (stop > 50) stop = 50;
	m_rect = rect;

	SetViewColor(40,40,40);
	SetLowColor(40,40,40);
	SetHighColor(255,255,255);
}

void stAboutView::Draw(BRect)
{
	int y = 16;
	if (count < 12)
	{
		DrawString("Sound generation and Editor Code:",	BPoint(44,y)); y+=8;
		DrawString("Jonas Norberg",						BPoint(44,y)); y+=8; y+=8;
		DrawString("Additional Editor Coding:",			BPoint(44,y)); y+=8;
		DrawString("Arvid Norberg",						BPoint(44,y)); y+=8; y+=8;
		DrawString("© Jonas Norberg 2001",				BPoint(44,y)); y+=8;
	}else if (count < 30)
	{
		DrawString("Greets to:",						BPoint(44,y)); y+=8;
		DrawString("lazze, wix, steffo, maze, 0xEE, jhg, viggo, kegie",	BPoint(44,y)); y+=8;
		DrawString("dew, NTG, skaer, marwin, iceman, Björn Kalzén",BPoint(44,y)); y+=8; y+=8;
		DrawString("Do not miss my other software:",	BPoint(44,y)); y+=8;
		DrawString("http://www.acc.umu.se/~bedev/",		BPoint(44,y)); y+=8;
	}
}

void stAboutView::MouseDown(BPoint)
{
	Window()->PostMessage(B_QUIT_REQUESTED);
}

void stAboutView::Pulse()
{
	if (count >= stop) Window()->PostMessage(B_QUIT_REQUESTED);
	count ++;
	
	if (count == 12) Invalidate();
	if (count == 30) Invalidate();
}
