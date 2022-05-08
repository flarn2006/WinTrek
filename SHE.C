/************************************
 *		 STAR TREK CLASSIC	 			
 *								  				
 * Shields MDI Window processing
 *
 * Revision:	$Revision:   1.4  $
 * Date:			$Date:   20 Feb 1993 09:27:52  $
 * Log File:	$Logfile:   C:/src/st/vcs/she.c_v  $
 * Log:			$Log:   C:/src/st/vcs/she.c_v  $
 * 
 *    Rev 1.4   20 Feb 1993 09:27:52   jaworski
 * Initial Revision
 *								  				
 ************************************/

#define	SHE_C
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


#define  XYLART     115,25			 					/* bitmap x, y positions 	*/
#define  XYTHERMO   10,90
#define  XYDILDIG   28,136
#define  XYSHEDIG   259,136
#define  XYBLOCK    130,130
#define  XYEXECUTE  110,180
#define  XYSHEDIL   10,125
#define  XYSHESHE   240,125

#define  RC_DILUP	  143,138,143+16,138+15
#define  RC_SHEUP	  176,138,176+16,138+15

#define  SIZ_BLOCK  77,34
#define  SIZ_SHESD  88,53
#define  SIZ_THERMO 318,30

#define	SID_SHEUP	0							/* 'fake' Control IDs	*/
#define	SID_DILUP	1
#define	SID_EXECUTE	2

				
static HBITMAP	hSheSheBmp;
static RECT		rcx[3];									/* PtInRect hittest rect's */
static int		nTmpShe, nTmpDil;			  /* local shield, dilithium values */
static const char szSpec[] = "%4d";
static int		nHit;
static RECT		rcOval;
static FARPROC lpfnOvalProc;



/*************************************************************************/

LRESULT CALLBACK _export SHEDefProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)

// This is the Shields MDI definition procedure. 
                                                           
/**************************************************************************/
{
	RECT		rc;
	int		i, nInc;
	DWORD		dwStyle;
	HDC		hDC;
	POINT		pt;
	char		szBuf[8];

	hSHEWnd = hWnd;


	switch(msg)
   	{
		case WM_CREATE:
			dwStyle = WS_CHILD | WS_CLIPSIBLINGS | WS_CAPTION |
				WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZE | WS_MINIMIZEBOX;
			SetWindowLong(hWnd, GWL_STYLE, (LPARAM)dwStyle);

			SetRect(&rcx[SID_SHEUP], RC_SHEUP);
			SetRect(&rcx[SID_DILUP], RC_DILUP);

			SetRect(&rcx[SID_EXECUTE], XYEXECUTE, SIZ_EXEC);
			rcx[SID_EXECUTE].right  += rcx[SID_EXECUTE].left;
			rcx[SID_EXECUTE].bottom += rcx[SID_EXECUTE].top;
			InflateRect(&rcx[SID_EXECUTE], -1, -1);

			nHit = -1;
			nTmpShe = nShields;
			nTmpDil = nDilithium;
			hSheSheBmp	= LoadBitmap(hLib, MAKEINTRESOURCE(BMP_SHESHE));
			break;

	 
		case WM_SETFOCUS:
			if(nTmpShe != nShields || nTmpDil != nDilithium) /* update values */
				{
				nTmpShe = nShields; 
				nTmpDil = nDilithium;
				InvalidateRect(hWnd, NULL, FALSE);
				}
			SetOvalProc();
        	return DefMDIChildProc(hWnd, msg, wParam, lParam);


		case WM_SIZE:
			if(wParam == SIZE_MINIMIZED)
				FreeOvalProc();
        	return DefMDIChildProc(hWnd, msg, wParam, lParam);


		case WM_GETMINMAXINFO:
			(((LPPOINT)(lParam))+3)->x = (((LPPOINT)(lParam))+4)->x =
				SIZ_CTLX  + GetSystemMetrics(SM_CXFRAME);
         (((LPPOINT)(lParam))+3)->y = (((LPPOINT)(lParam))+4)->y =
				SIZ_CTLY + GetSystemMetrics(SM_CYCAPTION) +
						GetSystemMetrics(SM_CYFRAME);
        	return DefMDIChildProc(hWnd, msg, wParam, lParam);


		case WM_PAINT:
			PaintSHE(hWnd);
			break;


		case WM_KEYDOWN:				  	 /* handle arrowkey generated settings */
			if(fSHERepair)
				break;
			switch(wParam)
				{
				case VK_NUMPAD8:
				case VK_UP:
				case '8':
					if(nTmpDil <= 150)
						break;

					nTmpDil -=100;			 			/* increase shields 100 */
					nTmpShe +=100;
					SetOvalProc();
					break;

				case VK_NUMPAD4:
				case VK_LEFT:
				case '4':
					if(nTmpDil <= 50)
						break;

					nTmpDil -=10;			 			/* increase shields */
					nTmpShe +=10;
					SetOvalProc();
					break;

				case VK_NUMPAD6:
				case VK_RIGHT:
				case '6':
					if(nTmpShe <=	10)
						{
						nTmpDil += nTmpShe;			/* reduce shields	*/
						nTmpShe  = 0;
						FreeOvalProc();
						}
					else
						{
						nTmpDil +=10;
						nTmpShe -=10;
						SetOvalProc();
						}
					break;

				case VK_NUMPAD2:
				case VK_DOWN:
				case '2':
					if(nTmpShe <=	100)
						{
						nTmpDil += nTmpShe;			/* reduce shields	100 */
						nTmpShe  = 0;
						FreeOvalProc();
						}
					else
						{
						nTmpDil +=100;
						nTmpShe -=100;
						}
					break;

				case VK_ADD:								/* Increase Course Number */
				case '=':
					if(nTmpDil <= 50)
						break;

					--nTmpDil;			 					/* increase shields 1*/
					++nTmpShe;
					SetOvalProc();
	 				break;

				case VK_SUBTRACT:							/* Decrease Course Number */
				case '_':
					if(nTmpShe <=	1)
						{
						nTmpDil += nTmpShe;			   /* reduce shields	1 */
						nTmpShe  = 0;
						}
					else
						{
						++nTmpDil;
						--nTmpShe;
						}
	 				break;


				case VK_RETURN:
					PlaySound(SND_EXEC, 0);
					nDilithium = nTmpDil;
					nShields   = nTmpShe;
					SetCondition();
					InvalidateRect(hPANWnd, NULL, FALSE);
					if(nINIAutoSRS)
						Activate(hSRSWnd);			/* handle activation options */
					else
						Activate(hWnd);
					++bNeedSave;
					return (LRESULT)0;


				default:
					return (LRESULT)0;
				}
				
			hDC = GetDC(hWnd);
			sprintf(szBuf, szSpec, nTmpDil);
			DrawNumber(hDC, szBuf, XYDILDIG, hDigitsRBmp);
			sprintf(szBuf, szSpec, nTmpShe);
			DrawNumber(hDC, szBuf, XYSHEDIG, hDigitsRBmp);
			DrawThermo(hDC);
			ReleaseDC(hWnd, hDC);
			break;


/* Handle fake button releasing */

	  	case WM_LBUTTONUP:
			if(nHit >=0)
				{
				hDC = GetDC(hWnd);
				InvertRect(hDC, &rcx[nHit]);
				ReleaseDC(hWnd, hDC);
				if((nHit == SID_EXECUTE) && 
					PtInRect(&rcx[nHit], MAKEPOINT(lParam))) /* Execute Rect? */
					{
					PlaySound(SND_EXEC, 0);
					nDilithium = nTmpDil;
					nShields   = nTmpShe;
					SetCondition();
					InvalidateRect(hPANWnd, NULL, FALSE);
					if(nINIAutoSRS)
						Activate(hSRSWnd);			/* handle activation options */
					else
						Activate(hWnd);
					++bNeedSave;
					}
				nHit = -1;
				}
			break;



/* Handle fake button pressing */


	  	case WM_LBUTTONDOWN:
			if(nHit >= 0 || bSRSBusy) 						 /* already hiliting */
				break;

			pt = MAKEPOINT(lParam);
			i = 0;
			while(!PtInRect(&rcx[i], pt))
				{
				if(++i > SID_EXECUTE)
					return (LPARAM)0;							 /* exit if not found */
				}

			if(fSHERepair)
				{								/* if broke, support only Execute btn */
				if(i == SID_EXECUTE)
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
				case SID_SHEUP:			 					/* Increase Shields */
					do
						{
						if(nTmpDil <= 50)
							break;
						nTmpDil -=nInc;
						nTmpShe +=nInc;
						SetOvalProc();
						sprintf(szBuf, szSpec, nTmpDil);
						DrawNumber(hDC, szBuf, XYDILDIG, hDigitsRBmp);
						sprintf(szBuf, szSpec, nTmpShe);
						DrawNumber(hDC, szBuf, XYSHEDIG, hDigitsRBmp);
						DrawThermo(hDC);
						} while(ButtonStillDown(100));
					break;

				case SID_DILUP:			 							/* reduce Shields */
					do
						{
						if(!nTmpShe)
							{
							FreeOvalProc();
							break;
							}

						if(nTmpShe <=	nInc)
							{
							nTmpDil += nTmpShe;						
							nTmpShe = 0;
							FreeOvalProc();
							}
						else
							{
							nTmpDil +=nInc;
							nTmpShe -=nInc;
							SetOvalProc();
							}
						sprintf(szBuf, szSpec, nTmpDil);
						DrawNumber(hDC, szBuf, XYDILDIG, hDigitsRBmp);
						sprintf(szBuf, szSpec, nTmpShe);
						DrawNumber(hDC, szBuf, XYSHEDIG, hDigitsRBmp);
						DrawThermo(hDC);
						} while(ButtonStillDown(100));
					break;


				default:
					break;

				} /* End switch(nHit) */

			ReleaseDC(hWnd, hDC);
			break;


		case WM_DESTROY:
			FreeOvalProc();								/* destroy timer */
			DeleteBitmap(hSheSheBmp);
			break;


		case WM_ICONERASEBKGND:
			{
			GetClientRect(hWnd, &rc);
			FillRect((HDC)wParam, &rc, GetStockObject(BLACK_BRUSH));
			return (LRESULT)0;
			}


     	default:
         	return(DefMDIChildProc(hWnd, msg, wParam, lParam));
         	break;
     	}


	return (LRESULT)0;

} /* End SHEDefProc */






/***************************************************************************/

void LOCAL PaintSHE(HWND hWnd)

//	This function draws the shield display an bitmaps.

/***************************************************************************/
{
	HDC			hDC, hMemDC;
	HBITMAP		hOldBM;
	PAINTSTRUCT	ps;
	char			szBuf[8];
	RECT			rc, rcDummy;
	HBRUSH		hDkRedBr, hRedBr;


	hDC = BeginPaint(hWnd, &ps);


	if(!fSHERepair)							 /* if broke- don't draw enterprise */
		{
		SetRect(&rc, XYLART, SIZ_LART);						  /* Draw Enterprise */
		rc.right  -= (rc.right /3);
		rc.bottom -= (rc.bottom/3);
		rc.right  += rc.left;
		rc.bottom += rc.top;
		CopyRect(&rcOval, &rc); 						  /* make a copy for later */
		if(IntersectRect(&rcDummy, &ps.rcPaint, &rc))
			{
			hMemDC = CreateCompatibleDC(hDC);				 	/* reduce it by 2 */
			hOldBM = SelectBitmap(hMemDC, hLineArtBmp);
			StretchBlt(hDC, XYLART, RCWIDTH(rc), RCHEIGHT(rc),
				hMemDC, 0, 0, SIZ_LART, SRCCOPY);
			SelectBitmap(hMemDC, hOldBM);
			DeleteDC(hMemDC);
			}
		}

	SetRect(&rc, XYBLOCK, SIZ_BLOCK);				 	    /* Draw Block panel */
	rc.right  += rc.left;
	rc.bottom += rc.top;
	if(IntersectRect(&rcDummy, &ps.rcPaint, &rc))
		DrawBitmap(hDC, XYBLOCK, hLRBlockBmp, SRCCOPY);

	SetRect(&rc, XYSHEDIL, SIZ_DIL);					 	    /* Draw Dilith panel*/
	rc.right  += rc.left;
	rc.bottom += rc.top;
	if(IntersectRect(&rcDummy, &ps.rcPaint, &rc))
		{
		DrawBitmap(hDC, XYSHEDIL, hDilBmp, SRCCOPY);
		if(!fSHERepair)
			{
			sprintf(szBuf, szSpec, nTmpDil);
			DrawNumber(hDC, szBuf, XYDILDIG, hDigitsRBmp);
			}
		}

	SetRect(&rc, XYSHESHE, SIZ_DIL);					 	    /* Draw Shield panel*/
	rc.right  += rc.left;
	rc.bottom += rc.top;
	if(IntersectRect(&rcDummy, &ps.rcPaint, &rc))
		{
		DrawBitmap(hDC, XYSHESHE, hSheSheBmp, SRCCOPY);
		if(!fSHERepair)
			{
			sprintf(szBuf, szSpec, nTmpShe);
			DrawNumber(hDC, szBuf, XYSHEDIG, hDigitsRBmp);
			}
		}

	SetRect(&rc, XYEXECUTE, SIZ_EXEC);			  		  /* Draw Execute panel */
	rc.right  += rc.left;
	rc.bottom += rc.top;
	if(IntersectRect(&rcDummy, &ps.rcPaint, &rc))
		DrawBitmap(hDC, XYEXECUTE, hExecuteBmp, SRCCOPY);

	if(fSHERepair)
		EndPaint(hWnd, &ps);


	SetRect(&rc, XYTHERMO, SIZ_THERMO);			  		  /* Draw Thermometer   */
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
		DrawThermo(hDC);
		}

	EndPaint(hWnd, &ps);


} /* End PaintSHE */





/***************************************************************************/

void LOCAL DrawThermo(HDC hDC)

//	This function draws the nTmpDil and nTmpShe values within the Thermo
//	rectangle.

/***************************************************************************/
{
	RECT		rc, rcDil, rcShe;
	double	fDil, fShe, fWidth, fInc;
	HBRUSH	hDilBr, hSheBr;

	hDilBr = CreateSolidBrush(RGB_DKPURPLE);
	hSheBr = CreateSolidBrush(RGB_DKAQUA);

	SetRect(&rc, XYTHERMO, SIZ_THERMO);						  		
	rc.right  += rc.left;
	rc.bottom += rc.top;
	InflateRect(&rc, -4, -8);	  							/* compute inner rect */

	fWidth = rc.right - rc.left;
	fDil   = (double)nTmpDil;
	fShe   = (double)nTmpShe;
	fInc   = fWidth / (fDil + fShe);	  					/* build increment val */

	SetRect(&rcDil, rc.left, rc.top, (int)(fInc * fDil) + rc.left, rc.bottom);
	SetRect(&rcShe, rcDil.right, rcDil.top, (int)(fInc * fShe) + rcDil.right,
		rc.bottom);

	FillRect(hDC, &rcDil, hDilBr);
	FillRect(hDC, &rcShe, hSheBr);

	DeleteBrush(hDilBr);
	DeleteBrush(hSheBr);


} /* End DrawThermo */




/***************************************************************************/

void LOCAL FreeOvalProc(void)

//	This function kills our oval timer if it is active.

/***************************************************************************/
{
	RECT	rc;

	if(lpfnOvalProc)					  				/* free it? */
		{
		KillTimer(hSHEWnd, 1);
		FreeProcInstance(lpfnOvalProc);
		lpfnOvalProc = NULL;
		CopyRect(&rc, &rcOval);
		InflateRect(&rc, 15, 15);
		InvalidateRect(hSHEWnd, &rc, TRUE);
		}

} /* End FreeOvalProc */



/***************************************************************************/

void LOCAL SetOvalProc(void)

//	This function sets our oval timer if it is not set, active.

/***************************************************************************/
{


	if(lpfnOvalProc == NULL && nTmpShe && !fSHERepair)
		{
		lpfnOvalProc = MakeProcInstance((FARPROC)OvalProc, hInst);
		SetTimer(hSHEWnd, 1, 75, (TIMERPROC)lpfnOvalProc);
		}

} /* End SetOvalProc */





/***************************************************************************/

WORD CALLBACK _export OvalProc(HWND hWnd, WORD wMsg, int TimID, DWORD dwTim)

//	This is the callback function for our oval shield effect drawn around
// the enterprise.

/***************************************************************************/
{
	HDC		hDC;
	DWORD		dwColor;
	HPEN		hOldPn, hRandPn;
	RECT		rc;


	if(IsIconic(hWnd) || fSHERepair || !nTmpShe || bPaused)
		return 0;												/* iconic, broke, none */

	dwColor = RGB(rand() & 0xff, rand() & 0xff, rand() & 0xff);
	CopyRect(&rc, &rcOval);
	InflateRect(&rc, 15, 15);

	hRandPn = CreatePen(PS_DOT, 1, dwColor);

	hDC = GetDC(hWnd);
	SelectBrush(hDC, GetStockObject(HOLLOW_BRUSH));
	SetROP2(hDC, R2_XORPEN);
	hOldPn = SelectPen(hDC, hRandPn);
	Ellipse(hDC, MAKECOORDS(rc));
	SelectPen(hDC, hOldPn);
	DeletePen(hRandPn);
	SetROP2(hDC, R2_COPYPEN);
	ReleaseDC(hWnd, hDC);
	return 0;

} /* End OvalProc */
