#include "cSslServerSock.h"

/**	@file
	@brief	SSL Server�\�P�b�g
	
	@author	TATSUYA
	@date	2007/02/28
*/

/******************************************************************************/
/** @brief	�R���X�g���N�^
	
	�N���X�̏��������s���܂��B
	OpenSSL�̏�������CTX�\���̂̏��������R���X�g���N�^�ōs���Ă��܂��B
*/
cSslServerSock::cSslServerSock(){

	// OpenSSL �̏�����
	SSL_library_init();
	// �G���[�𕶎���Ƃ��ĕ\��
	SSL_load_error_strings();

	// �����l�ݒ�
	m_CTX		= NULL;

	// CTX�\���̂̐ݒ�
	if((m_CTX = SSL_CTX_new(TLSv1_server_method())) == NULL){
		wsprintf(m_szErrMsg, "CTX�\���̂̐ݒ�Ɏ��s���܂����B\n"
							 "SSL Error:%s",
								ERR_error_string(ERR_get_error(), NULL));
		return;
	}

	// ����I�� �o�b�t�@���N���A
	ZeroMemory(m_szErrMsg, sizeof(m_szErrMsg));

}

/******************************************************************************/
/** @brief	�f�X�g���N�^
    
    �N���X�̉�����s���܂��B
*/
cSslServerSock::~cSslServerSock(){

	if(m_CTX != NULL){
		SSL_CTX_free(m_CTX);
	}

	ERR_free_strings();
	EVP_cleanup();
	CRYPTO_cleanup_all_ex_data();

}

/******************************************************************************/
/** 
	SSL�\�P�b�g�̐ڑ��v�����󂯓���܂��B
	
	@param[in] oSslSock cSslSock�̃I�u�W�F�N�g�|�C���^
	@return ���������Ƃ���TRUE��Ԃ��B
	���s�����Ƃ���FALSE��Ԃ��B
*/
BOOL cSslServerSock::Accept(cSslSock* oSslSock){

	// �ϐ��錾
	int	iRet	= 0;
	int iErrNo	= 0;

	// �ʏ��Accept�����s
	if(!cTcpServerSock::Accept(oSslSock)){
		return FALSE;
	}

	// SSL�\���̂̍Đ���
	if((oSslSock->m_SSL = SSL_new(oSslSock->m_CTX)) == NULL){
		wsprintf(m_szErrMsg, "SSL�\���̂̍Đ����Ɏ��s���܂����B\n"
							 "SSL Error:%s",
								ERR_error_string(ERR_get_error(), NULL));
		// TCP�\�P�b�g���ڑ����̏ꍇ�͐ؒf
		if(oSslSock->IsConnect()){
			oSslSock->Disconnect();
		}
		return FALSE;
	}

	// SSL�ƃ\�P�b�g�̊֘A�t��
	if(!SSL_set_fd(oSslSock->m_SSL, (int)oSslSock->m_Socket)){
		wsprintf(m_szErrMsg, "SSL�ƃ\�P�b�g�̊֘A�t���Ɏ��s���܂����B\n"
							 "SSL Error:%s",
								ERR_error_string(ERR_get_error(), NULL));
		// TCP�\�P�b�g���ڑ����̏ꍇ�͐ؒf
		if(oSslSock->IsConnect()){
			oSslSock->Disconnect();
		}
		return FALSE;
	}

	// SSL��Accept�����s
	do{
		iRet	= SSL_accept(oSslSock->m_SSL);
		iErrNo  = SSL_get_error(oSslSock->m_SSL, iRet);
	}while((iErrNo == SSL_ERROR_WANT_X509_LOOKUP || iErrNo == SSL_ERROR_WANT_READ) && iRet <= 0);

	if(iRet <= 0){
		wsprintf(m_szErrMsg, "SSL��Accept�Ɏ��s���܂����B\n"
							 "SSL Error:%s",
								ERR_error_string(iErrNo, NULL));
		// TCP�\�P�b�g���ڑ����̏ꍇ�͐ؒf
		if(oSslSock->IsConnect()){
			oSslSock->Disconnect();
		}
		return FALSE;
	}

 	return TRUE;

}

/******************************************************************************/
/** 
	�T�[�o�\�P�b�g�ŗ��p���Ă���CTX�\���̂��擾���܂��B
	
	@return CTX�\���̃|�C���^
*/
SSL_CTX *cSslServerSock::GetCTX(){

	return m_CTX;

}