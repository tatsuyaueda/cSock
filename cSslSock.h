#pragma once

#include <openssl/ssl.h>
#include <openssl/err.h>
#include "cTcpSock.h"

/**	@file
	@brief	SSL�\�P�b�g
	
	@author	TATSUYA
	@date	2007/02/28
*/

class cSslServerSock;

typedef struct tagX509Info{
	char	szCountry[3];
	LPSTR	szState;
	LPSTR	szLocality;
	LPSTR	szOrganization;
	LPSTR	szOrganizationUnit;
	LPSTR	szCommonName;
	LPSTR	szEMail;
} _X509Info;

typedef struct tagCertficate{
	_X509Info	x509Subject;
	_X509Info	x509Issuer;
	char		szStartDate[20];
	char		szEndDate[20];
} _Certificate;

/**	@brief	SSL�\�P�b�g�N���X

	SSL�ʐM���������Ă���N���X�ł��B
*/
class cSslSock : public cTcpSock{
	public:
		// cSslServerSock ���� private�ϐ� �ւ̃A�N�Z�X���\�ɂ���
		friend class cSslServerSock;

		cSslSock(SSL_CTX* = NULL);
		~cSslSock();

		LPCSTR			GetSSLCipher();
		BOOL			SetSSLCiphers(LPCSTR);
		int				GetSSLBits();
		LPCSTR			GetSSLVerion();

		BOOL			SetCert(LPCSTR, LPCSTR);
		BOOL			SetCaCert(LPCSTR);

		virtual BOOL	Connect(LPCSTR, USHORT, HWND, USHORT);
		virtual BOOL	Disconnect();

		virtual BOOL	SendData(LPVOID, DWORD);
		virtual BOOL	RecvData(LPVOID, DWORD, DWORD*);
		virtual BOOL	RecvDataOnce(LPVOID, DWORD, DWORD*);

		LPCSTR			Verify_SSL();
		BOOL			GetPeerCert(_Certificate*);
		void			DestroyCertInfo(_Certificate*);
	protected:
		/// CTX�\����
		SSL_CTX			*m_CTX;
		/// SSL�\����
		SSL				*m_SSL;
	private:
		void			_PushCertificate(_X509Info*, LPCSTR);
		void			_PushCertificate_(_X509Info*, LPCSTR);

};