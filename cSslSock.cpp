#include "cSslSock.h"

/**	@file
	@brief	SSLソケット
	
	@author	TATSUYA
	@date	2007/02/28
*/

/******************************************************************************/
/**	@brief	コンストラクタ

	クラスの初期化を行います。
	OpenSSLの初期化やCTX構造体の初期化もコンストラクタで行っています。
	@param[in] CTX	利用するCTX構造体のポインタ(既存のCTX構造体を利用する場合)
*/
cSslSock::cSslSock(SSL_CTX *CTX){

	// OpenSSL の初期化
	SSL_library_init();
	// エラーを文字列として表示
	SSL_load_error_strings();

	// 初期値を設定
	m_SSL		= NULL;
	m_CTX		= NULL;

	// バッファをクリア
	ZeroMemory(m_szErrMsg, sizeof(m_szErrMsg));

	// CTX構造体の設定(サーバ側の場合は、ServerSocketの物を引き継ぐ)
	if(CTX != NULL){
		m_CTX = CTX;
	}else{
		if((m_CTX = SSL_CTX_new(TLSv1_client_method())) == NULL){
			wsprintf(m_szErrMsg, "CTX構造体の設定に失敗しました。\n"
								 "SSL Error:%s",
										ERR_error_string(ERR_get_error(), NULL));
			return;
		}
	}

	if((m_SSL = SSL_new(m_CTX)) == NULL){
		wsprintf(m_szErrMsg, "SSL構造体の設定に失敗しました。\n"
							 "SSL Error:%s",
								ERR_error_string(ERR_get_error(), NULL));
		return;
	}

	// 正常終了 バッファをクリア
	ZeroMemory(m_szErrMsg, sizeof(m_szErrMsg));

}

/******************************************************************************/
/**	@brief	デストラクタ

	クラスの解放を行います。
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
	利用している暗号化方式を取得します。
	
	@return 利用している暗号化方式
*/
LPCSTR cSslSock::GetSSLCipher(){

	return SSL_get_cipher(m_SSL);

}

/******************************************************************************/
/** 
	利用する暗号化方式を設定します。
	
	@param[in] szCiphers 利用する暗号化方式
	@return 成功したときはTRUEを返す。
	失敗したときはFALSEを返す。
*/
BOOL cSslSock::SetSSLCiphers(LPCSTR szCiphers){

	if(!SSL_CTX_set_cipher_list(m_CTX, szCiphers)){
		wsprintf(m_szErrMsg, "SSLの暗号化方式に失敗しました。SSL Error:%s",
					ERR_error_string(ERR_get_error(), NULL));
		return FALSE;
	}

	// 正常終了 バッファをクリア
	ZeroMemory(m_szErrMsg, sizeof(m_szErrMsg));
	return TRUE;

}

/******************************************************************************/
/** 
	利用している暗号化方式のビット数を取得します。
	
	@return 利用している暗号化方式のビットビット数
*/
int cSslSock::GetSSLBits(){

	return SSL_get_cipher_bits(m_SSL, NULL);

}

/******************************************************************************/
/** 
	利用しているSSLのバージョンを取得します。
	
	@return 利用しているSSLのバージョン
*/
LPCSTR cSslSock::GetSSLVerion(){

	return SSL_get_version(m_SSL);

}

/******************************************************************************/
/** 
	クライアント証明書/秘密鍵を設定します。
	
	@param[in] szCert 証明書のパス
	@param[in] szKey  証明書に対する秘密鍵のパス
	@return 成功したときはTRUEを返す。
	失敗したときはFALSEを返す。
	
	@bug  秘密鍵にパスフレーズが付加されている場合に、処理が止まる
	@todo 秘密鍵にパスフレーズが付加されていても利用できるようにする。
*/
BOOL cSslSock::SetCert(LPCSTR szCert, LPCSTR szKey){

	if(SSL_CTX_use_certificate_file(m_CTX, szCert, SSL_FILETYPE_PEM) <= 0){
		wsprintf(m_szErrMsg, "クライアント証明書の読み込みに失敗しました。\n"
							 "SSL Error:%s",
								ERR_error_string(ERR_get_error(), NULL));
		return FALSE;
	}

	if(SSL_CTX_use_PrivateKey_file(m_CTX, szKey, SSL_FILETYPE_PEM) <= 0){
		wsprintf(m_szErrMsg, "クライアント証明書の秘密鍵の読み込みに失敗しました。\n"
							 "SSL Error:%s",
								ERR_error_string(ERR_get_error(), NULL));
		return FALSE;
	}

	if(!SSL_CTX_check_private_key(m_CTX)){
		wsprintf(m_szErrMsg, "クライアント証明書と秘密鍵が一致しません。\n"
							 "SSL Error:%s",
								ERR_error_string(ERR_get_error(), NULL));
		return FALSE;
	}

	// 正常終了 バッファをクリア
	ZeroMemory(m_szErrMsg, sizeof(m_szErrMsg));
	return TRUE;

}

/******************************************************************************/
/** 
	CA証明書を設定します。
	
	@param[in] szCaCert 証明書のパス
	@return 成功したときはTRUEを返す。
	失敗したときはFALSEを返す。
*/
BOOL cSslSock::SetCaCert(LPCSTR szCaCert){

	// CAの証明書を指定
	if(!SSL_CTX_load_verify_locations(m_CTX, szCaCert, NULL)){
		wsprintf(m_szErrMsg, "CA証明書の読み込みに失敗しました。\n"
							 "SSL Error:%s",
								ERR_error_string(ERR_get_error(), NULL));
		return FALSE;
	}

	SSL_CTX_set_verify(m_CTX, SSL_VERIFY_PEER, NULL);
	SSL_CTX_set_verify_depth(m_CTX, 1);

	// 正常終了 バッファをクリア
	ZeroMemory(m_szErrMsg, sizeof(m_szErrMsg));
	return TRUE;

}

/******************************************************************************/
/** 
	サーバに対してコネクションの確率要求を行います。
	
	@param[in] szHost	接続先ホストのホスト名またはIPアドレス
	@param[in] nPort	接続先ホストのポート番号
	@param[in] hWnd		非同期モードで利用する場合は、通知先のウィンドウハンドル<br>
						同期モードで利用する場合はNULL
	@param[in] nEventNo	非同期モードで利用する場合は、通知するメッセージID<br>
						同期モードで利用する場合はNULL
	@return 成功したときはTRUEを返す。
	失敗したときはFALSEを返す。
*/
BOOL cSslSock::Connect(LPCSTR szHost, USHORT nPort, HWND hWnd, USHORT nEventNo){

	// 変数宣言
	int	iRet	= 0;
	int iErrNo	= 0;

	// SSLの再生成
	if((m_SSL = SSL_new(m_CTX)) == NULL){
		wsprintf(m_szErrMsg, "SSL構造体の再生成に失敗しました。SSL Error:%s",
					ERR_error_string(ERR_get_error(), NULL));
		return FALSE;
	}

	// ソケットの接続
	if(!cTcpSock::Connect(szHost, nPort, hWnd, nEventNo)){
		return FALSE;
	}

	// SSLとソケットの関連付け
	if(!SSL_set_fd(m_SSL, (int)m_Socket)){
		wsprintf(m_szErrMsg, "SSLとソケットの関連付けに失敗しました。SSL Error:%s",
					ERR_error_string(ERR_get_error(), NULL));
		// ソケットがすでに接続中であれば切断する
		if(IsConnect()){
			Disconnect();
		}
		return FALSE;
	}

	// SSLでの接続
	do{
		iRet	= SSL_connect(m_SSL);
		iErrNo	= SSL_get_error(m_SSL, iRet);
	}while((iErrNo == SSL_ERROR_WANT_X509_LOOKUP || iErrNo == SSL_ERROR_WANT_READ) && iRet <= 0);

	if(iRet <= 0){
		wsprintf(m_szErrMsg, "SSL接続に失敗しました。\n"
							 "SSL Error:%s",
								ERR_error_string(ERR_get_error(), NULL));
		// ソケットがすでに接続中であれば切断する
		if(IsConnect()){
			Disconnect();
		}
		return FALSE;
	}

	// 正常終了 バッファをクリア
	ZeroMemory(m_szErrMsg, sizeof(m_szErrMsg));
	return TRUE;

}

/******************************************************************************/
/** 
	確立済みのコネクションを切断します。
	
	@return 成功したときはTRUEを返す。
	失敗したときはFALSEを返す。
	
	@todo 接続されていない場合の処理がない。<br>
		  cTcpSockにはある
*/
BOOL cSslSock::Disconnect(){

	// 変数宣言
	int	iRetNo	= 0;

	iRetNo = SSL_shutdown(m_SSL);

	if(iRetNo == 0){
		// SSLのシャットダウンが完了していないため、再度実行
		iRetNo = SSL_shutdown(m_SSL);
	}else if(iRetNo == -1){
		wsprintf(m_szErrMsg, "SSL接続の切断に失敗しました。\n"
							 "SSL Error:%s",
								ERR_error_string(ERR_get_error(), NULL));
	}

	// SSL情報の解放
	if(SSL_clear(m_SSL) != 1){
		wsprintf(m_szErrMsg, "SSL情報の解放に失敗しました。\n"
							 "SSL Error:%s",
								ERR_error_string(ERR_get_error(), NULL));
	}

	// ソケットの切断
	if(!cTcpSock::Disconnect()){
		return FALSE;
	}

	return TRUE;

}

/******************************************************************************/
/** 
	データの送信を行います。
	
	指定されたバイト数、送信するまで処理はブロッキングされます。
	
	@param[in] lpData	送信するデータの先頭ポインタ
	@param[in] dwBytes	送信するデータのバイト数
	@return 成功したときはTRUEを返す。
	失敗したときはFALSEを返す。
*/
BOOL cSslSock::SendData(LPVOID lpData, DWORD dwBytes){

	// 変数宣言
	DWORD	dwByteSend	= 0;
	int		iErrNo		= 0;
	LPBYTE	lpBuff		= (LPBYTE)lpData;

	// 全てのデータを送信するまでループを続ける
	do{
		do{
			dwByteSend	= SSL_write(m_SSL, lpBuff, dwBytes);
			iErrNo		= SSL_get_error(m_SSL, dwByteSend);
		}while(dwByteSend <= 0 && iErrNo == SSL_ERROR_WANT_READ);

		if(dwByteSend <= 0){
			wsprintf(m_szErrMsg, "データの送信に失敗しました。\n"
								 "ErrCode:%d\n"
								 "SSL Error:%s",
									WSAGetLastError(), ERR_error_string(ERR_get_error(), NULL));
			return FALSE;
		}
		// ポインタの移動
		lpBuff	+= dwByteSend;
		dwBytes	-= dwByteSend;
	}while(dwBytes > 0);

	// 正常終了 バッファをクリア
	ZeroMemory(m_szErrMsg, sizeof(m_szErrMsg));
	return TRUE;

}

/******************************************************************************/
/** 
	指定されたバイト数データの受信を行います。
	
	指定されたバイト数、受信するまで処理はブロッキングされます。

	@param[in] lpBuffer		受信したデータを格納する先頭ポインタ
	@param[in] dwBytes		格納先のバイト数
	@param[out] pdwRecvLen	実際に受信したバイト数
	@return 成功したときはTRUEを返す。
	失敗したときはFALSEを返す。
*/
BOOL cSslSock::RecvData(LPVOID lpBuffer, DWORD dwBytes, DWORD* pdwRecvLen){

	// 変数宣言
	DWORD	dwRead		= 0;
	DWORD	dwByteRecv	= 0;
	LPBYTE	lpBuff		= (LPBYTE)lpBuffer;

	// 指定したバイト数、受信するまでループを続ける
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

	// 受信したバイト数が0バイトであった場合
	if(dwByteRecv <= 0){
		wsprintf(m_szErrMsg, "データの受信に失敗しました。\n"
							 "ErrCode:%d\n"
							 "SSL Error:%s",
									WSAGetLastError(), ERR_error_string(ERR_get_error(), NULL));
		return FALSE;
	}

	if(pdwRecvLen != NULL){
		*pdwRecvLen = dwByteRecv;
	}

	// 正常終了 バッファをクリア
	ZeroMemory(m_szErrMsg, sizeof(m_szErrMsg));
	return TRUE;

}

/******************************************************************************/
/** 
	受信できるだけデータの受信を行います。

	@param[in] lpBuffer		受信したデータを格納する先頭ポインタ
	@param[in] dwBytes		格納先のバイト数
	@param[out] pdwRecvLen	実際に受信したバイト数
	@return 成功したときはTRUEを返す。
	失敗したときはFALSEを返す。
*/
BOOL cSslSock::RecvDataOnce(LPVOID lpBuffer, DWORD dwBytes, DWORD* pdwRecvLen){

	// 変数宣言
	DWORD dwByteRecv = 0;
	
	dwByteRecv = SSL_read(m_SSL, (LPSTR)lpBuffer, dwBytes);
	
	// 受信したバイト数が0バイトであった場合
	if(dwByteRecv == 0){
		wsprintf(m_szErrMsg, "データの受信に失敗しました。\n"
							 "ErrCode:%d\n"
							 "SSL Error:%s",
									WSAGetLastError(), ERR_error_string(ERR_get_error(), NULL));
		return FALSE;
	}
	
	if(pdwRecvLen != NULL){
		*pdwRecvLen = dwByteRecv;
	}

	// 正常終了 バッファをクリア
	ZeroMemory(m_szErrMsg, sizeof(m_szErrMsg));
	return TRUE;

}

/******************************************************************************/
/** 
	接続先ホストの証明書を取得します。
	
	@param[in] pCertificate	_Certificate構造体のポインタ
	@return 成功したときはTRUEを返す。
	失敗したときはFALSEを返す。
*/
BOOL cSslSock::GetPeerCert(_Certificate *pCertificate){

	// 変数宣言
	X509			*client_cert	= NULL;
	LPSTR			szBuffer		= NULL;
	char			temp[3];
	int				date[6];
	ASN1_TIME		*timeExpire;

	// 通信相手の証明書を取得
	client_cert = SSL_get_peer_certificate(m_SSL);

	if(client_cert == NULL){
		wsprintf(m_szErrMsg, "クライアント証明書を取得できませんでした。");
		return FALSE;
	}

	// 発行先の証明書情報を1行で取得
	szBuffer = X509_NAME_oneline(X509_get_subject_name(client_cert), 0, 0);
	_PushCertificate(&pCertificate->x509Subject,  szBuffer);

	// 発行元の証明書情報を1行で取得
	szBuffer = X509_NAME_oneline(X509_get_issuer_name(client_cert), 0, 0);
	_PushCertificate(&pCertificate->x509Issuer,  szBuffer);

	// 証明書の有効期限を取得
	timeExpire = X509_get_notBefore(client_cert);
	for(int y = 0; y < 6; y++){
		temp[0] = timeExpire->data[y*2];
		temp[1] = timeExpire->data[y*2+1];
		temp[2] = '\0';
		date[y] = atoi(temp);
	}
	wsprintf(pCertificate->szStartDate, "%04d/%02d/%02d %02d:%02d:%02d", date[0] + 2000, date[1], date[2], date[3], date[4], date[5]);

	// 証明書の有効期限を取得
	timeExpire  = X509_get_notAfter(client_cert);
	for(int y = 0; y < 6; y++){
		temp[0] = timeExpire->data[y*2];
		temp[1] = timeExpire->data[y*2+1];
		temp[2] = '\0';
		date[y] = atoi(temp);
	}
	wsprintf(pCertificate->szEndDate, "%04d/%02d/%02d %02d:%02d:%02d", date[0] + 2000, date[1], date[2], date[3], date[4], date[5]);

	// 取得した証明書の解放
	X509_free(client_cert);

	// 正常終了 バッファをクリア
	ZeroMemory(m_szErrMsg, sizeof(m_szErrMsg));
	return TRUE;

}

/******************************************************************************/
/** 
	証明書の格納を行います。
	
	X509_NAME_onelineで取得した情報を構造体に格納します。
	
	@param[in] pX509Info	_X509Info構造体のポインタ
	@param[in] szData		X509_NAME_onelineで取得した文字列
	
	@todo メソッドの実装が曖昧
*/
void cSslSock::_PushCertificate(_X509Info *pX509Info, LPCSTR szData){

	// 変数宣言
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
	証明書の格納を行います。
	
	X509_NAME_onelineで取得した情報を構造体に格納します。
	
	@param[in] pX509Info	_X509Info構造体のポインタ
	@param[in] szBuff		_PushCertificateで分割した文字列
	
	@todo メソッドの実装が曖昧
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
	証明書の破棄を行います。
	
	@param[in] pCertificate	_Certificate構造体のポインタ
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
	証明書の検証を行います。
	
	@retval 検証結果を返します。
*/
LPCSTR cSslSock::Verify_SSL(){

	// 変数宣言
	long	verify_result		= NULL;
	char	szVerifyMsg[2048];

	// バッファのクリア
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
			"証明書の有効期限が未来の日付に設定されています。",
			"証明書が有効になる日付にまだ達していません。コンピュータの日付設定を確認してください。"
		},
		{
			X509_V_ERR_CERT_HAS_EXPIRED,
			"証明書の有効期限がすでに切れています。",
			"証明書の有効期限がすでに切れています。",
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
			"メモリ確保エラー",
			"メモリを確保出来ませんでした。このエラーは起きるべきでは有りません。"
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
			"証明書が拒絶された",
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

	// SSL証明書の検証
	verify_result = SSL_get_verify_result(m_SSL);

	// 結果を判断する
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
