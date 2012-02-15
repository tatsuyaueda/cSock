#pragma once

#include <windows.h>

/**	@file
	@brief	Windows�\�P�b�g
	
	@author	TATSUYA
	@date	2007/02/28
*/

/**	@brief	Windows�\�P�b�g�N���X

	Windows�\�P�b�g�̏������E������������Ă���N���X�ł��B
*/
class cWinSock{
	public:
		cWinSock();
		~cWinSock();

		BOOL		Startup();
		BOOL		Release();
		void		ErrMsgBox();
	private:
		/// Windows�\�P�b�g�̃f�[�^
		WSADATA		m_wsadata;
		/// Windows�\�P�b�g���������t���O
		BOOL		m_bStartUp;
		/// �G���[���b�Z�[�W
		char		m_szErrMsg[2048];

};