#pragma once

#include "cTcpServerSock.h"
#include "cSslSock.h"

/**	@file
	@brief	SSL Server�\�P�b�g
	
	@author	TATSUYA
	@date	2007/02/28
*/

/**	@brief	SSL Server�\�P�b�g�N���X

	SSL�ʐM�̃T�[�o�@�\���������Ă���N���X�ł��B
*/
class cSslServerSock : public cTcpServerSock{
	public:
		cSslServerSock();
		~cSslServerSock();

		BOOL	Accept(cSslSock*);
		SSL_CTX	*GetCTX();
	private:
		/// SSL Server�ŗ��p����CTX�\����
		SSL_CTX	*m_CTX;
};