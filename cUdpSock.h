#pragma once

#include <windows.h>

/**	@file
	@brief	UDPソケット
	
	@author	TATSUYA
	@date	2007/03/01
*/

/**	@brief	UDPソケットクラス

	UDP通信を実装しているクラスです。
*/
class cUdpSock{
	public:
		cUdpSock(LPCSTR, USHORT);
		cUdpSock(USHORT, HWND, USHORT);
		~cUdpSock();

		BOOL			SendData(LPVOID, DWORD);
		BOOL			SendStr(LPCSTR);
		BOOL			RecvData(LPVOID, DWORD, DWORD*);
		BOOL			RecvDataOnce(LPVOID, DWORD, DWORD*);
		
		BOOL			MatchSock(SOCKET);

		LPCSTR			GetPeerIpAddress();
		USHORT			GetPeerPort();
		
		void			ErrMsgBox();
	private:
		BOOL			_InitSock();
		
		/// ソケットのアドレス情報
		SOCKADDR_IN		m_Addr;
		/// 接続先ホストのアドレス情報
		SOCKADDR_IN		m_AddrPeer;
		/// ソケットハンドル
		SOCKET			m_Socket;
		/// エラーメッセージ
		char			m_szErrMsg[2048];

};