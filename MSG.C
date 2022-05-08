/************************************
 *		 STAR TREK CLASSIC	 			
 *								  				
 *	Message MDI Window processing
 *
 * Revision:	$Revision:   1.4  $
 * Date:			$Date:   20 Feb 1993 09:27:50  $
 * Log File:	$Logfile:   C:/src/st/vcs/msg.c_v  $
 * Log:			$Log:   C:/src/st/vcs/msg.c_v  $
 * 
 *    Rev 1.4   20 Feb 1993 09:27:50   jaworski
 * Initial Revision
 *								  				
 ************************************/

#define	MSG_C
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


#define	MAXLINES 30
#define  TEXTCOLOR RGB_GREEN
#define  BACKCOLOR RGB_BLACK

static LOGFONT  lf = {-13, 0, 0, 0, 400, 0, 0, 0, 0, 3, 2, 1, 34, "Arial"};

static char	szLines[MAXLINES][64];	 								/* line buffer	*/
static int	nChar;
static int	nVScrollPos, nVScrollMax;

static HFONT hOldFont, hMsgFont;



/*************************************************************************/
LRESULT CALLBACK _export MsgDefProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)

// This is the Messages (MSG) MDI definition procedure. 
																			  
/**************************************************************************/
{
	RECT			rc;
	DWORD			dwStyle;
	TEXTMETRIC	tm;
	HDC			hDC;
	static int	nClientX, nClientY;
	int			nVScrollInc;


	hMSGWnd = hWnd;
	switch(msg)
		{
		case WM_CREATE:
			dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CAPTION |
				WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZE | WS_MINIMIZEBOX |
				WS_VSCROLL | WS_MAXIMIZEBOX;
			SetWindowLong(hWnd, GWL_STYLE, (LPARAM)dwStyle);
			hMsgFont = CreateFontIndirect(&lf);

			hDC = GetDC(hWnd);
			hOldFont = SelectFont(hDC, hMsgFont);
			GetTextMetrics(hDC, &tm);
			nChar = tm.tmHeight + tm.tmExternalLeading;  /* get char Height */
			SetTextColor(hDC, TEXTCOLOR);
			SetBkColor(hDC, BACKCOLOR);
			memset(&szLines[0][0], 0, MAXLINES * 64);			/* clear array */
			nVScrollPos = MAXLINES;
			break;


		case WM_ICONERASEBKGND:
			GetClientRect(hWnd, &rc);
			FillRect((HDC)wParam, &rc, GetStockObject(BLACK_BRUSH));
			return (LRESULT)0;


		case WM_SIZE:
			nClientX = LOWORD(lParam);
			nClientY = HIWORD(lParam);

			nVScrollMax = max(0,  MAXLINES - (nClientY/nChar));
			nVScrollPos = min(nVScrollPos, nVScrollMax);
			SetScrollRange(hWnd, SB_VERT, 0,	nVScrollMax, FALSE);
			SetScrollPos(hWnd, SB_VERT, nVScrollPos, TRUE);

		  	return(DefMDIChildProc(hWnd, msg, wParam, lParam));
			break;


		case WM_VSCROLL:
			switch(wParam)		 				/* Scroll the message lines */
				{
				case SB_LINEUP:
					nVScrollInc = -1;
					break;

				case SB_LINEDOWN:
					nVScrollInc = +1;
					break;

				case SB_PAGEUP:
					nVScrollInc = min(-1, -nClientY/nChar);
					break;

				case SB_PAGEDOWN:
					nVScrollInc = max(+1, -nClientY/nChar);
					break;

				case SB_TOP:
					nVScrollInc = -nVScrollPos;
					break;

				case SB_BOTTOM:
					nVScrollInc = nVScrollMax - nVScrollPos;
					break;

				case SB_THUMBTRACK:
					nVScrollInc = LOWORD(lParam) - nVScrollPos;
					break;

				default:
					nVScrollInc = 0;
					break;
				}

			if(nVScrollInc = max(-nVScrollPos,
					min(nVScrollInc, nVScrollMax - nVScrollPos)))
				{
				nVScrollPos += nVScrollInc;
				ScrollWindow(hWnd, 0, -nChar * nVScrollInc, NULL, NULL);
				SetScrollPos(hWnd, SB_VERT, nVScrollPos, TRUE);
				InvalidateRect(hWnd, NULL, TRUE);
				UpdateWindow(hWnd);
				}
			break;


		case WM_PAINT:
			PaintMSG(hWnd);
			break;


		case WM_DESTROY:
			hDC = GetDC(hWnd);
			SelectFont(hDC, hOldFont);
			DeleteFont(hMsgFont);
			break;


	  	default:
		  	return(DefMDIChildProc(hWnd, msg, wParam, lParam));
		  	break;

	  	}
	return (LRESULT)0;

} /* End MsgDefProc */




/***************************************************************************/

void LOCAL PaintMSG(HWND hWnd)

//	This function draws the SRS screen.	It draws the grid and ref numbers,
// then each bitmap in its location.

/***************************************************************************/
{
	HDC		   hDC;
	PAINTSTRUCT	ps;
	int			nY, i, nTopOffset, nPaintBeg, nPaintEnd;
	HBRUSH		hBr;

	hDC = BeginPaint(hWnd, &ps);

	if(fMSGRepair)						 		/* on Damage, fill with text color */
		{
		hBr = CreateSolidBrush(BACKCOLOR);
		FillRect(hDC, &ps.rcPaint, hBr);
		DeleteBrush(hBr);
		EndPaint(hWnd, &ps);
		return;
		}

	nTopOffset = ps.rcPaint.bottom % nChar;

	nPaintBeg = max(0, nVScrollPos + ps.rcPaint.top / nChar - 1);
	nPaintEnd = min(MAXLINES, nVScrollPos + ps.rcPaint.bottom / nChar);

	for(i = nPaintBeg; i < nPaintEnd; i++)
		{
		nY = nChar * (1 - nVScrollPos + i - 1) + nTopOffset;
		if(Condition == RedAlertOn || Condition == RedAlertOff)
			SetTextColor(hDC, RGB_RED);
		else if(Condition == YellAlertOn || Condition == YellAlertOff)
			SetTextColor(hDC, RGB_YELLOW);
		else
			SetTextColor(hDC, TEXTCOLOR);
        SetBkColor(hDC, RGB_BLACK);
		SelectFont(hDC, hMsgFont);
		TextOut(hDC, 2, nY, &szLines[i][0], lstrlen(&szLines[i][0]));
		}

	EndPaint(hWnd, &ps);




} /* End PaintMSG */




/***************************************************************************/

void ShowMessage(int nResID, ...)

//	This function displays text messages in the MSG window. The caller passes
//	the resource ID of the message, and one or more optional parameters that
//	will be processed by wvsprintf().

// If the bReRoute flag is true, the message is displayed in the COM Window.

/***************************************************************************/
{
	char  szL[80];
	char	szU[80];
	int	i;


	LoadString(hInst, nResID, szU, sizeof(szU));
	wvsprintf(szL, szU, (LPVOID)(LPSTR)(&nResID+1)); /* Format the string */

	if(bReRoute)
		{
		DisplayComMessage(szL);
		DisplayComMessage((LPSTR)"\n");
		}

	else if(!fMSGRepair)
		{
		for(i = 1; i < MAXLINES; i++)
			strcpy(&szLines[i-1][0], &szLines[i][0]);	 	/* shift it up by one */

		strcpy(&szLines[MAXLINES-1][0], szL);

		if(IsWindow(hMSGWnd))
			{
			InvalidateRect(hMSGWnd,	NULL, TRUE);
			SendMessage(hMSGWnd, WM_VSCROLL, SB_BOTTOM, 0L);
			}
		}

} /* End ShowMessage */

