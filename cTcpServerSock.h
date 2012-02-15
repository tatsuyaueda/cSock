#pragma once

#include "cTcpSock.h"

/**	@file
	@brief	TCP Server�\�P�b�g
	
	@author	TATSUYA
	@date	2007/03/01
*/

/**	@brief	TCP Server�\�P�b�g�N���X

	TCP�ʐM�̃T�[�o�@�\���������Ă���N���X�ł��B
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
		/// �҂��󂯃t���O
		BOOL			m_bIsListening;
		/// �T�[�o�\�P�b�g�̃A�h���X���
		SOCKADDR_IN		m_ServerAddr;
		/// �T�[�o�\�P�b�g�n���h��
		SOCKET			m_ServerSocket;
		/// �G���[���b�Z�[�W
		char			m_szErrMsg[2048];
};