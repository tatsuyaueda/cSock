#pragma once

#include "cTcpSock.h"

/**	@file
	@brief	TCP Serverソケット
	
	@author	TATSUYA
	@date	2007/03/01
*/

/**	@brief	TCP Serverソケットクラス

	TCP通信のサーバ機能を実装しているクラスです。
*/
class cTcpServerSock{
	public:
		cTcpServerSock();
		~cTcpServerSock();

		BOOL			IsListening();

		BOOL			Listen(USHORT, HWND, USHORT);
		virtual BOOL	Accept(cTcpSock*);
		BOOL			CloseListen();
		BOOL			SetAsync(HWND, USHORT);
		void			ErrMsgBox();
	protected:
		/// 待ち受けフラグ
		BOOL			m_bIsListening;
		/// サーバソケットのアドレス情報
		SOCKADDR_IN		m_ServerAddr;
		/// サーバソケットハンドル
		SOCKET			m_ServerSocket;
		/// エラーメッセージ
		char			m_szErrMsg[2048];
};