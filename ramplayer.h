#define PID_RAMPLAYER 1000472

#include "c4d.h"
#include "c4d_symbols.h"
#include <vector>

enum
{
	PLAYMODE_PLAY = 0,
	PLAYMODE_REV,

	PLAYMODE_DUMMY
};

enum
{
	MENU_LOAD = 2000,
	MENU_ADD,
	MENU_SAVE,
	MENU_SAVI,

	MENU_DUMMY
};

class PlayerArea : public GeUserArea
{
	private:
		BaseBitmap* map; //Current Frame image
		Bool gmap;
	public:
		PlayerArea(void);
		~PlayerArea(void);

		void SetMap(BaseBitmap *m) 
		{
			if(gmap)
			{
				BaseBitmap::Free(map);
				gmap = FALSE;
			}
			map = m;
		}
		virtual Bool Init();
		virtual Bool GetMinSize(LONG& w, LONG& h);
		virtual void DrawMsg      (LONG x1,LONG y1,LONG x2,LONG y2,const BaseContainer &msg);
		virtual void Sized(LONG w,LONG h);
};

class RamPlayerDlg : public GeDialog
{
	private:
	  typedef std::vector<BaseBitmap*> VideoFrames;
		VideoFrames all_frames;

		Filename FN_seq;
		LONG maxframes;

		void ShowFrame(LONG frame = 0);
		Bool InitMovie(Bool add = FALSE);
		Bool SaveMovie(void);

		PlayerArea p_area;
		LONG playmode;
		
		void free()
		{
		  if(!all_frames.empty())
			{
				for(VideoFrames::iterator i=all_frames.begin(); i != all_frames.end();++i)
					BaseBitmap::Free(*i);
				all_frames.clear();
			}
		}
	public:
		RamPlayerDlg()
		{
			playmode = 0;
			maxframes = 0;
		}
		~RamPlayerDlg()
		{
			free();
		}
		virtual Bool CreateLayout(void);
		virtual Bool InitValues(void);
		virtual Bool Command(LONG id,const BaseContainer &msg);
		virtual void Timer(const BaseContainer& msg);
		virtual Bool AskClose(void)
		{
			free();
			p_area.SetMap(NULL);
			maxframes = 0;
			return FALSE;
		};
};
