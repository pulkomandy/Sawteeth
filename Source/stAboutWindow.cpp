/* Copyright 2001, Arvid and Jonas Norberg
Distributed under the terms of the MIT Licence. */

#include <Application.h>
#include <Screen.h>
#include <Window.h>
#include <DataIO.h>
#include <translation/TranslationUtils.h>

#include "stAboutWindow.h"
#include "stAboutView.h"

const uint8 about_bitmap [] = {
#include "data/about_bitmap.h"
};

stAboutWindow::stAboutWindow(int hseconds):
	BWindow(BRect(10,10,200,200),"About Win", B_NO_BORDER_WINDOW_LOOK ,
	B_FLOATING_APP_WINDOW_FEEL, B_NOT_RESIZABLE | B_NOT_ZOOMABLE )
{
	BMemoryIO *io = new BMemoryIO((void*)about_bitmap, sizeof(about_bitmap));
	bm = BTranslationUtils::GetBitmap(io);

	BScreen screen(this);
	BRect bigr = screen.Frame();
	BRect rect = bm->Bounds();
	MoveTo((bigr.right-rect.right) / 2, (bigr.bottom-rect.bottom) / 2);
	
	AddChild(new stBmView(rect,bm));
	
	BRect r2 = rect;
	r2.top = r2.bottom;
	r2.bottom += 80;
	AddChild(new stAboutView(r2, hseconds));

	ResizeTo(r2.right,r2.bottom);
}

stAboutWindow::~stAboutWindow()
{
	delete bm;
}

