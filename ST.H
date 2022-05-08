/**************************************************************************
 *
 *  (c) 1992 Joe Jaworski
 *	 18405 Tamarind Street
 *  Fountain Valley, CA 92708
 *	 ALL RIGHTS RESERVED.
 *
 * Created and Written by Joe Jaworski
 * Microsoft C 6.00A, SDK 3.1
 *
 *
 * Revision:	$Revision:   1.4  $
 * Date:			$Date:   20 Feb 1993 09:27:52  $
 * Log File:	$Logfile:   C:/src/st/vcs/st.h_v  $
 * Log:			$Log:   C:/src/st/vcs/st.h_v  $
 * 
 *    Rev 1.4   20 Feb 1993 09:27:52   jaworski
 * Initial Revision
 *
 **************************************************************************/

/***********************************/
/*          Star Trek              */
/*         INCLUDE File            */
/***********************************/


#define LOCAL _near _pascal



/* Macro: Check menuitem if the BOOL value is TRUE */
#define BOOLCHECK(c) ((c) ? (WORD)MF_BYCOMMAND|MF_CHECKED : (WORD)MF_BYCOMMAND|MF_UNCHECKED)

/* Macro: Derives the logical Y-coord (0 thru 7) from a Quadrant */
#define XQUAD(q) (q%8)

/* Macro: Derives the logical X-coord (0 thru 7) from a Quadrant */
#define YQUAD(q) (q>>3)

/* Macro: Derives a Quad from a POINT structure */
#define QUADPT(_pt) (_pt.x + (_pt.y << 3))

/* Macro: Subracts dilithium energy and clamps at zero */
#define SubtractEnergy(n) ((nDilithium-(n)<= 0) ? (nDilithium = 0):(nDilithium = nDilithium-(n)))

/* Macro: Subracts time value and returns 0 if exceeded */
#define SubtractTime(t) (((fStarDNow+=((double)(t)))>=fStarDNow+fMissionL) ? (0):(1))

/* Macro: Gets the X,Y object byte from the current (enterprise) quadrant */
#define GetQObj(oy, ox) (LOBYTE(qd[nEntQ].Obj[oy][ox]))

/* Macro: Sets the X,Y object byte in the current (enterprise) quadrant */
#define SetQObj(obj, oy, ox) (qd[nEntQ].Obj[oy][ox] = MAKEWORD(obj,HIBYTE(qd[nEntQ].Obj[oy][ox]))) 

/* Macro: Gets the X,Y object incr byte from the current (enterprise) quadrant */
#define GetQInc(oy, ox) (HIBYTE(qd[nEntQ].Obj[oy][ox]))

/* Macro: Increments the X,Y object incr byte in the current (enterprise) quadrant */
#define IncQObjInc(oy, ox) (qd[nEntQ].Obj[oy][ox] += 0x0100) 

/* Macro: Fatal Error to MSG Window	*/
#define FATAL() ShowMessage(MG_FATAL, (LPSTR)__FILE__, __LINE__)
 
/* Macro: Set the Arrow or Wait Cursors */
#define ArrowCursorX() {ReleaseCapture();SetCursor(hMyCurs = LoadCursor(NULL, IDC_ARROW));}
#define WaitCursorX(_h)  {SetCapture(_h);SetCursor(hMyCurs = LoadCursor(NULL, IDC_WAIT));}
					 

/* default window positions/sizes */

#define POS_PANELX 10	 								  
#define POS_PANELY 5
#define SIZ_PANELX 189	 								  
#define SIZ_PANELY 236

#define POS_MSGX	 10
#define POS_MSGY	 266
#define SIZ_MSGX	 320
#define SIZ_MSGY	 82

#define POS_BTNX	 350
#define POS_BTNY	 262
#define SIZ_BTNX	 263
#define SIZ_BTNY	 90

#define POS_SRSX	 250
#define POS_SRSY	 5
#define SIZ_SRSX	 315
#define SIZ_SRSY	 235

#define POS_COMX	 250
#define POS_COMY	 5
#define SIZ_COMX	 360
#define SIZ_COMY	 230

#define POS_LRSX	 250
#define POS_LRSY	 5
#define SIZ_LRSX	 360
#define SIZ_LRSY	 230

#define POS_CTLX	 250
#define POS_CTLY	 5
#define SIZ_CTLX	 340
#define SIZ_CTLY	 225


/* Quadrant Obj[][] type defintions */

#define OBJ_NONE 		 0x00 
#define OBJ_ENTL 		 0x01 
#define OBJ_ENTR 		 0x02
#define OBJ_BASE		 0x04
#define OBJ_STAR0 	 0x08 
#define OBJ_STAR1 	 0x10 
#define OBJ_STAR2 	 0x18 
#define OBJ_KLINGON 	 0x20 
#define OBJ_ROMULAN 	 0x40 
#define OBJ_FARENG 	 0x60 
#define OBJ_BORG 		 0x80

#define OBJ_ENTMASK   0x03 
#define OBJ_STARMASK  0x18 
#define OBJ_ALIENMASK 0xE0 


#define QFLAG_LRS	 0x0001			 	 		/* LRS detected flag */
#define QFLAG_RIC	 0x0002			 	 		/* Ricochet on a star */

#define WM_NAVEXEC WM_USER+0x1000		/* internal SRS processed messages */
#define WM_PHOEXEC WM_USER+0x1001
#define WM_RICEXEC WM_USER+0x1002
#define WM_PHAEXEC WM_USER+0x1003



enum Alert								  	 		/* define Alert Condition modes */
		 
	{
	NoAlert,			BlueAlert,
	GreenAlert, 	RedAlertOn,
	RedAlertOff,	YellAlertOn,
	YellAlertOff,	GameOver
	} Condition;


enum GOAlert								  	 			 /* define GameOver types */
		 
	{
	NoGame,	  	NoEnergy,
	NoTimeLeft,	PlayerWon,
	NoSystems
	} GameOverType;


typedef struct tagQUADRANT             	  /* define Quadrant Info struct */
	{
	int	Stars;									  /* Total # of stars				*/
	int	Bases;									  /* Total # of starbases			*/
	int	Aliens;									  /* Total # of aliens				*/
	WORD  Flags;									  /* LRS logged, etc. flags      */
	WORD	Obj[8][8];								  /* Object types, each sector	*/	
   } QUADRANT;

typedef struct tagALIENINFO             	  /* define Alien Data struct	*/
	{
	int	Quad;											/* Quadrant of alien				*/
	int	SecX;									  		/* SectorX of alien				*/
	int	SecY;									  		/* SectorY of alien				*/
	BYTE	Obj;											/* Base Object type				*/
	int	Energy;									  	/* Total energy					*/
	DWORD	MTick;								  		/* # of ticks between moves	*/	
	DWORD	FTick;								  		/* # of ticks between shots	*/	
	DWORD	MStartTick;							  		/* starting tick time moves	*/	
	DWORD	FStartTick;							  		/* starting tick time fire 	*/	
   } ALIENINFO;
typedef ALIENINFO _near *  PALIENINFO;


typedef struct tagSTARS								/* Stars warping struct */
	{
	double	x, y, z;
	int 		ox, oy;
	DWORD		color;
	} STARS;


typedef struct tagGAMESTRUCT				 /* Game struct */
	{
	char			sig[4];						 /* signature */
	int			nSkill;						 /* skill level	(Crewman = 0)*/
	QUADRANT		qd[64];						 /* Quadrant structs				*/
	int			nStarBaseCnt;				 /* Total # of starbases		*/
	int			nTotStars;					 /* Total	# of stars			*/
	double		fStarDStart;				 /* Starting Stardate			*/
	double		fStarDNow;					 /* Time now in stardates		*/
	int			nAlienTotal;				 /* Total # of Aliens			*/
	int			nAlienCnt;					 /* Total # of Aliens	left	*/
	int			nDilithium;					 /* Energy (minus shields)		*/
	int			nShields;					 /* Total Shield Energy			*/
	int			nPhotonCnt;					 /* Total # of photons			*/
	double		fSRSRepair, fLRSRepair,	 /* Repair times in fDays		*/
					fNAVRepair,	fPHORepair,
					fPHARepair, fSHERepair,
					fCOMRepair,	fDAMRepair,
					fPANRepair,	fMSGRepair;
	double		fSRSRep, fLRSRep,			 /* Repair Time at stardate	 */
					fNAVRep,	fPHORep,
					fPHARep, fSHERep,
					fCOMRep,	fDAMRep,
					fPANRep,	fMSGRep;
	double		fMissionL;					 /* Mission length in stardates*/
	int			nEntQ, nEntX, nEntY;		 /* Enterprise's location		 */
	BOOL			bReRoute;					 /* MSG to COM reroute flag	 */
	} GAMESTRUCT;



HINSTANCE	hInst, hLib, hSnd;					/* apps and library's Instance*/
HMENU			hMenuMT, hMenuFL;						/* Menu Handle (2 types used)	*/
HACCEL		hAccel;									/* Accelerators					*/
HCURSOR		hMyCurs;
GAMESTRUCT	gs;
PSTR			szFrameClass,szPanelClass,			/* Class names (and Titles)	*/		
				szMsgClass, szBtnClass,
				szSRSClass, szLRSClass,
				szNAVClass, szPHOClass,
				szPHAClass, szSHEClass,
				szDAMClass, szCOMClass,
				szEXTClass;

HWND			hFrameWnd,hClientWnd,hPANWnd,   	/* Global Window Handles		*/
            hMSGWnd,	 hBTNWnd,   hSRSWnd,
				hLRSWnd,	 hNAVWnd,	hPHOWnd,
				hPHAWnd,	 hSHEWnd,   hDAMWnd,
				hCOMWnd;

char			szPath[_MAX_PATH];					/* DLL and scores path			*/
char			szHelp[_MAX_PATH];					/* Help File pathname			*/
char			szINI[_MAX_PATH];						/* INI File pathname				*/
OPENFILENAME ofn;										/* Game load/save related		*/
char			szGFname[16], szGPath[_MAX_PATH],
				szGFull[_MAX_PATH];
int			nINISkill,  nINIAutoSRS,			/* WIN.INI saved values			*/
				nINISingle, nINIPauseI,
				nINISounds;

HPEN			hGreenPen;
HBITMAP		hCourseBmp,  hDigitsGBmp,					  /* shared GDI objects */
				hDigitsRBmp, hExecuteBmp,
				hLineArtBmp, hLRBlockBmp,
				hDilBmp;

BOOL			bResetTick;						 			/* Alien Move/Fire timer	*/
BOOL			bPaused;						 	 			/* Game is paused 			*/
BOOL 			bSRSBusy;						 			/* If set, wait on execute */	
BOOL			bNeedSave;									/* AskToSave if true			*/

#define     SIZ_COURS 175,203								 /* Course draw related	*/
#define     RADIUS    50
#define  	SIZ_EXEC  117,31	  							 /* sizes of bmp's */
#define  	SIZ_LART  188,64
#define  	SIZ_DIL   88,53

#define 		DG_DASH	0								/* digits in bitmap (Y-coord) */
#define 		DG_BLANK	18		
#define 		DG_9		36		
#define 		DG_8		54		
#define 		DG_7		72		
#define 		DG_6		90 		
#define 		DG_5		108		
#define 		DG_4		126		
#define 		DG_3		144		
#define 		DG_2		162		
#define 		DG_1		180		
#define 		DG_0		198		
#define 		DG_DEC	216
#define 		DG_WIDTH	13							  	/* Width, height of each digit*/
#define 		DG_HEIGHT 18	


/* all of the global parms that are saved and restored */

int			nSkill;									/* skill level	(Crewman = 0)	*/
QUADRANT		qd[64];									/* Quadrant structs				*/
PALIENINFO	pAL;										/* handle to alien list			*/
int			nStarBaseCnt;							/* Total # of starbases			*/
int			nTotStars;								/* Total	# of stars				*/
double		fStarDStart;							/* Starting Stardate				*/
double		fStarDNow;								/* Time now in stardates		*/
int			nAlienTotal;							/* Total # of Aliens				*/
int			nAlienCnt;								/* Total # of Aliens	left		*/
int			nDilithium;								/* Energy (minus shields)		*/
int			nShields;								/* Total Shield Energy			*/
int			nPhotonCnt;								/* Total # of photons			*/
double		fSRSRepair, fLRSRepair,				/* Repair times in fDays		*/
				fNAVRepair,	fPHORepair,
				fPHARepair, fSHERepair,
				fCOMRepair,	fDAMRepair,
				fPANRepair,	fMSGRepair;
double		fSRSRep, fLRSRep,						/* Repair Time at stardate		*/
				fNAVRep,	fPHORep,
				fPHARep, fSHERep,
				fCOMRep,	fDAMRep,
				fPANRep,	fMSGRep;
double		fMissionL;								/* Mission length in stardates*/
int			nEntQ, nEntX, nEntY;					/* Enterprise's location		*/
BOOL			bReRoute;								/* MSG to COM reroute flag		*/
