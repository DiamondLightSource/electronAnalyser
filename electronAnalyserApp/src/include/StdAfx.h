// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__B66305C2_3805_11D5_AEAA_0050BAC6FF04__INCLUDED_)
#define AFX_STDAFX_H__B66305C2_3805_11D5_AEAA_0050BAC6FF04__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



#define WIN32_LEAN_AND_MEAN 
#if !defined WINVER
#define WINVER 0x0501
#endif

#include <afxwin.h>
#include <windows.h>
#include <stdio.h>
#include <winsock2.h>
#include "tiffio.h"


#if defined _MSC_VER
#if     _MSC_VER < 1400
extern int sprintf_s(char* buf, int dwlen, const char* cp, ...);
#endif
#endif
// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__B66305C2_3805_11D5_AEAA_0050BAC6FF04__INCLUDED_)
