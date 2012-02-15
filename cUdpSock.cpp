#include "cUdpSock.h"

/**	@file
	@brief	UDPソケット
	
	@author	TATSUYA
	@date	2007/02/28
*/

/******************************************************************************/
/**	@brief	コンストラクタ(データ送信側)

	データを送信する側としてクラスの初期化を行います。
	@param[in] szHost	接続先ホストのホスト名またはIPアドレス
	@param[in] nPort	接続先ホストのポート番号
*/
cUdpSock::cUdpSock(LPCSTR szHost, USHORT nPort){

	// 変数宣言
	const int	iFlagBroadcast	= 1;
	HOSTENT		*host			= NULL;

	// ソケットの初期化
	if(!_InitSock()){
		return;
	}

	// 構造体に接続先を登録する
	m_Addr.sin_family			= AF_INET;
	m_Addr.sin_port				= htons(nPort);
	m_Addr.sin_addr.S_un.S_addr	= inet_addr(szHost);

	if(m_Addr.sin_addr.S_un.S_addr == 0xFFFFFFFF){
		host = gethostbyname(szHost);
		if(host == NULL){
			wsprintf(m_szErrMsg, "ホスト名からIPアドレスを取得できませんした。\n"
								 "ErrCode:%d",
									WSAGetLastError());
			return;
		}
		m_Addr.sin_addr.S_un.S_addr = *(unsigned int *)host->h_addr_list[0];
	}

	// ブロードキャストの場合
	if(m_Addr.sin_addr.S_un.S_addr == 0xFFFFFFFF){
		if(setsockopt(m_Socket, SOL_SOCKET, SO_BROADCAST, (char *)&iFlagBroadcast, sizeof(iFlagBroadcast)) == SOCKET_ERROR){
			wsprintf(m_szErrMsg, "ソケットオプション(SO_BROADCAST)の設定に失敗しました。\n"
								 "ErrCode:%d",
									WSAGetLastError());
			return;
		}
	}

	return;

}

/******************************************************************************/
/**	@brief	コンストラクタ(データ受信側)

	データを受信する側としてクラスの初期化を行います。
	@param[in] nPort	待ち受けを行うポート番号
	@param[in] hWnd		非同期モードで利用する場合は、通知先のウィンドウハンドル<br>
						同期モードで利用する場合はNULL
	@param[in] nEventNo	非同期モードで利用する場合は、通知するメッセージID<br>
						同期モードで利用する場合はNULL
*/
cUdpSock::cUdpSock(USHORT nPort, HWND hWnd, USHORT nEventNo){

	// ソケットの初期化
	if(!_InitSock()){
		return;
	}

	// 構造体に接続先を登録する
	m_Addr.sin_family		= AF_INET;
	m_Addr.sin_port			= htons(nPort);
	m_Addr.sin_addr.s_addr	= INADDR_ANY;

	// エラーチェック
	if(bind(m_Socket, (struct sockaddr *)&m_Addr, sizeof(m_Addr)) == SOCKET_ERROR){
		wsprintf(m_szErrMsg, "ソケットのポート番号との関連付けに失敗しました。\n"
							 "ErrCode:%d",
								WSAGetLastError());
		return;
	}

	// 非同期処理を設定
	if(hWnd != NULL && nEventNo != 0){
		if(WSAAsyncSelect(m_Socket, hWnd, nEventNo, FD_READ) == SOCKET_ERROR){
			wsprintf(m_szErrMsg, "非同期処理の設定に失敗しました。\n"
								 "ErrCode:%d",
									WSAGetLastError());
			return;
		}
	}

	return;

}

/******************************************************************************/
/**	@brief	デストラクタ

	クラスの解放を行います。
*/
cUdpSock::~cUdpSock(){

	// ソケットのシャットダウン
	if(shutdown(m_Socket, 2) == SOCKET_ERROR){
		wsprintf(m_szErrMsg, "ソケットのシャットダウンに失敗しました。\n"
							 "ErrCode:%d",
								WSAGetLastError());
		return;
	}

	// ソケットを閉じる
	if(closesocket(m_Socket) == SOCKET_ERROR){
		wsprintf(m_szErrMsg, "ソケットの解放に失敗しました。\n"
							 "ErrCode:%d",
								WSAGetLastError());
		return;
	}

	return;

}

/******************************************************************************/
/** 
	接続先ホストのIPアドレスを取得します。
	
	@return 接続先ホストのIPアドレス
*/
LPCSTR cUdpSock::GetPeerIpAddress(){

	return inet_ntoa(m_AddrPeer.sin_addr);

}

/******************************************************************************/
/** 
	接続先ホストのポート番号を取得します。
	
	@return 接続先ホストのポート番号
*/
USHORT cUdpSock::GetPeerPort(){

	return ntohs(m_AddrPeer.sin_port);

}

/******************************************************************************/
/** 
	ソケットの初期化を行います。
	
	@return 成功したときはTRUEを返す。
	失敗したときはFALSEを返す。
*/
BOOL cUdpSock::_InitSock(){

	// バッファのクリア
	ZeroMemory(&m_Addr, sizeof(SOCKADDR_IN));
	ZeroMemory(&m_AddrPeer, sizeof(SOCKADDR_IN));

	// ソケットの生成
	m_Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	// エラーチェック
	if(m_Socket == INVALID_SOCKET){
		wsprintf(m_szErrMsg, "UDPソケットの生成に失敗しました。\n"
							 "ErrCode:%d",
								WSAGetLastError());
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
BOOL cUdpSock::SendData(LPVOID lpData, DWORD dwBytes){

	// 変数宣言
	int		iSendBytes	= 0;
	LPCSTR	lpBuff		= (LPCSTR)lpData;

	// 全てのデータを送信するまでループを続ける
	do{
		iSendBytes = sendto(m_Socket, lpBuff, dwBytes, 0, (LPSOCKADDR)&m_Addr, sizeof(m_Addr));
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

	return TRUE;

}

/******************************************************************************/
/** 
	文字列を送信します。
	
	@param[in] szStr	送信する文字列
	@return 成功したときはTRUEを返す。
	失敗したときはFALSEを返す。
*/
BOOL cUdpSock::SendStr(LPCSTR szStr){

	return this->SendData((LPVOID)szStr, (DWORD)strlen(szStr) + 1);

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
BOOL cUdpSock::RecvData(LPVOID lpBuffer, DWORD dwBytes, DWORD *pdwRecvLen){

	// 変数宣言
	DWORD	dwRecvBytes		= 0;
	LPSTR	lpBuff			= (LPSTR)lpBuffer;
	int		iByteRecv		= 0;
	int		iLenSockAddr	= sizeof(SOCKADDR_IN);

	// 指定したバイト数、受信するまでループを続ける
	while(1){
		iByteRecv = recvfrom(m_Socket, lpBuff, dwBytes - dwRecvBytes, 0, (LPSOCKADDR)&m_AddrPeer, &iLenSockAddr);
		if(iByteRecv == SOCKET_ERROR || iByteRecv == 0) {
			break;
		}
		dwRecvBytes += iByteRecv;
		lpBuff		 = (LPSTR)lpBuffer + dwRecvBytes;
		if(dwBytes <= dwRecvBytes){
			break;
		}
	}

	if(dwRecvBytes == 0){
		wsprintf(m_szErrMsg, "データの受信に失敗しました。\n"
							 "ErrCode:%d",
									WSAGetLastError());
		return FALSE;
	}

	if(pdwRecvLen){
		*pdwRecvLen = dwRecvBytes;
	}

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
BOOL cUdpSock::RecvDataOnce(LPVOID lpBuffer, DWORD dwBytes, DWORD *pdwRecvLen){

	// 変数宣言
	int	iByteRecv = 0;
	int	iLenSockAddr = sizeof(SOCKADDR_IN);

	iByteRecv = recvfrom(m_Socket, (LPSTR)lpBuffer, dwBytes, 0, (LPSOCKADDR)&m_AddrPeer, &iLenSockAddr);

	if(iByteRecv == SOCKET_ERROR){
		wsprintf(m_szErrMsg, "データの受信に失敗しました。\n"
							 "ErrCode:%d",
									WSAGetLastError());
		return FALSE;
	}

	if(pdwRecvLen){
		*pdwRecvLen = (DWORD)iByteRecv;
	}

	return TRUE;

}

/******************************************************************************/
/** 
	引数のソケットハンドルと同一のソケットか判定する。

	@param[in] sock		判定対象のソケットハンドル
	@return 一致したときはTRUEを返す。
	一致しなかったときはFALSEを返す。
*/
BOOL cUdpSock::MatchSock(SOCKET sock){

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
void cUdpSock::ErrMsgBox(){

	if(strlen(m_szErrMsg) > 0){
		MessageBox(NULL, m_szErrMsg, "Error[cUdpSock]", MB_OK | MB_ICONERROR);
	}

	return;

}