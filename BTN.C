/*************************************
 *		 STAR TREK CLASSIC	 			
 *								  				
 * Button MDI Window processing		
 *								  				
 * Revision:	$Revision:   1.4  $
 * Date:			$Date:   20 Feb 1993 09:27:50  $
 * Log File:	$Logfile:   C:/src/st/vcs/btn.c_v  $
 * Log:			$Log:   C:/src/st/vcs/btn.c_v  $
 * 
 *    Rev 1.4   20 Feb 1993 09:27:50   jaworski
 * Initial Revision
 *************************************/

#define	BTN_C
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


HBITMAP	hBtnBmp[8];

#define IDC_SRS 10
#define IDC_LRS 11
#define IDC_NAV 12
#define IDC_SHE 13
#define IDC_PHO 14
#define IDC_PHA 15
#define IDC_DAM 16
#define IDC_COM 17



/*************************************************************************/
LRESULT CALLBACK _export BtnDefProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)

// This is the Button (BTN) MDI definition procedure. 
                                                           
/**************************************************************************/
{
	RECT		rc;
	int		i;
	LPDRAWITEMSTRUCT	lpDIS;
	DWORD		dwStyle;
	char		szBtn[8];

	hBTNWnd = hWnd;

	switch(msg)
   	{
		case WM_CREATE:
			dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CAPTION |
				WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZE | WS_MINIMIZEBOX;
			SetWindowLong(hWnd, GWL_STYLE, (LPARAM)dwStyle);

			for(i = 0; i < 8; i++)
				hBtnBmp[i] = LoadBitmap(hLib, MAKEINTRESOURCE(BMP_BTNSRS + i));

			LoadString(hInst, RS_BTN, szBtn, sizeof(szBtn));
			CreateWindow(szBtn, NULL, BS_OWNERDRAW | WS_CHILD | WS_VISIBLE |
				WS_TABSTOP, 10,  10,  60, 36, hWnd, (HMENU)IDC_SRS, hInst, NULL);
			CreateWindow(szBtn, NULL, BS_OWNERDRAW | WS_CHILD | WS_VISIBLE |
				WS_TABSTOP, 72,  10,  60, 36, hWnd, (HMENU)IDC_LRS, hInst, NULL);
			CreateWindow(szBtn, NULL, BS_OWNERDRAW | WS_CHILD | WS_VISIBLE |
				WS_TABSTOP, 134, 10,  60, 36, hWnd, (HMENU)IDC_NAV, hInst, NULL);
			CreateWindow(szBtn, NULL, BS_OWNERDRAW | WS_CHILD | WS_VISIBLE |
				WS_TABSTOP, 196, 10,  60, 36, hWnd, (HMENU)IDC_SHE, hInst, NULL);
			CreateWindow(szBtn, NULL, BS_OWNERDRAW | WS_CHILD | WS_VISIBLE |
				WS_TABSTOP, 10,  48,  60, 36, hWnd, (HMENU)IDC_PHO, hInst, NULL);
			CreateWindow(szBtn, NULL, BS_OWNERDRAW | WS_CHILD | WS_VISIBLE |
				WS_TABSTOP, 72,  48,  60, 36, hWnd, (HMENU)IDC_PHA, hInst, NULL);
			CreateWindow(szBtn, NULL, BS_OWNERDRAW | WS_CHILD | WS_VISIBLE |
				WS_TABSTOP, 134, 48,  60, 36, hWnd, (HMENU)IDC_DAM, hInst, NULL);
			CreateWindow(szBtn, NULL, BS_OWNERDRAW | WS_CHILD | WS_VISIBLE |
				WS_TABSTOP, 196, 48,  60, 36, hWnd, (HMENU)IDC_COM, hInst, NULL);

			break;

		case WM_ICONERASEBKGND:
			GetClientRect(hWnd, &rc);
			FillRect((HDC)wParam, &rc, GetStockObject(BLACK_BRUSH));
			break;


		case WM_GETMINMAXINFO:
			(((LPPOINT)(lParam))+3)->x = SIZ_BTNX +
				GetSystemMetrics(SM_CXFRAME);
         (((LPPOINT)(lParam))+3)->y = SIZ_BTNY +
				GetSystemMetrics(SM_CYCAPTION) +	GetSystemMetrics(SM_CYFRAME);
        	return DefMDIChildProc(hWnd, msg, wParam, lParam);


		case WM_SIZE:
			if(wParam == SIZE_RESTORED)
				MoveButtons(hWnd, LOWORD(lParam), HIWORD(lParam));
			break;


		case WM_DESTROY:
			for(i = 0; i < 8; i++)
				{											  /* free bitmaps and windows */
				DeleteBitmap(hBtnBmp[i]);
				DestroyWindow(GetDlgItem(hWnd, IDC_SRS + i));
				}
			break;

		case WM_DRAWITEM:                                    /* Draw a button */
			if(!bSRSBusy)
				{
				lpDIS = (LPDRAWITEMSTRUCT)lParam;
				if(lpDIS->CtlType == ODT_BUTTON)
               OwnerDrawBitmap(lpDIS, hBtnBmp[lpDIS->CtlID-IDC_SRS]);
				}
         break;


		case WM_PAINT:
			PaintBtn(hWnd);
			break;


	  	case WM_COMMAND:
			if(!bSRSBusy)
				{
				switch(wParam)							/* based on control/menu ID's */
					{

	/* Process Button Hits */

					case IDC_SRS: Activate(hSRSWnd);	break;
					case IDC_LRS: Activate(hLRSWnd);	break;
					case IDC_NAV: Activate(hNAVWnd);	break;
					case IDC_SHE: Activate(hSHEWnd);	break;
					case IDC_PHO: Activate(hPHOWnd);	break;
					case IDC_PHA: Activate(hPHAWnd);	break;
					case IDC_DAM: Activate(hDAMWnd);	break;
					case IDC_COM: Activate(hCOMWnd);	break;
					}
				}
			break;


     	default:
         	return(DefMDIChildProc(hWnd, msg, wParam, lParam));
         	break;
     	}

	return (LRESULT)0;

} /* End BtnDefProc */






/***************************************************************************/

void LOCAL PaintBtn(HWND hWnd)

//	This function draws the button screws.

/***************************************************************************/
{
	RECT	rc;
	HDC			hDC;
	PAINTSTRUCT	ps;



	hDC = BeginPaint(hWnd, &ps);
	GetClientRect(hWnd, &rc);

	DrawScrew(hDC, rc.left  + 3, rc.top    + 3);
	DrawScrew(hDC, rc.left  + 3, rc.bottom - 6);
	DrawScrew(hDC, rc.right - 6, rc.top    + 3);
	DrawScrew(hDC, rc.right - 6, rc.bottom - 6);

	EndPaint(hWnd, &ps);


} /* End PaintBtn */




/***************************************************************************/

void LOCAL DrawScrew(HDC hDC, int nX, int nY)

//	This function draws the little window screw at the designated coordinates

/***************************************************************************/
{

	SetPixel(hDC, nX,   nY,   RGB_WHITE );
	SetPixel(hDC, nX+1, nY,   RGB_DKGRAY);
	SetPixel(hDC, nX+2, nY,   RGB_DKGRAY);
	SetPixel(hDC, nX,   nY+1, RGB_DKGRAY);
	SetPixel(hDC, nX+1, nY+1, RGB_DKGRAY);
	SetPixel(hDC, nX+2, nY+1, RGB_BLACK );
	SetPixel(hDC, nX,   nY+2, RGB_DKGRAY);
	SetPixel(hDC, nX+1, nY+2, RGB_BLACK );
	SetPixel(hDC, nX+2, nY+2, RGB_BLACK );


} /* End DrawScrew */






/***************************************************************************/

void LOCAL MoveButtons(HWND hWnd, int nWidth, int nHeight)

//	This function repositions the buttons after a WM_SIZE message has 
//	occured.
//

/***************************************************************************/
{
	int	i, nLeft, nTop, nXSpacing, nVSpacing;


	nXSpacing = (nWidth - 80) / 3;
	nLeft = 10;
	nVSpacing = (nHeight - 72) /3;
	nTop = nVSpacing + 5;

	for(i = IDC_SRS; i < IDC_COM + 1; i++)
		{
		MoveWindow(GetDlgItem(hWnd, i), nLeft, nTop, 60, 36, TRUE);
		if(i == IDC_SHE)
			{
			nTop  = (nVSpacing *2) + 36;
			nLeft = 10;
			}
		else
			nLeft += nXSpacing;
		}

} /* End MoveButtons */

