/************************************
 *		 STAR TREK CLASSIC	 			
 *								  				
 *		general purpose utilities	 	
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
 * Date:			$Date:   20 Feb 1993 09:27:48  $
 * Log File:	$Logfile:   C:/src/st/vcs/utils.c_v  $
 * Log:			$Log:   C:/src/st/vcs/utils.c_v  $
 * 
 *    Rev 1.4   20 Feb 1993 09:27:48   jaworski
 * Initial Revision
 *
 ************************************/

#define	INIT_C
#include "exclude.h"

#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include <toolhelp.h>
#include "joes.h"
#include	<stdlib.h>
#include	<string.h>
#include	<math.h>

#include "st.h"			 								/* Global include files		*/	
#include "resource.h"
#include "protos.h"



/***************************************************************************/
LPSTR ResToString(int nResID)

//  This function reads a resource into a single static string and returns
//	 a far pointer to the string. 

/***************************************************************************/
{

	static char     szBuffer[255];

   *szBuffer = 0;
	LoadString(hInst, nResID, szBuffer, sizeof(szBuffer));
   return (LPSTR)szBuffer;

} /* End ResToString */




/***************************************************************************/

POINT RandomXY(int nQuad, WORD wObj)

//	This function finds an empty sector in a quadrant. It then places the
// wObj object in the sector. The function guarantees that only one object
// exists in any given sector location at any given time. If wObj is 0xffff,
// only random coordinates are returned and no object is written.

// The return value is a POINT structure indicating the X and Y sector
// coordinates found.

/***************************************************************************/
{
	POINT	pt;

	while(1)
		{
		pt.x = rand() & 0x0007;		 			 /* randomly find an empty hole */
		pt.y = rand() & 0x0007;
		if(!qd[nQuad].Obj[pt.y][pt.x])
			break;
		}
	if(wObj != 0xffff)
		qd[nQuad].Obj[pt.y][pt.x] = wObj;
	return pt;

} /* End RandomXY */




/***************************************************************************/

void GetWindowParms(HWND hWnd, LPRECT lprc, LPWORD lpwStat)

//	This function retrieves the status and client coordinates of the
//	specified window. It is used for loading and saving game and INI 
//	position information. The wStat value can be passed directly to
// to ShowWindow().

/***************************************************************************/
{
	WINDOWPLACEMENT wp;

	wp.length = sizeof(WINDOWPLACEMENT);			 

	GetWindowPlacement(hWnd, &wp);
	ScreenToClient(hClientWnd, (LPPOINT)&wp.rcNormalPosition.left);
	ScreenToClient(hClientWnd, (LPPOINT)&wp.rcNormalPosition.right);
	CopyRect(lprc, &wp.rcNormalPosition);
	*lpwStat	= wp.showCmd;

} /* End GetWindowParms */



/***************************************************************************/

void SetWindowParms(HWND hWnd, LPRECT lprc, WORD wStat)

//	This function sets the display and position/size of the
//	specified window. It is used be restore operations from saved games and 
//	by saved INI positions.	The passed RECT is assumed to be the desired
// client coordinates.

/***************************************************************************/
{
	WINDOWPLACEMENT wp;

	wp.length = sizeof(WINDOWPLACEMENT);			 

	GetWindowPlacement(hWnd, &wp);
	wp.showCmd = wStat;
	CopyRect(&wp.rcNormalPosition, lprc);
	ClientToScreen(hClientWnd, (LPPOINT)&wp.rcNormalPosition.left);
	ClientToScreen(hClientWnd, (LPPOINT)&wp.rcNormalPosition.right);
	SetWindowPlacement(hWnd, &wp);

} /* End SetWindowParms */




/***************************************************************************/

int AdjQuad(int nQuad, int nXOffset, int nYOffset)

//	This function returns a quadrant number adjusted for the wrap-around
//	effect of the galaxy. The nQuad parm specifies the center point, and
//	the X and Y offsets indicate the positive or negative quadrant number
// to return relative to nQuad. This function will fail if nXOffset is
// beyond +/- 63, or nYOffset is beyond +\- 7. 

/***************************************************************************/
{
	int	nRetQ;

	nRetQ = nQuad + nXOffset;

	if(nRetQ > 63)
		nRetQ -=64;
	else if(nRetQ < 0)
		nRetQ +=64;
	else if( (nRetQ >> 3) > (nQuad >> 3) )
		nRetQ -=8;
	else if( (nRetQ >> 3) < (nQuad >> 3) )
		nRetQ +=8;

	nRetQ += (nYOffset * 8);
	if(nRetQ > 63)
		nRetQ -=64;
	else if(nRetQ < 0)
		nRetQ +=64;

	return nRetQ;

} /* End AdjQuad */




/***************************************************************************/

void DrawNumber(HDC hDC, PSTR szNum, int nXPos, int nYPos, HBITMAP hDigBmp)

//	This function draws the passed numeric ASCII string in into the bitmap
//	of the passed dc using the digits bitmap. Any non-recognizable characters
// will be drawn as blank.


/***************************************************************************/
{
	HDC		hMemDC, hDigDC;
	HBITMAP	hBM, hOldMemBM, hOldDigBM;
	PSTR		szDig = szNum;
	int		nY, nWidth, nXOff;

	hMemDC = CreateCompatibleDC(hDC);
	hDigDC = CreateCompatibleDC(hDC);
	if(strchr(szNum, '.'))					  /* decrease size on decimal point	*/
		nWidth = (strlen(szNum) - 1) * DG_WIDTH;
	else
		nWidth = strlen(szNum) * DG_WIDTH;

	hBM = CreateCompatibleBitmap(hDC, nWidth, DG_HEIGHT);
	hOldMemBM = SelectBitmap(hMemDC, hBM);
	hOldDigBM = SelectBitmap(hDigDC, hDigBmp);
	nXOff = 0;

	while(*szDig)
		{
		switch(*szDig)
			{
			case '-':nY = DG_DASH;	break;	
			case '9':nY = DG_9;		break;
			case '8':nY = DG_8;		break;
			case '7':nY = DG_7;		break;
			case '6':nY = DG_6;		break;	
			case '5':nY = DG_5;		break;	
			case '4':nY = DG_4;		break;	
			case '3':nY = DG_3;		break;	
			case '2':nY = DG_2;		break;	
			case '1':nY = DG_1;		break;	
			case '0':nY = DG_0;		break;	
			case '.':nY = DG_DEC;	break;	
			default :nY = DG_BLANK;	break;
			}
		if(nY == DG_DEC)
   		StretchBlt(hMemDC, nXOff-DG_WIDTH, 0, DG_WIDTH, DG_HEIGHT,
				hDigDC, 0, nY, DG_WIDTH, DG_HEIGHT, SRCPAINT);
		else
			{
   		StretchBlt(hMemDC, nXOff, 0, DG_WIDTH, DG_HEIGHT, hDigDC, 0, nY,
				DG_WIDTH, DG_HEIGHT, SRCCOPY);
 			nXOff +=DG_WIDTH;
			}
		++szDig;
		}
   BitBlt(hDC, nXPos, nYPos, nWidth, DG_HEIGHT, hMemDC, 0, 0, SRCCOPY);
	SelectBitmap(hMemDC, hOldMemBM);
	SelectBitmap(hDigDC, hOldDigBM);
	DeleteBitmap(hBM);
	DeleteDC(hMemDC);
	DeleteDC(hDigDC);

} /* End DrawNumber */





/***************************************************************************/

void DrawCourseNeedle(HDC hDC, int nX, int nY, int nCourse, int nNewCourse)

//	This function erases and draws the needle in a course bitmap. If
//	nCourse is not negative, the needle indicated by the int will be erased
//	by drawing with a black pen. if nNewCourse is not negative, The needle
// indicated by the int will be drawn with a green pen. Both nCourse and
// nNewCourse specify the value in degrees ( 0 - 359). The nX and nY parms
// specify the center point in client coordinates.

/***************************************************************************/
{
	HPEN	hOldPen;
	POINT	pt;

	if(nCourse >= 0)
		{
		hOldPen = SelectPen(hDC, GetStockObject(BLACK_PEN));
		pt = DegToCoords(nCourse, RADIUS);                /* make x,y coords */
		MoveTo(hDC, nX, nY);
		LineTo(hDC, nX+pt.x, nY+pt.y);
		SelectPen(hDC, hOldPen);
		}

	if(nNewCourse >= 0)
		{
		hOldPen = SelectPen(hDC, hGreenPen);
		pt = DegToCoords(nNewCourse, RADIUS);             /* make x,y coords */
		MoveTo(hDC, nX, nY);
		LineTo(hDC, nX+pt.x, nY+pt.y);
		SelectPen(hDC, hOldPen);
		}

} /* End DrawCourseNeedle */




/***************************************************************************/
POINT DegToCoords(int nDeg, int nRadius)

// This function converts a 0-360 degree value into x,y point coordinates.
// The return value is a POINT structure indicating the end point of the
// angle relative to the center of the passed radius, in MM_TEXT mapping
// mode. 

/***************************************************************************/
{
	#define RAD	(0.0174532925)
	POINT		pt;
	double   fRadius = (double)nRadius;

	nDeg -= 90;					 					  /* convert to Cartesian coords */
	if(nDeg < 0)
		nDeg += 360;

	pt.x = (int)(double)(fRadius * cos(RAD * (double)nDeg));
	pt.y = (int)(double)(fRadius * sin(RAD * (double)nDeg));

	return pt;


} /* End DegToCoords */



/***************************************************************************/
int CoordsToDeg(POINT pt)

// This function converts a POINT into a 0-360 degree value. The return value
// is the degrees representing the angle from 0, 0 center in MM_TEXT mapping
// mode.

/***************************************************************************/
{
	#define DEG	(57.29577951)
	double fDeg;

	if(!(pt.x) && pt.y >= 0)
		return 180;
	if(!(pt.x) && pt.y <  0)
		return 0;
	if(!(pt.y) && pt.x >= 0)
		return 90;
	if(!(pt.y) && pt.x <  0)
		return 270;


	if(pt.x > 0)
		{
		fDeg = atan((double)pt.y/(double)pt.x);
		fDeg *= DEG;
		return (int)fDeg + 90;
		}

	fDeg = atan((double)pt.x/(double)pt.y);
	fDeg *= DEG;
	if(pt.y < 0)
		return (int)(360.0 - fDeg);

	return (int)(180.0 - fDeg);

} /* End CoordsToDeg */




/***************************************************************************/

BOOL ButtonStillDown(int nDelay)

//	This function checks to see if the left mouse button is still down. If
//	it is, nDelay milliseconds occurs, then the function returns TRUE. If the
// mouse button is up (or goes up during the delay), the function immediately
// returns FALSE.

/***************************************************************************/
{
	DWORD	dwTime;

	dwTime = GetTickCountEx();
	dwTime += (DWORD)nDelay;

	if(!(GetAsyncKeyState(VK_LBUTTON) & 0x8000))	 	/* return if mouse up */
		return FALSE;

	do
		{
		MyYield();
		if(!(GetAsyncKeyState(VK_LBUTTON) & 0x8000))
			return FALSE;									/* return if mouse went up */
		} while(GetTickCountEx() < dwTime);

	return TRUE;

} /* End ButtonStillDown */






/***************************************************************************/

void MyYield(void)

//	This function performs normal message processing. It is called throughout
// the program to make sure other applications and sounds keep running.	It is
// also called within all functions that perform delay loops.

/***************************************************************************/
{
	MSG	msg;

//	if(!GetQueueStatus(QS_ALLINPUT))
//		return;

//	LockData(0);
	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
     	if(!TranslateMDISysAccel(hClientWnd, &msg) &&
      	!TranslateAccelerator(hFrameWnd, hAccel, &msg))
         {
         TranslateMessage (&msg);
         DispatchMessage (&msg);
         }
		}
//	UnlockData(0);

} /* End MyYield */





/***************************************************************************/

void Activate(HWND hWnd)

//	This function activates a control window. Depending upon the setting of
// the single control window parm (nINISingle) it may minimize all other
// control windows.

/***************************************************************************/
{

	if(!nINISingle)								 /* Single control flag is OFF */
		{
		ShowWindow(hWnd, SW_SHOWNORMAL);
		BringWindowToTop(hWnd);
		return;
		}

/* Minimize all other children */

	if(hWnd != hSRSWnd && !IsIconic(hSRSWnd))
		ShowWindow(hSRSWnd, SW_MINIMIZE);

	if(hWnd != hLRSWnd && !IsIconic(hLRSWnd))
		ShowWindow(hLRSWnd, SW_MINIMIZE);

	if(hWnd != hNAVWnd && !IsIconic(hNAVWnd))
		ShowWindow(hNAVWnd, SW_MINIMIZE);

	if(hWnd != hSHEWnd && !IsIconic(hSHEWnd))
		ShowWindow(hSHEWnd, SW_MINIMIZE);

	if(hWnd != hPHOWnd && !IsIconic(hPHOWnd))
		ShowWindow(hPHOWnd, SW_MINIMIZE);

	if(hWnd != hPHAWnd && !IsIconic(hPHAWnd))
		CloseWindow(hPHAWnd);

	if(hWnd != hDAMWnd && !IsIconic(hDAMWnd))
		ShowWindow(hDAMWnd, SW_MINIMIZE);

	if(hWnd != hCOMWnd && !IsIconic(hCOMWnd) && !bReRoute)
		ShowWindow(hCOMWnd, SW_MINIMIZE);

	ShowWindow(hWnd, SW_SHOWNORMAL);
	BringWindowToTop(hWnd);


} /* End Activate */





/***************************************************************************/

PSTR CourseToAscii(PSTR szBuf, int nCourse)

//	This function converts the course integer to an ASCII string of one to 
//	three digits, padded by leading spaces. It is faster than sprintf()
//	and needed in tight loops by NAV and PHO routines.	The return value is
// szBuf.

/***************************************************************************/
{
	char	szNum[8];

	strcpy(szBuf, "   ");
	if(nCourse < 0)
		return szBuf;

	itoa(nCourse, szNum, 10);
	switch(strlen(szNum))
		{
		case 1:
			*(szBuf+2) = *szNum;
			break;

		case 2:
			strcpy(szBuf+1, szNum);
			break;

		case 3:
			strcpy(szBuf, szNum);
			break;

		default:
			break;
		}

	return szBuf;


} /* End CourseToAscii */




/***************************************************************************/

void Delay(DWORD dwDelay)

//	This function delays nDelay milliseconds. It calls MyYield() during the
// delay loop, making sure sounds and other apps keep running.

/***************************************************************************/
{
	TIMERINFO tmi;
	DWORD	dwTime;

	tmi.dwSize = (DWORD)sizeof(TIMERINFO);
	TimerCount(&tmi);
	dwTime = tmi.dwmsSinceStart + dwDelay;

	do
		{
		MyYield();
		tmi.dwSize = (DWORD)sizeof(TIMERINFO);
		TimerCount(&tmi);
		} while(tmi.dwmsSinceStart < dwTime);


} /* End Delay */



/***************************************************************************/

DWORD GetTickCountEx(void)

//	This function works just like GetTickCount(), except it uses the
// the TOOLHELP TimerCount function for accuracy to the millisecond.

/***************************************************************************/
{
	TIMERINFO tmi;

	tmi.dwSize = (DWORD)sizeof(TIMERINFO);
	TimerCount(&tmi);
	return tmi.dwmsSinceStart;

} /* End GetTickCountEx */



/***************************************************************************/

int GetWarpFromPoints(int nXStart, int nYStart, int nXEnd, int nYEnd)

//	This function computes the total number of moves between two points. The
//	return value is the number of steps required to reach the end point.


/***************************************************************************/
{
	LINEDDAPROC	lpfnWFPProc;
	int			nWarp = 0;


	lpfnWFPProc = (LINEDDAPROC)MakeProcInstance((FARPROC)WFPProc, hInst);
	LineDDA(nXStart, nYStart, nXEnd, nYEnd, lpfnWFPProc,
		(LPARAM)(LPINT)&nWarp);
	FreeProcInstance((FARPROC)lpfnWFPProc);				/* call the callback	*/

	return nWarp;

} /* End GetWarpFromPoints */




/***************************************************************************/

void CALLBACK _export WFPProc(int nX, int nY, LPSTR lpInt)

//	This is the LineDDA() callback that handles movement calculations from
//	the GetWarpFormPoints() function. it increments the int pointed to by
// LPARAM until the end point is reached.

/***************************************************************************/
{

	*((LPINT)lpInt) +=1;
	
} /* End WFPProc */




/***************************************************************************/

int FindAlien(int nQuad, int nX, int nY)

//	This function finds the entry in the ALIENSTRUCT pAL with a matching
// quadrant and sector x,y. If the alien can't be found, the return value
//	value is -1. Otherwise, the return value is an index of the	ALIENINFO
// struct in the global array pAL[].

/***************************************************************************/
{
	int	i;

	for(i = 0; i < nAlienTotal; i++)
		{
		if(pAL[i].Quad == nQuad && pAL[i].SecX == nX && pAL[i].SecY == nY)
			return i;
		}
	return -1;


} /* End FindAlien */




/***************************************************************************/

void RepairShip(void)

//	This function checks the status of damage systems against the current
//	time and repairs any systems needed. This routine is called from the
//	SetCondition() function.

/***************************************************************************/
{
	int	nRepaired = 0;


	if(fSRSRepair > 0.0 && fSRSRep <= fStarDNow)
		{
		fSRSRepair = 0.0;
		ShowMessage(MG_ONSRS);
		InvalidateRect(hSRSWnd, NULL, 1);
		UpdateWindow(hSRSWnd);
		++nRepaired;
		}
		
	if(fLRSRepair > 0.0 && fLRSRep <= fStarDNow)
		{
		fLRSRepair = 0.0;
		ShowMessage(MG_ONLRS);
		InvalidateRect(hLRSWnd, NULL, 1);
		UpdateWindow(hLRSWnd);
		++nRepaired;
		}

	if(fNAVRepair > 0.0 && fNAVRep <= fStarDNow)
		{
		fNAVRepair = 0.0;
		ShowMessage(MG_ONNAV);
		InvalidateRect(hNAVWnd, NULL, 0);
		UpdateWindow(hNAVWnd);
		++nRepaired;
		}

	if(fSHERepair > 0.0 && fSHERep <= fStarDNow)
		{
		fSHERepair = 0.0;
		ShowMessage(MG_ONSHE);
		InvalidateRect(hSHEWnd, NULL, 1);
		UpdateWindow(hSHEWnd);
		++nRepaired;
		}

	if(fPHORepair > 0.0 && fPHORep <= fStarDNow)
		{
		fPHORepair = 0.0;
		ShowMessage(MG_ONPHO);
		InvalidateRect(hPHOWnd, NULL, 0);
		UpdateWindow(hPHOWnd);
		++nRepaired;
		}

	if(fPHARepair > 0.0 && fPHARep <= fStarDNow)
		{
		fPHARepair = 0.0;
		ShowMessage(MG_ONPHA);
		InvalidateRect(hPHAWnd, NULL, 0);
		UpdateWindow(hPHAWnd);
		++nRepaired;
		}

	if(fCOMRepair > 0.0 && fCOMRep <= fStarDNow)
		{
		fCOMRepair = 0.0;
		ShowMessage(MG_ONCOM);
		InvalidateRect(hCOMWnd, NULL, 0);
		UpdateWindow(hCOMWnd);
		++nRepaired;
		}

	if(fDAMRepair > 0.0 && fDAMRep <= fStarDNow)
		{
		fDAMRepair = 0.0;
		ShowMessage(MG_ONDAM);
		InvalidateRect(hDAMWnd, NULL, 0);
		UpdateWindow(hDAMWnd);
		++nRepaired;
		}

	if(fPANRepair > 0.0 && fPANRep <= fStarDNow)
		{
		fPANRepair = 0.0;
		ShowWindow(hPANWnd, SW_NORMAL);
		ShowMessage(MG_ONPAN);
		InvalidateRect(hPANWnd, NULL, 0);
		UpdateWindow(hPANWnd);
		++nRepaired;
		}

	if(fMSGRepair > 0.0 && fMSGRep <= fStarDNow)
		{
		fMSGRepair = 0.0;
		ShowWindow(hMSGWnd, SW_NORMAL);
		InvalidateRect(hMSGWnd, NULL, 1);
		UpdateWindow(hMSGWnd);
		ShowMessage(MG_ONMSG);
		++nRepaired;
		}

	if(nRepaired && !fDAMRepair && !IsIconic(hDAMWnd))
		InvalidateRect(hDAMWnd, NULL, 0);


} /* End RepairShip */





/***************************************************************************/

void PauseGame(void)

//	This function displays the paused game window.

/***************************************************************************/
{
	MSG		msg;
	WNDCLASS wcParms;
	RECT		rcP, rcScreen;
	HWND	   hPauseWnd;
	char		szClass[16];

	KillSound();
	LoadString(hInst, RS_EXTCLASS, szClass, sizeof(szClass)); 
	SetRect(&rcP, 0, 0, 200, 100);
	GetWindowRect(hFrameWnd, &rcScreen);
	CenterRect(&rcP, &rcScreen);

/* register and create */

	wcParms.style          = CS_SAVEBITS | CS_BYTEALIGNWINDOW;
   wcParms.lpfnWndProc    = (WNDPROC)PauseDefProc;
   wcParms.cbClsExtra     = 0;
   wcParms.cbWndExtra     = 0;
   wcParms.hInstance      = hInst;
   wcParms.hIcon          = NULL;
   wcParms.hCursor        = LoadCursor(NULL, IDC_ARROW);
   wcParms.hbrBackground  = GetStockObject(WHITE_BRUSH);
   wcParms.lpszMenuName   = NULL;
   wcParms.lpszClassName  = (LPSTR) szClass;  
	RegisterClass(&wcParms);

	hPauseWnd = CreateWindow(szClass, szClass, WS_POPUP | WS_BORDER,
		rcP.left, rcP.top, RCWIDTH(rcP), RCHEIGHT(rcP),
		hFrameWnd, NULL, hInst, NULL);
	ShowWindow(hPauseWnd, SW_NORMAL);
	UpdateWindow(hPauseWnd);
	EnableWindow(hFrameWnd, 0);

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
	EnableWindow(hFrameWnd, 1);
	DestroyWindow(hPauseWnd);
	UnregisterClass(szClass, hInst);
	bPaused = FALSE;
	EnableMenuItem(hMenuFL, IDM_PAUSE, MF_BYCOMMAND | MF_ENABLED);
			

} /* End PauseGame */




/*************************************************************************/

LRESULT CALLBACK _export PauseDefProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)

// This is the display window definition procedure for Pause.
                                                           
/**************************************************************************/
{

	HDC			hDC;
	PAINTSTRUCT	ps;
	RECT		  	rc;
	HBRUSH		hBr;

	switch(msg)
   	{
		case WM_PAINT:								  			/* paint bitmap & text	*/
			hDC = BeginPaint(hWnd, &ps);

			GetClientRect(hWnd, &rc);
			hBr = CreateSolidBrush(RGB_RED);
			FrameRect(hDC, &rc, hBr);
			InflateRect(&rc, -2, -2); FrameRect(hDC, &rc, hBr);
			InflateRect(&rc, -2, -2); FrameRect(hDC, &rc, hBr);
			DeleteBrush(hBr);


			SetTextColor(hDC, RGB_RED);
			DrawText(hDC, "PAUSE GAME", -1, &rc,
				DT_NOPREFIX | DT_SINGLELINE | DT_CENTER | DT_VCENTER);

			EndPaint(hWnd, &ps);
 			break;


     	default:
         return DefWindowProc(hWnd, msg, wParam, lParam);
         break;
     	}

	return (LRESULT)0;

} /* End PauseDefProc */





/***************************************************************************/

PSTR SkillToAscii(int nSK)

//	This function returns a pointer to an ASCII skill name based on the
// passed integer skill.

/***************************************************************************/
{

	switch(nSK)
		{
		case 5: return (PSTR)"Admiral"; 	break;
		case 4: return (PSTR)"Captain";  break;
		case 3: return (PSTR)"1stOfficer";break;
		case 2: return (PSTR)"Officer"; 	break;
		case 1: return (PSTR)"Yeoman";  	break;
		case 0: return (PSTR)"Crewman"; 	break;
		}
	return (PSTR)"";

} /* End SkillToAscii */





/***************************************************************************/

BOOL BlockedByStar(int nXStart, int nYStart, LPINT lpnXEnd, LPINT lpnYEnd)

//	This function plots successive sectors in the Enterprise quadrant to
//	determine whether or not the line is blocked by a star. It is called by
//	phaser fire routines to see if the target ship is blocked. 

// Upon entry, the starting point and a pointer to the requested ending point
// is passed. If a block does occur, the function returns TRUE and overwrites
// the ending values with the blocked destination. If the return value is
// FALSE, the line is not blocked and the ending values are not modified.

/***************************************************************************/
{
	LINEDDAPROC lpfn;
	POINT			pt;


	pt.x = pt.y = -1;
	lpfn = (LINEDDAPROC)MakeProcInstance((FARPROC)BlockProc, hInst);
	LineDDA(nXStart, nYStart, *lpnXEnd, *lpnYEnd, lpfn, (LPARAM)(LPPOINT)&pt);
	FreeProcInstance((FARPROC)lpfn);

	if(pt.x != -1)						 				/* if not -1 it was modified	*/
		{
		*lpnXEnd = pt.x;
		*lpnYEnd = pt.y;
		return TRUE;
		}

	return FALSE;



} /* End BlockedByStar */





/***************************************************************************/

void CALLBACK _export BlockProc(int nX, int nY, LPSTR lp)

//	This is the LineDDA() callback that handles the BlockedByStar() function.
//	It checks to see if the next x,y sector contains a star. If it does,
// the function modifies the POINT with the coordinates.

/***************************************************************************/
{
	LPPOINT	lppt;

	lppt = (LPPOINT)lp;
	if(lppt->x != -1)
		return;

	switch(GetQObj(nY, nX))				 	  /* see if an object is in the way */
		{
		case OBJ_STAR0:
		case OBJ_STAR1:
		case OBJ_STAR2:
			lppt->x = nX;				  		  /* its a star */
			lppt->y = nY;
			break;
		}


} /* End BlockProc */
