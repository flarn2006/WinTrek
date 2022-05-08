/************************************
 *		 STAR TREK CLASSIC	 			
 *								  				
 *  Phaser MDI Window processing
 *
 * Revision:	$Revision:   1.4  $
 * Date:			$Date:   20 Feb 1993 09:27:48  $
 * Log File:	$Logfile:   C:/src/st/vcs/pha.c_v  $
 * Log:			$Log:   C:/src/st/vcs/pha.c_v  $
 * 
 *    Rev 1.4   20 Feb 1993 09:27:48   jaworski
 * Initial Revision
 *								  				
 ************************************/

#define	PHA_C
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


#define  XYDISPERSE 45,20			 					/* bitmap x, y positions 	*/
#define  XYTHERMO   10,90
#define  XYDILDIG   28,136
#define  XYPHADIG   259,136
#define  XYBLOCK    130,130
#define  XYEXECUTE  110,180
#define  XYDIL   	  10,125
#define  XYPHA		  240,125
#define  XYCHKWIDE  55,50
#define  XYCHKSING  55,30
#define  XYLOCKED   210,20
#define  XYLOCKONOFF 224,35

#define  RC_DILUP	  143,138,143+16,138+15
#define  RC_PHAUP	  176,138,176+16,138+15
#define  RC_CHKSING 55,30,55+11,30+11
#define  RC_CHKWIDE 55,50,55+11,50+11

#define  SIZ_BLOCK    77,34
#define  SIZ_THERMO   318,30
#define  SIZ_DISPERSE 139,53
#define  SIZ_LOCKED   82,53
#define  SIZ_LOCKEDON 53,23

#define	PID_PHAUP	0							/* 'fake' Control IDs	*/
#define	PID_DILUP	1
#define	PID_SING		2
#define	PID_WIDE		3
#define	PID_EXECUTE	4

				
static HBITMAP	hPhaBmp, hLockedBmp, hSingWideBmp,
					hLockOnBmp, hCheckedBmp, hUnCheckedBmp;

static RECT		rcx[5];									/* PtInRect hittest rect's */
static int		nTmpPha, nTmpDil;			    /* local phaser, dilithium vals */
static const char szSpec[] = "%4d";
static int		nHit;
static TIMERPROC lpfnLockedProc;
static int		bSingle;


/*************************************************************************/
LRESULT CALLBACK _export PHADefProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)

// This is the Phasers MDI definition procedure. 
                                                           
/**************************************************************************/
{
	RECT		rc;
	int		i, nInc;
	DWORD		dwStyle;
	HDC		hDC;
	POINT		pt;
	char		szBuf[8];


	hPHAWnd = hWnd;

	switch(msg)
   	{
		case WM_CREATE:
			dwStyle = WS_CHILD | WS_CLIPSIBLINGS | WS_CAPTION |
				WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZE | WS_MINIMIZEBOX;
			SetWindowLong(hWnd, GWL_STYLE, (LPARAM)dwStyle);

			SetRect(&rcx[PID_PHAUP],RC_PHAUP);
			SetRect(&rcx[PID_DILUP],RC_DILUP);
			SetRect(&rcx[PID_WIDE], RC_CHKWIDE);
			SetRect(&rcx[PID_SING], RC_CHKSING);

			SetRect(&rcx[PID_EXECUTE], XYEXECUTE, SIZ_EXEC);
			rcx[PID_EXECUTE].right  += rcx[PID_EXECUTE].left;
			rcx[PID_EXECUTE].bottom += rcx[PID_EXECUTE].top;
			InflateRect(&rcx[PID_EXECUTE], -1, -1);

			nHit = -1;
			nTmpPha = 0;
			nTmpDil = nDilithium;
			bSingle = TRUE;
			hPhaBmp		 = LoadBitmap(hLib, MAKEINTRESOURCE(BMP_PHASBOX));
			hLockedBmp	 = LoadBitmap(hLib, MAKEINTRESOURCE(BMP_TLOCKBOX));
			hSingWideBmp = LoadBitmap(hLib, MAKEINTRESOURCE(BMP_SINGWIDE));
			hCheckedBmp  = LoadBitmap(hLib, MAKEINTRESOURCE(BMP_CHECKED));
			hUnCheckedBmp= LoadBitmap(hLib, MAKEINTRESOURCE(BMP_UNCHKED));
			hLockOnBmp	 = LoadBitmap(hLib, MAKEINTRESOURCE(BMP_TLOCKON));
			SetLockedProc();
			break;


		case WM_MDIACTIVATE:
			if(wParam)
			{
				nTmpDil = nDilithium - nTmpPha;						/* update values */
				if(nTmpDil <= 50)
					{
					nTmpDil = nDilithium;
					nTmpPha = 0;
					}
				SetLockedProc();
        	}
        	return DefMDIChildProc(hWnd, msg, wParam, lParam);
			break;


		case WM_SIZE:
			if(wParam == SIZE_MINIMIZED)
				FreeLockedProc();
        	return DefMDIChildProc(hWnd, msg, wParam, lParam);


		case WM_GETMINMAXINFO:
			(((LPPOINT)(lParam))+3)->x = (((LPPOINT)(lParam))+4)->x =
				SIZ_CTLX  + GetSystemMetrics(SM_CXFRAME);
         (((LPPOINT)(lParam))+3)->y = (((LPPOINT)(lParam))+4)->y =
				SIZ_CTLY + GetSystemMetrics(SM_CYCAPTION) +
						GetSystemMetrics(SM_CYFRAME);
        	return DefMDIChildProc(hWnd, msg, wParam, lParam);


		case WM_PAINT:
			PaintPHA(hWnd);
			break;


		case WM_KEYDOWN:				  	 /* handle arrowkey generated settings */
			if(fPHARepair || bSRSBusy)
				break;

			switch(wParam)
				{
				case VK_NUMPAD8:
				case VK_UP:
				case '8':
					if(nTmpDil <= 150)
						break;

					nTmpDil -=100;			 			/* increase phasers 100 */
					nTmpPha +=100;
					break;

				case VK_NUMPAD4:
				case VK_LEFT:
				case '4':
					if(nTmpDil <= 50)
						break;

					nTmpDil -=10;			 			/* increase phasers */
					nTmpPha +=10;
					break;

				case VK_NUMPAD2:
				case VK_DOWN:
				case '2':
					if(nTmpPha <=	100)
						{
						nTmpDil += nTmpPha;			/* reduce phasers	100 */
						nTmpPha  = 0;
						}
					else
						{
						nTmpDil +=100;
						nTmpPha -=100;
						}
					break;

				case VK_NUMPAD6:
				case VK_RIGHT:
				case '6':
					if(nTmpPha <=	10)
						{
						nTmpDil += nTmpPha;			/* reduce phasers	*/
						nTmpPha  = 0;
						}
					else
						{
						nTmpDil +=10;
						nTmpPha -=10;
						}
					break;

				case VK_ADD:								/* Increase Course Number */
				case '=':
					if(nTmpDil <= 50)
						break;

					--nTmpDil;			 					/* increase shields 1*/
					++nTmpPha;
	 				break;

				case VK_SUBTRACT:							/* Decrease Course Number */
				case '_':
					if(nTmpPha <=	1)
						{
						nTmpDil += nTmpPha;			   /* reduce shields	1 */
						nTmpPha  = 0;
						}
					else
						{
						++nTmpDil;
						--nTmpPha;
						}
	 				break;


				case VK_RETURN:
					nDilithium = nTmpDil;
					if(nINIAutoSRS)
						Activate(hSRSWnd);			/* handle activation options */
					else
						Activate(hWnd);

					PlaySound(SND_EXEC, 0);
					PostMessage(hSRSWnd, WM_PHAEXEC, (WPARAM)nTmpPha,
						MAKELPARAM(bSingle, 0));
					InvalidateRect(hPANWnd, NULL, FALSE);
					return (LRESULT)0;


				default:
					return (LRESULT)0;
				}
				
			hDC = GetDC(hWnd);
			sprintf(szBuf, szSpec, nTmpDil);
			DrawNumber(hDC, szBuf, XYDILDIG, hDigitsRBmp);
			sprintf(szBuf, szSpec, nTmpPha);
			DrawNumber(hDC, szBuf, XYPHADIG, hDigitsRBmp);
			DrawPhaThermo(hDC);
			ReleaseDC(hWnd, hDC);
			break;


/* Handle fake button releasing */

	  	case WM_LBUTTONUP:
			if(nHit >=0)
				{
				hDC = GetDC(hWnd);
				InvertRect(hDC, &rcx[nHit]);
				ReleaseDC(hWnd, hDC);
				if( (nHit == PID_EXECUTE) && !bSRSBusy &&	
					PtInRect(&rcx[nHit], MAKEPOINT(lParam))) /* Execute Rect? */
					{
					if(!fPHARepair)
						nDilithium = nTmpDil;

					if(nINIAutoSRS)
						Activate(hSRSWnd);			/* handle activation options */
					else
						Activate(hWnd);

					if(!fPHARepair)
						{
						PlaySound(SND_EXEC, 0);
						PostMessage(hSRSWnd, WM_PHAEXEC, (WPARAM)nTmpPha,
							MAKELPARAM(bSingle, 0));
						InvalidateRect(hPANWnd, NULL, FALSE);
						}
					}

				else if(nHit == PID_SING && !bSingle)
					{
					hDC = GetDC(hWnd);
					DrawBitmap(hDC, XYCHKSING, hCheckedBmp,  SRCCOPY);
					DrawBitmap(hDC, XYCHKWIDE, hUnCheckedBmp,SRCCOPY);
					ReleaseDC(hWnd, hDC);
					bSingle = TRUE;
					}

				else if(nHit == PID_WIDE && bSingle)
					{
					hDC = GetDC(hWnd);
					DrawBitmap(hDC, XYCHKSING, hUnCheckedBmp,SRCCOPY);
					DrawBitmap(hDC, XYCHKWIDE, hCheckedBmp,  SRCCOPY);
				  	ReleaseDC(hWnd, hDC);
					bSingle = FALSE;
					}

				nHit = -1;
				}
			break;



/* Handle fake button pressing */


	  	case WM_LBUTTONDOWN:
			if(nHit >= 0 || bSRSBusy) 						 	/* already hiliting? */
				break;

			pt = MAKEPOINT(lParam);
			i = 0;
			while(!PtInRect(&rcx[i], pt))
				{
				if(++i > PID_EXECUTE)
					return (LPARAM)0;							 /* exit if not found */
				}

			if(fPHARepair)
				{								/* if broke, support only Execute btn */
				if(i == PID_EXECUTE)
					{
					nHit = i;			  	     
					hDC = GetDC(hWnd);
					InvertRect(hDC, &rcx[i]);
					ReleaseDC(hWnd, hDC);
					}
				else
					break;
				}
			nHit = i;			  	     
			hDC = GetDC(hWnd);
			InvertRect(hDC, &rcx[i]);
			if(GetKeyState(VK_SHIFT) & 0x8000)
				nInc = 1;				 			      /* set for SHIFT key down */
			else
				nInc = 10;

			switch(nHit)
				{
				case PID_PHAUP:			 					/* Increase Phasers */
					do
						{
						if(nTmpDil <= 50)
							break;
						nTmpDil -=nInc;			 			/* increase dilith  */
						nTmpPha +=nInc;
						SetLockedProc();
						sprintf(szBuf, szSpec, nTmpDil);
						DrawNumber(hDC, szBuf, XYDILDIG, hDigitsRBmp);
						sprintf(szBuf, szSpec, nTmpPha);
						DrawNumber(hDC, szBuf, XYPHADIG, hDigitsRBmp);
						DrawPhaThermo(hDC);
						} while(ButtonStillDown(100));
					break;

				case PID_DILUP:			 					 /* reduce Phasers */
					do
						{
						if(!nTmpPha)
							break;

						if(nTmpPha <= nInc)
							{
							nTmpDil += nTmpPha;						
							nTmpPha = 0;
							}
						else
							{
							nTmpDil +=nInc;
							nTmpPha -=nInc;
							}
						sprintf(szBuf, szSpec, nTmpDil);
						DrawNumber(hDC, szBuf, XYDILDIG, hDigitsRBmp);
						sprintf(szBuf, szSpec, nTmpPha);
						DrawNumber(hDC, szBuf, XYPHADIG, hDigitsRBmp);
						DrawPhaThermo(hDC);
						} while(ButtonStillDown(100));
					break;


				default:
					break;

				} /* End switch(nHit) */

			ReleaseDC(hWnd, hDC);
			break;


		case WM_DESTROY:
			FreeLockedProc();										/* destroy timer */
			DeleteBitmap(hPhaBmp);
			DeleteBitmap(hLockedBmp);
			DeleteBitmap(hSingWideBmp);
			DeleteBitmap(hCheckedBmp);
			DeleteBitmap(hUnCheckedBmp);
			DeleteBitmap(hLockOnBmp);
			break;


		case WM_ICONERASEBKGND:
			GetClientRect(hWnd, &rc);
			FillRect((HDC)wParam, &rc, GetStockObject(BLACK_BRUSH));
			return (LRESULT)0;

     	default:
         	return DefMDIChildProc(hWnd, msg, wParam, lParam);
         	break;
     	}

	return (LRESULT)0;

} /* End PHADefProc */




/***************************************************************************/

void LOCAL PaintPHA(HWND hWnd)

//	This function draws the phasers display and all its bitmaps.

/***************************************************************************/
{
	HDC			hDC;
	PAINTSTRUCT	ps;
	char			szBuf[8];
	RECT			rc, rcDummy;
	HBRUSH		hDkRedBr, hRedBr;


	hDC = BeginPaint(hWnd, &ps);

	SetRect(&rc, XYDISPERSE, SIZ_DISPERSE);			/* Draw Dispersal panel */
	rc.right  += rc.left;
	rc.bottom += rc.top;
	if(IntersectRect(&rcDummy, &ps.rcPaint, &rc))
		DrawBitmap(hDC, XYDISPERSE, hSingWideBmp, SRCCOPY);
	if(!fPHARepair)								  	 			/* Draw Check boxes */
		{
		if(bSingle)
			{
			DrawBitmap(hDC, XYCHKSING, hCheckedBmp,  SRCCOPY);
			DrawBitmap(hDC, XYCHKWIDE, hUnCheckedBmp,SRCCOPY);
			}
		else
			{
			DrawBitmap(hDC, XYCHKSING, hUnCheckedBmp,SRCCOPY);
			DrawBitmap(hDC, XYCHKWIDE, hCheckedBmp,  SRCCOPY);
			}
		}
	else
		{
		DrawBitmap(hDC, XYCHKSING, hUnCheckedBmp, SRCCOPY);
		DrawBitmap(hDC, XYCHKWIDE, hUnCheckedBmp, SRCCOPY);
		}

	

	SetRect(&rc, XYLOCKED, SIZ_LOCKED);				 	  /* Draw Lock-On panel */
	rc.right  += rc.left;
	rc.bottom += rc.top;
	if(IntersectRect(&rcDummy, &ps.rcPaint, &rc))
		DrawBitmap(hDC, XYLOCKED, hLockedBmp, SRCCOPY);

	SetRect(&rc, XYBLOCK, SIZ_BLOCK);				 	    /* Draw Block panel */
	rc.right  += rc.left;
	rc.bottom += rc.top;
	if(IntersectRect(&rcDummy, &ps.rcPaint, &rc))
		DrawBitmap(hDC, XYBLOCK, hLRBlockBmp, SRCCOPY);

	SetRect(&rc, XYDIL, SIZ_DIL);					 	    	/* Draw Dilith panel*/
	rc.right  += rc.left;
	rc.bottom += rc.top;
	if(IntersectRect(&rcDummy, &ps.rcPaint, &rc))
		{
		DrawBitmap(hDC, XYDIL, hDilBmp, SRCCOPY);
		if(!fPHARepair)
			{
			sprintf(szBuf, szSpec, nTmpDil);
			DrawNumber(hDC, szBuf, XYDILDIG, hDigitsRBmp);
			}
		}


	SetRect(&rc, XYPHA, SIZ_DIL);					 	    /* Draw Phaser panel  */
	rc.right  += rc.left;
	rc.bottom += rc.top;
	if(IntersectRect(&rcDummy, &ps.rcPaint, &rc))
		{
		DrawBitmap(hDC, XYPHA, hPhaBmp, SRCCOPY);
		if(!fPHARepair)
			{
			sprintf(szBuf, szSpec, nTmpPha);
			DrawNumber(hDC, szBuf, XYPHADIG, hDigitsRBmp);
			}
		}


	SetRect(&rc, XYEXECUTE, SIZ_EXEC);			  		 /* Draw Execute panel */
	rc.right  += rc.left;
	rc.bottom += rc.top;
	if(IntersectRect(&rcDummy, &ps.rcPaint, &rc))
		DrawBitmap(hDC, XYEXECUTE, hExecuteBmp, SRCCOPY);

	if(fPHARepair)
		EndPaint(hWnd, &ps);


	SetRect(&rc, XYTHERMO, SIZ_THERMO);			  		 /* Draw Thermometer   */
	rc.right  += rc.left;
	rc.bottom += rc.top;
	if(IntersectRect(&rcDummy, &ps.rcPaint, &rc))
		{
		hDkRedBr= CreateSolidBrush(RGB_DKRED);
		hRedBr  = CreateSolidBrush(RGB_RED);
		FillRect(hDC, &rc, hDkRedBr);
		FrameRect(hDC, &rc, hRedBr);
		DeleteBrush(hDkRedBr);
		DeleteBrush(hRedBr);
		DrawPhaThermo(hDC);
		}

	EndPaint(hWnd, &ps);


} /* End PaintPHA */




/***************************************************************************/

void LOCAL DrawPhaThermo(HDC hDC)

//	This function draws the nTmpDil and nTmpPha values within the Thermo
//	rectangle.

/***************************************************************************/
{
	RECT		rc, rcDil, rcPha;
	double	fDil, fPha, fWidth, fInc;
	HBRUSH	hDilBr, hPhaBr;

	hDilBr = CreateSolidBrush(RGB_DKPURPLE);
	hPhaBr = CreateSolidBrush(RGB_DKYELLOW);

	SetRect(&rc, XYTHERMO, SIZ_THERMO);						  		
	rc.right  += rc.left;
	rc.bottom += rc.top;
	InflateRect(&rc, -4, -8);	  							/* compute inner rect */

	fWidth = rc.right - rc.left;
	fDil   = (double)nTmpDil;
	fPha   = (double)nTmpPha;
	fInc   = fWidth / (fDil + fPha);	  					/* build increment val */

	SetRect(&rcDil, rc.left, rc.top, (int)(fInc * fDil) + rc.left, rc.bottom);
	SetRect(&rcPha, rcDil.right, rcDil.top, (int)(fInc * fPha) + rcDil.right,
		rc.bottom);

	FillRect(hDC, &rcDil, hDilBr);
	FillRect(hDC, &rcPha, hPhaBr);

	DeleteBrush(hDilBr);
	DeleteBrush(hPhaBr);


} /* End DrawPhaThermo */






/***************************************************************************/

void FreeLockedProc(void)

//	This function kills our flashing Locked timer (if it's active).

/***************************************************************************/
{
	RECT	rc;

	if(lpfnLockedProc)					  				/* free it? */
		{
		KillTimer(hPHAWnd, 1);
		FreeProcInstance((FARPROC)lpfnLockedProc);
		lpfnLockedProc = NULL;
		SetRect(&rc, XYLOCKED, SIZ_LOCKED);			/* Make the inval rect */
		rc.right  += rc.left;
		rc.bottom += rc.top;
		InvalidateRect(hPHAWnd, &rc, FALSE);
		}

} /* End FreeLockedProc */




/***************************************************************************/

void LOCAL SetLockedProc(void)

//	This function sets our "Locked-On" flasher/timer.

/***************************************************************************/
{

	if(lpfnLockedProc == NULL && qd[nEntQ].Aliens && !fPHARepair)
		{
		lpfnLockedProc = (TIMERPROC)MakeProcInstance((FARPROC)LockedProc, hInst);
		SetTimer(hPHAWnd, 1, 300, lpfnLockedProc);
		}

} /* End SetLockedProc */





/***************************************************************************/

WORD CALLBACK _export LockedProc(HWND hWnd, WORD wMsg, int TimID, DWORD dwTim)

//	This is the callback function for our flashing "Locked-On" indicator.

/***************************************************************************/
{
	HDC			hDC;
	RECT			rc;
	static int	nLockCnt;


	if(IsIconic(hWnd) || fPHARepair || bPaused)			/* iconic or broke */
		return 0;

	hDC = GetDC(hWnd);
	if(nLockCnt++ & 0x0001)
		DrawBitmap(hDC, XYLOCKONOFF, hLockOnBmp, SRCCOPY);
	else
		{
		SetRect(&rc, XYLOCKONOFF, SIZ_LOCKEDON);
		rc.right  += rc.left;
		rc.bottom += rc.top;
		FillRect(hDC, &rc, GetStockObject(BLACK_BRUSH));
		}

	ReleaseDC(hWnd, hDC);
	return 0;

} /* End LockedProc */
