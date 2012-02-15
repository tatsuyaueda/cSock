#include "cTcpSock.h"

/**	@file
	@brief	TCPソケット
	
	@author	TATSUYA
	@date	2007/03/01
*/

/******************************************************************************/
/** @brief	コンストラクタ
	
	クラスの初期化を行います。
*/
cTcpSock::cTcpSock(){

	// 接続フラグを未接続に設定
	m_bIsConnect = FALSE;

}

/******************************************************************************/
/** @brief	デストラクタ
	
	クラスの解放を行います。
*/
cTcpSock::~cTcpSock(){

	// 接続中の場合は切断する
	if(IsConnect()){
		Disconnect();
	}

}

/******************************************************************************/
/** 
	接続先ホストのIPアドレスを取得します。
	
	@return 接続先ホストのIPアドレス
*/
LPCSTR cTcpSock::GetPeerIpAddress(){

	return inet_ntoa(m_Addr.sin_addr);

}

/******************************************************************************/
/** 
	接続先ホストのポート番号を取得します。
	
	@return 接続先ホストのポート番号
*/
USHORT cTcpSock::GetPeerPort(){

	return ntohs(m_Addr.sin_port);

}

/******************************************************************************/
/** 
	ソケットのコネクションが確立しているか確認します。
	
	@return コネクションが確立している場合にはTRUEを返す。
	コネクションが確立していない場合にはFALSEを返す。
*/
BOOL cTcpSock::IsConnect(){

	return m_bIsConnect;

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
BOOL cTcpSock::Connect(LPCSTR szHost, USHORT nPort, HWND hWnd, USHORT nEventNo){

	// 変数宣言
	const int	iYes	= 1;
	HOSTENT		*host	= NULL;

	// 構造体に接続先を登録する
	m_Addr.sin_family		= AF_INET;
	m_Addr.sin_port			= htons(nPort);
	m_Addr.sin_addr.s_addr	= inet_addr(szHost);

	if(m_Addr.sin_addr.S_un.S_addr == 0xffffffff){
		host = gethostbyname(szHost);
		if(host == NULL){
			wsprintf(m_szErrMsg, "ホスト名からIPアドレスを取得できませんした。\n"
								 "ErrCode:%d",
										WSAGetLastError());
			return FALSE;
		}
		m_Addr.sin_addr.S_un.S_addr = *(PUINT)host->h_addr_list[0];
	}

	// ソケットの生成
	m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// エラーチェック
	if(m_Socket == INVALID_SOCKET){
		wsprintf(m_szErrMsg, "ソケットの作成に失敗しました。\n"
							 "ErrCode:%d",
									WSAGetLastError());
		return FALSE;
	}

	// 非同期設定
	if(!SetAsync(hWnd, nEventNo)){
		return FALSE;
	}

	// TCP_NODELAY
	if(setsockopt(m_Socket, IPPROTO_TCP, TCP_NODELAY, (LPCSTR)&iYes, sizeof(iYes)) == SOCKET_ERROR){
		wsprintf(m_szErrMsg, "ソケットオプション(TCP_NODELAY)の設定に失敗しました。\n"
							 "ErrCode:%d",
									WSAGetLastError());
		return FALSE;
	}

	// エラーチェック
	if(connect(m_Socket, (LPSOCKADDR)&m_Addr,	sizeof(SOCKADDR)) == SOCKET_ERROR){
		if(WSAGetLastError() != WSAEWOULDBLOCK){
			wsprintf(m_szErrMsg, "接続失敗に失敗しました。\n"
								 "ErrCode:%d",
										WSAGetLastError());
			return FALSE;
		}
	}

	// 接続フラグを接続中に設定
	m_bIsConnect = TRUE;

	// 正常終了 バッファをクリア
	ZeroMemory(m_szErrMsg, sizeof(m_szErrMsg));
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
BOOL cTcpSock::SetAsync(HWND hWnd, USHORT nEventNo){

	// 非同期処理を設定
	if(hWnd != NULL && nEventNo != 0){
		if(WSAAsyncSelect(m_Socket, hWnd, nEventNo, FD_READ | FD_CLOSE) == SOCKET_ERROR){
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
	確立済みのコネクションを切断します。
	
	@return 成功したときはTRUEを返す。
	失敗したときはFALSEを返す。
*/
BOOL cTcpSock::Disconnect(){

	// 接続中でなければ実行しない
	if(!IsConnect()){
		wsprintf(m_szErrMsg, "ソケットが接続中ではないため切断できません。");
		return FALSE;
	}

	// ソケットのシャットダウン
	if(shutdown(m_Socket, 2) == SOCKET_ERROR){
		wsprintf(m_szErrMsg, "ソケットのシャットダウンに失敗しました。\n"
							 "ErrCode:%d",
									WSAGetLastError());
		return FALSE;
	}

	// ソケットを閉じる
	if(closesocket(m_Socket) == SOCKET_ERROR){
		wsprintf(m_szErrMsg, "ソケットの解放に失敗しました。\n"
							 "ErrCode:%d",
									WSAGetLastError());
		return FALSE;
	}

	// 接続フラグを未接続に設定
	m_bIsConnect = FALSE;

	// 正常終了 バッファをクリア
	ZeroMemory(m_szErrMsg, sizeof(m_szErrMsg));
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
BOOL cTcpSock::SendData(LPVOID lpData, DWORD dwBytes){

	// 変数宣言
	int		iSendBytes	= 0;
	LPSTR	lpBuff		= (LPSTR)lpData;

	// 全てのデータを送信するまでループを続ける
	do{
		iSendBytes = send(m_Socket, lpBuff, dwBytes, 0);
		if(iSendBytes == SOCKET_ERROR){
			wsprintf(m_szErrMsg, "データの送信に失敗しました。\n"
								 "ErrCode:%d",
										WSAGetLastError());
			return FALSE;
		}
		// ポインタの移動
		lpBuff	+= iSendBytes;
		dwBytes	-= iSendBytes;
	}while(dwBytes > 0);

	// 正常終了 バッファをクリア
	ZeroMemory(m_szErrMsg, sizeof(m_szErrMsg));
	return TRUE;

}

/******************************************************************************/
/** 
	文字列の送信を行います。
	
	@param[in] szStr	送信する文字列
	@return 成功したときはTRUEを返す。
	失敗したときはFALSEを返す。
*/
BOOL cTcpSock::SendStr(LPCSTR szStr){

	return SendData((LPVOID)szStr, (DWORD)strlen(szStr) + 1);

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
BOOL cTcpSock::RecvData(LPVOID lpBuffer, DWORD dwBytes, DWORD *pdwRecvLen){

	// 変数宣言
	DWORD	dwRecvBytes	= 0;
	LPSTR	lpBuff		= (LPSTR)lpBuffer;
	int		iByteRecv	= 0;

	// 指定したバイト数、受信するまでループを続ける
	while(1){
		iByteRecv = recv(m_Socket, (LPSTR)lpBuff, dwBytes - dwRecvBytes, 0);
		if(iByteRecv == SOCKET_ERROR || iByteRecv == 0) {
			break;
		}
		dwRecvBytes += iByteRecv;
		lpBuff		 = (LPSTR)lpBuffer +  dwRecvBytes;
		if(dwBytes <= dwRecvBytes){
			break;
		}
	}

	// 受信したバイト数が0バイトであった場合
	if(dwRecvBytes == 0){
		wsprintf(m_szErrMsg, "データの受信に失敗しました。\n"
							 "ErrCode:%d",
									WSAGetLastError());
		return FALSE;
	}

	if(pdwRecvLen != NULL){
		*pdwRecvLen = dwRecvBytes;
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
BOOL cTcpSock::RecvDataOnce(LPVOID lpBuffer, DWORD dwBytes, DWORD* pdwRecvLen){

	// 変数宣言
	int iByteRecv = 0;
	
	iByteRecv = recv(m_Socket, (LPSTR)lpBuffer, dwBytes, 0);
	
	// 受信したバイト数が0バイトであった場合
	if(iByteRecv == 0){
		wsprintf(m_szErrMsg, "データの受信に失敗しました。\n"
							 "ErrCode:%d",
									WSAGetLastError());
		return FALSE;
	}
	
	if(pdwRecvLen != NULL){
		*pdwRecvLen = (DWORD)iByteRecv;
	}

	// 正常終了 バッファをクリア
	ZeroMemory(m_szErrMsg, sizeof(m_szErrMsg));
	return TRUE;

}

/******************************************************************************/
/** 
	引数のソケットハンドルと同一のソケットか判定する。

	@param[in] sock		判定対象のソケットハンドル
	@return 一致したときはTRUEを返す。
	一致しなかったときはFALSEを返す。
*/
BOOL cTcpSock::MatchSock(SOCKET sock){

	if(m_Socket == sock){
		return TRUE;
	}else{
		return FALSE;
	}

}

/******************************************************************************/
/** 
	エラーメッセージの表示をします。
*/
void cTcpSock::ErrMsgBox(){

	if(strlen(m_szErrMsg) > 0){
		MessageBox(NULL, m_szErrMsg, "Error[cTcpSock]", MB_OK | MB_ICONERROR);
	}

	return;

}
