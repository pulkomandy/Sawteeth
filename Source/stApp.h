/* Copyright 2001, Arvid and Jonas Norberg
Distributed under the terms of the MIT Licence. */

#ifndef _ST_APP_H_
#define _ST_APP_H_

#include <Application.h>
#include <FilePanel.h>
#include <StorageKit.h>
#include "stMainWindow.h"

#define WINDOWS_TO_IGNORE 1

class stApp:public BApplication
{
public:
	stApp(char *signature);
	~stApp();
	virtual void MessageReceived(BMessage *);
	virtual void RefsReceived(BMessage *);
	virtual void AboutRequested();
	virtual void ReadyToRun();
	virtual void Pulse();
private:

	void InstallMimeType();
	BFilePanel *load;
	bool dropped;
};

#endif // _ST_APP_H_
