#include "cWinSock.h"

/**	@file
	@brief	Windowsソケット
	
	@author	TATSUYA
	@date	2007/02/28
*/

/******************************************************************************/
/**	@brief	コンストラクタ

	クラスの初期化を行います。
*/
cWinSock::cWinSock(){

	// WinSockの未初期化
	m_bStartUp = FALSE;

}

/******************************************************************************/
/**	@brief	デストラクタ

	クラスの解放を行います。
*/
cWinSock::~cWinSock(){

	// 解放
	Release();

}

/******************************************************************************/
/** 
	Windowsソケットの初期化をします。
	
	@return 成功したときはTRUEを返す。
	失敗したときはFALSEを返す。
*/
BOOL cWinSock::Startup(){

	if(m_bStartUp){
		if(!Release()){
			return FALSE;
		}
	}

	if(WSAStartup(MAKEWORD(2, 0), &m_wsadata) != 0){
		wsprintf(m_szErrMsg, "Windows Socketの初期化に失敗しました。\n"
							 "ErrCode:%d",
									WSAGetLastError());
		return FALSE;
	}

	if(m_wsadata.wVersion != MAKEWORD(2, 0)){
		wsprintf(m_szErrMsg, "Windows Socketのバージョンが要求したバージョンと異なります。\n"
							 "ErrCode:%d",
									WSAGetLastError());
		return FALSE;
	}

	m_bStartUp = TRUE;

	// 正常終了
	ZeroMemory(m_szErrMsg, sizeof(m_szErrMsg));
	return TRUE;

}

/******************************************************************************/
/** 
	Windowsソケットの解放をします。
	
	@return 成功したときはTRUEを返す。
	失敗したときはFALSEを返す。
*/
BOOL cWinSock::Release(){

	if(m_bStartUp){
		if(WSACleanup() != 0){
			wsprintf(m_szErrMsg, "Windows Socketの解放しました。\n"
								 "ErrCode:%d",
									WSAGetLastError());
			return FALSE;
		}
	}

	m_bStartUp = FALSE;

	// 正常終了
	ZeroMemory(m_szErrMsg, sizeof(m_szErrMsg));
	return TRUE;

}

/******************************************************************************/
/** 
	エラーメッセージの表示をします。
*/
void cWinSock::ErrMsgBox(){

	if(strlen(m_szErrMsg) > 0){
		MessageBox(NULL, m_szErrMsg, "Error[cWinSock]", MB_OK | MB_ICONERROR);
	}

	return;

}
