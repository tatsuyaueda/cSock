#include "cTcpServerSock.h"

/**	@file
	@brief	TCP Server�\�P�b�g
	
	@author	TATSUYA
	@date	2007/03/01
*/

/******************************************************************************/
/** @brief	�R���X�g���N�^
	
	�N���X�̏��������s���܂��B
*/
cTcpServerSock::cTcpServerSock(){

	// �Ҏ�t���O�𖢑Ҏ�ɐݒ�
	m_bIsListening = FALSE;

}

/******************************************************************************/
/** @brief	�f�X�g���N�^
	
	�N���X�̉�����s���܂��B
*/
cTcpServerSock::~cTcpServerSock(){

	if(IsListening()){
		CloseListen();
	}

}

/******************************************************************************/
/** 
	�T�[�o���҂��󂯂��s���Ă��邩�m�F���܂��B
	
	@return �҂��󂯂��s���Ă���ꍇ�ɂ�TRUE��Ԃ��B
	�҂��󂯂��s���Ă��Ȃ��ꍇ�ɂ�FALSE��Ԃ��B
*/
BOOL cTcpServerSock::IsListening(){

	return m_bIsListening;

}

/******************************************************************************/
/** 
	�҂��󂯂��J�n���܂��B
	
	@param[in] nPort	�҂��󂯂��s���|�[�g�ԍ�
	@param[in] hWnd		�񓯊����[�h�ŗ��p����ꍇ�́A�ʒm��̃E�B���h�E�n���h��<br>
						�������[�h�ŗ��p����ꍇ��NULL
	@param[in] nEventNo	�񓯊����[�h�ŗ��p����ꍇ�́A�ʒm���郁�b�Z�[�WID<br>
						�������[�h�ŗ��p����ꍇ��NULL
	@return ���������Ƃ���TRUE��Ԃ��B
	���s�����Ƃ���FALSE��Ԃ��B
*/
BOOL cTcpServerSock::Listen(USHORT nPort, HWND hWnd, USHORT nEventNo){

	// �ϐ��錾
	const int	iYes = 1;

	// Socket�̍쐬
	m_ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(m_ServerSocket == INVALID_SOCKET){
		wsprintf(m_szErrMsg, "�T�[�o�\�P�b�g�̐����Ɏ��s���܂����B\n"
							 "ErrCode:%d",
									WSAGetLastError());
		return FALSE;
	}

	// �񓯊��ݒ�
	if(!SetAsync(hWnd, nEventNo)){
		return FALSE;
	}

	// TCP_NODELAY
	if(setsockopt(m_ServerSocket, IPPROTO_TCP, TCP_NODELAY, (LPCSTR)&iYes, sizeof(iYes)) == SOCKET_ERROR){
		wsprintf(m_szErrMsg, "�\�P�b�g�I�v�V����(TCP_NODELAY)�̐ݒ�Ɏ��s���܂����B\n"
							 "ErrCode:%d",
									WSAGetLastError());
		return FALSE;
	}

	// �҂��󂯂���A�h���X���w��
	m_ServerAddr.sin_family				= AF_INET;
	m_ServerAddr.sin_port				= htons(nPort);
	m_ServerAddr.sin_addr.S_un.S_addr	= INADDR_ANY;
	
	// �\�P�b�g�ƃA�h���X�̊֘A�t��
	if(bind(m_ServerSocket, (LPSOCKADDR)&m_ServerAddr, sizeof(SOCKADDR)) == SOCKET_ERROR){
		wsprintf(m_szErrMsg, "�\�P�b�g�̃|�[�g�ԍ��Ƃ̊֘A�t���Ɏ��s���܂����B\n"
							 "ErrCode:%d",
									WSAGetLastError());
		closesocket(m_ServerSocket);
		return FALSE;
	}
	
	// �҂��󂯂̊J�n
	if(listen(m_ServerSocket, SOMAXCONN) == SOCKET_ERROR){
		wsprintf(m_szErrMsg, "�\�P�b�g�̑҂��󂯊J�n�Ɏ��s���܂����B\n"
							 "ErrCode:%d",
									WSAGetLastError());
		closesocket(m_ServerSocket);
		return FALSE;
	}

	// �Ҏ�t���O��Ҏ󒆂ɐݒ�
	m_bIsListening = TRUE;

	return TRUE;

}

/******************************************************************************/
/** 
	�񓯊����[�h�̐ݒ���s���܂��B
	
	@param[in] hWnd		�񓯊����[�h�ŗ��p����ꍇ�́A�ʒm��̃E�B���h�E�n���h��<br>
						�������[�h�ŗ��p����ꍇ��NULL
	@param[in] nEventNo	�񓯊����[�h�ŗ��p����ꍇ�́A�ʒm���郁�b�Z�[�WID<br>
						�������[�h�ŗ��p����ꍇ��NULL
	@return ���������Ƃ���TRUE��Ԃ��B
	���s�����Ƃ���FALSE��Ԃ��B
*/
BOOL cTcpServerSock::SetAsync(HWND hWnd, USHORT nEventNo){

	// �񓯊�������ݒ�
	if(hWnd != NULL && nEventNo != NULL){
		if(WSAAsyncSelect(m_ServerSocket, hWnd, nEventNo, FD_ACCEPT) == SOCKET_ERROR){
			wsprintf(m_szErrMsg, "�񓯊������̐ݒ�Ɏ��s���܂����B\n"
								 "ErrCode:%d",
										WSAGetLastError());
			return FALSE;
		}
	}

	// ����I�� �o�b�t�@���N���A
	ZeroMemory(m_szErrMsg, sizeof(m_szErrMsg));
	return TRUE;

}

/******************************************************************************/
/** 
	TCP�\�P�b�g�̐ڑ��v�����󂯓���܂��B
	
	@param[in] oTcpSock cTcpSock�̃I�u�W�F�N�g�|�C���^
	@return ���������Ƃ���TRUE��Ԃ��B
	���s�����Ƃ���FALSE��Ԃ��B
*/
BOOL cTcpServerSock::Accept(cTcpSock *oTcpSock){

	// �ϐ��錾
	int iSockAddrLen	= sizeof(oTcpSock->m_Addr);
	int iErrNo			= 0;

	oTcpSock->m_Socket = accept(m_ServerSocket, (LPSOCKADDR)&oTcpSock->m_Addr, &iSockAddrLen);

	if(oTcpSock->m_Socket == INVALID_SOCKET){
		if((iErrNo = WSAGetLastError()) != WSAEWOULDBLOCK){
			wsprintf(m_szErrMsg, "�V�K�ڑ��̎󂯓���Ɏ��s���܂����B\n"
								 "ErrCode:%d",
										iErrNo);
			return FALSE;
		}
	}

	// �ڑ��t���O��ڑ����ɐݒ�
	oTcpSock->m_bIsConnect = TRUE;

	return TRUE;

}

/******************************************************************************/
/** 
	�҂��󂯂��I�����܂��B
	
	@return ���������Ƃ���TRUE��Ԃ��B
	���s�����Ƃ���FALSE��Ԃ��B
*/
BOOL cTcpServerSock::CloseListen(){

	// �\�P�b�g�����
	if(closesocket(m_ServerSocket) == SOCKET_ERROR){
		wsprintf(m_szErrMsg, "�\�P�b�g�̉���Ɏ��s���܂����B\n"
							 "ErrCode:%d",
									WSAGetLastError());
		return FALSE;
	}

	// �ڑ��t���O�𖢐ڑ��ɐݒ�
	m_bIsListening = FALSE;

	return TRUE;

}

/******************************************************************************/
/** 
	�G���[���b�Z�[�W�̕\�������܂��B
*/
void cTcpServerSock::ErrMsgBox(){

	if(strlen(m_szErrMsg) > 0){
		MessageBox(NULL, m_szErrMsg, "Error[cTcpServerSock]", MB_OK | MB_ICONERROR);
	}

	return;

}
