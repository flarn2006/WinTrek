/******************************************
 *		 		STAR TREK CLASSIC	 				
 *								  						
 *	Opening graphics
 *
 * Revision:	$Revision:   1.4  $
 * Date:			$Date:   20 Feb 1993 09:27:50  $
 * Log File:	$Logfile:   C:/src/st/vcs/open.c_v  $
 * Log:			$Log:   C:/src/st/vcs/open.c_v  $
 * 
 *    Rev 1.4   20 Feb 1993 09:27:50   jaworski
 * Initial Revision
 *								  						
 ******************************************/

#define	OPEN_C
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


static int nX, nY;
static BITMAP  bm;
static HBITMAP hOrigBmp, hShipBmp;
static HDC		hOrigDC, hShipDC;


/***************************************************************************/

void Opening(void)

//	This function displays the opening graphics.

/***************************************************************************/
{
	WNDCLASS wcParms;
	HWND	   hOpenWnd;
	HDC		hDC;
	HBITMAP	hOldShipBmp, hOldOrigBmp;

	hShipBmp = LoadBitmap(hLib, MAKEINTRESOURCE(BMP_OPENENT));
	GetObject(hShipBmp, sizeof(BITMAP), (LPSTR)&bm);
	nX = GetSystemMetrics(SM_CXFULLSCREEN)+3;
	nY = GetSystemMetrics(SM_CYFULLSCREEN) / 3;

/* register and create */

	wcParms.style          = CS_SAVEBITS | CS_BYTEALIGNWINDOW;
   wcParms.lpfnWndProc    = (WNDPROC)DefWindowProc;
   wcParms.cbClsExtra     = 0;
   wcParms.cbWndExtra     = 0;
   wcParms.hInstance      = hInst;
   wcParms.hIcon          = NULL;
   wcParms.hCursor        = NULL;
   wcParms.hbrBackground  = GetStockObject(HOLLOW_BRUSH);
   wcParms.lpszMenuName   = NULL;
   wcParms.lpszClassName  = (LPSTR)szEXTClass;  
	RegisterClass(&wcParms);

	hOpenWnd = CreateWindow(szEXTClass, szEXTClass, WS_POPUP, 0, 0,
		GetSystemMetrics(SM_CXFULLSCREEN), GetSystemMetrics(SM_CYFULLSCREEN),
		NULL, NULL, hInst, NULL);

	WaitCursorX(hOpenWnd);
	ShowWindow(hOpenWnd, SW_NORMAL);
	UpdateWindow(hOpenWnd);

	hDC = GetDC(hOpenWnd);

	hOrigBmp  = CreateCompatibleBitmap(hDC, bm.bmWidth, bm.bmHeight);

	hShipDC  = CreateCompatibleDC(hDC);
	hOrigDC  = CreateCompatibleDC(hDC);
	hOldShipBmp = SelectBitmap(hShipDC, hShipBmp);
	hOldOrigBmp = SelectBitmap(hOrigDC, hOrigBmp);

	while(OpenProc(hOpenWnd, hDC));

	ArrowCursorX();
	ReleaseDC(hOpenWnd, hDC);
	SelectBitmap(hShipDC, hOldShipBmp);
	SelectBitmap(hOrigDC, hOldOrigBmp);
	DeleteDC(hShipDC);
	DeleteDC(hOrigDC);
	DeleteBitmap(hOrigBmp);
	DeleteBitmap(hShipBmp);
	ReleaseCapture();
	DestroyWindow(hOpenWnd);
	UnregisterClass(szEXTClass, hInst);


} /* End Opening */




/***************************************************************************/

WORD LOCAL OpenProc(HWND hWnd, HDC hDC)

//	This is the function for the opening ship movements.

/***************************************************************************/
{

	BitBlt(hDC, nX, nY, bm.bmWidth, bm.bmHeight, hOrigDC, 0, 0, SRCCOPY);
	nX -=3;
	BitBlt(hOrigDC, 0,0, bm.bmWidth, bm.bmHeight, hDC, nX, nY, SRCCOPY);
	BitBlt(hDC, nX, nY, bm.bmWidth, bm.bmHeight, hShipDC, 0, 0, SRCINVERT);


	if(nX < -bm.bmWidth)
		return 0;

	return 1;

} /* End OpenProc */
