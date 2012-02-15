#pragma once

#include <windows.h>

/**	@file
	@brief	TCPソケット
	
	@author	TATSUYA
	@date	2007/03/01
*/

class cTcpServerSock;

/**	@brief	TCPソケットクラス

	TCP通信を実装しているクラスです。
*/
class cTcpSock{
	public:
		// cTcpServerSock から private変数 へのアクセスを可能にする
		friend class cTcpServerSock;

		cTcpSock();
		~cTcpSock();

		BOOL			IsConnect();

		virtual BOOL	Connect(LPCSTR, USHORT, HWND, USHORT);
		virtual BOOL	Disconnect();

		virtual BOOL	SendData(LPVOID, DWORD);
		BOOL			SendStr(LPCSTR);
		virtual BOOL	RecvData(LPVOID, DWORD, DWORD*);
		virtual BOOL	RecvDataOnce(LPVOID, DWORD, DWORD*);

		BOOL			SetAsync(HWND, USHORT);
		BOOL			MatchSock(SOCKET);

		LPCSTR			GetPeerIpAddress();
		USHORT			GetPeerPort();
		
		void			ErrMsgBox();
	protected:
		/// 接続フラグ
		BOOL			m_bIsConnect;
		/// ソケットのアドレス情報
		SOCKADDR_IN		m_Addr;
		/// ソケットハンドル
		SOCKET			m_Socket;
		/// エラーメッセージ
		char			m_szErrMsg[2048];

};