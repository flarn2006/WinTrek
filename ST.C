/***************************************************************************
 *
 *
 *(c) 1989-1992 Joe Jaworski
 *		17960 Squirrel Haven
 *    Meadow Vista, CA 95722
 *	   ALL RIGHTS RESERVED.
 *
 * Written By Joe Jaworski
 * MSC 6.00A, SDK 3.1
 *
 * Revision:	$Revision:   1.4  $
 * Date:			$Date:   20 Feb 1993 09:27:50  $
 * Log File:	$Logfile:   C:/src/st/vcs/st.c_v  $
 * Log:			$Log:   C:/src/st/vcs/st.c_v  $
 * 
 *    Rev 1.4   20 Feb 1993 09:27:50   jaworski
 * Initial Revision
 *
 *
 ***************************************************************************/

#define	ST_C
#include "exclude.h"

#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "joes.h"

#include "st.h"
#include "resource.h"
#include "protos.h"




/* Global vars and constants only for this module */

#define REALTIME 45000L					  /* realtime increment in milliseconds*/

static LPINT  	lpnXStar, lpnYStar;					/* Stars paint vars				*/
static int	  	nXScreen, nYScreen, nBkStars;
static char* 	szFilter[] = {"Game Files (.GAM)",	"*.gam",
								  "All Files  (*.*)",	"*.*",
								  ""};
static WNDPROC	OrigClientDefProc;




/*************************************************************************/

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR lpszCmdLine, int nCmdShow)
															
//  Main Windows entry point. Here we register our classes and
//	 setup all of Star Trek's non-play-in-progress stuff.	

/*************************************************************************/

{
	MSG		msg;
	int		i;
	char		szBuf[128];
	static DWORD dwRealTime;					/* next real time increment		*/


	hInst = hInstance;                     /* set global Instance				*/
	LoadUpStrings();								/* get static resource strings	*/ 
	srand(LOWORD(GetTickCountEx()));			/* Seed random number gen			*/


/* Don't allow multiple instances */

	if(hPreInst)
		{
   	SetActiveWindow(FindWindow(szFrameClass, NULL));
		return 0;											  
   	}


/* build full path to DLL files and load 'em */

	GetModuleFileName(hInst, szPath, sizeof(szPath));
	*(strrchr(szPath, '\\')) = 0;
	ADDSLASH(szPath);
	strcpy(szBuf, szPath);
	strcat(szBuf, (PSTR)"WINTREK1.DLL");
	if((int)(hLib = LoadLibrary(szBuf)) < 32)
		return 0;
	strcpy(szBuf, szPath);
	strcat(szBuf, (PSTR)"WINTREK2.DLL");
	if((int)(hSnd = LoadLibrary(szBuf)) < 32)
		return 0;


/* same for HELP/INI items */

	strcpy(szHelp, szPath);
	strcat(szHelp, (PSTR)"WINTREK.HLP");
	strcpy(szINI,  szPath);
	strcat(szINI,  (PSTR)"WINTREK.INI");


	LoadINI();												/* Get INI settings		*/
	Opening();												/* Run opening graphics */
	SetupClasses();		  								/* register our classes	*/

	hMenuMT = LoadMenu(hInst, MAKEINTRESOURCE(MNU_EMPTY));
	hMenuFL = LoadMenu(hInst, MAKEINTRESOURCE(MNU_FULL));
	hAccel  = LoadAccelerators(hInst, MAKEINTRESOURCE(MNU_FULL));
	i = 64;

/* Create our main window */

	if(!CreateWindow (szFrameClass, szFrameClass, WS_OVERLAPPEDWINDOW |
		WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT,	HWND_DESKTOP, hMenuMT, hInst, NULL))
   	return 0;

	WaitCursorX(hFrameWnd);
	ShowWindow(hFrameWnd, SW_SHOWMAXIMIZED);
	UpdateWindow(hFrameWnd);
	dwRealTime = GetTickCountEx() + REALTIME;
	PlaySound(SND_OPEN, 0);
	ArrowCursorX();


/***********************************/
/*			Main Message Loop			  */
/***********************************/


	while(TRUE)
		{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{

			if(msg.message == WM_QUIT)
				{
				DestroyMenu(hMenuMT);
				DestroyMenu(hMenuFL);
				return (int)msg.wParam;
				}

     		if(!TranslateMDISysAccel(hClientWnd, &msg) &&
      		!TranslateAccelerator(hFrameWnd, hAccel, &msg))
         	{
         	TranslateMessage (&msg);
         	DispatchMessage (&msg);
         	}
			}
		else
			{											 			  /* do idle processing */
			ResumeBackground();

			if(bPaused || Condition == NoAlert)
				WaitMessage();									  /* nothing to do */

			else if(Condition == GameOver)
				GameIsOver();

			else if(Condition == RedAlertOn || Condition == RedAlertOff)
				ProcessRed();								   /* handle alien actions */

			else if(msg.time > dwRealTime)				 
				{
				fStarDNow += 0.1;
				--nDilithium;
				dwRealTime = msg.time + REALTIME;
				SetCondition();
				InvalidateRect(hPANWnd, NULL, 0);
				++bNeedSave;
				}

			else
			   WaitMessage();
			}
		}


} /* End WinMain */





/*************************************************************************/

LRESULT CALLBACK _export FrameDefProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)

// This is the frame (parent) window definition procedure. 
                                                           
/**************************************************************************/
{
	int	i;
	CLIENTCREATESTRUCT cc;
	HWND	hWndChild;

	
	hFrameWnd = hWnd;

	switch(message)
   	{
     	case WM_CREATE:

/* Init OpenFileName struct */

			strcpy(szGPath, szPath);
			REMSLASH(szGPath);

			ofn.lStructSize 		= sizeof(OPENFILENAME);
			ofn.hwndOwner 			= hWnd;
			ofn.lpstrFilter 		= szFilter[0];
			ofn.lpstrCustomFilter= (LPSTR) NULL;
			ofn.nMaxCustFilter 	= 0L;
			ofn.nFilterIndex 		= 1L;
			ofn.lpstrFile			= szGFull;
			ofn.nMaxFile 			= sizeof(szGFull);
			ofn.lpstrFileTitle 	= szGFname;
			ofn.nMaxFileTitle 	= sizeof(szGFname);
			ofn.lpstrInitialDir 	= szGPath;
			ofn.lpstrTitle 		= NULL;
			ofn.Flags 				= 0L;
			ofn.nFileOffset 		= 0;
			ofn.nFileExtension 	= 0;
			ofn.lpstrDefExt 		= "GAM";


/* Set global inits and load shared GDI objects */
					 
			hCourseBmp 	= LoadBitmap(hLib, MAKEINTRESOURCE(BMP_COURSE));
			hExecuteBmp	= LoadBitmap(hLib, MAKEINTRESOURCE(BMP_EXECUTE));
			hDigitsGBmp	= LoadBitmap(hLib, MAKEINTRESOURCE(BMP_DIGITSG));
			hDigitsRBmp	= LoadBitmap(hLib, MAKEINTRESOURCE(BMP_DIGITS));
			hLRBlockBmp	= LoadBitmap(hLib, MAKEINTRESOURCE(BMP_LRBLOCK));
			hDilBmp		= LoadBitmap(hLib, MAKEINTRESOURCE(BMP_SHEDIL));

			hGreenPen = CreatePen(PS_SOLID, 1, RGB_GREEN);

/* Build Stars table */

			nXScreen	= GetSystemMetrics(SM_CXFULLSCREEN);
			nYScreen	= GetSystemMetrics(SM_CYFULLSCREEN);
			nBkStars= (nXScreen >> 4) * (nYScreen >> 4);

			lpnXStar   = GlobalAllocPtr(GHND, (DWORD)(sizeof(int) * nBkStars));
			lpnYStar   = GlobalAllocPtr(GHND, (DWORD)(sizeof(int) * nBkStars));

			for(i = 0; i < nBkStars; i++)	  		/* fill with random points */
				lpnXStar[i] = GetRand(nXScreen);
			for(i = 0; i < nBkStars; i++)
				lpnYStar[i] = GetRand(nYScreen);


			nSkill = nINISkill;
			CheckMenuItem(hMenuMT, IDM_CREWMAN + nINISkill, MF_CHECKED);

			if(!SoundInit())										/* disable sounds? */
				{
				nINISounds = 0;
				CheckMenuItem(hMenuFL,  IDM_SOUNDS, BOOLCHECK(nINISounds));
				EnableMenuItem(hMenuFL, IDM_SOUNDS, MF_BYCOMMAND | MF_GRAYED);
				}
			hLineArtBmp = LoadBitmap(hLib, MAKEINTRESOURCE(BMP_LINEART));


/* Create MDI Client Window and subclass it*/

			cc.hWindowMenu = GetSubMenu(hMenuMT, 0);
			cc.idFirstChild = IDM_MDI;

			hClientWnd = CreateWindow("MDICLIENT", NULL, WS_CHILD | 
				WS_CLIPCHILDREN |	WS_VISIBLE, 0, 0,	0, 0, hWnd,
				(HMENU)1, hInst, (LPSTR)&cc);

			OrigClientDefProc = (WNDPROC)SetWindowLong(hClientWnd, GWL_WNDPROC,
				(LPARAM)(WNDPROC)ClientDefProc);
			InvalidateRect(hClientWnd, NULL, TRUE);
         break;


		case WM_CLOSE:							 				/* User wants to quit	*/
			if(AskToSave() != IDCANCEL)
				DestroyWindow(hWnd);
			break;


		case WM_QUERYENDSESSION:
			if(AskToSave() != IDCANCEL)			/* Win shutdown in progress	*/
				return (LRESULT)TRUE;
			break;


     	case WM_DESTROY:
			WinHelp(hWnd, szHelp, HELP_QUIT, 0);			   /* free help app	*/
			SetWindowLong(hClientWnd, GWL_WNDPROC,	(LPARAM)OrigClientDefProc);
			SoundShutDown();
			SaveINI();									/* Save any new INI settings	*/
			if(Condition != NoAlert)
				DestroyGame();
			GlobalFreePtr(lpnXStar);							/* kill stars memory	*/
			GlobalFreePtr(lpnYStar);

			DeleteBitmap(hLineArtBmp);		 					/* free bitmaps	*/
			DeleteBitmap(hCourseBmp); 
			DeleteBitmap(hExecuteBmp);
			DeleteBitmap(hDigitsGBmp);
			DeleteBitmap(hDigitsRBmp);
			DeleteBitmap(hLRBlockBmp);
			DeleteBitmap(hDilBmp);
			DeletePen(hGreenPen);

			SetMenu(hWnd, NULL);
			FreeLibrary(hLib);
			FreeLibrary(hSnd);
         PostQuitMessage(0);
         break;


/* See if pause when iconic */


		case WM_SIZE:
			if(nINIPauseI &&
				!(Condition == RedAlertOn || Condition == RedAlertOff))
				{
				if(wParam == SIZE_MINIMIZED)
					{
					bPaused = TRUE;
					KillSound();
					}
				else if(wParam == SIZE_MAXIMIZED)
					bPaused = FALSE;
				}
         return DefFrameProc(hWnd, hClientWnd, message, wParam, lParam);
         break;


		case WM_MOUSEMOVE:
			SetCursor(hMyCurs);		 					/* keep hour-glass, if set */
			break;



     	case WM_COMMAND:
         switch(wParam)
           	{

/***********************************/
/*		Process File Menu Hits		  */
/***********************************/
	
				case IDM_NEW:					   				/* Start a new game */
					if(AskToSave() == IDCANCEL)
							break;
					WaitCursorX(hFrameWnd);
					NewGame();
					++bNeedSave;
					ArrowCursorX();
					break;


				case IDM_OPEN:						   			/* Load a Game */
					if(AskToSave() == IDCANCEL)
						break;

					i = *szGFname;
					ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST|
						OFN_HIDEREADONLY;
					if(!GetOpenFileName(&ofn))
						{
						if(i)
							*szGFname = 0;
						break;
						}
					LoadGame();
					break;


				case IDM_ENDGAME:						   			/* End this Game */
					if(AskToSave() == IDCANCEL)
						break;
					WaitCursorX(hFrameWnd);
					*szGFname = 0;
					DestroyGame();
					ArrowCursorX();
					break;


				case IDM_SAVE:						/* Save File, same name */
				case IDM_SAVEAS:			 		/* Save under a different name */
				  	if(wParam == IDM_SAVE && *szGFname)
						{
						SaveGame();	  				  		/* save without prompting */
						break;
						}
					ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT |
						OFN_HIDEREADONLY;
					if(!GetSaveFileName(&ofn))
						break;		
					SaveGame();	  				
					break;


           	case IDM_EXIT:                      /* Exit the whole mess */
					if(AskToSave() != IDCANCEL)
						DestroyWindow(hWnd);
              	break;


/***********************************/
/*		Process Skill Menu Hits		  */
/***********************************/


				case IDM_CREWMAN:
				case IDM_YEOMAN:
				case IDM_OFFICER:
				case IDM_1OFFICER:
				case IDM_CAPTAIN:
				case IDM_ADMIRAL:
					if(nINISkill + IDM_CREWMAN == (int)wParam)
						break;
					CheckMenuItem(hMenuMT, IDM_CREWMAN + nINISkill, MF_UNCHECKED);
					CheckMenuItem(hMenuMT, wParam, MF_CHECKED);
					nINISkill = nSkill = wParam - IDM_CREWMAN;
					break;


/***********************************/
/*		Process Options Menu Hits	  */
/***********************************/


				case IDM_PAUSE:
					if(!bPaused)
						{
						bPaused = TRUE;
						PauseGame();
						}
					break;


				case IDM_AUTOSRS:
					nINIAutoSRS = !nINIAutoSRS;
					CheckMenuItem(hMenuFL, wParam, BOOLCHECK(nINIAutoSRS));
					break;

				case IDM_SINGLE:
					nINISingle = !nINISingle;
					CheckMenuItem(hMenuFL, wParam, BOOLCHECK(nINISingle));
					break;

				case IDM_PICONIC:
					nINIPauseI = !nINIPauseI;
					CheckMenuItem(hMenuFL, wParam, BOOLCHECK(nINIPauseI));
					break;

				case IDM_SOUNDS:
					nINISounds = !nINISounds;
					CheckMenuItem(hMenuFL, wParam, BOOLCHECK(nINISounds));
					if(!nINISounds)
						KillSound();
					break;



/***********************************/
/*		Process Window Menu Hits	  */
/***********************************/

				case IDM_CASCADE:
					SendMessage(hClientWnd, WM_MDICASCADE, 0, 0L);
					break;

				case IDM_TILE:
					SendMessage(hClientWnd, WM_MDITILE, 0, 0L);
					break;

				case IDM_ARRANGE:
					SendMessage(hClientWnd, WM_MDIICONARRANGE, 0, 0L);
					break;

				case IDM_DEFPOS:
					SetDefPos();
					break;

				case IDM_SAVEDEF:
					SavePos();
					break;

				case IDM_RESDEF:
					RestorePos();
					break;


/***********************************/
/*		Process Help Menu Hits 		  */
/***********************************/

				case IDM_HELP:
					WinHelp(hWnd, szHelp, HELP_INDEX, 0);
					break;

				case IDM_ABOUT:
					About();
					break;


/* default: WM_Command End */	

           	default:
					hWndChild = (HWND)LOWORD(SendMessage(hClientWnd,
							WM_MDIGETACTIVE, 0, 0L));
					if(IsWindow(hWndChild))
						SendMessage(hWndChild, WM_COMMAND, wParam, lParam);

         		return DefFrameProc(hWnd, hClientWnd, message, wParam,lParam);
              	break;    
				}

         break;


     	default:
         return DefFrameProc(hWnd, hClientWnd, message, wParam, lParam);
         break;
     	}
	return (LPARAM)0;

} /* End MainDefProc */





/***************************************************************************/

void LOCAL PaintMain(HWND hWnd)

//	This function draws the stars background and the lineart ship if
//	a game is not in progress and/or the timer is stopped.

/***************************************************************************/
{
	HDC			hDC;
	PAINTSTRUCT	ps;
	int			i, nOldBkMode;
	RECT		  	rc, rcTmp, rcCl, rcDummy;
	HFONT			hOldFont;
	COLORREF		clr;
	POINT			pt;


	hDC = BeginPaint(hWnd, &ps);

	
/* Draw any stars	that were invalidated */

	for(i = 0; i < nBkStars; i++)
		{
		pt.x = lpnXStar[i];
		pt.y = lpnYStar[i];
		if(PtInRect(&ps.rcPaint, pt))
			{
			switch(pt.x & 0x0003)
				{
				case 1:	clr = RGB_WHITE;	break;
				case 2:	clr = RGB_GRAY;	break;
				default:	clr = RGB_DKGRAY;	break;
				}
			SetPixel(hDC, pt.x, pt.y, clr);
			}
		}

/* Draw Play string and bitmap */

	if(Condition == NoAlert)
		{
		GetClientRect(hWnd, &rcCl);	  		/* Center bitmap in client */
		SetRect(&rc, 0, 0, SIZ_LART);
		CenterRect(&rc, &rcCl);

		hOldFont = SelectFont(hDC, GetStockObject(ANSI_VAR_FONT));
		SetTextColor(hDC, RGB_RED);
		nOldBkMode = SetBkMode(hDC, TRANSPARENT);
		SetRect(&rcTmp, rc.left, rc.bottom, rc.right, rc.bottom+10);
		DrawText(hDC, ResToString(RS_BEGIN), -1, &rcTmp,
			DT_NOPREFIX | DT_WORDBREAK | DT_CALCRECT | DT_CENTER);

		if(IntersectRect(&rcDummy, &rcTmp, &ps.rcPaint))
			DrawText(hDC, ResToString(RS_BEGIN), -1, &rcTmp,
				DT_NOPREFIX | DT_WORDBREAK | DT_CENTER);

		SetTextColor(hDC, RGB_BLACK);
		if(IntersectRect(&rcDummy, &rc, &ps.rcPaint))
			DrawBitmap(hDC, rc.left, rc.top, hLineArtBmp, SRCPAINT);

		SetBkMode(hDC, nOldBkMode);
		SelectFont(hDC, hOldFont);
		}
	
	EndPaint(hWnd, &ps);


} /* End PaintMain */




/*************************************************************************/

LRESULT CALLBACK _export ClientDefProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)

// This is the subclass client window definition procedure. We need this to
// draw our stars on the background.
                                                           
/**************************************************************************/
{
	HBRUSH	hOldBr;

	if(msg == WM_PAINT)
		{
		PaintMain(hWnd);
		return (LRESULT)0;
		}
	if(msg == WM_ERASEBKGND || msg == WM_ICONERASEBKGND)
		{
     	hOldBr = (HBRUSH)SetClassWord(hWnd, GCW_HBRBACKGROUND,
			(WPARAM)GetStockObject(BLACK_BRUSH)); 
      CallWindowProc(OrigClientDefProc, hWnd, msg, wParam, lParam); 
		SetClassWord(hWnd, GCW_HBRBACKGROUND, (WPARAM)hOldBr);
		return (LRESULT)0;
		}

	return CallWindowProc(OrigClientDefProc, hWnd, msg, wParam, lParam);

} /* End ClientDefProc */





/***************************************************************************/

void LOCAL SetDefPos(void)

//	This function sets all windows back to their default position.

/***************************************************************************/
{
	HDWP	hDef;
	int	cxF = GetSystemMetrics(SM_CXFRAME);
	int	cyF = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYFRAME);

	WaitCursorX(hFrameWnd);

	hDef = BeginDeferWindowPos(11);
	ShowWindow(hSRSWnd, SW_NORMAL);		/* Set default states */
	ShowWindow(hLRSWnd, SW_NORMAL);	
	ShowWindow(hNAVWnd, SW_NORMAL);
	ShowWindow(hPHOWnd, SW_NORMAL);
	ShowWindow(hPHAWnd, SW_NORMAL);
	ShowWindow(hSHEWnd, SW_NORMAL);
	ShowWindow(hDAMWnd, SW_NORMAL);
	ShowWindow(hCOMWnd, SW_NORMAL);
	ShowWindow(hPANWnd, SW_NORMAL);
	ShowWindow(hMSGWnd, SW_NORMAL);
	ShowWindow(hBTNWnd, SW_NORMAL);

	hDef = DeferWindowPos(hDef, hSRSWnd, NULL, POS_SRSX, POS_SRSY,
		SIZ_SRSX + cxF, SIZ_SRSY + cyF, SWP_NOZORDER);

	hDef = DeferWindowPos(hDef, hLRSWnd, NULL, POS_LRSX, POS_LRSY,
		SIZ_LRSX + cxF, SIZ_LRSY + cyF, SWP_NOZORDER);

	hDef = DeferWindowPos(hDef, hNAVWnd, NULL, POS_CTLX, POS_CTLY,
		SIZ_CTLX + cxF, SIZ_CTLY + cyF, SWP_NOZORDER);

	hDef = DeferWindowPos(hDef, hSHEWnd, NULL, POS_CTLX, POS_CTLY,
		SIZ_CTLX + cxF, SIZ_CTLY + cyF, SWP_NOZORDER);

	hDef = DeferWindowPos(hDef, hPHOWnd, NULL, POS_CTLX, POS_CTLY,
		SIZ_CTLX + cxF, SIZ_CTLY + cyF, SWP_NOZORDER);

	hDef = DeferWindowPos(hDef, hPHAWnd, NULL, POS_CTLX, POS_CTLY,
		SIZ_CTLX + cxF, SIZ_CTLY + cyF, SWP_NOZORDER);

	hDef = DeferWindowPos(hDef, hDAMWnd, NULL, POS_CTLX, POS_CTLY,
		SIZ_CTLX + cxF, SIZ_CTLY + cyF, SWP_NOZORDER);

	hDef = DeferWindowPos(hDef, hCOMWnd, NULL, POS_COMX, POS_COMY,
		SIZ_COMX + cxF, SIZ_COMY + cyF, SWP_NOZORDER);

	hDef = DeferWindowPos(hDef, hPANWnd, NULL, POS_PANELX, POS_PANELY,
		SIZ_PANELX + cxF, SIZ_PANELY + cyF,	SWP_NOZORDER);

	hDef = DeferWindowPos(hDef, hBTNWnd, NULL, POS_BTNX, POS_BTNY,
		SIZ_BTNX + cxF, SIZ_BTNY + cyF, SWP_NOZORDER);

	hDef = DeferWindowPos(hDef, hMSGWnd, NULL, POS_MSGX, POS_MSGY,
		SIZ_MSGX + cxF, SIZ_MSGY + cyF, SWP_NOZORDER);

	EndDeferWindowPos(hDef);

	MyYield();
	ShowWindow(hLRSWnd, SW_SHOWMINIMIZED);		/* Set default states */
	ShowWindow(hNAVWnd, SW_SHOWMINIMIZED);
	ShowWindow(hPHOWnd, SW_SHOWMINIMIZED);
	ShowWindow(hPHAWnd, SW_SHOWMINIMIZED);
	ShowWindow(hSHEWnd, SW_SHOWMINIMIZED);
	ShowWindow(hDAMWnd, SW_SHOWMINIMIZED);
	ShowWindow(hCOMWnd, SW_SHOWMINIMIZED);
	MyYield();
	ShowWindow(hSRSWnd, SW_NORMAL);
	ShowWindow(hPANWnd, SW_NORMAL);
	ShowWindow(hMSGWnd, SW_NORMAL);
	ShowWindow(hBTNWnd, SW_NORMAL);
	BringWindowToTop(hBTNWnd);
	ArrowCursorX();


} /* End SetDefPos */




/***************************************************************************/

void LOCAL SavePos(void)

//	This function saves all the window positions to our INI file.
//

/***************************************************************************/
{
	RECT		rc;
	WORD 		wStat;
	char		szBuf[32];

	WaitCursorX(hFrameWnd);

	GetWindowParms(hSRSWnd, &rc, &wStat);
	sprintf(szBuf, "%d,%d,%d,%d,%d", wStat, MAKECOORDS(rc));
	WritePrivProfString(hInst, RS_POS, RS_SRSVAL, szBuf, szINI);

	GetWindowParms(hLRSWnd, &rc, &wStat);
	sprintf(szBuf, "%d,%d,%d,%d,%d", wStat, MAKECOORDS(rc));
	WritePrivProfString(hInst, RS_POS, RS_LRSVAL, szBuf, szINI);

	GetWindowParms(hNAVWnd, &rc, &wStat);
	sprintf(szBuf, "%d,%d,%d,%d,%d", wStat, MAKECOORDS(rc));
	WritePrivProfString(hInst, RS_POS, RS_NAVVAL, szBuf, szINI);

	GetWindowParms(hPHOWnd, &rc, &wStat);
	sprintf(szBuf, "%d,%d,%d,%d,%d", wStat, MAKECOORDS(rc));
	WritePrivProfString(hInst, RS_POS, RS_PHOVAL, szBuf, szINI);

	GetWindowParms(hPHAWnd, &rc, &wStat);
	sprintf(szBuf, "%d,%d,%d,%d,%d", wStat, MAKECOORDS(rc));
	WritePrivProfString(hInst, RS_POS, RS_PHAVAL, szBuf, szINI);

	GetWindowParms(hSHEWnd, &rc, &wStat);
	sprintf(szBuf, "%d,%d,%d,%d,%d", wStat, MAKECOORDS(rc));
	WritePrivProfString(hInst, RS_POS, RS_SHEVAL, szBuf, szINI);

	GetWindowParms(hDAMWnd, &rc, &wStat);
	sprintf(szBuf, "%d,%d,%d,%d,%d", wStat, MAKECOORDS(rc));
	WritePrivProfString(hInst, RS_POS, RS_DAMVAL, szBuf, szINI);

	GetWindowParms(hCOMWnd, &rc, &wStat);
	sprintf(szBuf, "%d,%d,%d,%d,%d", wStat, MAKECOORDS(rc));
	WritePrivProfString(hInst, RS_POS, RS_COMVAL, szBuf, szINI);

	GetWindowParms(hBTNWnd, &rc, &wStat);
	sprintf(szBuf, "%d,%d,%d,%d,%d", wStat, MAKECOORDS(rc));
	WritePrivProfString(hInst, RS_POS, RS_BTNVAL, szBuf, szINI);

	GetWindowParms(hPANWnd, &rc, &wStat);
	sprintf(szBuf, "%d,%d,%d,%d,%d", wStat, MAKECOORDS(rc));
	WritePrivProfString(hInst, RS_POS, RS_PANVAL, szBuf, szINI);

	GetWindowParms(hMSGWnd, &rc, &wStat);
	sprintf(szBuf, "%d,%d,%d,%d,%d", wStat, MAKECOORDS(rc));
	WritePrivProfString(hInst, RS_POS, RS_MSGVAL, szBuf, szINI);

	FlushINI(hInst, szINI);
	ArrowCursorX();

} /* End SavePos */



/***************************************************************************/

void LOCAL RestorePos(void)

//	This function resores all the window positions from our INI file.
//

/***************************************************************************/
{
	RECT		rc;
	WORD 		wStat;
	char		szBuf[32], szNul[2];

	WaitCursorX(hFrameWnd);
	*szNul = 0;

	GetPrivProfString(hInst, RS_POS, RS_SRSVAL, szNul, szBuf, sizeof(szBuf),
		szINI);
	if(*szBuf)
		{
	   sscanf(szBuf, "%d,%d,%d,%d,%d", &wStat, &rc.left, &rc.top,
			&rc.right,	&rc.bottom);
		SetWindowParms(hSRSWnd, &rc, wStat);
		}

	GetPrivProfString(hInst, RS_POS, RS_LRSVAL, szNul, szBuf, sizeof(szBuf),
		szINI);
	if(*szBuf)
		{
	   sscanf(szBuf, "%d,%d,%d,%d,%d", &wStat, &rc.left, &rc.top,
			&rc.right,	&rc.bottom);
		SetWindowParms(hLRSWnd, &rc, wStat);
		}

	GetPrivProfString(hInst, RS_POS, RS_NAVVAL, szNul, szBuf, sizeof(szBuf),
		szINI);
	if(*szBuf)
		{
	   sscanf(szBuf, "%d,%d,%d,%d,%d", &wStat, &rc.left, &rc.top,
			&rc.right,	&rc.bottom);
		SetWindowParms(hNAVWnd, &rc, wStat);
		}

	GetPrivProfString(hInst, RS_POS, RS_PHOVAL, szNul, szBuf, sizeof(szBuf),
		szINI);
	if(*szBuf)
		{
	   sscanf(szBuf, "%d,%d,%d,%d,%d", &wStat, &rc.left, &rc.top,
			&rc.right,	&rc.bottom);
		SetWindowParms(hPHOWnd, &rc, wStat);
		}

	GetPrivProfString(hInst, RS_POS, RS_PHAVAL, szNul, szBuf, sizeof(szBuf),
		szINI);
	if(*szBuf)
		{
	   sscanf(szBuf, "%d,%d,%d,%d,%d", &wStat, &rc.left, &rc.top,
			&rc.right,
			&rc.bottom);
		SetWindowParms(hPHAWnd, &rc, wStat);
		}

	GetPrivProfString(hInst, RS_POS, RS_SHEVAL, szNul, szBuf, sizeof(szBuf),
		szINI);
	if(*szBuf)
		{
	   sscanf(szBuf, "%d,%d,%d,%d,%d", &wStat, &rc.left, &rc.top,
			&rc.right,	&rc.bottom);
		SetWindowParms(hSHEWnd, &rc, wStat);
		}

	GetPrivProfString(hInst, RS_POS, RS_DAMVAL, szNul, szBuf, sizeof(szBuf),
		szINI);
	if(*szBuf)
		{
	   sscanf(szBuf, "%d,%d,%d,%d,%d", &wStat, &rc.left, &rc.top,
			&rc.right,	&rc.bottom);
		SetWindowParms(hDAMWnd, &rc, wStat);
		}

	GetPrivProfString(hInst, RS_POS, RS_COMVAL, szNul, szBuf, sizeof(szBuf),
		szINI);
	if(*szBuf)
		{
	   sscanf(szBuf, "%d,%d,%d,%d,%d", &wStat, &rc.left, &rc.top,
			&rc.right,	&rc.bottom);
		SetWindowParms(hCOMWnd, &rc, wStat);
		}

	GetPrivProfString(hInst, RS_POS, RS_PANVAL, szNul, szBuf, sizeof(szBuf),
		szINI);
	if(*szBuf)
		{
	   sscanf(szBuf, "%d,%d,%d,%d,%d", &wStat, &rc.left, &rc.top,
			&rc.right,	&rc.bottom);
		SetWindowParms(hPANWnd, &rc, wStat);
		}

	GetPrivProfString(hInst, RS_POS, RS_MSGVAL, szNul, szBuf, sizeof(szBuf),
		szINI);
	if(*szBuf)
		{
	   sscanf(szBuf, "%d,%d,%d,%d,%d", &wStat, &rc.left, &rc.top,
			&rc.right,	&rc.bottom);
		SetWindowParms(hMSGWnd, &rc, wStat);
		}

	GetPrivProfString(hInst, RS_POS, RS_BTNVAL, szNul, szBuf, sizeof(szBuf),
		szINI);
	if(*szBuf)
		{
	   sscanf(szBuf, "%d,%d,%d,%d,%d", &wStat, &rc.left, &rc.top,
			&rc.right,	&rc.bottom);
		SetWindowParms(hBTNWnd, &rc, wStat);
		}
	ArrowCursorX();


} /* End RestorePos */
