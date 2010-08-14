/* Copyright 2001, Arvid and Jonas Norberg
Distributed under the terms of the MIT Licence. */

#include <stdio.h>
#include <string.h>

#include <Screen.h>
#include <MenuItem.h>
#include <NodeInfo.h>
#include <TextView.h>
#include <ScrollView.h>
#include <TranslationUtils.h>
#include <StorageKit.h>

#include "song.h"
#include "stMainWindow.h"
#include "stPartWindow.h"
#include "stAiffWindow.h"

#include "Source/data/preset_instruments.h"



stMainWindow::stMainWindow(BRect frame, txt &flat):
	BWindow(frame, "sawteeth Editor", B_DOCUMENT_WINDOW, B_CURRENT_WORKSPACE)
{

	file_name=new BPath();

	playing = false;
	zoom = .5;

	SetPulseRate(30000);

	
	memset(pw,0,PARTS*sizeof(stPartWindow*));
	memset(iw,0,INS*sizeof(stInstrumentWindow*));
	
	BPath path;
	find_directory(B_USER_DIRECTORY, &path);
	save = new BFilePanel(B_SAVE_PANEL,new BMessenger(this));
	save->SetPanelDirectory(path.Path());

	BMessage tmp(ST_EXPORT_AIFF_REQUESTED);
	aiff_save = new BFilePanel(B_SAVE_PANEL,new BMessenger(this));
	aiff_save->SetPanelDirectory(path.Path());
	aiff_save->SetMessage(&tmp);
	aiff_save->Window()->SetTitle("Export AIFF");

	tmp.what = ST_SAVE_RELEASE_REQUESTED;
	release_save = new BFilePanel(B_SAVE_PANEL,new BMessenger(this));
	release_save->SetPanelDirectory(path.Path());
	release_save->SetMessage(&tmp);
	release_save->Window()->SetTitle("Export Release");

// -------------------------

	BRect rect = BRect(0,0,0,0);
	BMenuItem *item;
	BMenu *sub;
	menu = new BMenuBar(rect, "MenuBar");
	sub = new BMenu("File");

	sub->AddItem(item=new BMenuItem("New Song", new BMessage(ST_NEW_SONG),'N'));
	item->SetTarget(be_app);
	sub->AddItem(item=new BMenuItem("Open Song…", new BMessage(ST_OPEN_SONG),'O'));
	sub->AddItem(new BMenuItem("Optimize Song", new BMessage(ST_OPTIMIZE),0));
	item->SetTarget(be_app);
	sub->AddItem(new BMenuItem("Close Song", new BMessage(B_CLOSE_REQUESTED),'W'));
	sub->AddSeparatorItem();
	sub->AddItem(save_item=new BMenuItem("Save Song", new BMessage(ST_SAVE_SONG),'S'));
	sub->AddItem(new BMenuItem("Save Song As…", new BMessage(ST_SAVE_SONG_AS),0));
	sub->AddItem(new BMenuItem("Export Release…", new BMessage(ST_SAVE_RELEASE_AS),0));
	sub->AddItem(new BMenuItem("Export AIFF…", new BMessage(ST_EXPORT_AIFF),0));
	sub->AddSeparatorItem();
	sub->AddItem(item=new BMenuItem("About…", new BMessage(B_ABOUT_REQUESTED),'A'));
	item->SetTarget(be_app);
	sub->AddSeparatorItem();
	sub->AddItem(item=new BMenuItem("Quit", new BMessage(B_CLOSE_REQUESTED),'Q'));
	item->SetTarget(be_app);
	menu->AddItem(sub);

	sub=new BMenu("Let The Music");
	play_item = new BMenuItem("Play from Focus", new BMessage(ST_PLAY_START),' ');
	sub->AddItem(play_item);
	sub->AddItem(new BMenuItem("Continue", new BMessage(ST_PLAY_CONT),'P'));
	menu->AddItem(sub);

	sub=new BMenu("Edit");
	sub->AddItem(new BMenuItem("Cut", new BMessage(ST_Cut),'X'));
	sub->AddItem(new BMenuItem("Copy", new BMessage(ST_Copy),'C'));
	sub->AddItem(new BMenuItem("Paste", new BMessage(ST_Paste),'V'));
	menu->AddItem(sub);

	sub=new BMenu("View");
	sub->AddItem(new BMenuItem("Zoom In", new BMessage(ST_ZoomIn),'+'));
	sub->AddItem(new BMenuItem("Zoom Out", new BMessage(ST_ZoomOut),'-'));
	sub->AddItem(new BMenuItem("Zoom Normal", new BMessage(ST_ZoomNormal),0));
	menu->AddItem(sub);

	sub=new BMenu("Channel");
	sub->AddItem(item = new BMenuItem("New", new BMessage(ST_NEW_CHANNEL),0));
	sub->AddItem(item = new BMenuItem("Delete", new BMessage(ST_DELETE_CHANNEL),0));
	menu->AddItem(sub);
	
	part_menu=new BMenu("Part");
	part_menu->AddItem(new BMenuItem("New", new BMessage(ST_NEW_PART),0));
	part_menu->AddItem(dup_part_menu = new BMenu("Duplicate"));
	part_menu->AddSeparatorItem();
	menu->AddItem(part_menu);

	BMenu *bm;
	instrument_menu=new BMenu("Instrument");
	instrument_menu->AddItem(bm = new BMenu("New"));
	instrument_menu->AddItem(dup_ins_menu = new BMenu("Duplicate"));
	bm->AddItem(item = new BMenuItem("Open…",new BMessage(ST_NEW_INS_OPEN)));
	item->SetEnabled(false);
	bm->AddSeparatorItem();
	bm->AddItem(new BMenuItem("Empty", new BMessage(ST_NEW_INS)));
	bm->AddItem(new BMenuItem("BassDrum",new BMessage(ST_NEW_INS)));
	bm->AddItem(new BMenuItem("SnareDrum",new BMessage(ST_NEW_INS)));
	bm->AddItem(new BMenuItem("HiHat",new BMessage(ST_NEW_INS)));
	bm->AddItem(new BMenuItem("Bass",new BMessage(ST_NEW_INS)));
	instrument_menu->AddSeparatorItem();
	menu->AddItem(instrument_menu);

	AddChild(menu);

// --------------------

	file_name->SetTo(flat.FileName());

	song=new stsp(flat);

	if (flat.IsMemory())save_item->SetEnabled(false);

	status_t temp;
	if ((temp = song->InitCheck()) != JNG_OK){
		init = temp;
		return;
	}else init = JNG_OK;
	
	rect = menu->Bounds();
	float hi = rect.bottom;
	rect = frame;

	rect.OffsetTo(0,0);
	rect.left = 0;
	rect.top += hi + 1;
//	rect.right -= B_V_SCROLL_BAR_WIDTH;
	rect.bottom -= B_H_SCROLL_BAR_HEIGHT;

	st_seq_view = new stSeqView(song,rect);
	st_scroll_view = new BScrollView("SawTeethView", st_seq_view, B_FOLLOW_ALL_SIDES,B_WILL_DRAW, true, false, B_NO_BORDER);
	AddChild(st_scroll_view	);

	mixwin = 0;
	ConstructMixWin();

	UpdateTitle();
	BuildPartMenu();
	BuildInstrumentMenu();


}

status_t stMainWindow::InitCheck(){	return init;};

stMainWindow::~stMainWindow()
{
	for (uint32 c = 0 ; c < PARTS ; c++){
		if (pw[c]){
//			fprintf(stderr,"removing pw[%d], (%p)",c,pw[c]);
			pw[c]->Lock();
			pw[c]->Quit();
//			fputs(" ok\n",stderr);
		}
	}

	for (uint32 c = 0 ; c < INS ; c++){
		if (iw[c]){
//			fprintf(stderr,"removing iw[%d], (%p)",c,pw[c]);
			iw[c]->Lock();
			iw[c]->Quit();
//			fputs(" ok\n",stderr);
		}
	}
	
	song->Stop();
	delete song;
	delete save;
	delete release_save;
	delete aiff_save;
	delete file_name;
	
	if (init == JNG_OK){
		mixwin->Lock();
		mixwin->Quit();
	}
}


Song *stMainWindow::GetSong()
{
	return (Song*)song;
}

#define H 17
#define H2 (H*2+10)
void stMainWindow::ConstructMixWin()
{
	if (mixwin){
		mixwin->Lock();
		mixwin->Quit();
	}
	
	BRect r(400,10,700,57+H2*song->ChannelCount());
	mixwin = new BWindow(r, "Controls",B_FLOATING_WINDOW_LOOK, B_FLOATING_SUBSET_WINDOW_FEEL ,
		B_NOT_ZOOMABLE | B_NOT_CLOSABLE | B_NOT_RESIZABLE | B_ASYNCHRONOUS_CONTROLS);

// BBox
	r.OffsetTo(0,0);
	r.bottom += 1;
	r.right += 1;

	BBox *box = new BBox(r);
	mixwin->AddChild(box);

// FIXA TEXTCONTROLS
	BTextControl *tc = new BTextControl(BRect(5,5,295,25),"song","Songname:",
		song->Name(),new BMessage(ST_SONGNAME_CHANGED));
	tc->SetTarget(this,this);
	tc->SetDivider(70);
	box->AddChild(tc);
	
	tc = new BTextControl(BRect(5,25,295,45),"author","Author / Info:",song->Author(),
		new BMessage(ST_AUTHOR_CHANGED));
	tc->SetTarget(this,this);
	tc->SetDivider(70);
	box->AddChild(tc);

// FIXA MIXER
	for (uint16 c = 0 ; c < song->ChannelCount() ; c++){

		//mute-knapp
		BMessage *msg = new BMessage(ST_MIXER_MUTE);
		msg->AddInt16("index",c);
		BCheckBox *checkbox;
		box->AddChild( checkbox = new BCheckBox(BRect(3,58+H2*c,20,70+H2*c),"mute",0,msg) );
		checkbox->SetTarget(this);
		
		const rgb_color fillc = { 255, 100, 40, 0};
		// LEFT
		msg = new BMessage(ST_MIXER);
		msg->AddInt16("index",c);
		msg->AddBool("left",true);
		BSlider *slide = new BSlider(
			BRect(20,50+H2*c,295,50+H2*c +(H)-1),
			"Amp", 0,
			msg,
			0, 255 );

		slide->UseFillColor(true, &fillc);
		slide->SetHashMarkCount(11);
		slide->SetHashMarks(B_HASH_MARKS_BOTTOM);
		slide->SetValue(song->GetChannel(c)->left);
 		slide->SetTarget(this);
		box->AddChild(slide);
		mw[c*2] = slide;
		// RIGHT

		msg = new BMessage(ST_MIXER);
		msg->AddInt16("index",c);
		msg->AddBool("left",false);
		slide = new BSlider(
			BRect(20,50+H2*c + H,295,50+H2*c + H2-1),
			"Amp", 0,
			msg,
			0, 255 );

		slide->UseFillColor(true, &fillc);
		slide->SetHashMarkCount(11);
		slide->SetHashMarks(B_HASH_MARKS_TOP);
		slide->SetValue(song->GetChannel(c)->right);
 		slide->SetTarget(this);
		box->AddChild(slide);
		mw[c*2+1] = slide;

	}
	
	mixwin->AddToSubset(this);
	mixwin->Show();
	
	Activate();
}

void stMainWindow::BuildPartMenu()
{
	while (part_menu->CountItems()>3) delete part_menu->RemoveItem(3);
	while (dup_part_menu->CountItems()>0) delete dup_part_menu->RemoveItem((int32)0);

	Part *part;
	BMessage *message;
	for (uint16 i=0;i<song->PartCount();i++)
	{
		part=song->GetPart(i);
		char *tmp = new char[strlen(part->name)+5];
		sprintf(tmp,"%d: %s",i,part->name);
		message=new BMessage(ST_EDIT_PART);
		message->AddInt32("index",i);

		part_menu->AddItem(new BMenuItem( tmp ,message,0));

		message=new BMessage(ST_DUP_PART);
		message->AddInt32("index",i);
		dup_part_menu->AddItem(new BMenuItem(tmp,message,0));
		delete tmp;
	}
}

void stMainWindow::BuildInstrumentMenu()
{
	while (instrument_menu->CountItems()>3) delete instrument_menu->RemoveItem(3);
	while (dup_ins_menu->CountItems()>0) delete dup_ins_menu->RemoveItem((int32)0);

	Ins *instrument;
	BMessage *message;
	for (uint16 i=1;i<song->InsCount();i++)
	{
		instrument=song->GetIns(i);
		char *tmp = new char[strlen(instrument->name)+4];
		sprintf(tmp,"%x: %s",i,instrument->name);
		message=new BMessage(ST_EDIT_INS);
		message->AddInt32("index",i);
		instrument_menu->AddItem(new BMenuItem(tmp,message,0));

		message=new BMessage(ST_DUP_INS);
		message->AddInt32("index",i);
		dup_ins_menu->AddItem(new BMenuItem(tmp,message,0));
		delete tmp;
	}
}

void stMainWindow::UpdateTitle()
{
	char *title =new char[strlen(song->Name())+strlen(song->Author())+20];
	strcpy(title,song->Name());
	strcat(title," by ");
	strcat(title,song->Author());
	SetTitle(title);
//	SetTitle(song->name);
	delete title;
}

void stMainWindow::UnmuteAll()
{
	for (int c = 0 ; c < song->ChannelCount() ; c++)
	{
			mixwin->Lock();

			song->GetChannel(c)->left = mw[c*2]->Value();
			song->GetChannel(c)->right = mw[c*2+1]->Value();
			mw[c*2]->SetEnabled(true);
			mw[c*2+1]->SetEnabled(true);

			mixwin->Unlock();
	}
}

void stMainWindow::MessageReceived(BMessage *message)
{
	uint32 type;
	int32 count;
//fprintf(stderr,"!!!%c%c%c%c\n",message->what >> 24,(message->what >> 16) & 255,(message->what >> 8) & 255,message->what & 255);

	switch(message->what)
	{
	case ST_MIXER_MUTE:{

		uint16 tmp = message->FindInt16("index");
		uint32 val = message->FindInt32("be:value");

		mixwin->Lock();

		if (tmp >= song->ChannelCount()) return;
		if (val)
		{
			song->GetChannel(tmp)->left = 0;
			song->GetChannel(tmp)->right = 0;
		}
		else
		{
			song->GetChannel(tmp)->left = mw[tmp*2]->Value();
			song->GetChannel(tmp)->right = mw[tmp*2+1]->Value();
		}

		mw[tmp*2]->SetEnabled(!val);
		mw[tmp*2+1]->SetEnabled(!val);

		mixwin->Unlock();
		
	} break;
	
	case ST_NEW_CHANNEL:{
		UnmuteAll();
		st_seq_view->NewChannel();
		ConstructMixWin();
	} break;

///////////////////////////////////////// ta bort kanal (semifult)
	case ST_DELETE_CHANNEL:{
		if (1 >= song->ChannelCount()) return;
		if (!st_seq_view->RemoveSelf()) return;
		if (!st_scroll_view->RemoveSelf()) return;

		UnmuteAll();

		delete st_seq_view;
		delete st_scroll_view;		
		printf("RemoveCh == %d\n",st_seq_view->SemiFocus());

		song->RemoveChannel(st_seq_view->SemiFocus());

		BRect rect = menu->Bounds();
		float hi = rect.bottom;
		rect = Bounds();

		rect.left = 0;
		rect.top += hi + 1;
		rect.bottom -= B_H_SCROLL_BAR_HEIGHT;

		st_seq_view = new stSeqView(song,rect);
		st_seq_view->SetZoom(zoom);
		AddChild(
			st_scroll_view = new BScrollView(
				"SawTeethView", st_seq_view, B_FOLLOW_ALL_SIDES,
				B_WILL_DRAW, true, false, B_NO_BORDER
			)
		);
		
		ConstructMixWin();
	} break;
/////////////////////////////////////////
	
	case ST_SONGNAME_CHANGED:{
			BTextControl *tc;
			if (B_OK != message->FindPointer("source",(void**)&tc)) return;
			song->SetName(tc->Text());
			UpdateTitle();
	} break;

	case ST_AUTHOR_CHANGED:{
			BTextControl *tc;
			if (B_OK != message->FindPointer("source",(void**)&tc)) return;
			song->SetAuthor(tc->Text());
			UpdateTitle();
	} break;

	case ST_MIXER:{
			uint16 tmp = message->FindInt16("index");
			bool left = message->FindBool("left");
			uint32 val = message->FindInt32("be:value");
//			fprintf(stderr,"%d:%ld\n",tmp,val);

			if (tmp >= song->ChannelCount()) return;
			if (left)
				song->GetChannel(tmp)->left = val;
			else
				song->GetChannel(tmp)->right = val;
	} break;

	case B_SIMPLE_DATA:
		message->GetInfo("refs", &type, &count); 
		if ( count>0 && type == B_REF_TYPE )
		{
			DetachCurrentMessage();
			message->what = B_REFS_RECEIVED;
			be_app->PostMessage(message);
		}
		break;

	case ST_PPLAY_START:
		if (playing){
			play_item->SetLabel("Play from Focus"); // fixa namn mm.
			song->Stop();
		}else{
			play_item->SetLabel("Stop"); // fixa namn mm.
			uint16 tmp = message->FindInt16("part");
			uint16 tmpstep = message->FindInt16("step");
//			fprintf(stderr,"Playing part %d(%d)\n",tmp,tmpstep);
			song->UsePartPlay(true);
			song->SetPartToPlay(tmp,tmpstep);
			song->Start();
		}
		playing = ! playing;
		break;

	case ST_PLAY_START:
		if (playing){
			play_item->SetLabel("Play from Focus"); // fixa namn mm.
			song->Stop();
		}else{
			play_item->SetLabel("Stop"); // fixa namn mm.
			int tmp = st_seq_view->GetFocusPosition();
//			fprintf(stderr,"Playing from pal(%d)\n",tmp);
			song->SetPos(tmp);
			song->UsePartPlay(false);
			song->Start();
		}
		playing = ! playing;
		break;

	case ST_PLAY_CONT:
		if (!playing){
			play_item->SetLabel("Stop"); // fixa namn mm.
			playing = true;
			song->Start();
		}
		break;

	case ST_OPTIMIZE:
		// stäng alla fönster för säkerhets skull...
		if (playing){
			play_item->SetLabel("Play from Focus"); // fixa namn mm.
			song->Stop();
		}
		if (JNG_OK == song->Optimize()){
			for (uint32 c = 0 ; c < PARTS ; c++) if (0 != pw[c]){ pw[c]->Lock(); pw[c]->Quit(); pw[c] = 0; }
			for (uint32 c = 0 ; c < INS ; c++) if (0 != iw[c]){ iw[c]->Lock(); iw[c]->Quit(); iw[c] = 0; }
			BuildPartMenu();
			BuildInstrumentMenu();
			st_seq_view->Update(true);
		}
		break;
	
	case ST_SAVE_SONG_AS:{
		save->Show();
	} break;
		
	case B_SAVE_REQUESTED:
	{
		const char *filename;
		entry_ref ref;
		message->FindString("name",&filename);
		message->FindRef("directory",&ref);

		BEntry temp(&ref);
		file_name->SetTo(&temp);
		file_name->Append(filename);
		save_item->SetEnabled(true);
	}	// OBS. här skall ingen break finnas!!!
	case ST_SAVE_SONG:
		UnmuteAll();
		song->Save((char *)file_name->Path(),false);	// textmode
//		song->Save((char *)file_name->Path(),true);		// binmode
		SetFileType((char *)file_name->Path());
	break;

	case ST_SAVE_RELEASE_AS:{
		release_save->Show();
	} break;
	case ST_SAVE_RELEASE_REQUESTED:{
		UnmuteAll();
		const char *filename;
		entry_ref ref;
		message->FindString("name",&filename);
		message->FindRef("directory",&ref);
		BEntry temp(&ref);
		BPath path(&temp);
		path.Append(filename);

		Song *temps = new Song(*song);
		temps->Optimize();
		temps->Save((char *)path.Path(),true,true);
		SetFileType((char *)path.Path());
		delete temps;
	}break;

	case ST_EXPORT_AIFF:{
		aiff_save->Show();
	} break;
	case ST_EXPORT_AIFF_REQUESTED:{
		const char *filename;
		entry_ref ref;
		message->FindString("name",&filename);
		message->FindRef("directory",&ref);
		BDirectory *bd = new BDirectory(&ref);
		BFile f(bd,filename,B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE );
				
		(new stAiffWindow(*song,f))->Show();
		delete bd;
	}break;


	case ST_Cut:
		st_seq_view->Cut();
		break;
	case ST_Copy:
		st_seq_view->Copy();
		break;
	case ST_Paste:
		st_seq_view->Paste();
		break;
	case ST_ZoomIn:
		zoom *= 1.1;
		st_seq_view->SetZoom(zoom);
		break;
	case ST_ZoomOut:
		zoom *= 0.9;
		st_seq_view->SetZoom(zoom);
		break;
	case ST_ZoomNormal:
		zoom = 1.0;
		st_seq_view->SetZoom(zoom);
		break;
	case ST_NEW_PART:{
		int temp = song->NewPart();
		if (0 == temp) { fputs("maxparts-error\n",stderr); return;}
		pw[temp] = new stPartWindow(this,BPoint(60,60), song->GetPart(temp), song);
		pw[temp]->AddToSubset(this);
		pw[temp]->Show();
		BuildPartMenu();
	}break;
	
	case ST_EDIT_PART:{
		int temp = message->FindInt32("index");
		if (pw[temp] == NULL) pw[temp] = new stPartWindow(this,BPoint(60,60), song->GetPart(temp), song);
		pw[temp]->AddToSubset(this);
		pw[temp]->Show();
		pw[temp]->Activate();

	}break;	

	case ST_DUP_PART:{
		int ind = message->FindInt32("index");
		if (ind < 0 ) return;
		
		int temp = song->NewPart( song->GetPart( ind ) );
		if (0 == temp) { fputs("maxpart-error\n",stderr); return;}
		pw[temp] = new stPartWindow(this,BPoint(60,60), song->GetPart(temp), song);
		pw[temp]->AddToSubset(this);
		pw[temp]->Show();
		BuildPartMenu();
	} break;

	case ST_DUP_INS:{
		int ind = message->FindInt32("index");
		if (ind < 0 ) return;
		
		int temp = song->NewIns( ind );
		if (0 == temp) { fputs("maxins-error\n",stderr); return;}
		iw[temp] = new stInstrumentWindow(temp,this,BPoint(60,60), song->GetIns(temp));
		iw[temp]->AddToSubset(this);
		iw[temp]->Show();
		BuildInstrumentMenu();
	} break;

	case ST_NEW_INS:{
		int ind = message->FindInt32("index") - 2;
		if (ind < 0 ) return;
		
		
		char *data;
		int size;
		switch ( ind ){
			case 0: data = preset_ins_empty;	size = sizeof(preset_ins_empty);break;
			case 1: data = preset_ins_bd;		size = sizeof(preset_ins_bd);	break;
			case 2: data = preset_ins_snare;	size = sizeof(preset_ins_snare);break;
			case 3: data = preset_ins_hh;		size = sizeof(preset_ins_hh);	break;
			case 4: data = preset_ins_bass;		size = sizeof(preset_ins_bass);	break;
			default: return;
		}
		
		txt t(data,size);
		int temp = song->NewIns(t);
		if (0 == temp) { fputs("maxins-error\n",stderr); return;}
		song->GetIns(temp)->SetName(t.NextString());
		iw[temp] = new stInstrumentWindow(temp,this,BPoint(60,60), song->GetIns(temp));
		iw[temp]->AddToSubset(this);
		iw[temp]->Show();
		BuildInstrumentMenu();
	}break;
	
	case ST_EDIT_INS:{
		int temp = message->FindInt32("index");
		if (iw[temp] == NULL)
			iw[temp] = new stInstrumentWindow(temp,this,BPoint(60,60), song->GetIns(temp));
		iw[temp]->AddToSubset(this);
		iw[temp]->Show();
	}break;


	case ST_PART_CHANGED : {
		BuildPartMenu();
		st_seq_view->Update(true);
	}break;

	case ST_INS_CHANGED : {
		BuildInstrumentMenu();
	}break;
	
	case ST_ADD_BP : {
		int32 cmd = message->FindInt32("cmd");
		int32 PAL = message->FindInt32("PAL");
		st_seq_view->AddBreakPoint(PAL,cmd);
	}break;

	case ST_DEL_BP : {
		int32 PAL = message->FindInt32("PAL");
		st_seq_view->DelBreakPoint(PAL);
	}break;

	default:
		BWindow::MessageReceived(message);
	}
}


void stMainWindow::SetFileType(const char *filename)
{
		BFile file(filename,B_READ_WRITE);
//		BFile file(filename,B_READ_ONLY);
		char *buffer="audio/x-sawteeth";
		int tmp = file.WriteAttr("BEOS:TYPE",B_STRING_TYPE,0,buffer,strlen(buffer)+1);
}


void stMainWindow::ClosePartWin(Part *part)
{
	pw[part - song->GetPart(0)] = NULL;
}

void stMainWindow::CloseInsWin(Ins *instrument_win)
{
	iw[instrument_win - song->GetIns(0)] = NULL;
}
