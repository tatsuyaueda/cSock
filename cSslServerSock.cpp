#include "cSslServerSock.h"

/**	@file
	@brief	SSL Serverソケット
	
	@author	TATSUYA
	@date	2007/02/28
*/

/******************************************************************************/
/** @brief	コンストラクタ
	
	クラスの初期化を行います。
	OpenSSLの初期化やCTX構造体の初期化もコンストラクタで行っています。
*/
cSslServerSock::cSslServerSock(){

	// OpenSSL の初期化
	SSL_library_init();
	// エラーを文字列として表示
	SSL_load_error_strings();

	// 初期値設定
	m_CTX		= NULL;

	// CTX構造体の設定
	if((m_CTX = SSL_CTX_new(TLSv1_server_method())) == NULL){
		wsprintf(m_szErrMsg, "CTX構造体の設定に失敗しました。\n"
							 "SSL Error:%s",
								ERR_error_string(ERR_get_error(), NULL));
		return;
	}

	// 正常終了 バッファをクリア
	ZeroMemory(m_szErrMsg, sizeof(m_szErrMsg));

}

/******************************************************************************/
/** @brief	デストラクタ
    
    クラスの解放を行います。
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
	SSLソケットの接続要求を受け入れます。
	
	@param[in] oSslSock cSslSockのオブジェクトポインタ
	@return 成功したときはTRUEを返す。
	失敗したときはFALSEを返す。
*/
BOOL cSslServerSock::Accept(cSslSock* oSslSock){

	// 変数宣言
	int	iRet	= 0;
	int iErrNo	= 0;

	// 通常のAcceptを実行
	if(!cTcpServerSock::Accept(oSslSock)){
		return FALSE;
	}

	// SSL構造体の再生成
	if((oSslSock->m_SSL = SSL_new(oSslSock->m_CTX)) == NULL){
		wsprintf(m_szErrMsg, "SSL構造体の再生成に失敗しました。\n"
							 "SSL Error:%s",
								ERR_error_string(ERR_get_error(), NULL));
		// TCPソケットが接続中の場合は切断
		if(oSslSock->IsConnect()){
			oSslSock->Disconnect();
		}
		return FALSE;
	}

	// SSLとソケットの関連付け
	if(!SSL_set_fd(oSslSock->m_SSL, (int)oSslSock->m_Socket)){
		wsprintf(m_szErrMsg, "SSLとソケットの関連付けに失敗しました。\n"
							 "SSL Error:%s",
								ERR_error_string(ERR_get_error(), NULL));
		// TCPソケットが接続中の場合は切断
		if(oSslSock->IsConnect()){
			oSslSock->Disconnect();
		}
		return FALSE;
	}

	// SSLのAcceptを実行
	do{
		iRet	= SSL_accept(oSslSock->m_SSL);
		iErrNo  = SSL_get_error(oSslSock->m_SSL, iRet);
	}while((iErrNo == SSL_ERROR_WANT_X509_LOOKUP || iErrNo == SSL_ERROR_WANT_READ) && iRet <= 0);

	if(iRet <= 0){
		wsprintf(m_szErrMsg, "SSLのAcceptに失敗しました。\n"
							 "SSL Error:%s",
								ERR_error_string(iErrNo, NULL));
		// TCPソケットが接続中の場合は切断
		if(oSslSock->IsConnect()){
			oSslSock->Disconnect();
		}
		return FALSE;
	}

 	return TRUE;

}

/******************************************************************************/
/** 
	サーバソケットで利用しているCTX構造体を取得します。
	
	@return CTX構造体ポインタ
*/
SSL_CTX *cSslServerSock::GetCTX(){

	return m_CTX;

}