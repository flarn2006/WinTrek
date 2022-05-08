/************************************
 *		 STAR TREK CLASSIC	 			
 *								  				
 *  Panel MDI Window processing
 *
 * Revision:	$Revision:   1.4  $
 * Date:			$Date:   20 Feb 1993 09:27:52  $
 * Log File:	$Logfile:   C:/src/st/vcs/panel.c_v  $
 * Log:			$Log:   C:/src/st/vcs/panel.c_v  $
 * 
 *    Rev 1.4   20 Feb 1993 09:27:52   jaworski
 * Initial Revision
 *								  				
 ************************************/

#define	PANEL_C
#include "exclude.h"

#include <windows.h>
#include <windowsx.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include <commdlg.h>
#include "joes.h"

#include "st.h"			 								/* Global include files		*/	
#include "resource.h"
#include "protos.h"



static HBITMAP		hPanelBmp, hCondBmp;
static FARPROC 	lpfnFlashProc;


#define XY_STARDATE 110, 15								/* Numeric placement */
#define XY_QUAD	  110, 42
#define XY_SECTOR	  110, 69
#define XY_DILITH	  110, 96 
#define XY_SHE		  110, 123
#define XY_PHO		  110, 150
#define XY_ALIENS	  110, 204
#define XY_COND	  121, 173



/*************************************************************************/

LRESULT CALLBACK _export PanelDefProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)

// This is the Panel MDI definition procedure. 
                                                           
/**************************************************************************/
{
	RECT		rc;
	DWORD		dwStyle;


	hPANWnd = hWnd;
	switch(msg)
   	{

		case WM_CREATE:
			dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CAPTION |
				WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZE | WS_MINIMIZEBOX;
			SetWindowLong(hWnd, GWL_STYLE, (LPARAM)dwStyle);
			hPanelBmp = LoadBitmap(hLib, MAKEINTRESOURCE(BMP_PANEL));
			hCondBmp  = LoadBitmap(hLib, MAKEINTRESOURCE(BMP_ALERT));
			SetCondition();
			break;

		case WM_ICONERASEBKGND:
			GetClientRect(hWnd, &rc);
			FillRect((HDC)wParam, &rc, GetStockObject(BLACK_BRUSH));
			break;

		case WM_PAINT:
			PaintPanel(hWnd);
			break;


		case WM_GETMINMAXINFO:
			(((LPPOINT)(lParam))+3)->x = (((LPPOINT)(lParam))+4)->x =
				SIZ_PANELX  + GetSystemMetrics(SM_CXFRAME);
         (((LPPOINT)(lParam))+3)->y = (((LPPOINT)(lParam))+4)->y =
				SIZ_PANELY + GetSystemMetrics(SM_CYCAPTION) +
						GetSystemMetrics(SM_CYFRAME);
        	return DefMDIChildProc(hWnd, msg, wParam, lParam);

		case WM_DESTROY:
			if(lpfnFlashProc)					  				/* free flash timer */
				{
				KillTimer(hPANWnd, 1);
				FreeProcInstance(lpfnFlashProc);
				lpfnFlashProc = NULL;
				}
			DeleteBitmap(hPanelBmp);
			DeleteBitmap(hCondBmp);
			break;


     	default:
         	return DefMDIChildProc(hWnd, msg, wParam, lParam);
         	break;
     	}

	return (LRESULT)0;

} /* End PanelDefProc */




/***************************************************************************/

void LOCAL PaintPanel(HWND hWnd)

//	This function draws the panel and updates numeric displays.

/***************************************************************************/
{
	HDC			hDC, hMemDC;
	PAINTSTRUCT	ps;
	HBITMAP		hCpyBM, hOldCpyBM;
	char			szBuf[8];
	const char	szSpec[] = "%5d";

	hDC = BeginPaint(hWnd, &ps);
	hCpyBM = CopyBitmap(hPanelBmp);
	hMemDC = CreateCompatibleDC(hDC);
	hOldCpyBM = SelectBitmap(hMemDC, hCpyBM);

	if(!fPANRepair)								 				 /* Panel is working */
		{
		sprintf(szBuf, "%5.1f", fStarDNow);
		DrawNumber(hMemDC, szBuf, XY_STARDATE, hDigitsRBmp);

		sprintf(szBuf, "  %d-%d", XQUAD(nEntQ)+1, YQUAD(nEntQ)+1);
		DrawNumber(hMemDC, szBuf, XY_QUAD, hDigitsRBmp);

		sprintf(szBuf, "  %d-%d", nEntX+1, nEntY+1);
		DrawNumber(hMemDC, szBuf, XY_SECTOR, hDigitsRBmp);

		sprintf(szBuf, szSpec, nDilithium);
		DrawNumber(hMemDC, szBuf, XY_DILITH, hDigitsRBmp);

		sprintf(szBuf, szSpec, nShields);
		DrawNumber(hMemDC, szBuf, XY_SHE, hDigitsRBmp);

		sprintf(szBuf, szSpec, nPhotonCnt);
		DrawNumber(hMemDC, szBuf, XY_PHO, hDigitsRBmp);

		sprintf(szBuf, szSpec, nAlienCnt);
		DrawNumber(hMemDC, szBuf, XY_ALIENS, hDigitsRBmp);
		DrawCondition(hMemDC);
		}
	else
		{ 																	/* Panel is broke */
		strcpy(szBuf, "     ");
		DrawNumber(hMemDC, szBuf, XY_STARDATE, hDigitsRBmp);
		DrawNumber(hMemDC, szBuf, XY_QUAD, 		hDigitsRBmp);
		DrawNumber(hMemDC, szBuf, XY_SECTOR, 	hDigitsRBmp);
		DrawNumber(hMemDC, szBuf, XY_DILITH, 	hDigitsRBmp);
		DrawNumber(hMemDC, szBuf, XY_SHE, 		hDigitsRBmp);
		DrawNumber(hMemDC, szBuf, XY_PHO, 		hDigitsRBmp);
		DrawNumber(hMemDC, szBuf, XY_ALIENS, 	hDigitsRBmp);
		}
   BitBlt(hDC, 0, 0, SIZ_PANELX, SIZ_PANELY, hMemDC, 0, 0, SRCCOPY);
	SelectBitmap(hMemDC, hOldCpyBM);
	DeleteDC(hMemDC);
	DeleteBitmap(hCpyBM);


	EndPaint(hWnd, &ps);


} /* End PaintPanel */






/***************************************************************************/

void LOCAL DrawCondition(HDC hDC)

//	This function draws the condition (Red Alert, etc.) in the bitmap
//	of the passed device context.
//

/***************************************************************************/
{
	HDC		hMemDC;
	int		nY;
	HBITMAP	hOldBM;

	switch(Condition)
		{
		case GreenAlert:	 nY = 0;   break;	      /* set bitmap y index */
		case BlueAlert:	 nY = 25;  break;
		case RedAlertOff:	 nY = 50;  break;
		case RedAlertOn:	 nY = 75;  break;
		case YellAlertOff: nY = 100; break;	
		case YellAlertOn:	 nY = 125; break;
		default:				 return;   break;
		}

	hMemDC = CreateCompatibleDC(hDC);
	hOldBM = SelectBitmap(hMemDC, hCondBmp);
   StretchBlt(hDC, XY_COND, 40, 25, hMemDC, 0, nY, 40, 25, SRCCOPY);
	SelectBitmap(hMemDC, hOldBM);
	DeleteDC(hMemDC);


} /* End DrawCondition */



/***************************************************************************/

enum Alert SetCondition(void)

//	This function examines the current location of the Enterprise and sets
//	the global Condition variable. The return value	is the new Condition.

/***************************************************************************/
{
	int	i, j, nX, nY;
	BOOL	bBool;


	RepairShip();													/* repair any damage */

/* first check for game over condition */


	if(nDilithium <= 0)											/* no energy left */
		{
		if(lpfnFlashProc)
			{
			KillTimer(hPANWnd, 1);
			FreeProcInstance(lpfnFlashProc);
			lpfnFlashProc = NULL;
			}
		GameOverType = NoEnergy;
		return (Condition = GameOver);
		}

	if(fStarDNow >= fStarDStart + fMissionL)				/* time is up		  */
		{
		if(lpfnFlashProc)
			{
			KillTimer(hPANWnd, 1);
			FreeProcInstance(lpfnFlashProc);
			lpfnFlashProc = NULL;
			}
		GameOverType = NoTimeLeft;
		return (Condition = GameOver);
		}

	if(!nAlienCnt)
		{
		if(lpfnFlashProc)
			{
			KillTimer(hPANWnd, 1);
			FreeProcInstance(lpfnFlashProc);
			lpfnFlashProc = NULL;
			}
		GameOverType = PlayerWon;
		return (Condition = GameOver);						/* Player won */
		}

	if(fSRSRepair && fLRSRepair && fNAVRepair && fPHORepair &&
		fPHARepair && fSHERepair && fCOMRepair &&	fDAMRepair &&
		fPANRepair && fMSGRepair)							   /* everything broke? */
		{
		if(lpfnFlashProc)
			{
			KillTimer(hPANWnd, 1);
			FreeProcInstance(lpfnFlashProc);
			lpfnFlashProc = NULL;
			}
		GameOverType = NoSystems;
		return (Condition = GameOver);
		}


/* Check for condition red */

	bBool = FALSE;
	for(i = 0; i < 8; i++)		 /* scan every sector in quadrant for aliens */
		{
		for(j = 0; j < 8; j++)
			{
			if(qd[nEntQ].Obj[i][j] & (WORD)OBJ_ALIENMASK)
				{
				bBool = TRUE;
				break;
				}
			}
		if(bBool)
			break;
		}
	if(bBool)
		{
		if(Condition == RedAlertOn || Condition == RedAlertOff)
			return (Condition);					  		  /* already condition red */
		bResetTick = TRUE;
		ShowMessage(MG_RED);								  /* say were in red */
		EnableMenuItem(hMenuFL, IDM_PAUSE, MF_BYCOMMAND | MF_GRAYED);
		EnableMenuItem(hMenuFL, IDM_SAVE,  MF_BYCOMMAND | MF_GRAYED);
		EnableMenuItem(hMenuFL, IDM_SAVEAS,MF_BYCOMMAND | MF_GRAYED);
		PlaySound(SND_REDA, 0);
		if(lpfnFlashProc == NULL)
			{
			lpfnFlashProc = MakeProcInstance((FARPROC)FlashProc, hInst);
			SetTimer(hPANWnd, 1, 500, (TIMERPROC)lpfnFlashProc);
		   InvalidateRect(hPANWnd, NULL, 0);
			return (Condition = RedAlertOff);
			}
		}


/* check to see if we're docked (condition blue) */

	bBool = FALSE;
	for(i = 1; i < 9; i++)		     /* scan surrounding sectors in quadrant */
		{
		switch(i)
			{
			case 1: nX = -1; nY = -1; break;
			case 2: nX =  0; nY = -1; break;
			case 3: nX = +1; nY = -1; break;
			case 4: nX = -1; nY =  0; break;
			case 5: nX = +1; nY =  0; break;
			case 6: nX = -1; nY = +1; break;
			case 7: nX =  0; nY = +1; break;
			case 8: nX = +1; nY = +1; break;
			}
		nX += nEntX;
		nY += nEntY;									    /* adjust for wrap-around */
		if(nX > 7 || nX < 0 || nY > 7 || nY < 0)
			continue;

		if(GetQObj(nY, nX) == OBJ_BASE)
			{
			bBool = TRUE;
			break;
			}
		}

	if(bBool)
		{
		if(Condition != BlueAlert)
			{
			if(lpfnFlashProc)								 	 /* set condition blue */
				{
				KillTimer(hPANWnd, 1);
				FreeProcInstance(lpfnFlashProc);
				lpfnFlashProc = NULL;
				}
			bReRoute = FALSE;
			nDilithium = 3000;
			nShields	  = 0;	
			nPhotonCnt = 10;
			if(fSRSRepair) fSRSRepair = 0.1;
			if(fLRSRepair) fLRSRepair = 0.1;
			if(fNAVRepair) fNAVRepair = 0.1;
			if(fPHORepair) fPHORepair = 0.1;
			if(fPHARepair) fPHARepair = 0.1;
			if(fSHERepair) fSHERepair = 0.1;
			if(fCOMRepair) fCOMRepair = 0.1;
			if(fDAMRepair) fDAMRepair = 0.1;
			if(fPANRepair) fPANRepair = 0.1;
			if(fMSGRepair) fMSGRepair = 0.1;
			fSRSRep = fLRSRep = fNAVRep = fPHORep =   		/* all fixed */
			fPHARep = fSHERep = fCOMRep =
			fDAMRep = fPANRep = fMSGRep = fStarDNow;
			EnableMenuItem(hMenuFL, IDM_PAUSE, MF_BYCOMMAND | MF_ENABLED);
			EnableMenuItem(hMenuFL, IDM_SAVE,  MF_BYCOMMAND | MF_ENABLED);
			EnableMenuItem(hMenuFL, IDM_SAVEAS,MF_BYCOMMAND | MF_ENABLED);
			ShowMessage(MG_DOCKED);				/* say were docked on 1st time */
			InvalidateRect(hPANWnd, NULL, 0);
			RepairShip();
			UpdateLRS(nEntQ);
			FreeLockedProc();
			}
		return (Condition = BlueAlert);
		}


/* Check for condition yellow */

	if(nDilithium <= 300)							  					 /* low energy */
		{
		if(Condition == YellAlertOn || Condition == YellAlertOff)
			return (Condition);					    /* already condition yellow */

		FreeLockedProc();
		EnableMenuItem(hMenuFL, IDM_PAUSE, MF_BYCOMMAND | MF_ENABLED);
		EnableMenuItem(hMenuFL, IDM_SAVE,  MF_BYCOMMAND | MF_ENABLED);
		EnableMenuItem(hMenuFL, IDM_SAVEAS,MF_BYCOMMAND | MF_ENABLED);
		ShowMessage(MG_YELLOW);								  /* say were in yellow */
		if(lpfnFlashProc == NULL)
			{
			lpfnFlashProc = MakeProcInstance((FARPROC)FlashProc, hInst);
			SetTimer(hPANWnd, 1, 500, (TIMERPROC)lpfnFlashProc);
	    	InvalidateRect(hPANWnd, NULL, 0);
  			return (Condition = YellAlertOff);
			}
		}




/* it must be condition green */

	if(Condition == GreenAlert)
		return (Condition);					  		/* already condition green */
	
	FreeLockedProc();
	EnableMenuItem(hMenuFL, IDM_PAUSE, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(hMenuFL, IDM_SAVE,  MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(hMenuFL, IDM_SAVEAS,MF_BYCOMMAND | MF_ENABLED);
	if(lpfnFlashProc)
		{
		KillTimer(hPANWnd, 1);
		FreeProcInstance(lpfnFlashProc);
		lpfnFlashProc = NULL;
		}
	ShowMessage(MG_GREEN);								  /* say were in green */
	InvalidateRect(hPANWnd, NULL, 0);
	return (Condition = GreenAlert);


} /* End SetCondition */





/***************************************************************************/

WORD CALLBACK _export FlashProc(HWND hWnd, WORD wMsg, int TimID, DWORD dwTim)

//	This is the callback function for our flashing condition indicator. It
// cycles between the on and off states. This routine is never called unless
// its a red or yellow alert. 

/***************************************************************************/
{
	static WORD	wToggle;

	if(fPANRepair || bPaused)
		return 0;

	if(wToggle++ & 0x0001)
		{
		switch(Condition)
			{
			case RedAlertOn:					/* toggle to alternate bitmap */
				Condition = RedAlertOff;
				break;
			case RedAlertOff:
				Condition = RedAlertOn;
				break;
			case YellAlertOn:
				Condition = YellAlertOff;
				break;
			case YellAlertOff:
				Condition = YellAlertOn;
				break;
			default:
				return 0;
				break;
			}
		DrawCondition(GetDC(hPANWnd));
		}
	return 0;

} /* End FlashProc */



