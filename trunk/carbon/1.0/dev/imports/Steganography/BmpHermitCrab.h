#pragma once
/*
	Description:	this class help you to hide a file in a bmp file.
	
	Principle :		it refer to http://www.codeproject.com/useritems/HideIt.asp article.
					it using the pixel color micro-changed, and we almost can't feel it.
					with our bulit binary table, we can put it.

					the code has none remark for the principle.
					
					if you want to know more detail information, please refer to  
					http://www.codeproject.com/useritems/HideIt.asp article and its code.
	
	Author:			origin arithmetic is from Ahmed Osama.
					this code is from Neil Xu.

	Restrict:		please keep the origin declare in your applicaiton.

    Limited:		we don't test in all kind of  BMP file formats, so we don't ensure 
				    it does work well for all kind of BMP file formats.

 */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#define _AFXDLL

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

class CBmpHermitCrab
{
public:
	CBmpHermitCrab(void);
	~CBmpHermitCrab(void);
private:
	static char* m_binaryMap[];
	static WORD	 m_wHermitCrabFlag;
	typedef struct tagHERMITCRAB_HEAD {
		WORD	wFlag;
		BYTE	bPasswordLen;
		BYTE    bFileNameLen;
		int		iFileLength;
	} HERMITCRAB_HEAD, *LPHERMITCRAB_HEAD;
public:
	bool	Hermit(const char* pszDestFileName,const char* pszSrcFileName,const char* pszPassword=NULL,const char* pszResultFile=NULL); 
	bool	IsHermitCrabBMPFile(const char* pszFileName);
	bool	RecoverCrabFile(const char* pszFileName,const char* pszPassword=NULL,const char* pszPath=NULL);
};
