/******************************************
 *		 		STAR TREK CLASSIC
 *
 * Damage Report MDI Window processing
 *
 * Revision:	$Revision:   1.4  $
 * Date:			$Date:   20 Feb 1993 09:27:50  $
 * Log File:	$Logfile:   C:/src/st/vcs/dam.c_v  $
 * Log:			$Log:   C:/src/st/vcs/dam.c_v  $
 * 
 *    Rev 1.4   20 Feb 1993 09:27:50   jaworski
 * Initial Revision
 *								  				
 *
 ******************************************/

#define	DAM_C
#include "exclude.h"

#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include "joes.h"
#include	<stdlib.h>
#include	<string.h>
#include	<stdio.h>

#include "st.h"			 								/* Global include files		*/	
#include "resource.h"
#include "protos.h"

static BOOL bAbs;											/* absolute/relative flag */

#define XY_SHE 230,84
#define XY_SRS 230,102
#define XY_LRS 230,120
#define XY_NAV 230,138
#define XY_PHA 230,156
#define XY_PHO 230,174
#define XY_COM 230,192


/*************************************************************************/

LRESULT CALLBACK _export DAMDefProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)

// This is the Damage Report MDI definition procedure. 
                                                           
/**************************************************************************/
{
	RECT		rc;
	DWORD		dwStyle;


	hDAMWnd = hWnd;

	switch(msg)
   	{
		case WM_CREATE:
			dwStyle = WS_CHILD | WS_CLIPSIBLINGS | WS_CAPTION |
				WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZE | WS_MINIMIZEBOX;
			SetWindowLong(hWnd, GWL_STYLE, (LPARAM)dwStyle);
			break;


		case WM_ICONERASEBKGND:
			GetClientRect(hWnd, &rc);
			FillRect((HDC)wParam, &rc, GetStockObject(BLACK_BRUSH));
			break;


		case WM_GETMINMAXINFO:
			(((LPPOINT)(lParam))+3)->x = (((LPPOINT)(lParam))+4)->x =
				SIZ_CTLX  + GetSystemMetrics(SM_CXFRAME);
         (((LPPOINT)(lParam))+3)->y = (((LPPOINT)(lParam))+4)->y =
				SIZ_CTLY + GetSystemMetrics(SM_CYCAPTION) +
						GetSystemMetrics(SM_CYFRAME);
        	return DefMDIChildProc(hWnd, msg, wParam, lParam);


	  	case WM_LBUTTONDOWN:			     /* switch absolute/relative readings */
			if(fDAMRepair)
				break;
			bAbs = !bAbs;
			InvalidateRect(hWnd, NULL, FALSE);
			break;


		case WM_PAINT:
			PaintDAM(hWnd);
			break;


     	default:
        	return(DefMDIChildProc(hWnd, msg, wParam,lParam));
        	break;
     	}

	return (LRESULT)0;

} /* End DAMDefProc */




/***************************************************************************/

void LOCAL PaintDAM(HWND hWnd)

//	This function draws the Damage Report Window.

/***************************************************************************/
{
	HDC			hDC, hMemDC;
	PAINTSTRUCT	ps;
	HBITMAP		hBM, hOldBM;
	char			szBuf[8];
	LOGFONT		lf = {-12, 0, 0, 0, 400, 0, 0, 0, 0, 3, 2, 1, 34, "Arial"};
	HFONT			hFont, hOldFont;
	BITMAP		bm;
	const char	szSpec[] = "%-4.1f";
	const char	szOK[] =   " - OK -";
	const char	szDM[] =   "DESTROYED";


	hDC = BeginPaint(hWnd, &ps);

	hBM = LoadBitmap(hLib, MAKEINTRESOURCE(BMP_DAM));
	hFont = CreateFontIndirect(&lf);
	hMemDC = CreateCompatibleDC(hDC);
	hOldBM = SelectBitmap(hMemDC, hBM);
	hOldFont = SelectFont(hMemDC, hFont);
	SetBkColor(hMemDC, RGB_BLACK);
	SetTextColor(hMemDC, RGB_YELLOW);


	if(!fDAMRepair)
		{
		if(fSHERepair > 0.0)
			{
			sprintf(szBuf, szSpec, ((bAbs)? fSHERep-fStarDNow : fSHERep));
			TextOut(hMemDC, XY_SHE, szBuf, strlen(szBuf));
			}
		else if(!fSHERepair)
			TextOut(hMemDC, XY_SHE, szOK, strlen(szOK));
		else
			TextOut(hMemDC, XY_SHE, szDM, strlen(szDM));


		if(fSRSRepair > 0.0)
			{
			sprintf(szBuf, szSpec, ((bAbs)? fSRSRep-fStarDNow : fSRSRep));
			TextOut(hMemDC, XY_SRS, szBuf, strlen(szBuf));
			}
		else if(!fSRSRepair)
			TextOut(hMemDC, XY_SRS, szOK, strlen(szOK));
		else
			TextOut(hMemDC, XY_SRS, szDM, strlen(szDM));


		if(fLRSRepair > 0.0)
			{
			sprintf(szBuf, szSpec, ((bAbs)? fLRSRep-fStarDNow : fLRSRep));
			TextOut(hMemDC, XY_LRS, szBuf, strlen(szBuf));
			}
		else if(!fLRSRepair)
			TextOut(hMemDC, XY_LRS, szOK, strlen(szOK));
		else
			TextOut(hMemDC, XY_LRS, szDM, strlen(szDM));


		if(fNAVRepair > 0.0)
			{
			sprintf(szBuf, szSpec, ((bAbs)? fNAVRep-fStarDNow : fNAVRep));
			TextOut(hMemDC, XY_NAV, szBuf, strlen(szBuf));
			}
		else if(!fNAVRepair)
			TextOut(hMemDC, XY_NAV, szOK, strlen(szOK));
		else
			TextOut(hMemDC, XY_NAV, szDM, strlen(szDM));


		if(fPHARepair > 0.0)
			{
			sprintf(szBuf, szSpec, ((bAbs)? fPHARep-fStarDNow : fPHARep));
			TextOut(hMemDC, XY_PHA, szBuf, strlen(szBuf));
			}
		else if(!fPHARepair)
			TextOut(hMemDC, XY_PHA, szOK, strlen(szOK));
		else
			TextOut(hMemDC, XY_PHA, szDM, strlen(szDM));


		if(fPHORepair > 0.0)
			{
			sprintf(szBuf, szSpec, ((bAbs)? fPHORep-fStarDNow : fPHORep));
			TextOut(hMemDC, XY_PHO, szBuf, strlen(szBuf));
			}
		else if(!fPHORepair)
			TextOut(hMemDC, XY_PHO, szOK, strlen(szOK));
		else
			TextOut(hMemDC, XY_PHO, szDM, strlen(szDM));


		if(fCOMRepair > 0.0)
			{
			sprintf(szBuf, szSpec, ((bAbs)? fCOMRep-fStarDNow : fCOMRep));
			TextOut(hMemDC, XY_COM, szBuf, strlen(szBuf));
			}
		else if(!fCOMRepair)
			TextOut(hMemDC, XY_COM, szOK, strlen(szOK));
		else
			TextOut(hMemDC, XY_COM, szDM, strlen(szDM));
		}

	GetObject(hBM, sizeof(BITMAP), (LPSTR)&bm);
   BitBlt(hDC, 12, 6, bm.bmWidth, bm.bmHeight, hMemDC, 0, 0, SRCCOPY);
	SelectBitmap(hMemDC, hOldBM);
	SelectFont(hMemDC, hOldFont);
	DeleteDC(hMemDC);
	DeleteBitmap(hBM);
	DeleteFont(hFont);

	EndPaint(hWnd, &ps);


} /* End PaintDAM */
