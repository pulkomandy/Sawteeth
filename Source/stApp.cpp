/* Copyright 2001, Arvid and Jonas Norberg
Distributed under the terms of the MIT Licence. */

#include "stApp.h"
#include "stMainWindow.h"
#include "stAboutWindow.h"

#include <FilePanel.h>

uchar defsong [] = {
#include "Source/data/defaultsong.h"
};

stApp::stApp(const char *signature):BApplication(signature)
{
	dropped = false;
	(new stAboutWindow())->Show();
	SetPulseRate(1000000);

	InstallMimeType();
	
	BPath path;
	find_directory(B_USER_DIRECTORY, &path);
	load = new BFilePanel(B_OPEN_PANEL);
	load->SetPanelDirectory(path.Path());
}

stApp::~stApp()
{
	delete load;
}

void stApp::AboutRequested()
{
	(new stAboutWindow(100))->Show();
}

#include <stdio.h>
void stApp::ReadyToRun()
{
	if ( dropped ) return;
	txt t((char*)defsong,(int)sizeof(defsong));
	stMainWindow *win=new stMainWindow(BRect(10,29,450,150),t);
	win->Show();
	win->Activate();
}

void stApp::MessageReceived(BMessage *message)
{
	switch(message->what){
	case ST_OPEN_SONG:
		load->Show();
		break;
	case ST_NEW_SONG:
	{
		txt t((char*)defsong,(int)sizeof(defsong));
		stMainWindow *win = new stMainWindow(BRect(10,29,450,150),t);
		win->Show();
		win->Activate();
	}break;
	default:
		BApplication::MessageReceived(message);
	}
}

void stApp::Pulse()
{
	if(!IsLaunching() && CountWindows() <= WINDOWS_TO_IGNORE)
	{
		PostMessage(B_QUIT_REQUESTED);
	}
}

void stApp::RefsReceived(BMessage *message)
{
	uint32 type; 
	int32 count;
	entry_ref ref;
	BPath path;

	dropped = true;
	
	message->GetInfo("refs", &type, &count); 
	if ( type != B_REF_TYPE ) return; 

	for ( long i = --count; i >= 0; i-- ) { 
		if ( message->FindRef("refs", i, &ref) == B_OK ) { 
			BEntry entry(&ref, true);
			if ( entry.IsFile() && entry.GetPath(&path)==B_OK){

				txt t(path.Path());
				if (JNG_OK !=t.InitCheck()){
					BAlert *alert=new BAlert(
						"Not Valid","ERROR\n\nNot a valid file",
						"OK",0,0,B_WIDTH_AS_USUAL,B_STOP_ALERT);
					alert->Go();
				}
				stMainWindow *win = new stMainWindow(BRect(10,29,450,150),t);

				status_t temp = win->InitCheck();
				if (JNG_OK != temp) {
					char tempc[512];
					sprintf(tempc,
						"ERROR\n\nLoader version = %.2f < Song version = %.2f\n\nDownload newer software at:\nwww.acc.umu.se/~bedev/software/sawteeth",
						win->GetSong()->LoaderVersion(),win->GetSong()->Version());

					win->Quit();
					
					if (temp == JNG_ERROR_VERSION){
						BAlert *alert=new BAlert(
							"Not Valid",tempc,
							"OK",0,0,B_WIDTH_AS_USUAL,B_STOP_ALERT);
						alert->Go();
					} else {
						BAlert *alert=new BAlert(
							"Not Valid","ERROR\n\nNot a valid sawteeth song",
							"OK",0,0,B_WIDTH_AS_USUAL,B_STOP_ALERT);
						alert->Go();
					}
				} else {
					win->Show();
					win->Activate();
				}
			}
		} 
	} 
}

#include "data/Bitmaps.h"

void stApp::InstallMimeType()
{
	BMimeType mt("audio/x-sawteeth");
	
	if (mt.IsInstalled()){
		mt.Delete();
//		puts("mimetype exists...");
//		return;
	}
	
	BMessage *bm = new BMessage('noop');
	bm->AddString("extensions","St");
	bm->AddString("extensions","st");
	bm->AddString("extensions","ST");

	mt.SetFileExtensions(bm);
	delete bm;
	
	BBitmap large(BRect(0,0,31,31),B_COLOR_8_BIT);
	large.SetBits(kLargeIconBits,sizeof(kLargeIconBits),0,B_COLOR_8_BIT);
	BBitmap small(BRect(0,0,15,15),B_COLOR_8_BIT);
	small.SetBits(kSmallIconBits,sizeof(kSmallIconBits),0,B_COLOR_8_BIT);

	if (B_OK != mt.SetIcon( &large, B_LARGE_ICON))
		puts("licon set error");

	if (B_OK != mt.SetIcon( &small, B_MINI_ICON))
		puts("sicon set error");
	
	if (B_OK != mt.SetShortDescription("Sawteeth song"))
		puts("shortd set error");
	if (B_OK != mt.SetLongDescription("Sawteeth song"))
		puts("longd set error");
	
	if (B_OK != mt.SetPreferredApp("application/x-vnd.Jonas_Norberg-Sawteeth"))
		puts("app set error");

	mt.Install();	
//	puts("mimetype installed...");
}

//#include "/boot/develop/BeIDE/profiling/libprof/libprof.h"

int main()
{
//	PROFILE_INIT(30);
	
	new stApp("application/x-vnd.Jonas_Norberg-Sawteeth");
	be_app->Run();

	delete be_app;
	
//	PROFILE_DUMP("/boot/home/Desktop/profile");

}
