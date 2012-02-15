#pragma once

#include <windows.h>

/**	@file
	@brief	UDP�\�P�b�g
	
	@author	TATSUYA
	@date	2007/03/01
*/

/**	@brief	UDP�\�P�b�g�N���X

	UDP�ʐM���������Ă���N���X�ł��B
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
		
		/// �\�P�b�g�̃A�h���X���
		SOCKADDR_IN		m_Addr;
		/// �ڑ���z�X�g�̃A�h���X���
		SOCKADDR_IN		m_AddrPeer;
		/// �\�P�b�g�n���h��
		SOCKET			m_Socket;
		/// �G���[���b�Z�[�W
		char			m_szErrMsg[2048];

};