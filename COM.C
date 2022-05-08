/*************************************
 *		 STAR TREK CLASSIC
 *
 *								  				
 * Computer MDI Window processing
 *
 * Revision:	$Revision:   1.4  $
 * Date:			$Date:   20 Feb 1993 09:27:50  $
 * Log File:	$Logfile:   C:/src/st/vcs/com.c_v  $
 * Log:			$Log:   C:/src/st/vcs/com.c_v  $
 * 
 *    Rev 1.4   20 Feb 1993 09:27:50   jaworski
 * Initial Revision
 *								  				
 *************************************/

#define	COM_C
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


#define BUFFER(_x, _y) *(lpBuffer + ((_y) * cxBuffer) + (_x))
#define ComCursor()  SendMessage(hCOMWnd, WM_CHAR, (int)'>', 1L)


static int		cxChar, cyChar, cxClient, cyClient,
					cxBuffer, cyBuffer, xCaret, yCaret;
static int  	nIndentX, nIndentY;
static LPSTR 	lpBuffer;
static HBITMAP	hMemBM;	  										/* memory DC's bitmap */


/*************************************************************************/

LRESULT CALLBACK _export ComDefProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)

// This is the Computer(COM) MDI definition procedure. 
                                                           
/**************************************************************************/
{
	HDC				hDC;
	RECT				rc;
	TEXTMETRIC		tm;

	hCOMWnd = hWnd;

	switch(msg)
   	{
		case WM_CREATE:
			hDC = GetDC(hWnd);					/* Set font and static size refs */
			SelectFont(hDC, GetStockObject(ANSI_FIXED_FONT));
			GetTextMetrics(hDC, &tm);
			cxChar = tm.tmAveCharWidth;
			cyChar = tm.tmHeight;
	      	SetBkColor(hDC, RGB_DKBLUE);
			SetTextColor(hDC, RGB_WHITE);
			GetClientRect(hWnd, &rc);
			hMemBM = CreateCompatibleBitmap(hDC, rc.right, rc.bottom);
			ReleaseDC(hWnd, hDC);
			nIndentX = 3 * cxChar;
			nIndentY = 2 * cyChar;
			break;


		case WM_SIZE:							/* Resize and reformat the buffer */
			if(wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED)
				{
				ReAllocBuffer(hWnd, LOWORD(lParam), HIWORD(lParam));
				DeleteBitmap(hMemBM);
			   hDC = GetDC(hWnd);
				hMemBM = CreateCompatibleBitmap(hDC, LOWORD(lParam), HIWORD(lParam));
			   ReleaseDC(hWnd, hDC);
				}
			return(DefMDIChildProc(hWnd, msg, wParam, lParam));
			break;


		case WM_SETFOCUS:
			if(!fCOMRepair)
				{
				CreateCaret(hWnd, (HBITMAP)1, cxChar, cyChar);
				SetCaretPos((xCaret*cxChar) + nIndentX, (yCaret*cyChar)
					+ nIndentY);
				ShowCaret(hWnd);
				}
			return(DefMDIChildProc(hWnd, msg, wParam, lParam));
			break;


		case WM_KILLFOCUS:
			if(!fCOMRepair)
				{
				HideCaret(hWnd);
				DestroyCaret();
				}
			return(DefMDIChildProc(hWnd, msg, wParam, lParam));
			break;


		case WM_KEYDOWN:						  		/* handle virtual keystrokes */
			if(!fCOMRepair)
				VirtualKeyStroke(hWnd, wParam);
			break;


		case WM_CHAR:						  		   	/* handle char input */
			if(!fCOMRepair)
				CharKeyStroke(hWnd, wParam);
			break;


		case WM_PAINT:
			PaintCOM(hWnd);
			break;


		case WM_DESTROY:
			if(lpBuffer)					  
				GlobalFreePtr(lpBuffer);				/* release memory */
			lpBuffer = NULL;
			DeleteBitmap(hMemBM); 
			cxChar = cyChar = cxClient = cyClient =
			cxBuffer = cyBuffer = xCaret = yCaret = 0;
			break;


		case WM_ICONERASEBKGND:
			GetClientRect(hWnd, &rc);
			FillRect((HDC)wParam, &rc, GetStockObject(BLACK_BRUSH));
			break;


     	default:
        	return(DefMDIChildProc(hWnd, msg, wParam, lParam));
        	break;
     	}

	return (LRESULT)0;

} /* End ComDefProc */




/***************************************************************************/

void LOCAL ReAllocBuffer(HWND hWnd, int nNewX, int nNewY)

//	This function reallocates the size of the memory buffer when a 
//	WM_SIZE message occurs. It reformats each line of text to fit the
//	new buffer size. If the new buffer is smaller than the old, lines
// are truncated. All global vars related to the caret and buffers
// arew updated.

/***************************************************************************/
{
	int 	cxClientNew, cyClientNew,
		 	cxBufferNew, cyBufferNew,
			nY;
	LPSTR	lpBuffer2;


	cxClientNew = ((nNewX / cxChar) * cxChar) - (2 * nIndentX);
	cyClientNew = ((nNewY / cyChar) * cyChar) - (2 * nIndentY);

	if(cxClient == cxClientNew	&& cyClient == cyClientNew)
		{
		InvalidateRect(hWnd, NULL, FALSE);
		UpdateWindow(hWnd);
		return;													/* no change in size */
		}


/* calculate new window size in chars and alloc new memory */

	cxBufferNew = max(1, cxClientNew / cxChar);
	cyBufferNew = max(1, cyClientNew / cyChar);
	lpBuffer2 = GlobalAllocPtr(GHND, (DWORD)(cxBufferNew * cyBufferNew));
	_fmemset(lpBuffer2, ' ', cxBufferNew * cyBufferNew);


/* If previous buffer exists, reformat each line */

	if(lpBuffer)
		{
		for(nY = 0; nY < min(cyBuffer, cyBufferNew); nY++)	  
			_fmemmove(lpBuffer2+(nY*cxBufferNew), lpBuffer+(nY*cxBuffer),
				min(cxBuffer, cxBufferNew));
		GlobalFreePtr(lpBuffer);
		}

	xCaret = LIMIT(xCaret, 0, cxBufferNew -1);
	yCaret = min(yCaret, cyBufferNew - 1);
	lpBuffer = lpBuffer2;
	cxBuffer = cxBufferNew;
	cyBuffer = cyBufferNew;
	cxClient = cxClientNew;
	cyClient = cyClientNew;

	if(hWnd == GetFocus())
		SetCaretPos(xCaret*cxChar + nIndentX, yCaret*cyChar + nIndentY);


} /* End ReAllocBuffer */




/***************************************************************************/

void LOCAL PaintCOM(HWND hWnd)

//	This function redraws all the text in the COM screen.

/***************************************************************************/
{
	HDC			hDC, hMemDC;
	RECT			rcCl;
	PAINTSTRUCT	ps;
	HPEN			hOldPen, hRedPen;
	HBRUSH		hOldBr, hBlueBr, hDkRedBr;
	HBITMAP		hOldBM;
	int			nY;


	hDC = BeginPaint(hWnd, &ps);
	hMemDC = CreateCompatibleDC(hDC);
	SelectFont(hMemDC, GetStockObject(ANSI_FIXED_FONT));
	hOldBM = SelectBitmap(hMemDC, hMemBM);
	hDkRedBr = CreateSolidBrush(RGB_DKRED);
	GetClientRect(hWnd, &rcCl);
	FillRect(hMemDC, &rcCl, hDkRedBr);
	DeleteBrush(hDkRedBr);

	SetBkColor(hMemDC, RGB_DKBLUE);						/* Draw Rounded Rect */
	SetTextColor(hMemDC, RGB_WHITE);
	hRedPen = CreatePen(PS_SOLID, 3, RGB_RED);
	if(fCOMRepair)
		hBlueBr = CreateSolidBrush(RGB_BLACK);
	else
		hBlueBr = CreateSolidBrush(RGB_DKBLUE);
	hOldPen = SelectPen(hMemDC, hRedPen);
	hOldBr  = SelectBrush(hMemDC, hBlueBr);
	RoundRect(hMemDC,	nIndentX-cxChar, nIndentY-cyChar,
		rcCl.right-nIndentX + cxChar, rcCl.bottom-nIndentY + cyChar,
		cxChar, cyChar);

	SelectPen(hMemDC, hOldPen);
	SelectBrush(hMemDC, hOldBr);
	DeletePen(hRedPen);
	DeleteBrush(hBlueBr);

	if(!fCOMRepair)
		{
		for(nY = 0; nY < cyBuffer; nY++)
			TextOut(hMemDC, nIndentX, (nY * cyChar) + nIndentY,
					  &BUFFER(0, nY), cxBuffer);
		}

	BitBlt(hDC, 0, 0, rcCl.right, rcCl.bottom, hMemDC,	0, 0, SRCCOPY);

	SelectBitmap(hMemDC, hOldBM);
	DeleteDC(hMemDC);


	EndPaint(hWnd, &ps);


} /* End PaintCom */






/***************************************************************************/

void LOCAL VirtualKeyStroke(HWND hWnd, int nChar)

//	This function processes virtual key strokes generated by a WM_KEYDOWN
//	message from our main loop.

/***************************************************************************/
{
	int	nX;
	HDC	hDC;


	switch(nChar)
		{
		case VK_HOME:
			xCaret = 0;					  				 /* erase and go to beginning */
			yCaret = 0;				 	 
			_fmemset(lpBuffer, ' ', cxBuffer * cyBuffer);
			ComCursor();
			InvalidateRect(hWnd, NULL, FALSE);
			break;


		case VK_END:
			xCaret = cxBuffer-1;		  				 /* erase and go to beginning */
			yCaret = cyBuffer-1;				 	 
			_fmemset(lpBuffer, ' ', cxBuffer * cyBuffer);
			ComCursor();
			InvalidateRect(hWnd, NULL, FALSE);
			break;


		case VK_DELETE:								 /* perform a backspace/del 	*/
			for(nX = xCaret; nX < cxBuffer - 1; nX++)
				BUFFER(nX, yCaret) = BUFFER(nX+1, yCaret);

			BUFFER(cxBuffer - 1, yCaret) = ' ';
			HideCaret(hWnd);
			hDC = GetDC(hWnd);
			SelectFont(hDC, GetStockObject(ANSI_FIXED_FONT));
	      	SetBkColor(hDC, RGB_DKBLUE);
			SetTextColor(hDC, RGB_WHITE);
			TextOut(hDC, xCaret * cxChar + nIndentX, yCaret * cyChar + nIndentY,
				&BUFFER(xCaret, yCaret), cxBuffer - xCaret);
			ShowCaret(hWnd);
			ReleaseDC(hWnd, hDC);
			break;


		case VK_PRIOR:											/* Page Up key */
			yCaret = 0;				 	 
			break;


		case VK_NEXT:
			yCaret = cyBuffer -1;				 			/* Page Down key */ 
			break;


		case VK_LEFT:
			xCaret = max(xCaret - 1, 0);					/* left arrow */ 
			break;


		case VK_RIGHT:
			xCaret = min(xCaret + 1, cxBuffer - 1);	/* right arrow */ 
			break;


		case VK_UP:
			yCaret = max(yCaret - 1, 0);					/* up arrow */ 
			break;


		case VK_DOWN:
			yCaret = min(yCaret + 1, cyBuffer - 1);	/* down arrow */ 
			break;


		default:
			break;
		}

	SetCaretPos((xCaret*cxChar) + nIndentX, (yCaret*cyChar) + nIndentY);


} /* End VirtualKeyStroke */






/***************************************************************************/

void LOCAL CharKeyStroke(HWND hWnd, int nChar)

//	This function processes WM_CHAR key strokes from our main message loop.

/***************************************************************************/
{
	int	nY;
	HDC	hDC;


	switch(nChar)
		{
		case '\t':												/* Tab key */
			do
				{
				SendMessage(hWnd, WM_CHAR, ' ', 1L);
				} while(xCaret % 4 != 0);
			break;


		case '\b':												/* Backspace key */
			if(xCaret > 1)
				{
				--xCaret;
				SendMessage(hWnd, WM_KEYDOWN, VK_DELETE, 0L);
				}
			break;


		case '\x1B':											/* Escape Key */
			if(nINIAutoSRS)
				Activate(hSRSWnd);							/* switch back to SRS */
			else
				Activate(hWnd);
			break;
			

		case '\r':												/* Carraige return */
		case '\n':
			xCaret = 0;
			if(yCaret == cyBuffer - 1)
				{												 	/* scroll the window */
				for(nY = 1; nY < cyBuffer; nY++)	  
					_fmemmove(&BUFFER(0, nY-1), &BUFFER(0, nY), cxBuffer);
				_fmemset(&BUFFER(0, cyBuffer-1), ' ', cxBuffer);
				InvalidateRect(hWnd, NULL, FALSE);
				UpdateWindow(hWnd);
				}
			else
				++yCaret;

			if(!ExecuteCommand(&BUFFER(0, yCaret-1)))	/* execute it		 */
				{
				if(nINIAutoSRS)
					Activate(hSRSWnd);						/* switch back to SRS */
				else
					Activate(hWnd);
				break;
				}

			break;


		default:

	/* Write the character to the buffer and display it */
	
			BUFFER(xCaret, yCaret) = (char)toupper(nChar);	
			HideCaret(hWnd);
			hDC = GetDC(hWnd);
			SelectFont(hDC, GetStockObject(ANSI_FIXED_FONT));
	      	SetBkColor(hDC, RGB_DKBLUE);
			SetTextColor(hDC, RGB_WHITE);
			TextOut(hDC, (xCaret * cxChar) + nIndentX, (yCaret * cyChar) +
				nIndentY, &BUFFER(xCaret, yCaret), 1);
			ShowCaret(hWnd);
			ReleaseDC(hWnd, hDC);

			if(++xCaret == cxBuffer)						 /* move to new line?  */
				{
				xCaret = 0;
				if(yCaret == cyBuffer - 1)
					{												 /* scroll the window? */
					for(nY = 1; nY < cyBuffer; nY++)	  
						_fmemmove(&BUFFER(0, nY-1), &BUFFER(0, nY), cxBuffer);
					_fmemset(&BUFFER(0, cyBuffer-1), ' ', cxBuffer);
					InvalidateRect(hWnd, NULL, FALSE);
					}
				else
					++yCaret;
				}
			break;

		}

	SetCaretPos((xCaret*cxChar) + nIndentX, (yCaret*cyChar) + nIndentY);


} /* End CharKeyStroke */




/***************************************************************************/

int LOCAL ExecuteCommand(LPSTR lpCmd)

//	This function interprets a line of ASCII input as a command. The line
// pointed to by lpCmd DOES NOT contain a null terminator character.
//
// The return value is zero if the caller should close the COM window.
// Otherwise, the return value is non-zero.

/***************************************************************************/
{
	char			szStr[128],	szStr2[128];
	int			i, j, nX, nY;
	static int  nErrCount;		 				/* auto-help message counter */
	POINT			pt;
	PSTR			pStr;
	int			nDeg, nDist;
	double		fAdv;

	memset(szStr, 0, sizeof(szStr));
	_fstrncpy((LPSTR)szStr, lpCmd, min(sizeof(szStr)-1, cxBuffer-1));

	if(!strncmp(szStr, "  ", 2)) 	 				/* no command- just a caret */
		{
		ComCursor();
		return -1;
		}

/* Process Computer Commands */


/* BASE COMMAND */

	if(strstr(szStr, "BAS"))
		{
		for(i = 0; i < 64; i++)
			{
			if(qd[i].Bases)
				DisplayBaseInfo(i);						/* display each base info */
			}
		ComCursor();
		}


/* SCAN COMMAND */

	else if(strstr(szStr, "SCA"))
		{
		DisplayComMessage(MAKEINTRESOURCE(COM_SCAN1));
		for(i = 0; i < 8; i++)
			{
			for(j = 0; j <	8; j++)
				DisplayObjectInfo(i, j);		 /* Scan the sector and display */
			}
		ComCursor();
		}


/* RICOCHET COMMAND */

	else if(strstr(szStr, "RIC"))
		{
		if(qd[nEntQ].Stars)						 /* are there stars here? */
			{
			if(qd[nEntQ].Flags & QFLAG_RIC)
				{
				DisplayComMessage(MAKEINTRESOURCE(COM_COMRIC));    /* go do it */
				DoRicochet();
				ComCursor();
				return 0;
				}
			else
				DisplayComMessage(MAKEINTRESOURCE(COM_NORIC)); /* none qualify */
			}
		else
			DisplayComMessage(MAKEINTRESOURCE(COM_NORIC2));/* no stars at all */
		}


/* LOCATION COMMAND */

	else if(strstr(szStr, "LOC"))						  		
		DisplayComMessage(MAKEINTRESOURCE(COM_CURRLOC), XQUAD(nEntQ)+1,
			YQUAD(nEntQ)+1, nEntX+1, nEntY+1);


/* STATUS COMMAND */

	else if(strstr(szStr, "STA"))						  		
		{
		LoadString(hInst, COM_STARDATE, szStr2, sizeof(szStr2));
		sprintf(szStr, szStr2, fStarDNow, fMissionL,	fStarDStart + fMissionL,
			fStarDNow - fStarDStart, (fStarDStart + fMissionL) - fStarDNow);
		DisplayComMessage(szStr);
		}


/* POWER COMMAND */

	else if(strstr(szStr, "POW"))						  					  
		DisplayComMessage(MAKEINTRESOURCE(COM_POWER), nDilithium, nShields,
			nDilithium + nShields);


/* ALIENS COMMAND */

	else if(strstr(szStr, "ALI"))	 					  					  
		DisplayComMessage(MAKEINTRESOURCE(COM_ALIEN), nAlienTotal,
			nAlienTotal - nAlienCnt, nAlienCnt);


/* DOS COMMAND */

	else if(strstr(szStr, "CD") || strstr(szStr, "DIR"))
		DisplayComMessage(MAKEINTRESOURCE(COM_DOSCMD));


/* HELP COMMAND */

	else if(strstr(szStr, "HEL"))						  					
		DisplayComMessage(MAKEINTRESOURCE(COM_MANUAL));

/* SKILL COMMAND */

	else if(strstr(szStr, "SKI"))
		{
		DisplayComMessage(SkillToAscii(nSkill));
		DisplayComMessage("\n>");
		}

/* QUAD INFO COMMAND */

	else if(pStr = strstr(szStr, "GETQ"))
		{
		pStr +=5;
		nX = (*pStr) - 0x31;
		nY = (*(pStr+2)) - 0x31;
		if(nX > 7 || nX < 0 || nY > 7 || nY < 0)
			{
			DisplayComMessage(MAKEINTRESOURCE(COM_XYERR));
			return -1;
			}
		pt.x = nX - XQUAD(nEntQ);
		pt.y = nY - YQUAD(nEntQ);
		if(pt.x == 0 && pt.y == 0)
			nDeg = nDist = 0;
		else
			{
			nDeg  = CoordsToDeg(pt);
			nDist = GetWarpFromPoints(XQUAD(nEntQ), YQUAD(nEntQ), nX, nY);
			}
		DisplayComMessage(MAKEINTRESOURCE(COM_BASEDIR), nDist, nDeg);
		ComCursor();
		}


/* ADVANCE COMMAND */

	else if((pStr = strstr(szStr, "ADV")))
		{
		fAdv = 0.0;
		pStr +=4;
		i = (*pStr);
		if(i != '.' && i != '0')
			{							  /* if 1st not decimal, assume whole number */
			i -= 0x30;
			if(i > 9 || i < 0)
				{
				DisplayComMessage(MAKEINTRESOURCE(COM_BADADV));
				return -1;
				}
			fAdv +=(double)i;
			}
		if(i != '.')
			++pStr;

		if(*pStr == '.')			 								/* advance to decimal */
			{
			++pStr;
			i = (*pStr) - 0x30;
			if(i > 9 || i < 0)
				{
				DisplayComMessage(MAKEINTRESOURCE(COM_BADADV));
				return -1;
				}
			fAdv += ((double)(i)	/ 10.0);
			}

		if(fAdv)
			{
			SubtractTime(fAdv);
			RepairShip();
			InvalidateRect(hPANWnd, NULL, 0);
			++bNeedSave;
			ComCursor();
			}
		else
			{
			DisplayComMessage(MAKEINTRESOURCE(COM_BADADV));
			return -1;
			}
		}


/* SECTOR INFO COMMAND */

	else if(pStr = strstr(szStr, "GETS"))
		{
		pStr +=5;
		nX = (*pStr) - 0x31;
		nY = (*(pStr+2)) - 0x31;
		if(nX > 7 || nX < 0 || nY > 7 || nY < 0)
			{
			DisplayComMessage(MAKEINTRESOURCE(COM_XYERR));
			return -1;
			}
		pt.x = nX - nEntX;
		pt.y = nY - nEntY;
		if(pt.x == 0 && pt.y == 0)
			nDeg = nDist = 0;
		else
			{
			nDeg = CoordsToDeg(pt);
			nDist = GetWarpFromPoints(nEntX, nEntY, nX, nY);
			}
		DisplayComMessage(MAKEINTRESOURCE(COM_SECTDIR), nDist, nDeg);
		ComCursor();
		}


/* REROUTE COMMAND */

	else if(strstr(szStr, "RER"))
		{
		ShowMessage(COM_MSGRER);
		DisplayComMessage(MAKEINTRESOURCE(COM_REROUTE));
		bReRoute = TRUE;
		}

/* UNROUTE COMMAND */

	else if(strstr(szStr, "UNR"))
		{
		bReRoute = FALSE;
		ShowMessage(COM_MSGUNR);
		DisplayComMessage(MAKEINTRESOURCE(COM_UNROUTE));
		}

/* DAMAGE COMMAND */

	else if(strstr(szStr, "DAM"))						  					
		DamReport();


/* ANALYSIS COMMAND */

	else if(strstr(szStr, "ANA"))						  					
		AnalysisReport();


/* BYE, EXIT */

	else if(strstr(szStr, "BYE") || strstr(szStr, "EXI"))
		{
		ComCursor();
		PlaySound(SND_EXEC, 0);
		return 0;
		}


/* No Command at all */

	else if(strstr(szStr, "> "))
		ComCursor();


/* Unknown Command */
	
	else															
		{
		if(++nErrCount == 8)
			{
			DisplayComMessage(MAKEINTRESOURCE(COM_MANUAL));
			nErrCount = 0;
			}
		else
			DisplayComMessage(MAKEINTRESOURCE(COM_INVALID));
		}

	return -1;


} /* End ExecuteCommand */





/***************************************************************************/

void DisplayComMessage(LPCSTR lpszCmd, ...)

//	This function displays text messages in the COM window. The caller passes
//	the resource ID or LPSTR, and one or more optional parameters that
//	will be processed by wvsprintf().

/***************************************************************************/
{
	char  szL[128];
	char	szU[128];
	int	i, nY;
	HDC	hDC;


	if(HIWORD((DWORD)lpszCmd))						  		/* resource or string? */
		wvsprintf(szL, lpszCmd, (LPSTR)(&lpszCmd + 1));	 /* Format the string */
	else
		{
		LoadString(hInst, LOWORD((DWORD)lpszCmd), szU, sizeof(szU));
		wvsprintf(szL, szU, (LPSTR)(&lpszCmd + 1));
		}

	HideCaret(hCOMWnd);
	hDC = GetDC(hCOMWnd);
	SelectFont(hDC, GetStockObject(ANSI_FIXED_FONT));
   	SetBkColor(hDC, RGB_DKBLUE);
	SetTextColor(hDC, RGB_WHITE);

	for(i = 0; i < (int)strlen(szL); i++)
		{
		if(szL[i] == '\n')
			{
			xCaret = 0;
			if(yCaret == cyBuffer - 1)
				{												 /* scroll the window? */
				for(nY = 1; nY < cyBuffer; nY++)	  
					_fmemmove(&BUFFER(0, nY-1), &BUFFER(0, nY), cxBuffer);
				_fmemset(&BUFFER(0, cyBuffer-1), ' ', cxBuffer);
				InvalidateRect(hCOMWnd, NULL, FALSE);
				UpdateWindow(hCOMWnd);
				MyYield();
				}
			else
				++yCaret;
			continue;
			}

		BUFFER(xCaret, yCaret) = szL[i];	
		TextOut(hDC, (xCaret * cxChar) + nIndentX, (yCaret * cyChar) +
			nIndentY, &BUFFER(xCaret, yCaret), 1);

		if(++xCaret == cxBuffer)						 /* move to new line?  */
			{
			xCaret = 0;
			if(yCaret == cyBuffer - 1)
				{												 /* scroll the window? */
				for(nY = 1; nY < cyBuffer; nY++)	  
					_fmemmove(&BUFFER(0, nY-1), &BUFFER(0, nY), cxBuffer);
				_fmemset(&BUFFER(0, cyBuffer-1), ' ', cxBuffer);
				InvalidateRect(hCOMWnd, NULL, FALSE);
				UpdateWindow(hCOMWnd);
				}
			else
				++yCaret;
			}
		}
	ShowCaret(hCOMWnd);
	ReleaseDC(hCOMWnd, hDC);

	SetCaretPos((xCaret*cxChar) + nIndentX, (yCaret*cyChar) + nIndentY);


} /* End DisplayComMessage */





/***************************************************************************/

void LOCAL DisplayBaseInfo(int nQuad)

//	This function displays the base info for the specified quadrant. The
//	base quadrant is displayed along with the warp coordinates. If the
//	quadrant has not been scanned via LRS, errors are introduced.

/***************************************************************************/
{
	int	nWarp, nDeg;
	POINT	pt, ptd;
	int	nEX, nEY;

	nEX = XQUAD(nEntQ) + 1;
	nEY = YQUAD(nEntQ) + 1;

	if(qd[nQuad].Flags & QFLAG_LRS)						/* no chance of error */
		{
		pt.x = XQUAD(nQuad)+1;
		pt.y = YQUAD(nQuad)+1;
		DisplayComMessage(MAKEINTRESOURCE(COM_BASELOC), pt.x, pt.y);
		}
	else
		{
		pt.x = XQUAD(nQuad)+(rand()&0x0003);
		pt.y = YQUAD(nQuad)+(rand()&0x0003);


/* If the new random quad is visible, skip it entirely */

		if(!pt.x) ++pt.x;
		if(!pt.y) ++pt.y;												/* normalize it */
		if(pt.x > 8) pt.x = 8;
		if(pt.y > 8) pt.y = 8;
		DisplayComMessage(MAKEINTRESOURCE(COM_BASEERR), pt.x, pt.y);
		}

/* Display Warp and Degrees info */

	if(nQuad	!= nEntQ)
		{
		ptd.x = pt.x - nEX;
		ptd.y = pt.y - nEY;
		nDeg = CoordsToDeg(ptd);
		nWarp = GetWarpFromPoints(nEX, nEY, pt.x, pt.y);
		}
	else
		nDeg = nWarp = 0;
	DisplayComMessage(MAKEINTRESOURCE(COM_BASEDIR), nWarp, nDeg);


} /* End DisplayBaseInfo */




/***************************************************************************/

void LOCAL DisplayObjectInfo(int nY, int nX)

//	This function checks a sector location for an object, and displays
//	the info for the object. Locations are always given, followed by energy
//	and photon data.

/***************************************************************************/
{
	int		nAL;
	POINT		pt;
	int		nDeg, nDist;
	char		cStar;

  	switch(GetQObj(nY, nX))
		{
		case OBJ_BASE:
			DisplayComMessage(MAKEINTRESOURCE(COM_SCANBAS), nX+1, nY+1);
			break;

		case OBJ_STAR0:
			cStar = 'A';
			DisplayComMessage(MAKEINTRESOURCE(COM_SCANSTR), cStar, nX+1, nY+1);
			break;

		case OBJ_STAR1:
			cStar = 'B';
			DisplayComMessage(MAKEINTRESOURCE(COM_SCANSTR), cStar, nX+1, nY+1);
			break;

		case OBJ_STAR2:
			cStar = 'X';
			DisplayComMessage(MAKEINTRESOURCE(COM_SCANSTR), cStar, nX+1, nY+1);
			if(qd[nEntQ].Flags & QFLAG_RIC)
				DisplayComMessage(MAKEINTRESOURCE(COM_SCANSTR2));
			break;

		case OBJ_KLINGON:
			if((nAL = FindAlien(nEntQ, nX, nY)) == -1)
				{
				FATAL();
				break;
				}
			DisplayComMessage(MAKEINTRESOURCE(COM_SCANKLI), nX+1, nY+1,
				pAL[nAL].Energy);
			pt.x = nX - nEntX;
			pt.y = nY - nEntY;
			nDeg = CoordsToDeg(pt);
			nDist = GetWarpFromPoints(nEntX, nEntY, nX, nY);
			DisplayComMessage(MAKEINTRESOURCE(COM_PHODATA), nDeg, nDist);
			break;

		case OBJ_ROMULAN:
			if((nAL = FindAlien(nEntQ, nX, nY)) == -1)
				{
				FATAL();
				break;
				}
			DisplayComMessage(MAKEINTRESOURCE(COM_SCANROM), nX+1, nY+1,
				pAL[nAL].Energy);
			pt.x = nX - nEntX;
			pt.y = nY - nEntY;
			nDeg = CoordsToDeg(pt);
			nDist = GetWarpFromPoints(nEntX, nEntY, nX, nY);
			DisplayComMessage(MAKEINTRESOURCE(COM_PHODATA), nDeg, nDist);
			break;

		case OBJ_FARENG:
			if((nAL = FindAlien(nEntQ, nX, nY)) == -1)
				{
				FATAL();
				break;
				}
			DisplayComMessage(MAKEINTRESOURCE(COM_SCANFRG), nX+1, nY+1,
				pAL[nAL].Energy);
			pt.x = nX - nEntX;
			pt.y = nY - nEntY;
			nDeg = CoordsToDeg(pt);
			nDist = GetWarpFromPoints(nEntX, nEntY, nX, nY);
			DisplayComMessage(MAKEINTRESOURCE(COM_PHODATA), nDeg, nDist);
			break;

		case OBJ_BORG:
			if((nAL = FindAlien(nEntQ, nX, nY)) == -1)
				{
				FATAL();
				break;
				}
			DisplayComMessage(MAKEINTRESOURCE(COM_SCANBRG), nX+1, nY+1,
				pAL[nAL].Energy);
			pt.x = nX - nEntX;
			pt.y = nY - nEntY;
			nDeg = CoordsToDeg(pt);
			nDist = GetWarpFromPoints(nEntX, nEntY, nX, nY);
			DisplayComMessage(MAKEINTRESOURCE(COM_PHODATA), nDeg, nDist);
			break;

		case OBJ_ENTL:
		case OBJ_ENTR:
		default:
			break;
		}

} /* End DisplayObjectInfo */




/***************************************************************************/

void LOCAL DoRicochet(void)

//	This function initiates the ricochet effect.

/***************************************************************************/
{
	int	i;


	if(bSRSBusy)						 			/* Don't attempt is SRS action */
		{
		DisplayComMessage(MAKEINTRESOURCE(COM_SRSBUSY));
		return;
		}

	
	for(i = 10; i > -1; i--)
		{
		DisplayComMessage(MAKEINTRESOURCE(COM_RICCOUNT), i);
		Delay(500L);
		}
	PostMessage(hSRSWnd, WM_RICEXEC, 0, 0L);	  			/* let SRS do it */


} /* End DoRicochet */





/***************************************************************************/

void LOCAL DamReport(void)

//	This computer command displays damage report status. 

/***************************************************************************/
{
	const char	szSpec[] = "%-4.1f";
	const char	szOK[]  =   "OK";
	const char	szDM[]  =   "DESTROYED";
	char	szBuf[128];

	if(fMSGRepair > 0.0)
		sprintf(szBuf, szSpec, fMSGRep);
	else if(!fMSGRepair)
		strcpy(szBuf, szOK);
	else
		strcpy(szBuf, szDM);
	DisplayComMessage(MAKEINTRESOURCE(COM_DAMMSG), (LPSTR)szBuf);
	

	if(fPANRepair > 0.0)
		sprintf(szBuf, szSpec, fPANRep);
	else if(!fPANRepair)
		strcpy(szBuf, szOK);
	else
		strcpy(szBuf, szDM);
	DisplayComMessage(MAKEINTRESOURCE(COM_DAMPAN), (LPSTR)szBuf);


	if(fSRSRepair > 0.0)
		sprintf(szBuf, szSpec, fSRSRep);
	else if(!fSRSRepair)
		strcpy(szBuf, szOK);
	else
		strcpy(szBuf, szDM);
	DisplayComMessage(MAKEINTRESOURCE(COM_DAMSRS), (LPSTR)szBuf);


	if(fLRSRepair > 0.0)
		sprintf(szBuf, szSpec, fLRSRep);
	else if(!fLRSRepair)
		strcpy(szBuf, szOK);
	else
		strcpy(szBuf, szDM);
	DisplayComMessage(MAKEINTRESOURCE(COM_DAMLRS), (LPSTR)szBuf);


	if(fNAVRepair > 0.0)
		sprintf(szBuf, szSpec, fNAVRep);
	else if(!fNAVRepair)
		strcpy(szBuf, szOK);
	else
		strcpy(szBuf, szDM);
	DisplayComMessage(MAKEINTRESOURCE(COM_DAMNAV), (LPSTR)szBuf);


	if(fSHERepair > 0.0)
		sprintf(szBuf, szSpec, fSHERep);
	else if(!fSHERepair)
		strcpy(szBuf, szOK);
	else
		strcpy(szBuf, szDM);
	DisplayComMessage(MAKEINTRESOURCE(COM_DAMSHE), (LPSTR)szBuf);


	if(fPHARepair > 0.0)
		sprintf(szBuf, szSpec, fPHARep);
	else if(!fPHARepair)
		strcpy(szBuf, szOK);
	else
		strcpy(szBuf, szDM);
	DisplayComMessage(MAKEINTRESOURCE(COM_DAMPHA), (LPSTR)szBuf);


	if(fPHORepair > 0.0)
		sprintf(szBuf, szSpec, fPHORep);
	else if(!fPHORepair)
		strcpy(szBuf, szOK);
	else
		strcpy(szBuf, szDM);
	DisplayComMessage(MAKEINTRESOURCE(COM_DAMPHO), (LPSTR)szBuf);


	if(fDAMRepair > 0.0)
		sprintf(szBuf, szSpec, fDAMRep);
	else if(!fDAMRepair)
		strcpy(szBuf, szOK);
	else
		strcpy(szBuf, szDM);
	DisplayComMessage(MAKEINTRESOURCE(COM_DAMDAM), (LPSTR)szBuf);


} /* End DamReport */




/***************************************************************************/

void LOCAL AnalysisReport(void)

//	This computer command handles the analysis command. 

/***************************************************************************/
{
	char		szRes[128], szBuf[128];
	double	fMissionReq, fCurr, fWin;


/* get all the float values */

	fMissionReq = fMissionL/(double)(nAlienTotal);
	if(nAlienTotal-nAlienCnt > 5)
		fCurr = (fStarDNow-fStarDStart)/(double)(nAlienTotal-nAlienCnt);
	else
		fCurr = 0.0;
	fWin  = ((fStarDStart + fMissionL) - fStarDNow)/(double)(nAlienCnt);


/* display it */

	DisplayComMessage(MAKEINTRESOURCE(COM_ANAHEAD));

	LoadString(hInst,	COM_ANAGAM, szRes, sizeof(szRes));
	sprintf(szBuf, szRes, fMissionReq);
	DisplayComMessage(szBuf);
	
	if(fCurr)
		{
		LoadString(hInst,	COM_ANANOW1, szRes, sizeof(szRes));
		sprintf(szBuf, szRes, fCurr);
		DisplayComMessage(szBuf);
		}
	else
		DisplayComMessage(MAKEINTRESOURCE(COM_ANANOW2));

	LoadString(hInst,	COM_ANAREQ, szRes, sizeof(szRes));
	sprintf(szBuf, szRes, fWin);
	DisplayComMessage(szBuf);

	if(nAlienTotal-nAlienCnt >= (nAlienTotal / 4))			/* analyze play? */
		{
		if(fCurr)
			{
			if(fCurr > fMissionReq) 
				DisplayComMessage(MAKEINTRESOURCE(COM_ANAANA2));
			else
				DisplayComMessage(MAKEINTRESOURCE(COM_ANAANA1));
			}
		else
			ComCursor();
		}
	else
		ComCursor();

} /* End AnalysisReport */
