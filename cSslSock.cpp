#include "cSslSock.h"

/**	@file
	@brief	SSL�\�P�b�g
	
	@author	TATSUYA
	@date	2007/02/28
*/

/******************************************************************************/
/**	@brief	�R���X�g���N�^

	�N���X�̏��������s���܂��B
	OpenSSL�̏�������CTX�\���̂̏��������R���X�g���N�^�ōs���Ă��܂��B
	@param[in] CTX	���p����CTX�\���̂̃|�C���^(������CTX�\���̂𗘗p����ꍇ)
*/
cSslSock::cSslSock(SSL_CTX *CTX){

	// OpenSSL �̏�����
	SSL_library_init();
	// �G���[�𕶎���Ƃ��ĕ\��
	SSL_load_error_strings();

	// �����l��ݒ�
	m_SSL		= NULL;
	m_CTX		= NULL;

	// �o�b�t�@���N���A
	ZeroMemory(m_szErrMsg, sizeof(m_szErrMsg));

	// CTX�\���̂̐ݒ�(�T�[�o���̏ꍇ�́AServerSocket�̕��������p��)
	if(CTX != NULL){
		m_CTX = CTX;
	}else{
		if((m_CTX = SSL_CTX_new(TLSv1_client_method())) == NULL){
			wsprintf(m_szErrMsg, "CTX�\���̂̐ݒ�Ɏ��s���܂����B\n"
								 "SSL Error:%s",
										ERR_error_string(ERR_get_error(), NULL));
			return;
		}
	}

	if((m_SSL = SSL_new(m_CTX)) == NULL){
		wsprintf(m_szErrMsg, "SSL�\���̂̐ݒ�Ɏ��s���܂����B\n"
							 "SSL Error:%s",
								ERR_error_string(ERR_get_error(), NULL));
		return;
	}

	// ����I�� �o�b�t�@���N���A
	ZeroMemory(m_szErrMsg, sizeof(m_szErrMsg));

}

/******************************************************************************/
/**	@brief	�f�X�g���N�^

	�N���X�̉�����s���܂��B
*/
cSslSock::~cSslSock(){

	if(m_SSL != NULL){
		SSL_free(m_SSL);
	}

	if(m_CTX != NULL){
		SSL_CTX_free(m_CTX);
	}

	ERR_free_strings();
	EVP_cleanup();
	CRYPTO_cleanup_all_ex_data();

}

/******************************************************************************/
/** 
	���p���Ă���Í����������擾���܂��B
	
	@return ���p���Ă���Í�������
*/
LPCSTR cSslSock::GetSSLCipher(){

	return SSL_get_cipher(m_SSL);

}

/******************************************************************************/
/** 
	���p����Í���������ݒ肵�܂��B
	
	@param[in] szCiphers ���p����Í�������
	@return ���������Ƃ���TRUE��Ԃ��B
	���s�����Ƃ���FALSE��Ԃ��B
*/
BOOL cSslSock::SetSSLCiphers(LPCSTR szCiphers){

	if(!SSL_CTX_set_cipher_list(m_CTX, szCiphers)){
		wsprintf(m_szErrMsg, "SSL�̈Í��������Ɏ��s���܂����BSSL Error:%s",
					ERR_error_string(ERR_get_error(), NULL));
		return FALSE;
	}

	// ����I�� �o�b�t�@���N���A
	ZeroMemory(m_szErrMsg, sizeof(m_szErrMsg));
	return TRUE;

}

/******************************************************************************/
/** 
	���p���Ă���Í��������̃r�b�g�����擾���܂��B
	
	@return ���p���Ă���Í��������̃r�b�g�r�b�g��
*/
int cSslSock::GetSSLBits(){

	return SSL_get_cipher_bits(m_SSL, NULL);

}

/******************************************************************************/
/** 
	���p���Ă���SSL�̃o�[�W�������擾���܂��B
	
	@return ���p���Ă���SSL�̃o�[�W����
*/
LPCSTR cSslSock::GetSSLVerion(){

	return SSL_get_version(m_SSL);

}

/******************************************************************************/
/** 
	�N���C�A���g�ؖ���/�閧����ݒ肵�܂��B
	
	@param[in] szCert �ؖ����̃p�X
	@param[in] szKey  �ؖ����ɑ΂���閧���̃p�X
	@return ���������Ƃ���TRUE��Ԃ��B
	���s�����Ƃ���FALSE��Ԃ��B
	
	@bug  �閧���Ƀp�X�t���[�Y���t������Ă���ꍇ�ɁA�������~�܂�
	@todo �閧���Ƀp�X�t���[�Y���t������Ă��Ă����p�ł���悤�ɂ���B
*/
BOOL cSslSock::SetCert(LPCSTR szCert, LPCSTR szKey){

	if(SSL_CTX_use_certificate_file(m_CTX, szCert, SSL_FILETYPE_PEM) <= 0){
		wsprintf(m_szErrMsg, "�N���C�A���g�ؖ����̓ǂݍ��݂Ɏ��s���܂����B\n"
							 "SSL Error:%s",
								ERR_error_string(ERR_get_error(), NULL));
		return FALSE;
	}

	if(SSL_CTX_use_PrivateKey_file(m_CTX, szKey, SSL_FILETYPE_PEM) <= 0){
		wsprintf(m_szErrMsg, "�N���C�A���g�ؖ����̔閧���̓ǂݍ��݂Ɏ��s���܂����B\n"
							 "SSL Error:%s",
								ERR_error_string(ERR_get_error(), NULL));
		return FALSE;
	}

	if(!SSL_CTX_check_private_key(m_CTX)){
		wsprintf(m_szErrMsg, "�N���C�A���g�ؖ����Ɣ閧������v���܂���B\n"
							 "SSL Error:%s",
								ERR_error_string(ERR_get_error(), NULL));
		return FALSE;
	}

	// ����I�� �o�b�t�@���N���A
	ZeroMemory(m_szErrMsg, sizeof(m_szErrMsg));
	return TRUE;

}

/******************************************************************************/
/** 
	CA�ؖ�����ݒ肵�܂��B
	
	@param[in] szCaCert �ؖ����̃p�X
	@return ���������Ƃ���TRUE��Ԃ��B
	���s�����Ƃ���FALSE��Ԃ��B
*/
BOOL cSslSock::SetCaCert(LPCSTR szCaCert){

	// CA�̏ؖ������w��
	if(!SSL_CTX_load_verify_locations(m_CTX, szCaCert, NULL)){
		wsprintf(m_szErrMsg, "CA�ؖ����̓ǂݍ��݂Ɏ��s���܂����B\n"
							 "SSL Error:%s",
								ERR_error_string(ERR_get_error(), NULL));
		return FALSE;
	}

	SSL_CTX_set_verify(m_CTX, SSL_VERIFY_PEER, NULL);
	SSL_CTX_set_verify_depth(m_CTX, 1);

	// ����I�� �o�b�t�@���N���A
	ZeroMemory(m_szErrMsg, sizeof(m_szErrMsg));
	return TRUE;

}

/******************************************************************************/
/** 
	�T�[�o�ɑ΂��ăR�l�N�V�����̊m���v�����s���܂��B
	
	@param[in] szHost	�ڑ���z�X�g�̃z�X�g���܂���IP�A�h���X
	@param[in] nPort	�ڑ���z�X�g�̃|�[�g�ԍ�
	@param[in] hWnd		�񓯊����[�h�ŗ��p����ꍇ�́A�ʒm��̃E�B���h�E�n���h��<br>
						�������[�h�ŗ��p����ꍇ��NULL
	@param[in] nEventNo	�񓯊����[�h�ŗ��p����ꍇ�́A�ʒm���郁�b�Z�[�WID<br>
						�������[�h�ŗ��p����ꍇ��NULL
	@return ���������Ƃ���TRUE��Ԃ��B
	���s�����Ƃ���FALSE��Ԃ��B
*/
BOOL cSslSock::Connect(LPCSTR szHost, USHORT nPort, HWND hWnd, USHORT nEventNo){

	// �ϐ��錾
	int	iRet	= 0;
	int iErrNo	= 0;

	// SSL�̍Đ���
	if((m_SSL = SSL_new(m_CTX)) == NULL){
		wsprintf(m_szErrMsg, "SSL�\���̂̍Đ����Ɏ��s���܂����BSSL Error:%s",
					ERR_error_string(ERR_get_error(), NULL));
		return FALSE;
	}

	// �\�P�b�g�̐ڑ�
	if(!cTcpSock::Connect(szHost, nPort, hWnd, nEventNo)){
		return FALSE;
	}

	// SSL�ƃ\�P�b�g�̊֘A�t��
	if(!SSL_set_fd(m_SSL, (int)m_Socket)){
		wsprintf(m_szErrMsg, "SSL�ƃ\�P�b�g�̊֘A�t���Ɏ��s���܂����BSSL Error:%s",
					ERR_error_string(ERR_get_error(), NULL));
		// �\�P�b�g�����łɐڑ����ł���ΐؒf����
		if(IsConnect()){
			Disconnect();
		}
		return FALSE;
	}

	// SSL�ł̐ڑ�
	do{
		iRet	= SSL_connect(m_SSL);
		iErrNo	= SSL_get_error(m_SSL, iRet);
	}while((iErrNo == SSL_ERROR_WANT_X509_LOOKUP || iErrNo == SSL_ERROR_WANT_READ) && iRet <= 0);

	if(iRet <= 0){
		wsprintf(m_szErrMsg, "SSL�ڑ��Ɏ��s���܂����B\n"
							 "SSL Error:%s",
								ERR_error_string(ERR_get_error(), NULL));
		// �\�P�b�g�����łɐڑ����ł���ΐؒf����
		if(IsConnect()){
			Disconnect();
		}
		return FALSE;
	}

	// ����I�� �o�b�t�@���N���A
	ZeroMemory(m_szErrMsg, sizeof(m_szErrMsg));
	return TRUE;

}

/******************************************************************************/
/** 
	�m���ς݂̃R�l�N�V������ؒf���܂��B
	
	@return ���������Ƃ���TRUE��Ԃ��B
	���s�����Ƃ���FALSE��Ԃ��B
	
	@todo �ڑ�����Ă��Ȃ��ꍇ�̏������Ȃ��B<br>
		  cTcpSock�ɂ͂���
*/
BOOL cSslSock::Disconnect(){

	// �ϐ��錾
	int	iRetNo	= 0;

	iRetNo = SSL_shutdown(m_SSL);

	if(iRetNo == 0){
		// SSL�̃V���b�g�_�E�����������Ă��Ȃ����߁A�ēx���s
		iRetNo = SSL_shutdown(m_SSL);
	}else if(iRetNo == -1){
		wsprintf(m_szErrMsg, "SSL�ڑ��̐ؒf�Ɏ��s���܂����B\n"
							 "SSL Error:%s",
								ERR_error_string(ERR_get_error(), NULL));
	}

	// SSL���̉��
	if(SSL_clear(m_SSL) != 1){
		wsprintf(m_szErrMsg, "SSL���̉���Ɏ��s���܂����B\n"
							 "SSL Error:%s",
								ERR_error_string(ERR_get_error(), NULL));
	}

	// �\�P�b�g�̐ؒf
	if(!cTcpSock::Disconnect()){
		return FALSE;
	}

	return TRUE;

}

/******************************************************************************/
/** 
	�f�[�^�̑��M���s���܂��B
	
	�w�肳�ꂽ�o�C�g���A���M����܂ŏ����̓u���b�L���O����܂��B
	
	@param[in] lpData	���M����f�[�^�̐擪�|�C���^
	@param[in] dwBytes	���M����f�[�^�̃o�C�g��
	@return ���������Ƃ���TRUE��Ԃ��B
	���s�����Ƃ���FALSE��Ԃ��B
*/
BOOL cSslSock::SendData(LPVOID lpData, DWORD dwBytes){

	// �ϐ��錾
	DWORD	dwByteSend	= 0;
	int		iErrNo		= 0;
	LPBYTE	lpBuff		= (LPBYTE)lpData;

	// �S�Ẵf�[�^�𑗐M����܂Ń��[�v�𑱂���
	do{
		do{
			dwByteSend	= SSL_write(m_SSL, lpBuff, dwBytes);
			iErrNo		= SSL_get_error(m_SSL, dwByteSend);
		}while(dwByteSend <= 0 && iErrNo == SSL_ERROR_WANT_READ);

		if(dwByteSend <= 0){
			wsprintf(m_szErrMsg, "�f�[�^�̑��M�Ɏ��s���܂����B\n"
								 "ErrCode:%d\n"
								 "SSL Error:%s",
									WSAGetLastError(), ERR_error_string(ERR_get_error(), NULL));
			return FALSE;
		}
		// �|�C���^�̈ړ�
		lpBuff	+= dwByteSend;
		dwBytes	-= dwByteSend;
	}while(dwBytes > 0);

	// ����I�� �o�b�t�@���N���A
	ZeroMemory(m_szErrMsg, sizeof(m_szErrMsg));
	return TRUE;

}

/******************************************************************************/
/** 
	�w�肳�ꂽ�o�C�g���f�[�^�̎�M���s���܂��B
	
	�w�肳�ꂽ�o�C�g���A��M����܂ŏ����̓u���b�L���O����܂��B

	@param[in] lpBuffer		��M�����f�[�^���i�[����擪�|�C���^
	@param[in] dwBytes		�i�[��̃o�C�g��
	@param[out] pdwRecvLen	���ۂɎ�M�����o�C�g��
	@return ���������Ƃ���TRUE��Ԃ��B
	���s�����Ƃ���FALSE��Ԃ��B
*/
BOOL cSslSock::RecvData(LPVOID lpBuffer, DWORD dwBytes, DWORD* pdwRecvLen){

	// �ϐ��錾
	DWORD	dwRead		= 0;
	DWORD	dwByteRecv	= 0;
	LPBYTE	lpBuff		= (LPBYTE)lpBuffer;

	// �w�肵���o�C�g���A��M����܂Ń��[�v�𑱂���
	while(1){
		dwRead = SSL_read(m_SSL, lpBuff, dwBytes - dwByteRecv);
		if(dwRead <= 0){
			break;
		}
		dwByteRecv += dwRead;
		lpBuff		= (LPBYTE)lpBuffer + dwByteRecv;
		if(dwBytes <= dwByteRecv){
			break;
		}
	}

	// ��M�����o�C�g����0�o�C�g�ł������ꍇ
	if(dwByteRecv <= 0){
		wsprintf(m_szErrMsg, "�f�[�^�̎�M�Ɏ��s���܂����B\n"
							 "ErrCode:%d\n"
							 "SSL Error:%s",
									WSAGetLastError(), ERR_error_string(ERR_get_error(), NULL));
		return FALSE;
	}

	if(pdwRecvLen != NULL){
		*pdwRecvLen = dwByteRecv;
	}

	// ����I�� �o�b�t�@���N���A
	ZeroMemory(m_szErrMsg, sizeof(m_szErrMsg));
	return TRUE;

}

/******************************************************************************/
/** 
	��M�ł��邾���f�[�^�̎�M���s���܂��B

	@param[in] lpBuffer		��M�����f�[�^���i�[����擪�|�C���^
	@param[in] dwBytes		�i�[��̃o�C�g��
	@param[out] pdwRecvLen	���ۂɎ�M�����o�C�g��
	@return ���������Ƃ���TRUE��Ԃ��B
	���s�����Ƃ���FALSE��Ԃ��B
*/
BOOL cSslSock::RecvDataOnce(LPVOID lpBuffer, DWORD dwBytes, DWORD* pdwRecvLen){

	// �ϐ��錾
	DWORD dwByteRecv = 0;
	
	dwByteRecv = SSL_read(m_SSL, (LPSTR)lpBuffer, dwBytes);
	
	// ��M�����o�C�g����0�o�C�g�ł������ꍇ
	if(dwByteRecv == 0){
		wsprintf(m_szErrMsg, "�f�[�^�̎�M�Ɏ��s���܂����B\n"
							 "ErrCode:%d\n"
							 "SSL Error:%s",
									WSAGetLastError(), ERR_error_string(ERR_get_error(), NULL));
		return FALSE;
	}
	
	if(pdwRecvLen != NULL){
		*pdwRecvLen = dwByteRecv;
	}

	// ����I�� �o�b�t�@���N���A
	ZeroMemory(m_szErrMsg, sizeof(m_szErrMsg));
	return TRUE;

}

/******************************************************************************/
/** 
	�ڑ���z�X�g�̏ؖ������擾���܂��B
	
	@param[in] pCertificate	_Certificate�\���̂̃|�C���^
	@return ���������Ƃ���TRUE��Ԃ��B
	���s�����Ƃ���FALSE��Ԃ��B
*/
BOOL cSslSock::GetPeerCert(_Certificate *pCertificate){

	// �ϐ��錾
	X509			*client_cert	= NULL;
	LPSTR			szBuffer		= NULL;
	char			temp[3];
	int				date[6];
	ASN1_TIME		*timeExpire;

	// �ʐM����̏ؖ������擾
	client_cert = SSL_get_peer_certificate(m_SSL);

	if(client_cert == NULL){
		wsprintf(m_szErrMsg, "�N���C�A���g�ؖ������擾�ł��܂���ł����B");
		return FALSE;
	}

	// ���s��̏ؖ�������1�s�Ŏ擾
	szBuffer = X509_NAME_oneline(X509_get_subject_name(client_cert), 0, 0);
	_PushCertificate(&pCertificate->x509Subject,  szBuffer);

	// ���s���̏ؖ�������1�s�Ŏ擾
	szBuffer = X509_NAME_oneline(X509_get_issuer_name(client_cert), 0, 0);
	_PushCertificate(&pCertificate->x509Issuer,  szBuffer);

	// �ؖ����̗L���������擾
	timeExpire = X509_get_notBefore(client_cert);
	for(int y = 0; y < 6; y++){
		temp[0] = timeExpire->data[y*2];
		temp[1] = timeExpire->data[y*2+1];
		temp[2] = '\0';
		date[y] = atoi(temp);
	}
	wsprintf(pCertificate->szStartDate, "%04d/%02d/%02d %02d:%02d:%02d", date[0] + 2000, date[1], date[2], date[3], date[4], date[5]);

	// �ؖ����̗L���������擾
	timeExpire  = X509_get_notAfter(client_cert);
	for(int y = 0; y < 6; y++){
		temp[0] = timeExpire->data[y*2];
		temp[1] = timeExpire->data[y*2+1];
		temp[2] = '\0';
		date[y] = atoi(temp);
	}
	wsprintf(pCertificate->szEndDate, "%04d/%02d/%02d %02d:%02d:%02d", date[0] + 2000, date[1], date[2], date[3], date[4], date[5]);

	// �擾�����ؖ����̉��
	X509_free(client_cert);

	// ����I�� �o�b�t�@���N���A
	ZeroMemory(m_szErrMsg, sizeof(m_szErrMsg));
	return TRUE;

}

/******************************************************************************/
/** 
	�ؖ����̊i�[���s���܂��B
	
	X509_NAME_oneline�Ŏ擾���������\���̂Ɋi�[���܂��B
	
	@param[in] pX509Info	_X509Info�\���̂̃|�C���^
	@param[in] szData		X509_NAME_oneline�Ŏ擾����������
	
	@todo ���\�b�h�̎������B��
*/
void cSslSock::_PushCertificate(_X509Info *pX509Info, LPCSTR szData){

	// �ϐ��錾
	LPSTR	szBuff = NULL;

	szBuff = strtok((char*)szData, "/");

	_PushCertificate_(pX509Info, szBuff);

	while(szBuff != NULL){
		szBuff = strtok(NULL, "/");
		if(szBuff != NULL){
			_PushCertificate_(pX509Info, szBuff);
		}
	}
	
	return;

}

/******************************************************************************/
/** 
	�ؖ����̊i�[���s���܂��B
	
	X509_NAME_oneline�Ŏ擾���������\���̂Ɋi�[���܂��B
	
	@param[in] pX509Info	_X509Info�\���̂̃|�C���^
	@param[in] szBuff		_PushCertificate�ŕ�������������
	
	@todo ���\�b�h�̎������B��
*/
void cSslSock::_PushCertificate_(_X509Info *pX509Info, LPCSTR szBuff){

	if(!strncmp("C=", szBuff, 2)){
		strcpy(pX509Info->szCountry, szBuff + 2);
	}else if(!strncmp("ST=", szBuff, 3)){
		pX509Info->szState = (LPSTR)GlobalAlloc(GPTR, lstrlen(szBuff + 3) + 1);
		strcpy(pX509Info->szState, szBuff + 3);
	}else if(!strncmp("L=", szBuff, 2)){
		pX509Info->szLocality = (LPSTR)GlobalAlloc(GPTR, lstrlen(szBuff + 2) + 1);
		strcpy(pX509Info->szLocality, szBuff + 2);
	}else if(!strncmp("O=", szBuff, 2)){
		pX509Info->szOrganization = (LPSTR)GlobalAlloc(GPTR, lstrlen(szBuff + 2) + 1);
		strcpy(pX509Info->szOrganization, szBuff + 2);
	}else if(!strncmp("OU=", szBuff, 3)){
		pX509Info->szOrganizationUnit = (LPSTR)GlobalAlloc(GPTR, lstrlen(szBuff + 3) + 1);
		strcpy(pX509Info->szOrganizationUnit, szBuff + 3);
	}else if(!strncmp("CN=", szBuff, 3)){
		pX509Info->szCommonName = (LPSTR)GlobalAlloc(GPTR, lstrlen(szBuff + 3) + 1);
		strcpy(pX509Info->szCommonName, szBuff + 3);
	}else if(!strncmp("emailAddress=", szBuff, 13)){
		pX509Info->szEMail = (LPSTR)GlobalAlloc(GPTR, lstrlen(szBuff + 13) + 1);
		strcpy(pX509Info->szEMail, szBuff + 13);
	}

	return;

}

/******************************************************************************/
/** 
	�ؖ����̔j�����s���܂��B
	
	@param[in] pCertificate	_Certificate�\���̂̃|�C���^
*/
void cSslSock::DestroyCertInfo(_Certificate* pCertificate){

	if(pCertificate == NULL){
		return;
	}

	if(pCertificate->x509Subject.szState != NULL){
		GlobalFree(pCertificate->x509Subject.szState);
	}

	if(pCertificate->x509Subject.szLocality != NULL){
		GlobalFree(pCertificate->x509Subject.szLocality);
	}

	if(pCertificate->x509Subject.szOrganization != NULL){
		GlobalFree(pCertificate->x509Subject.szOrganization);
	}

	if(pCertificate->x509Subject.szOrganizationUnit != NULL){
		GlobalFree(pCertificate->x509Subject.szOrganizationUnit);
	}

	if(pCertificate->x509Subject.szCommonName != NULL){
		GlobalFree(pCertificate->x509Subject.szCommonName);
	}

	if(pCertificate->x509Subject.szEMail != NULL){
		GlobalFree(pCertificate->x509Subject.szEMail);
	}
	////
	if(pCertificate->x509Issuer.szState != NULL){
		GlobalFree(pCertificate->x509Issuer.szState);
	}

	if(pCertificate->x509Issuer.szLocality != NULL){
		GlobalFree(pCertificate->x509Issuer.szLocality);
	}

	if(pCertificate->x509Issuer.szOrganization != NULL){
		GlobalFree(pCertificate->x509Issuer.szOrganization);
	}

	if(pCertificate->x509Issuer.szOrganizationUnit != NULL){
		GlobalFree(pCertificate->x509Issuer.szOrganizationUnit);
	}

	if(pCertificate->x509Issuer.szCommonName != NULL){
		GlobalFree(pCertificate->x509Issuer.szCommonName);
	}

	if(pCertificate->x509Issuer.szEMail != NULL){
		GlobalFree(pCertificate->x509Issuer.szEMail);
	}

	return;

}

/******************************************************************************/
/** 
	�ؖ����̌��؂��s���܂��B
	
	@retval ���،��ʂ�Ԃ��܂��B
*/
LPCSTR cSslSock::Verify_SSL(){

	// �ϐ��錾
	long	verify_result		= NULL;
	char	szVerifyMsg[2048];

	// �o�b�t�@�̃N���A
	ZeroMemory(szVerifyMsg, sizeof(szVerifyMsg));

	struct sslerror_t{
		long result_code;
		char *ssl_error;
		char *ssl_errordetail;
	};

	struct sslerror_t error_table[] = {
		{
			X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT,
			"unable to get issuer certificate. ",
			"the issuer certificate could not be found: this occurs if the"
			"issuer certificate of an untrusted certificate cannot be found."
		},
		{
			X509_V_ERR_UNABLE_TO_GET_CRL,
			"unable to get certificate CRL. ",
			"the CRL of a certificate could not be found. Unused."
		},
		{
			X509_V_ERR_UNABLE_TO_DECRYPT_CERT_SIGNATURE,
			"unable to decrypt certificate's signature",
			"the certificate signature could not be decrypted. This means that"
			"the actual signature value could not be determined rather than it"
			"not matching the expected value, this is only meaningful for RSA keys"
		},
		{
			X509_V_ERR_UNABLE_TO_DECRYPT_CRL_SIGNATURE,
			"unable to decrypt CRL's signature",
			"the CRL signature could not be decrypted: this means that the"
			"actual signature value could not be determined rather than it not"
			"matching the expected value. Unused."
		},
		{
			X509_V_ERR_UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY,
			"unable to decode issuer public key",
			"the public key in the certificate SubjectPublicKeyInfo could not be read"
		},
		{
			X509_V_ERR_CERT_SIGNATURE_FAILURE,
			"certificate signature failure",
			"the signature of the certificate is invalid"
		},
		{
			X509_V_ERR_CRL_SIGNATURE_FAILURE,
			"CRL signature failure",
			"the signature of the certificate is invalid. Unused"
		},
		{
			X509_V_ERR_CERT_NOT_YET_VALID,
			"�ؖ����̗L�������������̓��t�ɐݒ肳��Ă��܂��B",
			"�ؖ������L���ɂȂ���t�ɂ܂��B���Ă��܂���B�R���s���[�^�̓��t�ݒ���m�F���Ă��������B"
		},
		{
			X509_V_ERR_CERT_HAS_EXPIRED,
			"�ؖ����̗L�����������łɐ؂�Ă��܂��B",
			"�ؖ����̗L�����������łɐ؂�Ă��܂��B",
		},
		{
			X509_V_ERR_CRL_NOT_YET_VALID,
			"CRL is not yet valid",
			"the CRL is not yet valid. Unused."
		},
		{
			X509_V_ERR_CRL_HAS_EXPIRED,
			"CRL has expired",
			"the CRL has expired. Unused."
		},
		{
			X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD,
			"format error in certificate's notBefore field",
			"the certificate notBefore field contains an invalid time"
		},
		{
			X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD,
			"format error in certificate's notAfter field",
			"the certificate notAfter field contains an invalid time."
		},
		{
			X509_V_ERR_ERROR_IN_CRL_LAST_UPDATE_FIELD,
			"format error in CRL's lastUpdate field",
			"the CRL lastUpdate field contains an invalid time. Unused."
		},
		{
			X509_V_ERR_ERROR_IN_CRL_NEXT_UPDATE_FIELD,
			"format error in CRL's nextUpdate field",
			"the CRL nextUpdate field contains an invalid time. Unused.",
		},
		{
			X509_V_ERR_OUT_OF_MEM,
			"�������m�ۃG���[",
			"���������m�ۏo���܂���ł����B���̃G���[�͋N����ׂ��ł͗L��܂���B"
		},
		{
			X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT,
			"self signed certificate",
			"the passed certificate is self signed and the same certificate"
			"cannot be found in the list of trusted certificates."
		},
		{
			X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN,
			"self signed certificate in certificate chain",
			"the certificate chain could be built up using the untrusted"
			"certificates but the root could not be found locally."
		},
		{
			X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY,
			"unable to get local issuer certificate",
			"the issuer certificate of a locally looked up certificate could not"
			"be found. This normally means the list of trusted certificates is"
			"not complete."
		},
		{
			X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE,
			"unable to verify the first certificate",
			"no signatures could be verified because the chain contains only one"
			"certificate and it is not self signed."
		},
		{
			X509_V_ERR_CERT_CHAIN_TOO_LONG,
			"certificate chain too long",
			"the certificate chain length is greater than the supplied maximum"
			"depth. Unused."
		},
		{
			X509_V_ERR_CERT_REVOKED,
			"certificate revoked",
			"the certificate has been revoked. Unused."
		},
		{
			X509_V_ERR_INVALID_CA,
			"invalid CA certificate",
			"a CA certificate is invalid. Either it is not a CA or its"
			"extensions are not consistent with the supplied purpose."
		},
		{
			X509_V_ERR_PATH_LENGTH_EXCEEDED,
			"path length constraint exceeded",
			"the basicConstraints pathlength parameter has been exceeded."
		},
		{X509_V_ERR_INVALID_PURPOSE,
		"unsupported certificate purpose",
		"the supplied certificate cannot be used for the specified purpose."
		},
		{
			X509_V_ERR_CERT_UNTRUSTED,
			"certificate not trusted",
			"the root CA is not marked as trusted for the specified purpose."
		},
		{
			X509_V_ERR_CERT_REJECTED,
			"�ؖ��������₳�ꂽ",
			"the root CA is marked to reject the specified purpose.",
		},
		{
			X509_V_ERR_SUBJECT_ISSUER_MISMATCH,
			"subject issuer mismatch",
			"the current candidate issuer certificate was rejected because its"
			"subject name did not match the issuer name of the current"
			"certificate. Only displayed when the -issuer_checks option is set."
		},
		{
			X509_V_ERR_AKID_SKID_MISMATCH,
			"authority and subject key identifier mismatch",
			"the current candidate issuer certificate was rejected because its"
			"subject key identifier was present and did not match the authority"
			"key identifier current certificate. Only displayed when the"
			"-issuer_checks option is set."
		},
		{
			X509_V_ERR_AKID_ISSUER_SERIAL_MISMATCH,
			"authority and issuer serial number mismatch",
			"the current candidate issuer certificate was rejected because its"
			"issuer name and serial number was present and did not match the"
			"authority key identifier of the current certificate. Only displayed"
			"when the -issuer_checks option is set."
		},
		{
			X509_V_ERR_KEYUSAGE_NO_CERTSIGN,
			"key usage does not include certificate signing",
			"the current candidate issuer certificate was rejected because its"
			"keyUsage extension does not permit certificate signing."
		},
		{
			X509_V_ERR_APPLICATION_VERIFICATION,
			"application verification failure",
			"an application specific error. Unused."
		}
	};

	// SSL�ؖ����̌���
	verify_result = SSL_get_verify_result(m_SSL);

	// ���ʂ𔻒f����
	if(verify_result == X509_V_OK){
		return NULL;
	}else{
		for(int i = 0; i < sizeof(error_table) / sizeof(error_table[0]); i++){
			if(error_table[i].result_code == verify_result){
				wsprintf(m_szErrMsg, "%s\n"
									 "ErrCode:%d",
										error_table[i].ssl_error, (int)verify_result);
				break;
			}
		}
	}

	return m_szErrMsg;

}
