#pragma once

#include <windows.h>

/**	@file
	@brief	Windowsソケット
	
	@author	TATSUYA
	@date	2007/02/28
*/

/**	@brief	Windowsソケットクラス

	Windowsソケットの初期化・解放を実装しているクラスです。
*/
class cWinSock{
	public:
		cWinSock();
		~cWinSock();

		BOOL		Startup();
		BOOL		Release();
		void		ErrMsgBox();
	private:
		/// Windowsソケットのデータ
		WSADATA		m_wsadata;
		/// Windowsソケットが初期化フラグ
		BOOL		m_bStartUp;
		/// エラーメッセージ
		char		m_szErrMsg[2048];

};