/**************************************************************************
 *                               WINTREK DLL LIBRARY- 2
 *  (c) 1992 Joe Jaworski
 *   18405 Tamarind Street
 *  Fountain Valley, CA 92708
 *   ALL RIGHTS RESERVED.
 *
 * Created and Written by Joe Jaworski
 * Microsoft C 6.00A, SDK 3.1
 *
 *
 * Revision:    $Revision:   1.5  $
 * Date:            $Date:   05 Dec 1994 21:14:04  $
 * Log File:    $Logfile:   C:/src/st/vcs/stdll2.c_v  $
 * Log:         $Log:   C:/src/st/vcs/stdll2.c_v  $
 * 
 *    Rev 1.5   05 Dec 1994 21:14:04   jaworski
 * remove _windows
 * 
 *    Rev 1.4   20 Feb 1993 09:27:50   jaworski
 * Initial Revision
 *
 ***************************************************************************/

#define  _WINDLL
#include <windows.h>

/* Prototypes */

int FAR PASCAL LibMain(HANDLE hInstance, WORD wDataSeg, WORD cbHeapSize, LPSTR lpszCmdLine);
int FAR PASCAL WEP(int nExit);



/**************************************************************************/
int FAR PASCAL LibMain(HANDLE hInstance, WORD wDataSeg, WORD cbHeapSize, LPSTR lpszCmdLine)
//
//  Main Windows Library entry point to initialize the library.
//
/**************************************************************************/
{

    return 1;

} /* End LibMain */



/**************************************************************************/

int FAR PASCAL WEP(int nExit)

//  DLL Library exit procedure.

/**************************************************************************/
{
    return(1);

} /* End WEP */


