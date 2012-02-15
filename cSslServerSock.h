#pragma once

#include "cTcpServerSock.h"
#include "cSslSock.h"

/**	@file
	@brief	SSL Serverソケット
	
	@author	TATSUYA
	@date	2007/02/28
*/

/**	@brief	SSL Serverソケットクラス

	SSL通信のサーバ機能を実装しているクラスです。
*/
class cSslServerSock : public cTcpServerSock{
	public:
		cSslServerSock();
		~cSslServerSock();

		BOOL	Accept(cSslSock*);
		SSL_CTX	*GetCTX();
	private:
		/// SSL Serverで利用するCTX構造体
		SSL_CTX	*m_CTX;
};