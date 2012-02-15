#include "cTcpSock.h"

/**	@file
	@brief	TCP�\�P�b�g
	
	@author	TATSUYA
	@date	2007/03/01
*/

/******************************************************************************/
/** @brief	�R���X�g���N�^
	
	�N���X�̏��������s���܂��B
*/
cTcpSock::cTcpSock(){

	// �ڑ��t���O�𖢐ڑ��ɐݒ�
	m_bIsConnect = FALSE;

}

/******************************************************************************/
/** @brief	�f�X�g���N�^
	
	�N���X�̉�����s���܂��B
*/
cTcpSock::~cTcpSock(){

	// �ڑ����̏ꍇ�͐ؒf����
	if(IsConnect()){
		Disconnect();
	}

}

/******************************************************************************/
/** 
	�ڑ���z�X�g��IP�A�h���X���擾���܂��B
	
	@return �ڑ���z�X�g��IP�A�h���X
*/
LPCSTR cTcpSock::GetPeerIpAddress(){

	return inet_ntoa(m_Addr.sin_addr);

}

/******************************************************************************/
/** 
	�ڑ���z�X�g�̃|�[�g�ԍ����擾���܂��B
	
	@return �ڑ���z�X�g�̃|�[�g�ԍ�
*/
USHORT cTcpSock::GetPeerPort(){

	return ntohs(m_Addr.sin_port);

}

/******************************************************************************/
/** 
	�\�P�b�g�̃R�l�N�V�������m�����Ă��邩�m�F���܂��B
	
	@return �R�l�N�V�������m�����Ă���ꍇ�ɂ�TRUE��Ԃ��B
	�R�l�N�V�������m�����Ă��Ȃ��ꍇ�ɂ�FALSE��Ԃ��B
*/
BOOL cTcpSock::IsConnect(){

	return m_bIsConnect;

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
BOOL cTcpSock::Connect(LPCSTR szHost, USHORT nPort, HWND hWnd, USHORT nEventNo){

	// �ϐ��錾
	const int	iYes	= 1;
	HOSTENT		*host	= NULL;

	// �\���̂ɐڑ����o�^����
	m_Addr.sin_family		= AF_INET;
	m_Addr.sin_port			= htons(nPort);
	m_Addr.sin_addr.s_addr	= inet_addr(szHost);

	if(m_Addr.sin_addr.S_un.S_addr == 0xffffffff){
		host = gethostbyname(szHost);
		if(host == NULL){
			wsprintf(m_szErrMsg, "�z�X�g������IP�A�h���X���擾�ł��܂��񂵂��B\n"
								 "ErrCode:%d",
										WSAGetLastError());
			return FALSE;
		}
		m_Addr.sin_addr.S_un.S_addr = *(PUINT)host->h_addr_list[0];
	}

	// �\�P�b�g�̐���
	m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// �G���[�`�F�b�N
	if(m_Socket == INVALID_SOCKET){
		wsprintf(m_szErrMsg, "�\�P�b�g�̍쐬�Ɏ��s���܂����B\n"
							 "ErrCode:%d",
									WSAGetLastError());
		return FALSE;
	}

	// �񓯊��ݒ�
	if(!SetAsync(hWnd, nEventNo)){
		return FALSE;
	}

	// TCP_NODELAY
	if(setsockopt(m_Socket, IPPROTO_TCP, TCP_NODELAY, (LPCSTR)&iYes, sizeof(iYes)) == SOCKET_ERROR){
		wsprintf(m_szErrMsg, "�\�P�b�g�I�v�V����(TCP_NODELAY)�̐ݒ�Ɏ��s���܂����B\n"
							 "ErrCode:%d",
									WSAGetLastError());
		return FALSE;
	}

	// �G���[�`�F�b�N
	if(connect(m_Socket, (LPSOCKADDR)&m_Addr,	sizeof(SOCKADDR)) == SOCKET_ERROR){
		if(WSAGetLastError() != WSAEWOULDBLOCK){
			wsprintf(m_szErrMsg, "�ڑ����s�Ɏ��s���܂����B\n"
								 "ErrCode:%d",
										WSAGetLastError());
			return FALSE;
		}
	}

	// �ڑ��t���O��ڑ����ɐݒ�
	m_bIsConnect = TRUE;

	// ����I�� �o�b�t�@���N���A
	ZeroMemory(m_szErrMsg, sizeof(m_szErrMsg));
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
BOOL cTcpSock::SetAsync(HWND hWnd, USHORT nEventNo){

	// �񓯊�������ݒ�
	if(hWnd != NULL && nEventNo != 0){
		if(WSAAsyncSelect(m_Socket, hWnd, nEventNo, FD_READ | FD_CLOSE) == SOCKET_ERROR){
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
	�m���ς݂̃R�l�N�V������ؒf���܂��B
	
	@return ���������Ƃ���TRUE��Ԃ��B
	���s�����Ƃ���FALSE��Ԃ��B
*/
BOOL cTcpSock::Disconnect(){

	// �ڑ����łȂ���Ύ��s���Ȃ�
	if(!IsConnect()){
		wsprintf(m_szErrMsg, "�\�P�b�g���ڑ����ł͂Ȃ����ߐؒf�ł��܂���B");
		return FALSE;
	}

	// �\�P�b�g�̃V���b�g�_�E��
	if(shutdown(m_Socket, 2) == SOCKET_ERROR){
		wsprintf(m_szErrMsg, "�\�P�b�g�̃V���b�g�_�E���Ɏ��s���܂����B\n"
							 "ErrCode:%d",
									WSAGetLastError());
		return FALSE;
	}

	// �\�P�b�g�����
	if(closesocket(m_Socket) == SOCKET_ERROR){
		wsprintf(m_szErrMsg, "�\�P�b�g�̉���Ɏ��s���܂����B\n"
							 "ErrCode:%d",
									WSAGetLastError());
		return FALSE;
	}

	// �ڑ��t���O�𖢐ڑ��ɐݒ�
	m_bIsConnect = FALSE;

	// ����I�� �o�b�t�@���N���A
	ZeroMemory(m_szErrMsg, sizeof(m_szErrMsg));
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
BOOL cTcpSock::SendData(LPVOID lpData, DWORD dwBytes){

	// �ϐ��錾
	int		iSendBytes	= 0;
	LPSTR	lpBuff		= (LPSTR)lpData;

	// �S�Ẵf�[�^�𑗐M����܂Ń��[�v�𑱂���
	do{
		iSendBytes = send(m_Socket, lpBuff, dwBytes, 0);
		if(iSendBytes == SOCKET_ERROR){
			wsprintf(m_szErrMsg, "�f�[�^�̑��M�Ɏ��s���܂����B\n"
								 "ErrCode:%d",
										WSAGetLastError());
			return FALSE;
		}
		// �|�C���^�̈ړ�
		lpBuff	+= iSendBytes;
		dwBytes	-= iSendBytes;
	}while(dwBytes > 0);

	// ����I�� �o�b�t�@���N���A
	ZeroMemory(m_szErrMsg, sizeof(m_szErrMsg));
	return TRUE;

}

/******************************************************************************/
/** 
	������̑��M���s���܂��B
	
	@param[in] szStr	���M���镶����
	@return ���������Ƃ���TRUE��Ԃ��B
	���s�����Ƃ���FALSE��Ԃ��B
*/
BOOL cTcpSock::SendStr(LPCSTR szStr){

	return SendData((LPVOID)szStr, (DWORD)strlen(szStr) + 1);

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
BOOL cTcpSock::RecvData(LPVOID lpBuffer, DWORD dwBytes, DWORD *pdwRecvLen){

	// �ϐ��錾
	DWORD	dwRecvBytes	= 0;
	LPSTR	lpBuff		= (LPSTR)lpBuffer;
	int		iByteRecv	= 0;

	// �w�肵���o�C�g���A��M����܂Ń��[�v�𑱂���
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

	// ��M�����o�C�g����0�o�C�g�ł������ꍇ
	if(dwRecvBytes == 0){
		wsprintf(m_szErrMsg, "�f�[�^�̎�M�Ɏ��s���܂����B\n"
							 "ErrCode:%d",
									WSAGetLastError());
		return FALSE;
	}

	if(pdwRecvLen != NULL){
		*pdwRecvLen = dwRecvBytes;
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
BOOL cTcpSock::RecvDataOnce(LPVOID lpBuffer, DWORD dwBytes, DWORD* pdwRecvLen){

	// �ϐ��錾
	int iByteRecv = 0;
	
	iByteRecv = recv(m_Socket, (LPSTR)lpBuffer, dwBytes, 0);
	
	// ��M�����o�C�g����0�o�C�g�ł������ꍇ
	if(iByteRecv == 0){
		wsprintf(m_szErrMsg, "�f�[�^�̎�M�Ɏ��s���܂����B\n"
							 "ErrCode:%d",
									WSAGetLastError());
		return FALSE;
	}
	
	if(pdwRecvLen != NULL){
		*pdwRecvLen = (DWORD)iByteRecv;
	}

	// ����I�� �o�b�t�@���N���A
	ZeroMemory(m_szErrMsg, sizeof(m_szErrMsg));
	return TRUE;

}

/******************************************************************************/
/** 
	�����̃\�P�b�g�n���h���Ɠ���̃\�P�b�g�����肷��B

	@param[in] sock		����Ώۂ̃\�P�b�g�n���h��
	@return ��v�����Ƃ���TRUE��Ԃ��B
	��v���Ȃ������Ƃ���FALSE��Ԃ��B
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
	�G���[���b�Z�[�W�̕\�������܂��B
*/
void cTcpSock::ErrMsgBox(){

	if(strlen(m_szErrMsg) > 0){
		MessageBox(NULL, m_szErrMsg, "Error[cTcpSock]", MB_OK | MB_ICONERROR);
	}

	return;

}
