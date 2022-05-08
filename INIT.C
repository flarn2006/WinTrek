/************************************
 *		 STAR TREK CLASSIC	 			
 *								  				
 *		initialization utilities	 	
 * Revision:	$Revision:   1.4  $
 * Date:			$Date:   20 Feb 1993 09:27:50  $
 * Log File:	$Logfile:   C:/src/st/vcs/init.c_v  $
 * Log:			$Log:   C:/src/st/vcs/init.c_v  $
 * 
 *    Rev 1.4   20 Feb 1993 09:27:50   jaworski
 * Initial Revision
 *
 ************************************/

#define	INIT_C
#include "exclude.h"

#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include "joes.h"
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

#include "st.h"			 								/* Global include files		*/	
#include "resource.h"
#include "protos.h"





/***************************************************************************/

void LoadINI(void)

//	This functions loads up previously saved WIN.INI settings

/***************************************************************************/
{

	nINISkill = GetPrivProfInt(hInst, RS_GEN, RS_HSKILL, 0, szINI);
	if(nINISkill > 5 || nINISkill	< 0)
		nINISkill = 0;
	nINIAutoSRS = GetPrivBoolINI(hInst, RS_GEN, RS_AUTOSRS,1, szINI);
	nINISingle  = GetPrivBoolINI(hInst, RS_GEN, RS_SINGLE, 1, szINI);
	nINIPauseI  = GetPrivBoolINI(hInst, RS_GEN, RS_PAUSEI, 0, szINI);
	nINISounds  = GetPrivBoolINI(hInst, RS_GEN, RS_SOUND,  1, szINI);
	

} /* End LoadINI */




/***************************************************************************/

void SaveINI(void)

//	This functions loads up previously saved WIN.INI settings

/***************************************************************************/
{

	WritePrivProfInt(hInst, RS_GEN, RS_HSKILL, nINISkill, szINI);
	WritePrivBoolINI(hInst, RS_GEN, RS_AUTOSRS, nINIAutoSRS, 0, szINI);
	WritePrivBoolINI(hInst, RS_GEN, RS_SINGLE,  nINISingle,  0, szINI);
	WritePrivBoolINI(hInst, RS_GEN, RS_PAUSEI,  nINIPauseI,  0, szINI);
	WritePrivBoolINI(hInst, RS_GEN, RS_SOUND,   nINISounds,  0, szINI);
	FlushINI(hInst, szINI);


} /* End LoadINI */




/***************************************************************************/

void LoadUpStrings(void)

//	This function loads up our static strings.
//
//

/***************************************************************************/
{

	szFrameClass=	"WinTrek";
	szPanelClass=	"Status";					
	szMsgClass	=	"Messages";
	szBtnClass	= 	"Controls";
	szSRSClass	= 	"Short Range Scan";
	szLRSClass	= 	"Long Range Scan";
	szNAVClass	= 	"Navigation";
	szPHOClass	= 	"Photon Torpedoes";
	szPHAClass	= 	"Phasers";
	szSHEClass	= 	"Shields";
	szDAMClass	= 	"Damage Report";
	szCOMClass	= 	"Computer";
	szEXTClass  =  "SText";


} /* End LoadUpStrings */




/***************************************************************************/

void SetupClasses(void)

//	This functions registers all the classes for this app. 

/***************************************************************************/
{
	WNDCLASS       wcParms;



/* register frame class */

	wcParms.style          = CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNWINDOW;;
   wcParms.lpfnWndProc    = (WNDPROC)FrameDefProc;
   wcParms.cbClsExtra     = 0;
   wcParms.cbWndExtra     = 0;
   wcParms.hInstance      = hInst;
   wcParms.hIcon          = LoadIcon(hInst, MAKEINTRESOURCE(ICN_MAIN));
   wcParms.hCursor        = LoadCursor(NULL, IDC_ARROW);
   wcParms.hbrBackground  = GetStockObject(BLACK_BRUSH);
   wcParms.lpszMenuName   = NULL;
   wcParms.lpszClassName  = (LPSTR)szFrameClass;  
	RegisterClass(&wcParms);

/* register Panel class */

   wcParms.style          = CS_HREDRAW | CS_VREDRAW | CS_NOCLOSE | CS_CLASSDC |
									 CS_BYTEALIGNWINDOW;
   wcParms.lpfnWndProc    = (WNDPROC)PanelDefProc;
   wcParms.cbClsExtra     = 0;
   wcParms.cbWndExtra     = 0;
   wcParms.hInstance      = hInst;
   wcParms.hIcon          = LoadIcon(hLib, MAKEINTRESOURCE(ICN_PANEL));
   wcParms.hCursor        = LoadCursor(NULL, IDC_ARROW);
   wcParms.hbrBackground  = GetStockObject(HOLLOW_BRUSH);
   wcParms.lpszMenuName   = NULL;
   wcParms.lpszClassName  = (LPSTR)szPanelClass;
   RegisterClass(&wcParms);

/* register Message class */
   
   wcParms.style          = CS_HREDRAW | CS_VREDRAW | CS_NOCLOSE | CS_CLASSDC |
									 CS_BYTEALIGNWINDOW | CS_SAVEBITS;
   wcParms.lpfnWndProc    = (WNDPROC)MsgDefProc;
   wcParms.cbClsExtra     = 0;
   wcParms.cbWndExtra     = 0;
   wcParms.hInstance      = hInst;
   wcParms.hIcon          = LoadIcon(hLib, MAKEINTRESOURCE(ICN_MSG));
   wcParms.hCursor        = LoadCursor(NULL, IDC_ARROW);
   wcParms.hbrBackground  = GetStockObject(BLACK_BRUSH);
   wcParms.lpszMenuName   = NULL;
   wcParms.lpszClassName  = (LPSTR)szMsgClass;
   RegisterClass(&wcParms);

/* register Button class */

   wcParms.style          = CS_HREDRAW | CS_VREDRAW | CS_NOCLOSE | CS_CLASSDC |
									 CS_BYTEALIGNWINDOW | CS_SAVEBITS;
   wcParms.lpfnWndProc    = (WNDPROC)BtnDefProc;
   wcParms.cbClsExtra     = 0;
   wcParms.cbWndExtra     = 0;
   wcParms.hInstance      = hInst;
   wcParms.hIcon          = LoadIcon(hLib, MAKEINTRESOURCE(ICN_BTN));
   wcParms.hCursor        = LoadCursor(NULL, IDC_ARROW);
   wcParms.hbrBackground  = GetStockObject(LTGRAY_BRUSH);
   wcParms.lpszMenuName   = NULL;
   wcParms.lpszClassName  = (LPSTR)szBtnClass;
   RegisterClass(&wcParms);

/* register all the con classes */

   wcParms.style          = CS_HREDRAW | CS_VREDRAW | CS_NOCLOSE | CS_CLASSDC |
									 CS_BYTEALIGNWINDOW | CS_SAVEBITS;
   wcParms.lpfnWndProc    = (WNDPROC)SRSDefProc;
   wcParms.cbClsExtra     = 0;
   wcParms.cbWndExtra     = 0;
   wcParms.hInstance      = hInst;
   wcParms.hIcon          = LoadIcon(hLib, MAKEINTRESOURCE(ICN_SRS));
   wcParms.hCursor        = LoadCursor(NULL, IDC_ARROW);
   wcParms.hbrBackground  = GetStockObject(BLACK_BRUSH);
   wcParms.lpszMenuName   = NULL;
   wcParms.lpszClassName  = (LPSTR)szSRSClass;
   RegisterClass(&wcParms);

   wcParms.style          = CS_HREDRAW | CS_VREDRAW | CS_NOCLOSE | CS_CLASSDC | CS_CLASSDC |
									 CS_BYTEALIGNWINDOW | CS_SAVEBITS;
   wcParms.lpfnWndProc    = (WNDPROC)LRSDefProc;
   wcParms.cbClsExtra     = 0;
   wcParms.cbWndExtra     = 0;
   wcParms.hInstance      = hInst;
   wcParms.hIcon          = LoadIcon(hLib, MAKEINTRESOURCE(ICN_LRS));
   wcParms.hCursor        = LoadCursor(NULL, IDC_ARROW);
   wcParms.hbrBackground  = GetStockObject(BLACK_BRUSH);
   wcParms.lpszMenuName   = NULL;
   wcParms.lpszClassName  = (LPSTR)szLRSClass;
   RegisterClass(&wcParms);

   wcParms.style          = CS_HREDRAW | CS_VREDRAW | CS_NOCLOSE | CS_CLASSDC |
									 CS_BYTEALIGNWINDOW | CS_SAVEBITS;
   wcParms.lpfnWndProc    = (WNDPROC)NAVDefProc;
   wcParms.cbClsExtra     = 0;
   wcParms.cbWndExtra     = 0;
   wcParms.hInstance      = hInst;
   wcParms.hIcon          = LoadIcon(hLib, MAKEINTRESOURCE(ICN_NAV));
   wcParms.hCursor        = LoadCursor(NULL, IDC_ARROW);
   wcParms.hbrBackground  = GetStockObject(BLACK_BRUSH);
   wcParms.lpszMenuName   = NULL;
   wcParms.lpszClassName  = (LPSTR)szNAVClass;
   RegisterClass(&wcParms);

   wcParms.style          = CS_HREDRAW | CS_VREDRAW | CS_NOCLOSE | CS_CLASSDC |
									 CS_BYTEALIGNWINDOW | CS_SAVEBITS;
   wcParms.lpfnWndProc    = (WNDPROC)PHODefProc;
   wcParms.cbClsExtra     = 0;
   wcParms.cbWndExtra     = 0;
   wcParms.hInstance      = hInst;
   wcParms.hIcon          = LoadIcon(hLib, MAKEINTRESOURCE(ICN_PHO));
   wcParms.hCursor        = LoadCursor(NULL, IDC_ARROW);
   wcParms.hbrBackground  = GetStockObject(BLACK_BRUSH);
   wcParms.lpszMenuName   = NULL;
   wcParms.lpszClassName  = (LPSTR)szPHOClass;
   RegisterClass(&wcParms);

   wcParms.style          = CS_HREDRAW | CS_VREDRAW | CS_NOCLOSE | CS_CLASSDC |
									 CS_BYTEALIGNWINDOW | CS_SAVEBITS;
   wcParms.lpfnWndProc    = (WNDPROC)PHADefProc;
   wcParms.cbClsExtra     = 0;
   wcParms.cbWndExtra     = 0;
   wcParms.hInstance      = hInst;
   wcParms.hIcon          = LoadIcon(hLib, MAKEINTRESOURCE(ICN_PHA));
   wcParms.hCursor        = LoadCursor(NULL, IDC_ARROW);
   wcParms.hbrBackground  = GetStockObject(BLACK_BRUSH);
   wcParms.lpszMenuName   = NULL;
   wcParms.lpszClassName  = (LPSTR)szPHAClass;
   RegisterClass(&wcParms);

   wcParms.style          = CS_HREDRAW | CS_VREDRAW | CS_NOCLOSE | CS_CLASSDC |
									 CS_BYTEALIGNWINDOW | CS_SAVEBITS;
   wcParms.lpfnWndProc    = (WNDPROC)DAMDefProc;
   wcParms.cbClsExtra     = 0;
   wcParms.cbWndExtra     = 0;
   wcParms.hInstance      = hInst;
   wcParms.hIcon          = LoadIcon(hLib, MAKEINTRESOURCE(ICN_DAM));
   wcParms.hCursor        = LoadCursor(NULL, IDC_ARROW);
   wcParms.hbrBackground  = GetStockObject(BLACK_BRUSH);
   wcParms.lpszMenuName   = NULL;
   wcParms.lpszClassName  = (LPSTR)szDAMClass;
   RegisterClass(&wcParms);

   wcParms.style          = CS_HREDRAW | CS_VREDRAW | CS_NOCLOSE | CS_CLASSDC |
									 CS_BYTEALIGNWINDOW | CS_SAVEBITS;
   wcParms.lpfnWndProc    = (WNDPROC)SHEDefProc;
   wcParms.cbClsExtra     = 0;
   wcParms.cbWndExtra     = 0;
   wcParms.hInstance      = hInst;
   wcParms.hIcon          = LoadIcon(hLib, MAKEINTRESOURCE(ICN_SHE));
   wcParms.hCursor        = LoadCursor(NULL, IDC_ARROW);
   wcParms.hbrBackground  = GetStockObject(BLACK_BRUSH);
   wcParms.lpszMenuName   = NULL;
   wcParms.lpszClassName  = (LPSTR)szSHEClass;
   RegisterClass(&wcParms);

   wcParms.style          = CS_HREDRAW | CS_VREDRAW | CS_NOCLOSE | CS_CLASSDC |
									 CS_BYTEALIGNWINDOW | CS_SAVEBITS;
   wcParms.lpfnWndProc    = (WNDPROC)COMDefProc;
   wcParms.cbClsExtra     = 0;
   wcParms.cbWndExtra     = 0;
   wcParms.hInstance      = hInst;
   wcParms.hIcon          = LoadIcon(hLib, MAKEINTRESOURCE(ICN_COM));
   wcParms.hCursor        = LoadCursor(NULL, IDC_ARROW);
   wcParms.hbrBackground  = GetStockObject(HOLLOW_BRUSH);
   wcParms.lpszMenuName   = NULL;
   wcParms.lpszClassName  = (LPSTR)szCOMClass;
   RegisterClass(&wcParms);

     	
} /* End SetupClasses */




/***************************************************************************/

void DestroyGame(void)

//	This function destroys the current game in progress. It is called when
//	a new or loaded game is invoked, or the user exits the this app. It 
// destroys all the MDI windows, clears out the AlienList, and frees	up

/***************************************************************************/
{
	int	i, j, k;


	KillSound();

/* Destroy all the MDI children */

	SendMessage(hClientWnd, WM_MDIDESTROY, (WPARAM)hMSGWnd, 0L);
	SendMessage(hClientWnd, WM_MDIDESTROY, (WPARAM)hPANWnd, 0L);
	SendMessage(hClientWnd, WM_MDIDESTROY, (WPARAM)hSRSWnd, 0L);
	SendMessage(hClientWnd, WM_MDIDESTROY, (WPARAM)hLRSWnd, 0L);
	SendMessage(hClientWnd, WM_MDIDESTROY, (WPARAM)hBTNWnd, 0L);
	SendMessage(hClientWnd, WM_MDIDESTROY, (WPARAM)hNAVWnd, 0L);
	SendMessage(hClientWnd, WM_MDIDESTROY, (WPARAM)hSHEWnd, 0L);
	SendMessage(hClientWnd, WM_MDIDESTROY, (WPARAM)hPHOWnd, 0L);
	SendMessage(hClientWnd, WM_MDIDESTROY, (WPARAM)hPHAWnd, 0L);
	SendMessage(hClientWnd, WM_MDIDESTROY, (WPARAM)hDAMWnd, 0L);
	SendMessage(hClientWnd, WM_MDIDESTROY, (WPARAM)hCOMWnd, 0L);
		


/* Reset game parms- in case of end game call */

	nSkill = nINISkill;		 									/* Reset Skill */
	LocalFree((HLOCAL)(PSTR)pAL); 							/* free alien list */
	pAL = NULL;
	for(i = 0; i < 64; i++)
		{
		qd[i].Stars	= 0;											/* zero out quadrant */
		qd[i].Bases	= 0;
		qd[i].Aliens= 0;
		qd[i].Flags = 0;
		for(j = 0; j < 8; j++)
			{
			for(k = 0; k < 8; k++)
				{
				qd[i].Obj[k][j] = 0;
				}
			}
		}

	Condition = NoAlert;							  /* init global enum indicators */
	bResetTick = TRUE;
	GameOverType = NoGame;
	if(IsWindow(hClientWnd))
		SendMessage(hClientWnd, WM_MDISETMENU,	0,
				MAKELONG(hMenuMT, GetSubMenu(hMenuMT, 0)));
	InvalidateRect(hFrameWnd, NULL, TRUE);
	DrawMenuBar(hFrameWnd);

} /* End DestroyGame */




/***************************************************************************/

void CreateGame(void)

//	This function creates a new game. It is called after a new or loaded game
// has initialized all of the game variables.

/***************************************************************************/
{
	MDICREATESTRUCT   mdic;

	SendMessage(hClientWnd, WM_MDISETMENU,	0,
		MAKELONG(hMenuFL, GetSubMenu(hMenuFL, 2)));
	CheckMenuItem(hMenuFL, IDM_AUTOSRS, BOOLCHECK(nINIAutoSRS));
	CheckMenuItem(hMenuFL, IDM_SINGLE,  BOOLCHECK(nINISingle));
	CheckMenuItem(hMenuFL, IDM_PICONIC, BOOLCHECK(nINIPauseI));
	CheckMenuItem(hMenuFL, IDM_SOUNDS,  BOOLCHECK(nINISounds));
	DrawMenuBar(hFrameWnd);


	mdic.szClass = (LPSTR)szSRSClass;
	mdic.szTitle = (LPSTR)szSRSClass;
	mdic.hOwner  = hInst ;
	mdic.x       = POS_SRSX;
	mdic.y       = POS_SRSY;
	mdic.cx      = SIZ_SRSX + GetSystemMetrics(SM_CXFRAME);
	mdic.cy      = SIZ_SRSY + GetSystemMetrics(SM_CYCAPTION) +
						GetSystemMetrics(SM_CYFRAME);
	mdic.style   = WS_MINIMIZE;
	mdic.lParam  = NULL;
	SendMessage(hClientWnd, WM_MDICREATE, 0,(LPARAM)(LPMDICREATESTRUCT)&mdic);

	mdic.szClass = (LPSTR)szLRSClass;
	mdic.szTitle = (LPSTR)szLRSClass;
	mdic.hOwner  = hInst ;
	mdic.x       = POS_LRSX;
	mdic.y       = POS_LRSY;
	mdic.cx      = SIZ_LRSX + GetSystemMetrics(SM_CXFRAME);
	mdic.cy      = SIZ_LRSY + GetSystemMetrics(SM_CYCAPTION) +
						GetSystemMetrics(SM_CYFRAME);
	mdic.style   = WS_MINIMIZE;
	mdic.lParam  = NULL;
	SendMessage(hClientWnd, WM_MDICREATE, 0,(LPARAM)(LPMDICREATESTRUCT)&mdic);
	ShowWindow(hLRSWnd, SW_HIDE);

	mdic.szClass = (LPSTR)szNAVClass;
	mdic.szTitle = (LPSTR)szNAVClass;
	mdic.hOwner  = hInst ;
	mdic.x       = POS_CTLX;
	mdic.y       = POS_CTLY;
	mdic.cx      = SIZ_CTLX + GetSystemMetrics(SM_CXFRAME);
	mdic.cy      = SIZ_CTLY + GetSystemMetrics(SM_CYCAPTION) +
						GetSystemMetrics(SM_CYFRAME);
	mdic.style   = WS_MINIMIZE;
	mdic.lParam  = NULL;
	SendMessage(hClientWnd, WM_MDICREATE, 0,(LPARAM)(LPMDICREATESTRUCT)&mdic);

	mdic.szClass = (LPSTR)szPHOClass;
	mdic.szTitle = (LPSTR)szPHOClass;
	mdic.hOwner  = hInst ;
	mdic.x       = POS_CTLX;
	mdic.y       = POS_CTLY;
	mdic.cx      = SIZ_CTLX + GetSystemMetrics(SM_CXFRAME);
	mdic.cy      = SIZ_CTLY + GetSystemMetrics(SM_CYCAPTION) +
						GetSystemMetrics(SM_CYFRAME);
	mdic.style   = WS_MINIMIZE;
	mdic.lParam  = NULL;
	SendMessage(hClientWnd, WM_MDICREATE, 0,(LPARAM)(LPMDICREATESTRUCT)&mdic);

	mdic.szClass = (LPSTR)szPHAClass;
	mdic.szTitle = (LPSTR)szPHAClass;
	mdic.hOwner  = hInst ;
	mdic.x       = POS_CTLX;
	mdic.y       = POS_CTLY;
	mdic.cx      = SIZ_CTLX + GetSystemMetrics(SM_CXFRAME);
	mdic.cy      = SIZ_CTLY + GetSystemMetrics(SM_CYCAPTION) +
						GetSystemMetrics(SM_CYFRAME);
	mdic.style   = WS_MINIMIZE;
	mdic.lParam  = NULL;
	SendMessage(hClientWnd, WM_MDICREATE, 0,(LPARAM)(LPMDICREATESTRUCT)&mdic);

	mdic.szClass = (LPSTR)szSHEClass;
	mdic.szTitle = (LPSTR)szSHEClass;
	mdic.hOwner  = hInst ;
	mdic.x       = POS_CTLX;
	mdic.y       = POS_CTLY;
	mdic.cx      = SIZ_CTLX + GetSystemMetrics(SM_CXFRAME);
	mdic.cy      = SIZ_CTLY + GetSystemMetrics(SM_CYCAPTION) +
						GetSystemMetrics(SM_CYFRAME);
	mdic.style   = WS_MINIMIZE;
	mdic.lParam  = NULL;
	SendMessage(hClientWnd, WM_MDICREATE, 0,(LPARAM)(LPMDICREATESTRUCT)&mdic);

	mdic.szClass = (LPSTR)szDAMClass;
	mdic.szTitle = (LPSTR)szDAMClass;
	mdic.hOwner  = hInst ;
	mdic.x       = POS_CTLX;
	mdic.y       = POS_CTLY;
	mdic.cx      = SIZ_CTLX + GetSystemMetrics(SM_CXFRAME);
	mdic.cy      = SIZ_CTLY + GetSystemMetrics(SM_CYCAPTION) +
						GetSystemMetrics(SM_CYFRAME);
	mdic.style   = WS_MINIMIZE;
	mdic.lParam  = NULL;
	SendMessage(hClientWnd, WM_MDICREATE, 0,(LPARAM)(LPMDICREATESTRUCT)&mdic);

	mdic.szClass = (LPSTR)szCOMClass;
	mdic.szTitle = (LPSTR)szCOMClass;
	mdic.hOwner  = hInst ;
	mdic.x       = POS_COMX;
	mdic.y       = POS_COMY;
	mdic.cx      = SIZ_COMX + GetSystemMetrics(SM_CXFRAME);
	mdic.cy      = SIZ_COMY + GetSystemMetrics(SM_CYCAPTION) +
						GetSystemMetrics(SM_CYFRAME);
	mdic.style   = WS_MINIMIZE;
	mdic.lParam  = NULL;
	SendMessage(hClientWnd, WM_MDICREATE, 0,(LPARAM)(LPMDICREATESTRUCT)&mdic);
	
	mdic.szClass = (LPSTR)szPanelClass;
	mdic.szTitle = (LPSTR)szPanelClass;
	mdic.hOwner  = hInst;
	mdic.x       = POS_PANELX;
	mdic.y       = POS_PANELY;
	mdic.cx      = SIZ_PANELX + GetSystemMetrics(SM_CXFRAME);
	mdic.cy      = SIZ_PANELY + GetSystemMetrics(SM_CYCAPTION) +
						GetSystemMetrics(SM_CYFRAME);
	mdic.style   = WS_MINIMIZE;
	mdic.lParam  = NULL;
	SendMessage(hClientWnd, WM_MDICREATE, 0,(LPARAM)(LPMDICREATESTRUCT)&mdic);

	mdic.szClass = (LPSTR)szMsgClass;
	mdic.szTitle = (LPSTR)szMsgClass;
	mdic.hOwner  = hInst ;
	mdic.x       = POS_MSGX;
	mdic.y       = POS_MSGY;
	mdic.cx      = SIZ_MSGX + GetSystemMetrics(SM_CXFRAME);
	mdic.cy      = SIZ_MSGY + GetSystemMetrics(SM_CYCAPTION) +
						GetSystemMetrics(SM_CYFRAME);
	mdic.style   = WS_MINIMIZE | WS_VSCROLL;
	mdic.lParam  = NULL;
	SendMessage(hClientWnd, WM_MDICREATE, 0,(LPARAM)(LPMDICREATESTRUCT)&mdic);

	mdic.szClass = (LPSTR)szBtnClass;
	mdic.szTitle = (LPSTR)szBtnClass;
	mdic.hOwner  = hInst ;
	mdic.x       = POS_BTNX;
	mdic.y       = POS_BTNY;
	mdic.cx      = SIZ_BTNX + GetSystemMetrics(SM_CXFRAME);
	mdic.cy      = SIZ_BTNY + GetSystemMetrics(SM_CYCAPTION) +
						GetSystemMetrics(SM_CYFRAME);
	mdic.style   = 0;
	mdic.lParam  = NULL;
	SendMessage(hClientWnd, WM_MDICREATE, 0,(LPARAM)(LPMDICREATESTRUCT)&mdic);


/* Activate default windows */

	InvalidateRect(hClientWnd, NULL, 0);
	PostMessage(hFrameWnd, WM_COMMAND, IDM_DEFPOS, 0L);

} /* End CreateGame */





/***************************************************************************/

void StarFleetMemo(LPSTR lpszMemo)

//	This function displays the star fleet memo window. The data for the window
// is displayed from the lpszMemo parameter.

/***************************************************************************/
{
	MSG		msg;
	WNDCLASS wcParms;
	BITMAP	bm;
	RECT		rcBM, rcScreen;
	HBITMAP	hMemoBmp;
	HWND	   hMemoWnd;

	hMemoBmp = LoadBitmap(hLib, MAKEINTRESOURCE(BMP_MEMO));
	GetObject(hMemoBmp, sizeof(BITMAP), (LPSTR)&bm);
	SetRect(&rcBM, 0, 0, bm.bmWidth+20, bm.bmHeight + 320);
	GetWindowRect(hFrameWnd, &rcScreen);
	CenterRect(&rcBM, &rcScreen);
	DeleteBitmap(hMemoBmp);

/* register and create */

	wcParms.style          = CS_SAVEBITS | CS_BYTEALIGNWINDOW;
   wcParms.lpfnWndProc    = (WNDPROC)MemoDefProc;
   wcParms.cbClsExtra     = 0;
   wcParms.cbWndExtra     = 0;
   wcParms.hInstance      = hInst;
   wcParms.hIcon          = NULL;
   wcParms.hCursor        = LoadCursor(NULL, IDC_ARROW);
   wcParms.hbrBackground  = GetStockObject(WHITE_BRUSH);
   wcParms.lpszMenuName   = NULL;
   wcParms.lpszClassName  = (LPSTR)szEXTClass;  
	RegisterClass(&wcParms);

	hMemoWnd = CreateWindow(szEXTClass, szEXTClass, WS_POPUP | WS_BORDER,
		rcBM.left, rcBM.top, RCWIDTH(rcBM), RCHEIGHT(rcBM),
		hFrameWnd, NULL, hInst, lpszMemo);
	ShowWindow(hMemoWnd, SW_NORMAL);
	UpdateWindow(hMemoWnd);
	EnableWindow(hFrameWnd, 0);
	ArrowCursorX();
	SetCapture(hMemoWnd);

	while(1)
		{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
			if(msg.message == WM_CHAR || msg.message == WM_LBUTTONDOWN)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			}
		WaitMessage();
		}
	ReleaseCapture();
	EnableWindow(hFrameWnd, 1);
	DestroyWindow(hMemoWnd);
	UnregisterClass(szEXTClass, hInst);

			

} /* End StarFleetMemo */




/*************************************************************************/

LRESULT CALLBACK _export MemoDefProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)

// This is the memo display window definition procedure. It is destroyed
// if the user hits a key or clicks in the window.
                                                           
/**************************************************************************/
{

	HDC			hDC;
	PAINTSTRUCT	ps;
	LOGFONT		lf;
	RECT		  	rc;
	HFONT			hFont, hOldFont;
	HBITMAP		hMemoBmp;
	HBRUSH		hBr;
	LPCREATESTRUCT lpc;
	static LPSTR lpMemo;

	switch(msg)
   	{
		case WM_CREATE:
			lpc = (LPCREATESTRUCT)lParam;
			lpMemo = lpc->lpCreateParams;
			break;

		case WM_PAINT:								  			/* paint bitmap & text	*/
			hDC = BeginPaint(hWnd, &ps);

			hMemoBmp = LoadBitmap(hLib, MAKEINTRESOURCE(BMP_MEMO));
			DrawBitmap(hDC, 10, 6, hMemoBmp, SRCCOPY);
			DeleteBitmap(hMemoBmp);

			GetClientRect(hWnd, &rc);
			hBr = CreateSolidBrush(RGB_RED);
			FrameRect(hDC, &rc, hBr);
			InflateRect(&rc, -2, -2); FrameRect(hDC, &rc, hBr);
			InflateRect(&rc, -2, -2); FrameRect(hDC, &rc, hBr);
			DeleteBrush(hBr);
			InflateRect(&rc, 4, 4);
			rc.left 	 += 20;
			rc.right	 -= 20;
			rc.top     = rc.bottom - 300;
			rc.bottom -= 20;

			lf.lfHeight				= -14;
			lf.lfWidth				= 0;
			lf.lfEscapement		= 0;
			lf.lfOrientation		= 0;
			lf.lfWeight				= 400;
			lf.lfItalic				= 0;
			lf.lfUnderline			= 0;
			lf.lfStrikeOut			= 0;
			lf.lfCharSet			= 0;
			lf.lfOutPrecision		= 3;
			lf.lfClipPrecision	= 2;
			lf.lfQuality			= 1;
			lf.lfPitchAndFamily	= 49;
			strcpy(lf.lfFaceName, "Courier New");
			hFont = CreateFontIndirect(&lf);
			hOldFont = SelectFont(hDC, hFont);

			SetTextColor(hDC, RGB_BLACK);
			DrawText(hDC, lpMemo, -1, &rc,
				DT_NOPREFIX | DT_WORDBREAK | DT_LEFT | DT_NOCLIP);

			SelectFont(hDC, hOldFont);
			DeleteFont(hFont);
			EndPaint(hWnd, &ps);
 			break;


     	default:
         return DefWindowProc(hWnd, msg, wParam, lParam);
         break;
     	}

	return (LRESULT)0;

} /* End MemoDefProc */




/***************************************************************************/

void GameIsOver(void)

//	This function handles the end of game sequence. At this point, the player
//	has either won or lost the game. If a win	condition, check for scores
//	and allow the player to enter name.	Also compute final score.

/***************************************************************************/
{
	HRSRC		hSrcRes;
	HGLOBAL	hGRes;
	char		szBuf1[512], szBuf2[512];
	int		nScore, i, j;
	char		szPlayers[10][16];
	char		szSkills [10][16];	  				/* File saved scores values */
	char		szScores[10][8];                 
	FILE *	stream;
	DLGPROC	lpfn;


	if(IsIconic(hFrameWnd))
		ShowWindow(hFrameWnd, SW_NORMAL);


	switch(GameOverType)
		{
		case NoEnergy:
			hSrcRes = FindResource(hLib, MAKEINTRESOURCE(RD_ENERGY), RT_RCDATA);
			hGRes = LoadResource(hLib,	hSrcRes);
			_fstrcpy(szBuf1, (LPSTR)LockResource(hGRes));
			sprintf(szBuf2, szBuf1, rand(), fStarDNow);
			*szGFname = 0;
			DestroyGame();
			MyYield();
			PlaySound(SND_SONG, 0);
			StarFleetMemo((LPSTR)szBuf2);
			UnlockResource(hGRes);
			FreeResource(hGRes);
			ShowScores();
			return;

		case NoTimeLeft:
			hSrcRes = FindResource(hLib, MAKEINTRESOURCE(RD_TIME), RT_RCDATA);
			hGRes = LoadResource(hLib,	hSrcRes);
			_fstrcpy(szBuf1, (LPSTR)LockResource(hGRes));
			sprintf(szBuf2, szBuf1, rand());
			*szGFname = 0;
			DestroyGame();
			MyYield();
			PlaySound(SND_SONG, 0);
			StarFleetMemo((LPSTR)szBuf2);
			UnlockResource(hGRes);
			FreeResource(hGRes);
			ShowScores();
			return;


		case NoSystems:
			hSrcRes = FindResource(hLib, MAKEINTRESOURCE(RD_SYS), RT_RCDATA);
			hGRes = LoadResource(hLib,	hSrcRes);
			_fstrcpy(szBuf1, (LPSTR)LockResource(hGRes));
			sprintf(szBuf2, szBuf1, rand(), fStarDNow);
			*szGFname = 0;
			DestroyGame();
			MyYield();
			PlaySound(SND_SONG, 0);
			StarFleetMemo((LPSTR)szBuf2);
			UnlockResource(hGRes);
			FreeResource(hGRes);
			ShowScores();
			return;

		default:
			hSrcRes = FindResource(hLib, MAKEINTRESOURCE(RD_WIN), RT_RCDATA);
			hGRes = LoadResource(hLib,	hSrcRes);
			_fstrcpy(szBuf1, (LPSTR)LockResource(hGRes));
			sprintf(szBuf2, szBuf1, rand());
			*szGFname = 0;
			DestroyGame();
			MyYield();
			PlaySound(SND_SONG, 0);
			StarFleetMemo((LPSTR)szBuf2);
			UnlockResource(hGRes);
			FreeResource(hGRes);
			break;
		}


/* Player has won the game */
/*   Compute the score		*/


	nScore = (int)( (fMissionL-(fStarDNow - fStarDStart)) * 100.0 ) 
				+ (nAlienTotal * 10) 
				- (nStarBaseCnt * 12);
	if(nStarBaseCnt == 1)
		nScore += 100;

	strcpy(szBuf1, szPath);
	strcat(szBuf1, (PSTR)"WINTREK.SCR");
	if((stream = fopen(szBuf1, "r")) == NULL)
		{                                              /* file doesn't exist */
		stream = fopen(szBuf1, "a+");
		for(i = 0; i < 10; i++)
			fprintf(stream, "-\n-\n-\n");	  			/* write all empty records */
		fclose(stream);
		stream = fopen(szBuf1, "r");
		}

	for(i = 0; i < 10; i++)
		fscanf(stream, "%s\n%s\n%s\n", &szPlayers[i][0], &szSkills[i][0],
			&szScores[i][0]);
	fclose(stream);

/* if new high score, add it */

	for(i = 0; i < 10; i++)
		{
		if(nScore > atoi(&szScores[i][0]))
			{											 					/* new high score */
			lpfn = (DLGPROC)MakeProcInstance((FARPROC)HiScoreProc, hInst);
			DialogBoxParam(hInst, MAKEINTRESOURCE(DLG_SCORE), hFrameWnd,
				lpfn, (LPARAM)(LPSTR)&szPlayers[i][0]);
			FreeProcInstance((FARPROC)lpfn);
			if(*(strpbrk(&szPlayers[i][0], "\t ")))
				*(strpbrk(&szPlayers[i][0], "\t ")) = 0;
			strcpy(&szSkills[i][0], SkillToAscii(nSkill));
			itoa(nScore, &szScores[i][0], 10);

			stream = fopen(szBuf1, "w");
			for(j = 0; j < 10; j++)
				fprintf(stream, "%s\n%s\n%s\n", &szPlayers[j][0],
					&szSkills[j][0], &szScores[j][0]);
			fclose(stream);
			break;
			}
		}

/* Display Scores */

	ShowScores();
	return;


} /* End GameIsOver */





/**************************************************************************/

int CALLBACK _export HiScoreProc(HWND hDlg, WORD message, WPARAM wParam, LPARAM lParam)

//  This function handles the Get High Score dialog. The User enters his/her
//  name	into LPARAM.

/**************************************************************************/
{
	static LPSTR lpszName;

	switch(message)
	{
	case WM_INITDIALOG:
		lpszName = (LPSTR)lParam;
		SendDlgItemMessage(hDlg, 100, EM_LIMITTEXT, 15, 0L);
		SetFocus(GetDlgItem(hDlg, 100));
		return FALSE;
		break;

	case WM_COMMAND:
			switch(wParam)								  /* process Control hits */
				{

				case IDOK:									  /* OK Button clicked */
					GetDlgItemText(hDlg, 100, lpszName, 16);
					EndDialog(hDlg, TRUE);
					return TRUE;
					break;

				default:
					return FALSE;
				}
			break;


	case WM_CLOSE:
		PostMessage(hDlg, WM_COMMAND, IDOK, 0L);
		return TRUE;
		break;

	default:
		return FALSE;
		break;
	}

	return FALSE;

} /* End HiScoreProc */






/***************************************************************************/

void LOCAL ShowScores(void)

//	This function displays the scores window.

/***************************************************************************/
{
	MSG		msg;
	WNDCLASS wcParms;
	BITMAP	bm;
	RECT		rcBM, rcScreen;
	HBITMAP	hScoreBmp;
	HWND	   hScoreWnd;

	hScoreBmp = LoadBitmap(hLib, MAKEINTRESOURCE(BMP_SCORE));
	GetObject(hScoreBmp, sizeof(BITMAP), (LPSTR)&bm);
	SetRect(&rcBM, 0, 0, bm.bmWidth+20, bm.bmHeight + 250);
	GetWindowRect(hFrameWnd, &rcScreen);
	CenterRect(&rcBM, &rcScreen);
	DeleteBitmap(hScoreBmp);

/* register and create */

	wcParms.style          = CS_SAVEBITS | CS_BYTEALIGNWINDOW;
   wcParms.lpfnWndProc    = (WNDPROC)ScoresDefProc;
   wcParms.cbClsExtra     = 0;
   wcParms.cbWndExtra     = 0;
   wcParms.hInstance      = hInst;
   wcParms.hIcon          = NULL;
   wcParms.hCursor        = LoadCursor(NULL, IDC_ARROW);
   wcParms.hbrBackground  = GetStockObject(WHITE_BRUSH);
   wcParms.lpszMenuName   = NULL;
   wcParms.lpszClassName  = (LPSTR)szEXTClass;  
	RegisterClass(&wcParms);

	hScoreWnd = CreateWindow(szEXTClass, szEXTClass, WS_POPUP | WS_BORDER,
		rcBM.left, rcBM.top, RCWIDTH(rcBM), RCHEIGHT(rcBM),
		hFrameWnd, NULL, hInst, NULL);
	ShowWindow(hScoreWnd, SW_NORMAL);
	UpdateWindow(hScoreWnd);
	EnableWindow(hFrameWnd, 0);
	ArrowCursorX();
	SetCapture(hScoreWnd);

	while(1)
		{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
			if(msg.message == WM_CHAR || msg.message == WM_LBUTTONDOWN)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			}
		WaitMessage();
		}
	ReleaseCapture();
	EnableWindow(hFrameWnd, 1);
	DestroyWindow(hScoreWnd);
	UnregisterClass(szEXTClass, hInst);

} /* End ShowScores */






/*************************************************************************/

LRESULT CALLBACK _export ScoresDefProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)

// This is the Scores display window definition procedure. It is destroyed
// if the user hits a key or clicks in the window.
                                                           
/**************************************************************************/
{

	HDC			hDC;
	PAINTSTRUCT	ps;
	LOGFONT		lf;
	RECT		  	rc;
	HFONT			hFont, hOldFont;
	HBITMAP		hScoresBmp;
	HBRUSH		hBr;
	int			i, nVert, nLine;
	char			szPlayer[32], szSkill[32], szScore[32];
	char			szBuf[_MAX_PATH];
	FILE *		stream;

	switch(msg)
   	{
		case WM_PAINT:								  			/* paint bitmap & text	*/
			hDC = BeginPaint(hWnd, &ps);

			hScoresBmp = LoadBitmap(hLib, MAKEINTRESOURCE(BMP_SCORE));
			DrawBitmap(hDC, 10, 10, hScoresBmp, SRCCOPY);
			DeleteBitmap(hScoresBmp);

			GetClientRect(hWnd, &rc);
			hBr = CreateSolidBrush(RGB_RED);
			FrameRect(hDC, &rc, hBr);
			InflateRect(&rc, -2, -2); FrameRect(hDC, &rc, hBr);
			InflateRect(&rc, -2, -2); FrameRect(hDC, &rc, hBr);
			DeleteBrush(hBr);
			InflateRect(&rc, 4, 4);
			rc.left 	 += 20;
			rc.right	 -= 20;
			rc.top     = rc.bottom - 280;
			rc.bottom -= 20;

			lf.lfHeight				= -16;
			lf.lfWidth				= 0;
			lf.lfEscapement		= 0;
			lf.lfOrientation		= 0;
			lf.lfWeight				= 700;
			lf.lfItalic				= 0;
			lf.lfUnderline			= 0;
			lf.lfStrikeOut			= 0;
			lf.lfCharSet			= 0;
			lf.lfOutPrecision		= 3;
			lf.lfClipPrecision	= 2;
			lf.lfQuality			= 1;
			lf.lfPitchAndFamily	= 34;
			strcpy(lf.lfFaceName, "Arial");
			hFont = CreateFontIndirect(&lf);
			hOldFont = SelectFont(hDC, hFont);

			SetTextColor(hDC, RGB_BLACK);
			nVert = 120;
			nLine = 2 + HIWORD(GetTextExtent(hDC, "0" , 1));

			strcpy(szBuf, szPath);
			strcat(szBuf, (PSTR)"WINTREK.SCR");
			if((stream = fopen(szBuf, "r")) == NULL)
				{                                      /* file doesn't exist */
				stream = fopen(szBuf, "a+");
				for(i = 0; i < 10; i++)
					fprintf(stream, "-\n-\n-\n");	 /* write all empty records */
				fclose(stream);
				stream = fopen(szBuf, "r");
				}

			for(i = 0; i < 10; i++)
				{
				fscanf(stream, "%s\n%s\n%s\n", szPlayer, szSkill, szScore);
				TextOut(hDC, 45,   nVert, szPlayer,strlen(szPlayer));
				TextOut(hDC, 200, nVert, szSkill, strlen(szSkill));
				TextOut(hDC, 360, nVert, szScore, strlen(szScore));
				nVert += nLine;
				}
			fclose(stream);

			SelectFont(hDC, hOldFont);
			DeleteFont(hFont);
			EndPaint(hWnd, &ps);
 			break;


     	default:
         return DefWindowProc(hWnd, msg, wParam, lParam);
         break;
     	}

	return (LRESULT)0;

} /* End ScoresDefProc */








/***************************************************************************/

void About(void)

//	This function displays the About Window.

/***************************************************************************/
{
	MSG		msg;
	WNDCLASS wcParms;
	BITMAP	bm;
	RECT		rcBM, rcScreen;
	HBITMAP	hAboutBmp;
	HWND	   hAboutWnd;

	LoadString(hInst, RS_EXTCLASS, szEXTClass, sizeof(szEXTClass)); 
	hAboutBmp = LoadBitmap(hLib, MAKEINTRESOURCE(BMP_ABTSHIP));
	GetObject(hAboutBmp, sizeof(BITMAP), (LPSTR)&bm);
	SetRect(&rcBM, 0, 0, bm.bmWidth+20, bm.bmHeight + 100);
	GetWindowRect(hFrameWnd, &rcScreen);
	CenterRect(&rcBM, &rcScreen);
	DeleteBitmap(hAboutBmp);

/* register and create */

	wcParms.style          = CS_SAVEBITS | CS_BYTEALIGNWINDOW;
   wcParms.lpfnWndProc    = (WNDPROC)AboutDefProc;
   wcParms.cbClsExtra     = 0;
   wcParms.cbWndExtra     = 0;
   wcParms.hInstance      = hInst;
   wcParms.hIcon          = NULL;
   wcParms.hCursor        = LoadCursor(NULL, IDC_ARROW);
   wcParms.hbrBackground  = GetStockObject(BLACK_BRUSH);
   wcParms.lpszMenuName   = NULL;
   wcParms.lpszClassName  = (LPSTR) szEXTClass;  
	RegisterClass(&wcParms);

	hAboutWnd = CreateWindow(szEXTClass, szEXTClass, WS_POPUP | WS_BORDER,
		rcBM.left, rcBM.top, RCWIDTH(rcBM), RCHEIGHT(rcBM),
		hFrameWnd, NULL, hInst, NULL);
	ShowWindow(hAboutWnd, SW_NORMAL);
	UpdateWindow(hAboutWnd);
	EnableWindow(hFrameWnd, 0);
	ArrowCursorX();
	SetCapture(hAboutWnd);

	while(1)
		{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
			if(msg.message == WM_CHAR || msg.message == WM_LBUTTONDOWN)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			}
		WaitMessage();
		}
	ReleaseCapture();
	EnableWindow(hFrameWnd, 1);
	DestroyWindow(hAboutWnd);
	UnregisterClass(szEXTClass, hInst);

			

} /* End About */




/*************************************************************************/

LRESULT CALLBACK _export AboutDefProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)

// This is the About display window definition procedure. It is destroyed
// if the user hits a key or clicks in the window.
                                                           
/**************************************************************************/
{

	HDC			hDC;
	PAINTSTRUCT	ps;
	LOGFONT		lf;
	RECT		  	rc;
	HFONT			hFont, hOldFont;
	HBRUSH		hBr;
	char 			szAbout[128];
	LPSTR			lpRes;
	HRSRC			hBMRes;
	HGLOBAL		hGRes;
	HPALETTE		hOldPal, hPal;

	switch(msg)
   	{
		case WM_PAINT:								  			/* paint bitmap & text	*/
			hDC = BeginPaint(hWnd, &ps);

			hBMRes = FindResource(hLib, MAKEINTRESOURCE(BMP_ABTSHIP), RT_BITMAP);
			hGRes = LoadResource(hLib,	hBMRes);
			lpRes = (LPSTR)LockResource(hGRes);
			hPal = CreateBIPalette((LPBITMAPINFOHEADER)lpRes);
			hOldPal = SelectPalette(hDC, hPal, 0);
			RealizePalette(hDC);
			SetDIBitsToDevice(hDC, 10, 6, 358, 87, 0, 0, 0, 87,
				OffsetToBits(lpRes), (LPBITMAPINFO)lpRes, DIB_RGB_COLORS);
		  	SelectPalette(hDC, hOldPal, 0);
			UnlockResource(hGRes);
			FreeResource(hGRes);
			DeletePalette(hPal);


			GetClientRect(hWnd, &rc);
			hBr = CreateSolidBrush(RGB_RED);
			FrameRect(hDC, &rc, hBr);
			InflateRect(&rc, -2, -2); FrameRect(hDC, &rc, hBr);
			InflateRect(&rc, -2, -2); FrameRect(hDC, &rc, hBr);
			DeleteBrush(hBr);
			InflateRect(&rc, 4, 4);
			rc.left 	 += 10;
			rc.right	 -= 10;
			rc.top     = rc.bottom - 80;
			rc.bottom -= 20;

			lf.lfHeight				= -16;
			lf.lfWidth				= 0;
			lf.lfEscapement		= 0;
			lf.lfOrientation		= 0;
			lf.lfWeight				= 700;
			lf.lfItalic				= 0;
			lf.lfUnderline			= 0;
			lf.lfStrikeOut			= 0;
			lf.lfCharSet			= 0;
			lf.lfOutPrecision		= 3;
			lf.lfClipPrecision	= 2;
			lf.lfQuality			= 1;
			lf.lfPitchAndFamily	= 18;
			strcpy(lf.lfFaceName, "Times New Roman");
			hFont = CreateFontIndirect(&lf);
			hOldFont = SelectFont(hDC, hFont);
			SetTextColor(hDC, RGB_RED);
			SetBkColor(hDC, RGB_BLACK);

			LoadString(hInst, RS_ABOUT, szAbout, sizeof(szAbout));
			DrawText(hDC, szAbout, -1, &rc,
				DT_NOPREFIX | DT_WORDBREAK | DT_CENTER | DT_VCENTER | DT_NOCLIP);

			SelectFont(hDC, hOldFont);
			DeleteFont(hFont);
			EndPaint(hWnd, &ps);
 			break;


     	default:
         return DefWindowProc(hWnd, msg, wParam, lParam);
         break;
     	}

	return (LRESULT)0;

} /* End AboutDefProc */
