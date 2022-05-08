/////////////////////////////////////////////////////////////////////////
//
//  File Functions library 
// 
//  File: joes.h 
//  header for mlibjoes library
// 
//  (c) 1993 Joe Jaworski
//    17960 Squirrel Haven
//    Meadow Vista, CA 95722
//    ALL RIGHTS RESERVED.
//      
//  $Revision$
//      $Date$
//  $Log File$
//       $Log$
// 
// 
/////////////////////////////////////////////////////////////////////////


#ifdef __cplusplus
extern "C" {                     /* Assume C declarations for C++ */
#endif	/* __cplusplus */


/* Global Defines */

#ifndef _WINDOWS
#error <joes.h> included before <windows.h>
#endif


#define RGB_BLACK     (COLORREF)0x00000000L  /* RGB COLORREFs (0x00bbggrr) */
#define RGB_DKRED     (COLORREF)0x00000080L
#define RGB_DKGREEN   (COLORREF)0x00008000L
#define RGB_DKYELLOW  (COLORREF)0x00008080L
#define RGB_DKBLUE    (COLORREF)0x00800000L
#define RGB_DKPURPLE  (COLORREF)0x00800080L
#define RGB_DKAQUA    (COLORREF)0x00808000L
#define RGB_GRAY      (COLORREF)0x00C0C0C0L
#define RGB_DKGRAY    (COLORREF)0x00808080L
#define RGB_RED       (COLORREF)0x000000FFL
#define RGB_GREEN     (COLORREF)0x0000FF00L
#define RGB_YELLOW    (COLORREF)0x0000FFFFL
#define RGB_BLUE      (COLORREF)0x00FF0000L
#define RGB_PURPLE    (COLORREF)0x00FF00FFL
#define RGB_AQUA      (COLORREF)0x00FFFF00L
#define RGB_WHITE     (COLORREF)0x00FFFFFFL


#ifndef HPSTR
typedef  char huge    *HPSTR;
typedef  BYTE huge    *HPBYTE;
#endif


/* globally   defined vars */

#ifdef _JOES_INVOKED
char             _temp_[_MAX_PATH];                /* temp char var       */
OFSTRUCT       _of_;                               /* OpenFile structure  */
struct find_t  _ft_;                               /* Find File struct    */
#endif



/* Macros */

#if !defined(Assert)
#if defined(DEBUG)
#define Assert(exp) {if(exp){char szAA[40];\
sprintf(szAA, "File[%s]\nLine[%d]\nValue[%d]", __FILE__, __LINE__,(int)exp);\
MessageBox(NULL, szAA, "Assertion Trap", MB_OK|MB_ICONEXCLAMATION);}}
#else
#define Assert(exp)
#endif
#endif



#ifndef WaitCursor
#define WaitCursor() SetCursor(LoadCursor(NULL,IDC_WAIT))
#endif

#ifndef ArrowCursor
#define ArrowCursor() SetCursor(LoadCursor(NULL,IDC_ARROW))
#endif

#ifndef MAKECOORDS
#define MAKECOORDS(_c) _c.left, _c.top, _c.right, _c.bottom
#endif

#ifndef GetRand
#define GetRand(_c) ((int)(!_c) ? (0) : (rand() % _c))
#endif

#ifndef ADDSLASH
#define ADDSLASH(_c) (*(_c+(_fstrlen((LPSTR)_c)-1))!='\\') ? _fstrcat((LPSTR)_c,"\\") : ((LPSTR)_c)
#endif

#ifndef REMSLASH
#define REMSLASH(_c) (_c[(_fstrlen((LPSTR)_c))-1]=='\\') ? (_c[(_fstrlen((LPSTR)_c))-1]='\0') : (0)
#endif

#ifndef RCWIDTH
#define RCWIDTH(_c) ((int)_c.right-_c.left)
#endif

#ifndef RCHEIGHT
#define RCHEIGHT(_c) ((int)_c.bottom-_c.top)
#endif

#ifndef GlobalKill
#define GlobalKill(_c) {GlobalUnlock(_c);GlobalFree(_c);}
#endif

#ifndef MAKEWORD
#define MAKEWORD(low, high) ((WORD)(((BYTE)(low)) | ((WORD)((BYTE)(high))) << 8))
#endif

/* Macro: Determine the round off of the given value to the closest byte */
#ifndef WIDTHBYTES
#define WIDTHBYTES(i)   ((i+31)/32*4)
#endif

/* Macro: aligns given value to the closest DWORD (unsigned long ) */
#ifndef ALIGNULONG
#define ALIGNULONG(i)   ((i+3)/4*4)
#endif

/* Macro: Flush WIN.INI */
#ifndef FlushWININI
#define FlushWININI() WriteProfileString(NULL,NULL,NULL)
#endif

/* Macro: Restricts a given value to an upper or lower boundary value */
#ifndef LIMIT
#define LIMIT(x,min,max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))
#endif


/* Macro: Swaps two values */
#ifndef SWAP
#define SWAP(x,y)   ((x)^=(y)^=(x)^=(y))
#endif


/*  Function Prototypes */

#ifdef _MT
#define _FAR_ _far
#else
#define _FAR_
#endif

         
int   _FAR_ _cdecl ResBox(HINSTANCE, HWND, int, int, WORD);
int   _FAR_ _cdecl ResBoxEx(HINSTANCE, HWND, int, int, WORD, ...);                            
int   _FAR_ _cdecl ResizeWindow(HWND, LPRECT, BOOL, BOOL);
void  _FAR_ _cdecl GetRealClientRect(HWND, LPRECT);
LPSTR _FAR_ _cdecl CommaFormat(LPSTR);                                   
void  _FAR_ _cdecl HugeCopy(LPVOID, LPVOID, DWORD);                          
HGLOBAL _FAR_ _cdecl CopyBlock(HGLOBAL);
void  _FAR_ _cdecl CenterRect(LPRECT, LPRECT);
int   _FAR_ _cdecl MagnifyRect(LPRECT, int);
int   _FAR_ _cdecl CompareRect(LPRECT, LPRECT);
void  _FAR_ _cdecl NormalizeRect(LPRECT);
int   _FAR_ _cdecl CallDialog(int, HWND, DLGPROC);
BOOL CALLBACK _export OKDlgProc(HWND, unsigned, WORD, DWORD);



BOOL _FAR_ _cdecl    DrawBitmap(HDC, int, int, HBITMAP, DWORD);
HBITMAP _FAR_ _cdecl BitmapFromDib(LPBITMAPINFOHEADER, HPALETTE);
WORD _FAR_ _cdecl    PaletteSize(LPVOID);
HPALETTE _FAR_ _cdecl CreateBIPalette(LPBITMAPINFOHEADER);
WORD _FAR_ _cdecl    DibNumColors(LPVOID);
HBITMAP _FAR_ _cdecl LoadBMPFile(LPSTR);
HBITMAP _FAR_ _cdecl CropBitmap(HBITMAP, LPRECT);
DWORD _FAR_ _cdecl   BitmapSize(LPVOID);                                       
HGLOBAL _FAR_ _cdecl DibFromBitmap(HBITMAP, HPALETTE);                  
int _FAR_ _cdecl     FillSysPalette(LPBITMAPINFOHEADER);                               
DWORD _FAR_ _cdecl   GetDIBDimensions(LPVOID);
LPSTR _FAR_ _cdecl   OffsetToBits(LPVOID);                                       
HBITMAP _FAR_ _cdecl CopyBitmap(HBITMAP);                                                 
void _FAR_ _cdecl    OwnerDrawBitmap(LPDRAWITEMSTRUCT, HBITMAP);
void _FAR_ _cdecl    OwnerDrawIcon(LPDRAWITEMSTRUCT, HICON);
HPALETTE _FAR_ _cdecl CopyPalette(HPALETTE);                                               


DWORD _FAR_ _cdecl  HugeRead(HFILE, LPSTR, DWORD);
DWORD _FAR_ _cdecl  HugeWrite(HFILE, LPSTR, DWORD);                           
BOOL  _FAR_ _cdecl  ClosePath(LPSTR);                                   
HGLOBAL _FAR_ _cdecl ReadFile(LPSTR, LPDWORD);
DWORD  _FAR_ _cdecl WriteFile(LPSTR, HGLOBAL, DWORD);

BOOL _FAR_ _cdecl GetPrivBoolINI(HINSTANCE, int, int, BOOL, LPCSTR);
BOOL _FAR_ _cdecl GetBoolINI(HINSTANCE, int, int, BOOL);
BOOL _FAR_ _cdecl WritePrivBoolINI(HINSTANCE, int, int, BOOL, WORD, LPCSTR);
BOOL _FAR_ _cdecl WriteBoolINI(HINSTANCE, int, int, BOOL, WORD);
BOOL _FAR_ _cdecl WritePrivProfInt(HINSTANCE, int, int, int, LPCSTR);
BOOL _FAR_ _cdecl WriteProfInt(HINSTANCE, int, int, int);
BOOL _FAR_ _cdecl WritePrivProfLong(HINSTANCE, int, int, DWORD, LPCSTR);
BOOL _FAR_ _cdecl WriteProfLong(HINSTANCE, int, int, DWORD);
WORD _FAR_ _cdecl GetPrivProfInt(HINSTANCE, int, int, int, LPCSTR);
DWORD _FAR_ _cdecl GetPrivProfLong(HINSTANCE, int, int, DWORD, LPCSTR);
WORD _FAR_ _cdecl GetProfInt(HINSTANCE, int, int, int);
DWORD _FAR_ _cdecl GetProfLong(HINSTANCE, int, int, DWORD);
int  _FAR_ _cdecl GetProfString(HINSTANCE, int, int, LPSTR, LPSTR, int);
int  _FAR_ _cdecl GetPrivProfString(HINSTANCE, int, int, LPSTR, LPSTR, int, LPCSTR);
BOOL _FAR_ _cdecl WriteProfString(HINSTANCE, int, int, LPSTR);
BOOL _FAR_ _cdecl WritePrivProfString(HINSTANCE, int, int, LPSTR, LPCSTR);
BOOL _FAR_ _cdecl FlushINI(HINSTANCE, LPCSTR);


#ifdef __cplusplus
}                       /* End of extern "C" { */
#endif	/* __cplusplus */


