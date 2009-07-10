#pragma once
// include the MZFC library header file
#include <mzfc_inc.h>

class UiControlBackground :
	public UiWin
{
public:
	UiControlBackground(void);
	~UiControlBackground(void);
	virtual void PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate){
		UiWin::PaintWin(hdcDst, prcWin, prcUpdate);
		MzDrawGridPopupMenuBG_480(hdcDst,prcWin);
	}
};
