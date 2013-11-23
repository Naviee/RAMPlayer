//This plugin loads a sequence of numbered images to the it's UserArea
//Then you can play back the images in the UA with the help of the Timer() method
//Or save the sequence to a .avi or .qt video file

#include "ramplayer.h"

void RamPlayerDlg::ShowFrame(LONG frame)
{
	if(frame<maxframes && frame>=0)
	{
		p_area.SetMap(all_frames.at(frame));
		p_area.Redraw();
		SetLong(RP_FRAME,frame,0,maxframes-1);
	}
}

inline Bool IsZahl(const String &v)
{
	if(!v.Compare("0") || !v.Compare("1") ||
		!v.Compare("2") || !v.Compare("3") ||
		!v.Compare("4") || !v.Compare("5") ||
		!v.Compare("6") || !v.Compare("7") ||
		!v.Compare("8") || !v.Compare("9"))
		return TRUE;
	return FALSE;
}

Bool DetectSequenceProps(const Filename ofn, LONG &start, String &dst_s, LONG &ncnt)
{
	Filename fn = ofn.GetFile();
	fn.ClearSuffix();
	String name = fn.GetString();
	LONG ocnt = name.GetLength();
	
	ncnt = 0;

	//We are only interested in the last 4 characters NameXXXX (or 3)
	for(LONG i=ocnt-1;i>=ocnt-5;--i)
	{
		//Check for digits
		if(IsZahl(name.SubStr(i,1)))
		{
			++ncnt;
			continue;
		}
		else if(i==(ocnt-1))
			return FALSE;
	}
	if(ncnt>0)
	{
		String res = name.SubStr(ocnt-ncnt,ncnt);
		//start = res.StringToLong();    //R11.5 version to get the number value from a string
        GeData d;

        d = StringToNumber(res, FORMAT_LONG, NULL, NULL);
        start = d.GetLong();
		dst_s = name.SubStr(0,ocnt-ncnt);
	}
	return TRUE;
}

//Check for the next frame
Bool GetNextFrame(const Filename ofn, const String& o_str, const LONG digcnt, const LONG& last_f_nr)
{
	Filename fn = ofn.GetFile();
	fn.ClearSuffix();
	String name = fn.GetString();
	LONG ocnt = name.GetLength();
	
	if(digcnt>0)
	{
		String res = name.SubStr(ocnt-digcnt,digcnt);
		//LONG start = res.StringToLong();  //R11.5 version to get the number value from a string
        GeData d;

        d = StringToNumber(res, FORMAT_LONG, NULL, NULL);
        LONG start = d.GetLong();

		if(start==(last_f_nr+1) && !name.SubStr(0,ocnt-digcnt).Compare(o_str))
			return TRUE;
		else 
			return FALSE;
	}
	return FALSE;
}

Bool RamPlayerDlg::InitMovie(Bool add)
{
	LONG s_frame = 0;
	LONG s_cnt = 0;
	String dst;

	//Fill 
	if(!DetectSequenceProps(FN_seq,s_frame,dst,s_cnt)) return FALSE;

	if(!add)
	{
    free();
		maxframes = 0;
	}

	AutoAlloc<BrowseFiles> bf;
	bf->Init(FN_seq.GetDirectory(),FALSE);

	//We skip the first one already
	if(!bf->GetNext()) return FALSE;

	Bool first = TRUE;
	all_frames.reserve(1000); //reserve memory
	while(bf->GetNext()) //Run thru folder
	{
		if(!GetNextFrame(bf->GetFilename(),dst,s_cnt,s_frame)) continue;
		++s_frame;

		BaseBitmap* temp = BaseBitmap::Alloc();
		temp->Init(bf->GetFilename());
		all_frames.push_back(temp);
		++maxframes;
		if(first)
		{
			SetString(RP_SIZE,LongToString(temp->GetBw())+" x "+LongToString(temp->GetBh()));
			first = FALSE;
		}
		
	}
	SetString(RP_PATH,FN_seq.GetString());
	
	ShowFrame(0);
	return TRUE;
}

Bool RamPlayerDlg::SaveMovie(void)
{
	for(LONG i=0;i<maxframes;++i)
	{
    //Not implemented..
	}
	return TRUE;
}

/****************************/
/* Hard Coded Dialog Layout */
/****************************/
Bool RamPlayerDlg::CreateLayout(void)
{
	// first call the parent instance
	Bool res = GeDialog::CreateLayout();

	res = LoadDialogResource(ID_RAMPLAYER,NULL,0);
	AttachUserArea(p_area,RP_MOVIE);
	
	//Add Menus
	MenuSubBegin(GeLoadString(RP_MENU_FILE));
		MenuAddString(MENU_LOAD,GeLoadString(RP_MENU_LOAD));		
		MenuAddString(MENU_ADD,GeLoadString(RP_MENU_MERGE));
		MenuAddString(MENU_SAVE,GeLoadString(RP_MENU_SAVE_QT));
		MenuAddString(MENU_SAVI,GeLoadString(RP_MENU_SAVE_AVI));
	MenuSubEnd();
	MenuFinished();
	return res;
}

Bool RamPlayerDlg::InitValues(void)
{
	// first call the parent instance
	if (!GeDialog::InitValues()) return FALSE;

	SetLong(RP_FRAME,0.0,0,maxframes-1);
	SetLong(RP_FPS,25,2,60);
	return TRUE;
}

void RamPlayerDlg::Timer(const BaseContainer& msg)
{
	LONG frame;
	GetLong(RP_FRAME,frame);

	if(!playmode)
	{
		if(frame+1>=maxframes)
		{
			ShowFrame(0);
			return;
		}
		ShowFrame(frame+1);
	}
	else
	{
		if(frame-1<0)
		{
			SetTimer(0);
			return;
		}
		ShowFrame(frame-1);
	}
}

Bool RamPlayerDlg::Command(LONG id,const BaseContainer &msg)
{
	LONG frame;
	GetLong(RP_FRAME,frame);
	switch (id)
	{
		case RP_FRAME:
			ShowFrame(frame);
			break;
		case RP_TOSTART:
			ShowFrame(0);
			break;
		case RP_ONEBACK:
			SetTimer(0);
			ShowFrame(frame-1);
			break;
		case RP_STOP:
			SetTimer(0);
			break;
		case RP_REWIND:
			{
				LONG fps;
				GetLong(RP_FPS,fps);

				playmode	=	1;
				BaseDocument* doc = GetActiveDocument();
				SetTimer(1000.0f/fps);
			}
			break;
		case RP_PLAY:
			{
				LONG fps;
				GetLong(RP_FPS,fps);

				playmode	=	0;
				BaseDocument* doc = GetActiveDocument();
				SetTimer(1000.0f/fps);
			}
			break;
		case RP_ONEFOR:
			SetTimer(0);
			ShowFrame(frame+1);
			break;
		case RP_TOEND:
			SetTimer(0);
			ShowFrame(maxframes-1);
			break;
		//MENU CHECK
		case MENU_LOAD:
			{
				if(FN_seq.FileSelect(FILESELECTTYPE_IMAGES, FILESELECT_LOAD, "Choose Footage sequence..."))
				{
					InitMovie();					
				}
			}
			break;
		case MENU_ADD:
			{
				if(FN_seq.FileSelect(FILESELECTTYPE_IMAGES, FILESELECT_LOAD, "Choose Footage sequence..."))
				{
					InitMovie(TRUE);
				}
			}
			break;
		case MENU_SAVI:
		case MENU_SAVE:
			{
				LONG fps;
				GetLong(RP_FPS,fps);

				Filename fn;
				if(fn.FileSelect(FILESELECTTYPE_IMAGES, FILESELECT_SAVE, "Please specify a name for the movie."))
				{
					AutoAlloc<MovieSaver> ms;

					fn.ClearSuffix();
					fn.SetSuffix(id==MENU_SAVI ? "avi" : "mov");

					BaseContainer bc;
					if(id==MENU_SAVI)
					{
						if(ms->Choose(FILTER_AVI,&bc))
						{
							ms->Open(fn,all_frames.at(0),fps,1122,&bc,SAVEBIT_0);
							
							for(VideoFrames::iterator i=all_frames.begin(); i != all_frames.end();++i)
							  ms->Write(*i);
						}
					}
					else
					{
						if(ms->Choose(FILTER_MOVIE,&bc))
						{
							ms->Open(fn,all_frames.at(0),fps,1125,&bc,SAVEBIT_0);
							for(VideoFrames::iterator i=all_frames.begin(); i != all_frames.end();++i)
							  ms->Write(*i);
						}
					}
				}
				
			}
			break;
	}
	return GeDialog::Command(id,msg);
}

class RamPlayer : public CommandData
{
	private:
		RamPlayerDlg dlg;

	public:

		virtual Bool Execute(BaseDocument *doc)
		{
			return dlg.Open(DLG_TYPE_ASYNC,PID_RAMPLAYER,-1,-1,500,300);
		}

		virtual LONG GetState(BaseDocument *doc)
		{
			return CMD_ENABLED;
		}

		virtual Bool RestoreLayout(void *secret)
		{
			return dlg.RestoreLayout(PID_RAMPLAYER,0,secret);
		}
};

Bool RegisterRamPlayer(void)
{
	return RegisterCommandPlugin(PID_RAMPLAYER, GeLoadString(ID_RAMPLAYER), 0, NULL, String("Help text Goes here"), gNew RamPlayer);
}
