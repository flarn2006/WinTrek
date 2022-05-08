/*********************************************
 *		 STAR TREK CLASSIC	 						
 *								  							
 * Long Range Sensor MDI Window processing	
 * Revision:	$Revision:   1.4  $
 * Date:			$Date:   20 Feb 1993 09:27:48  $
 * Log File:	$Logfile:   C:/src/st/vcs/lrs.c_v  $
 * Log:			$Log:   C:/src/st/vcs/lrs.c_v  $
 * 
 *    Rev 1.4   20 Feb 1993 09:27:48   jaworski
 * Initial Revision
 *								  							
 *********************************************/

#define	LRS_C
#include "exclude.h"

#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include "joes.h"
#include	<stdlib.h>
#include	<string.h>

#include "st.h"			 								/* Global include files		*/	
#include "resource.h"
#include "protos.h"


static int	nXSpacing, nYSpacing;					/* Numeric block sizes	*/



/*************************************************************************/
LRESULT CALLBACK _export LRSDefProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)

// This is the Long Range Sensor MDI definition procedure. 
                                                           
/**************************************************************************/
{
	static LOGFONT	lf = {-13, 0, 0, 0, 400, 0, 0, 0, 0, 3, 2, 1, 34, "Arial"};
	static HFONT	hFont, hOldFont;
	static FARPROC	lpfnNumFlashProc;			 /* Callback for flashing numbers */
	RECT		rc;
	HDC		hDC;


	hLRSWnd = hWnd;
	switch(msg)
   	{
		case WM_CREATE:
			hDC = GetDC(hWnd);					/* Set font and flash procedure */
			hFont = CreateFontIndirect(&lf);
			hOldFont = SelectFont(hDC, hFont);
	        SetBkColor(hDC, RGB_BLACK);
 			ReleaseDC(hWnd, hDC);
			lpfnNumFlashProc = MakeProcInstance((FARPROC)NumFlashProc, hInst);
			SetTimer(hWnd, 2, 280, (TIMERPROC)lpfnNumFlashProc);
			break;

		
		case WM_PAINT:
			PaintLRS(hWnd);
			break;


		case WM_ICONERASEBKGND:
			GetClientRect(hWnd, &rc);
			FillRect((HDC)wParam, &rc, GetStockObject(BLACK_BRUSH));
			return (LRESULT)0;


		case WM_SIZE:
			nXSpacing = LOWORD(lParam) / 9;	/* calc new number block size	*/
			nYSpacing = HIWORD(lParam) / 9;
			lf.lfHeight = -(nYSpacing - 8);	/* and build scalable font		*/
			hDC = GetDC(hWnd);
			SelectFont(hDC, hOldFont);
			DeleteFont(hFont);
			hFont = CreateFontIndirect(&lf);
			hOldFont = SelectFont(hDC, hFont);
			ReleaseDC(hWnd, hDC);
			return(DefMDIChildProc(hWnd, msg, wParam, lParam));


		case WM_CHAR:
			if(wParam == 0x1B)								  /* Escape Key press */
				{
				if(nINIAutoSRS)
					Activate(hSRSWnd);						  /* switch back to SRS */
				else
					Activate(hWnd);
				}
			break;


		case WM_GETMINMAXINFO:
			(((LPPOINT)(lParam))+3)->x = 88 + GetSystemMetrics(SM_CXFRAME);
         (((LPPOINT)(lParam))+3)->y = 60 + GetSystemMetrics(SM_CYCAPTION) +
				GetSystemMetrics(SM_CYFRAME);
        	return DefMDIChildProc(hWnd, msg, wParam, lParam);


		case WM_DESTROY:
			KillTimer(hWnd, 2);
			FreeProcInstance(lpfnNumFlashProc);
			hDC = GetDC(hWnd);
			SelectFont(hDC, hOldFont);
			ReleaseDC(hWnd, hDC);
			DeleteFont(hFont);
			break;

     	default:
         	return(DefMDIChildProc(hWnd, msg, wParam, lParam));
         	break;
     	}

	return (LRESULT)0;

} /* End LRSDefProc */




/***************************************************************************/

void LOCAL PaintLRS(HWND hWnd)

//	This function draws the LRS screen.	Everything is scaled based upon the
// size of the client area.

/***************************************************************************/
{
	RECT			rc, rcTx;
	HDC			hDC;
	PAINTSTRUCT	ps;
	HPEN			hOldPen, hBluePen;
	int			i;
	char			sz[4];


	hDC = BeginPaint(hWnd, &ps);

/* Draw the grid */

	GetClientRect(hWnd, &rc);
	hBluePen = CreatePen(PS_SOLID, 1, RGB_BLUE);
	hOldPen = SelectPen(hDC, hBluePen);

	for(i = 8; i >= 0; i--)
		{
		MoveTo(hDC, rc.right, rc.bottom - (nYSpacing * i));
		LineTo(hDC, rc.right - (nXSpacing * 8), rc.bottom - (nYSpacing * i));
		}
	for(i = 1; i < 9; i++)
		{
		MoveTo(hDC, rc.right - (nXSpacing * i), rc.bottom);
		LineTo(hDC, rc.right - (nXSpacing * i), rc.bottom - (nYSpacing * 8));
		}
	SelectPen(hDC, hOldPen);
	DeletePen(hBluePen);



	if(fLRSRepair)
		{
		EndPaint(hWnd, &ps);
		return;
		}


/* Draw the grid reference numbers */

	SetTextColor(hDC, RGB_BLUE);
    SetBkColor(hDC, RGB_BLACK);
	*sz = '1';
	for(i = 8; i > 0; i--)
		{
		SetRect(&rcTx, rc.right - (nXSpacing * i), 0, 0, nYSpacing);
		rcTx.right = rcTx.left + nXSpacing;
		InflateRect(&rcTx, -2, -2);
		DrawText(hDC, sz, 1, &rcTx,	DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX |
			DT_CENTER | DT_NOCLIP);
		++*sz;
		}
	*sz = '8';
	for(i = 1; i < 9; i++)
		{
		SetRect(&rcTx, 0, rc.bottom - (nYSpacing * i), nXSpacing, 0);
		rcTx.bottom = rcTx.top + nYSpacing;
		InflateRect(&rcTx, -2, -2);
		DrawText(hDC, sz, 1, &rcTx, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX |
			DT_CENTER | DT_NOCLIP);
		--*sz;
		}

/* Draw the Actual LRS data */

	SetTextColor(hDC, RGB_RED);
	SetRect(&rcTx, rc.right - nXSpacing, rc.bottom - nYSpacing,
		rc.right, rc.bottom);

	for(i = 63; i >= 0; i--)
		{
		if(qd[i].Flags & QFLAG_LRS)						 /* only if marked */
			{
			InflateRect(&rcTx, -2, -2);
			sz[0] = (char)'0' + (char)qd[i].Aliens;
			sz[1] = (char)'0' + (char)qd[i].Bases;
			sz[2] = (char)'0' + (char)qd[i].Stars;
			DrawText(hDC, sz, 3, &rcTx, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX |
				DT_CENTER);
			InflateRect(&rcTx, 2, 2);
			}
		if(!(i % 8))
			{
			rcTx.left    = rc.right - nXSpacing;
			rcTx.right   = rc.right;
			rcTx.top    -= nYSpacing;
			rcTx.bottom -= nYSpacing;
			}
		else
			{
			rcTx.left   -= nXSpacing;
			rcTx.right  -= nXSpacing;
			}
		}

	EndPaint(hWnd, &ps);


} /* End PaintLRS */





/***************************************************************************/

WORD CALLBACK _export NumFlashProc(HWND hWnd, WORD wMsg, int TimID, DWORD dwTim)

//	This is the callback function for our flashing enterprise sector
// indicator. to avoid lengthy redraw delays, it redraws the colors in the
// enterprise quadrant only, without generating a new WM_PAINT.

/***************************************************************************/
{
	static WORD	wToggle;
	HDC			hDC;
	RECT			rc;
	char			sz[4];

	if(IsIconic(hWnd) || fLRSRepair || bPaused)			/* iconic or broke */
		return 0;

	hDC = GetDC(hWnd);
	GetClientRect(hWnd, &rc);

	rc.left   = rc.right - ((8 - XQUAD(nEntQ)) * nXSpacing);
	rc.right  = rc.left + nXSpacing;
	rc.top    = rc.bottom -((8 - YQUAD(nEntQ)) * nYSpacing);
	rc.bottom = rc.top + nYSpacing;
	InflateRect(&rc, -2, -2);

	if(wToggle++ & 0x0001)
		SetTextColor(hDC, RGB_DKRED);
	else
		SetTextColor(hDC, RGB_RED);
    SetBkColor(hDC, RGB_BLACK);

	sz[0] = (char)'0' + (char)qd[nEntQ].Aliens;
	sz[1] = (char)'0' + (char)qd[nEntQ].Bases;
	sz[2] = (char)'0' + (char)qd[nEntQ].Stars;
	DrawText(hDC, sz, 3, &rc, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX |
			DT_CENTER);
	ReleaseDC(hWnd, hDC);
	ValidateRect(hWnd, &rc);

	return 0;

} /* End NumFlashProc */





/***************************************************************************/

void UpdateLRS(int nQuad)

//	This function updates the LRS scan Flag in the global quadrant structure
// qd[].	This makes visible all surrounding objects in the LRS display.
//	It also invalidates the LRS window.	This routine is normally called when
// the Enterprise enters a new quadrant.

/***************************************************************************/
{

	if(fLRSRepair)		 							/* I'm broke- don't attempt it */
		return;
		
	qd[AdjQuad(nQuad, -1, -1)].Flags |= QFLAG_LRS;
	qd[AdjQuad(nQuad,  0, -1)].Flags |= QFLAG_LRS;
	qd[AdjQuad(nQuad, +1, -1)].Flags |= QFLAG_LRS;
	qd[AdjQuad(nQuad, -1,  0)].Flags |= QFLAG_LRS;
	qd[AdjQuad(nQuad,  0,  0)].Flags |= QFLAG_LRS;
	qd[AdjQuad(nQuad, +1,  0)].Flags |= QFLAG_LRS;
	qd[AdjQuad(nQuad, -1, +1)].Flags |= QFLAG_LRS;
	qd[AdjQuad(nQuad,  0, +1)].Flags |= QFLAG_LRS;
	qd[AdjQuad(nQuad, +1, +1)].Flags |= QFLAG_LRS;
	if(IsWindow(hLRSWnd))
		{
		if(!IsIconic(hLRSWnd))
			InvalidateRect(hLRSWnd, NULL, 1);
		}

} /* End UpdateLRS */
