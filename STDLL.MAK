#*************************************************************************
#*
#*  (c) 1992 Joe Jaworski
#*   18405 Tamarind Street
#*  Fountain Valley, CA 92708
#*   ALL RIGHTS RESERVED.
#*
#* Created and Written by Joe Jaworski
#* Microsoft C 6.00A, SDK 3.1
#*
#*
#* Revision:    $Revision:   1.5  $
#* Date:            $Date:   05 Dec 1994 21:17:40  $
#* Log File:    $Logfile:   C:/src/st/vcs/stdll.mav  $
#* Log:         $Log:   C:/src/st/vcs/stdll.mav  $
#* 
#*    Rev 1.5   05 Dec 1994 21:17:40   jaworski
#* add /noe
#* 
#*    Rev 1.4   20 Feb 1993 09:27:50   jaworski
#* Initial Revision
#*
#**************************************************************************
all: STDLL1.DLL STDLL2.DLL

CLF=-c -Asnw -Gsw2 -Os -Zp -W3  -WX
LNK= /NOD /align:16 /NOE
RES= resource.h pix\*.bmp pix\*.ico snd\*.wav

STDLL1.obj: STDLL1.c
    cl $(CLF) STDLL1.c

STDLL1.res: STDLL1.rc $(RES)
    rc -r STDLL1.rc

STDLL1.dll: STDLL1.obj STDLL1.def STDLL1.res
    link $(LNK) STDLL1.obj libentry.obj, STDLL1.dll,, libw sdllcew, STDLL1.def
    rc STDLL1.res STDLL1.dll
    del WINTREK1.DLL
    rename STDLL1.dll WINTREK1.DLL


STDLL2.obj: STDLL2.c
    cl $(CLF) STDLL2.c

STDLL2.res: STDLL2.rc $(RES)
    rc -r STDLL2.rc

STDLL2.dll: STDLL2.obj STDLL2.def STDLL2.res
    link $(LNK) STDLL2.obj libentry.obj, STDLL2.dll,, libw sdllcew, STDLL2.def
    rc STDLL2.res STDLL2.dll
    del WINTREK2.DLL
    rename STDLL2.dll WINTREK2.DLL
