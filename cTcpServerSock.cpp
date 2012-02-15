#include "cTcpServerSock.h"

/**	@file
	@brief	TCP Serverソケット
	
	@author	TATSUYA
	@date	2007/03/01
*/

/******************************************************************************/
/** @brief	コンストラクタ
	
	クラスの初期化を行います。
*/
cTcpServerSock::cTcpServerSock(){

	// 待受フラグを未待受に設定
	m_bIsListening = FALSE;

}

/******************************************************************************/
/** @brief	デストラクタ
	
	クラスの解放を行います。
*/
cTcpServerSock::~cTcpServerSock(){

	if(IsListening()){
		CloseListen();
	}

}

/******************************************************************************/
/** 
	サーバが待ち受けを行っているか確認します。
	
	@return 待ち受けを行っている場合にはTRUEを返す。
	待ち受けを行っていない場合にはFALSEを返す。
*/
BOOL cTcpServerSock::IsListening(){

	return m_bIsListening;

}

/******************************************************************************/
/** 
	待ち受けを開始します。
	
	@param[in] nPort	待ち受けを行うポート番号
	@param[in] hWnd		非同期モードで利用する場合は、通知先のウィンドウハンドル<br>
						同期モードで利用する場合はNULL
	@param[in] nEventNo	非同期モードで利用する場合は、通知するメッセージID<br>
						同期モードで利用する場合はNULL
	@return 成功したときはTRUEを返す。
	失敗したときはFALSEを返す。
*/
BOOL cTcpServerSock::Listen(USHORT nPort, HWND hWnd, USHORT nEventNo){

	// 変数宣言
	const int	iYes = 1;

	// Socketの作成
	m_ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(m_ServerSocket == INVALID_SOCKET){
		wsprintf(m_szErrMsg, "サーバソケットの生成に失敗しました。\n"
							 "ErrCode:%d",
									WSAGetLastError());
		return FALSE;
	}

	// 非同期設定
	if(!SetAsync(hWnd, nEventNo)){
		return FALSE;
	}

	// TCP_NODELAY
	if(setsockopt(m_ServerSocket, IPPROTO_TCP, TCP_NODELAY, (LPCSTR)&iYes, sizeof(iYes)) == SOCKET_ERROR){
		wsprintf(m_szErrMsg, "ソケットオプション(TCP_NODELAY)の設定に失敗しました。\n"
							 "ErrCode:%d",
									WSAGetLastError());
		return FALSE;
	}

	// 待ち受けするアドレスを指定
	m_ServerAddr.sin_family				= AF_INET;
	m_ServerAddr.sin_port				= htons(nPort);
	m_ServerAddr.sin_addr.S_un.S_addr	= INADDR_ANY;
	
	// ソケットとアドレスの関連付け
	if(bind(m_ServerSocket, (LPSOCKADDR)&m_ServerAddr, sizeof(SOCKADDR)) == SOCKET_ERROR){
		wsprintf(m_szErrMsg, "ソケットのポート番号との関連付けに失敗しました。\n"
							 "ErrCode:%d",
									WSAGetLastError());
		closesocket(m_ServerSocket);
		return FALSE;
	}
	
	// 待ち受けの開始
	if(listen(m_ServerSocket, SOMAXCONN) == SOCKET_ERROR){
		wsprintf(m_szErrMsg, "ソケットの待ち受け開始に失敗しました。\n"
							 "ErrCode:%d",
									WSAGetLastError());
		closesocket(m_ServerSocket);
		return FALSE;
	}

	// 待受フラグを待受中に設定
	m_bIsListening = TRUE;

	return TRUE;

}

/******************************************************************************/
/** 
	非同期モードの設定を行います。
	
	@param[in] hWnd		非同期モードで利用する場合は、通知先のウィンドウハンドル<br>
						同期モードで利用する場合はNULL
	@param[in] nEventNo	非同期モードで利用する場合は、通知するメッセージID<br>
						同期モードで利用する場合はNULL
	@return 成功したときはTRUEを返す。
	失敗したときはFALSEを返す。
*/
BOOL cTcpServerSock::SetAsync(HWND hWnd, USHORT nEventNo){

	// 非同期処理を設定
	if(hWnd != NULL && nEventNo != NULL){
		if(WSAAsyncSelect(m_ServerSocket, hWnd, nEventNo, FD_ACCEPT) == SOCKET_ERROR){
			wsprintf(m_szErrMsg, "非同期処理の設定に失敗しました。\n"
								 "ErrCode:%d",
										WSAGetLastError());
			return FALSE;
		}
	}

	// 正常終了 バッファをクリア
	ZeroMemory(m_szErrMsg, sizeof(m_szErrMsg));
	return TRUE;

}

/******************************************************************************/
/** 
	TCPソケットの接続要求を受け入れます。
	
	@param[in] oTcpSock cTcpSockのオブジェクトポインタ
	@return 成功したときはTRUEを返す。
	失敗したときはFALSEを返す。
*/
BOOL cTcpServerSock::Accept(cTcpSock *oTcpSock){

	// 変数宣言
	int iSockAddrLen	= sizeof(oTcpSock->m_Addr);
	int iErrNo			= 0;

	oTcpSock->m_Socket = accept(m_ServerSocket, (LPSOCKADDR)&oTcpSock->m_Addr, &iSockAddrLen);

	if(oTcpSock->m_Socket == INVALID_SOCKET){
		if((iErrNo = WSAGetLastError()) != WSAEWOULDBLOCK){
			wsprintf(m_szErrMsg, "新規接続の受け入れに失敗しました。\n"
								 "ErrCode:%d",
										iErrNo);
			return FALSE;
		}
	}

	// 接続フラグを接続中に設定
	oTcpSock->m_bIsConnect = TRUE;

	return TRUE;

}

/******************************************************************************/
/** 
	待ち受けを終了します。
	
	@return 成功したときはTRUEを返す。
	失敗したときはFALSEを返す。
*/
BOOL cTcpServerSock::CloseListen(){

	// ソケットを閉じる
	if(closesocket(m_ServerSocket) == SOCKET_ERROR){
		wsprintf(m_szErrMsg, "ソケットの解放に失敗しました。\n"
							 "ErrCode:%d",
									WSAGetLastError());
		return FALSE;
	}

	// 接続フラグを未接続に設定
	m_bIsListening = FALSE;

	return TRUE;

}

/******************************************************************************/
/** 
	エラーメッセージの表示をします。
*/
void cTcpServerSock::ErrMsgBox(){

	if(strlen(m_szErrMsg) > 0){
		MessageBox(NULL, m_szErrMsg, "Error[cTcpServerSock]", MB_OK | MB_ICONERROR);
	}

	return;

}
