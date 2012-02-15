#pragma once

#include <windows.h>

/**	@file
	@brief	TCP�\�P�b�g
	
	@author	TATSUYA
	@date	2007/03/01
*/

class cTcpServerSock;

/**	@brief	TCP�\�P�b�g�N���X

	TCP�ʐM���������Ă���N���X�ł��B
*/
class cTcpSock{
	public:
		// cTcpServerSock ���� private�ϐ� �ւ̃A�N�Z�X���\�ɂ���
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
		/// �ڑ��t���O
		BOOL			m_bIsConnect;
		/// �\�P�b�g�̃A�h���X���
		SOCKADDR_IN		m_Addr;
		/// �\�P�b�g�n���h��
		SOCKET			m_Socket;
		/// �G���[���b�Z�[�W
		char			m_szErrMsg[2048];

};