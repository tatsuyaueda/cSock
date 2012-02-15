#include "cUdpSock.h"

/**	@file
	@brief	UDP�\�P�b�g
	
	@author	TATSUYA
	@date	2007/02/28
*/

/******************************************************************************/
/**	@brief	�R���X�g���N�^(�f�[�^���M��)

	�f�[�^�𑗐M���鑤�Ƃ��ăN���X�̏��������s���܂��B
	@param[in] szHost	�ڑ���z�X�g�̃z�X�g���܂���IP�A�h���X
	@param[in] nPort	�ڑ���z�X�g�̃|�[�g�ԍ�
*/
cUdpSock::cUdpSock(LPCSTR szHost, USHORT nPort){

	// �ϐ��錾
	const int	iFlagBroadcast	= 1;
	HOSTENT		*host			= NULL;

	// �\�P�b�g�̏�����
	if(!_InitSock()){
		return;
	}

	// �\���̂ɐڑ����o�^����
	m_Addr.sin_family			= AF_INET;
	m_Addr.sin_port				= htons(nPort);
	m_Addr.sin_addr.S_un.S_addr	= inet_addr(szHost);

	if(m_Addr.sin_addr.S_un.S_addr == 0xFFFFFFFF){
		host = gethostbyname(szHost);
		if(host == NULL){
			wsprintf(m_szErrMsg, "�z�X�g������IP�A�h���X���擾�ł��܂��񂵂��B\n"
								 "ErrCode:%d",
									WSAGetLastError());
			return;
		}
		m_Addr.sin_addr.S_un.S_addr = *(unsigned int *)host->h_addr_list[0];
	}

	// �u���[�h�L���X�g�̏ꍇ
	if(m_Addr.sin_addr.S_un.S_addr == 0xFFFFFFFF){
		if(setsockopt(m_Socket, SOL_SOCKET, SO_BROADCAST, (char *)&iFlagBroadcast, sizeof(iFlagBroadcast)) == SOCKET_ERROR){
			wsprintf(m_szErrMsg, "�\�P�b�g�I�v�V����(SO_BROADCAST)�̐ݒ�Ɏ��s���܂����B\n"
								 "ErrCode:%d",
									WSAGetLastError());
			return;
		}
	}

	return;

}

/******************************************************************************/
/**	@brief	�R���X�g���N�^(�f�[�^��M��)

	�f�[�^����M���鑤�Ƃ��ăN���X�̏��������s���܂��B
	@param[in] nPort	�҂��󂯂��s���|�[�g�ԍ�
	@param[in] hWnd		�񓯊����[�h�ŗ��p����ꍇ�́A�ʒm��̃E�B���h�E�n���h��<br>
						�������[�h�ŗ��p����ꍇ��NULL
	@param[in] nEventNo	�񓯊����[�h�ŗ��p����ꍇ�́A�ʒm���郁�b�Z�[�WID<br>
						�������[�h�ŗ��p����ꍇ��NULL
*/
cUdpSock::cUdpSock(USHORT nPort, HWND hWnd, USHORT nEventNo){

	// �\�P�b�g�̏�����
	if(!_InitSock()){
		return;
	}

	// �\���̂ɐڑ����o�^����
	m_Addr.sin_family		= AF_INET;
	m_Addr.sin_port			= htons(nPort);
	m_Addr.sin_addr.s_addr	= INADDR_ANY;

	// �G���[�`�F�b�N
	if(bind(m_Socket, (struct sockaddr *)&m_Addr, sizeof(m_Addr)) == SOCKET_ERROR){
		wsprintf(m_szErrMsg, "�\�P�b�g�̃|�[�g�ԍ��Ƃ̊֘A�t���Ɏ��s���܂����B\n"
							 "ErrCode:%d",
								WSAGetLastError());
		return;
	}

	// �񓯊�������ݒ�
	if(hWnd != NULL && nEventNo != 0){
		if(WSAAsyncSelect(m_Socket, hWnd, nEventNo, FD_READ) == SOCKET_ERROR){
			wsprintf(m_szErrMsg, "�񓯊������̐ݒ�Ɏ��s���܂����B\n"
								 "ErrCode:%d",
									WSAGetLastError());
			return;
		}
	}

	return;

}

/******************************************************************************/
/**	@brief	�f�X�g���N�^

	�N���X�̉�����s���܂��B
*/
cUdpSock::~cUdpSock(){

	// �\�P�b�g�̃V���b�g�_�E��
	if(shutdown(m_Socket, 2) == SOCKET_ERROR){
		wsprintf(m_szErrMsg, "�\�P�b�g�̃V���b�g�_�E���Ɏ��s���܂����B\n"
							 "ErrCode:%d",
								WSAGetLastError());
		return;
	}

	// �\�P�b�g�����
	if(closesocket(m_Socket) == SOCKET_ERROR){
		wsprintf(m_szErrMsg, "�\�P�b�g�̉���Ɏ��s���܂����B\n"
							 "ErrCode:%d",
								WSAGetLastError());
		return;
	}

	return;

}

/******************************************************************************/
/** 
	�ڑ���z�X�g��IP�A�h���X���擾���܂��B
	
	@return �ڑ���z�X�g��IP�A�h���X
*/
LPCSTR cUdpSock::GetPeerIpAddress(){

	return inet_ntoa(m_AddrPeer.sin_addr);

}

/******************************************************************************/
/** 
	�ڑ���z�X�g�̃|�[�g�ԍ����擾���܂��B
	
	@return �ڑ���z�X�g�̃|�[�g�ԍ�
*/
USHORT cUdpSock::GetPeerPort(){

	return ntohs(m_AddrPeer.sin_port);

}

/******************************************************************************/
/** 
	�\�P�b�g�̏��������s���܂��B
	
	@return ���������Ƃ���TRUE��Ԃ��B
	���s�����Ƃ���FALSE��Ԃ��B
*/
BOOL cUdpSock::_InitSock(){

	// �o�b�t�@�̃N���A
	ZeroMemory(&m_Addr, sizeof(SOCKADDR_IN));
	ZeroMemory(&m_AddrPeer, sizeof(SOCKADDR_IN));

	// �\�P�b�g�̐���
	m_Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	// �G���[�`�F�b�N
	if(m_Socket == INVALID_SOCKET){
		wsprintf(m_szErrMsg, "UDP�\�P�b�g�̐����Ɏ��s���܂����B\n"
							 "ErrCode:%d",
								WSAGetLastError());
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
BOOL cUdpSock::SendData(LPVOID lpData, DWORD dwBytes){

	// �ϐ��錾
	int		iSendBytes	= 0;
	LPCSTR	lpBuff		= (LPCSTR)lpData;

	// �S�Ẵf�[�^�𑗐M����܂Ń��[�v�𑱂���
	do{
		iSendBytes = sendto(m_Socket, lpBuff, dwBytes, 0, (LPSOCKADDR)&m_Addr, sizeof(m_Addr));
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

	return TRUE;

}

/******************************************************************************/
/** 
	������𑗐M���܂��B
	
	@param[in] szStr	���M���镶����
	@return ���������Ƃ���TRUE��Ԃ��B
	���s�����Ƃ���FALSE��Ԃ��B
*/
BOOL cUdpSock::SendStr(LPCSTR szStr){

	return this->SendData((LPVOID)szStr, (DWORD)strlen(szStr) + 1);

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
BOOL cUdpSock::RecvData(LPVOID lpBuffer, DWORD dwBytes, DWORD *pdwRecvLen){

	// �ϐ��錾
	DWORD	dwRecvBytes		= 0;
	LPSTR	lpBuff			= (LPSTR)lpBuffer;
	int		iByteRecv		= 0;
	int		iLenSockAddr	= sizeof(SOCKADDR_IN);

	// �w�肵���o�C�g���A��M����܂Ń��[�v�𑱂���
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
		wsprintf(m_szErrMsg, "�f�[�^�̎�M�Ɏ��s���܂����B\n"
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
	��M�ł��邾���f�[�^�̎�M���s���܂��B

	@param[in] lpBuffer		��M�����f�[�^���i�[����擪�|�C���^
	@param[in] dwBytes		�i�[��̃o�C�g��
	@param[out] pdwRecvLen	���ۂɎ�M�����o�C�g��
	@return ���������Ƃ���TRUE��Ԃ��B
	���s�����Ƃ���FALSE��Ԃ��B
*/
BOOL cUdpSock::RecvDataOnce(LPVOID lpBuffer, DWORD dwBytes, DWORD *pdwRecvLen){

	// �ϐ��錾
	int	iByteRecv = 0;
	int	iLenSockAddr = sizeof(SOCKADDR_IN);

	iByteRecv = recvfrom(m_Socket, (LPSTR)lpBuffer, dwBytes, 0, (LPSOCKADDR)&m_AddrPeer, &iLenSockAddr);

	if(iByteRecv == SOCKET_ERROR){
		wsprintf(m_szErrMsg, "�f�[�^�̎�M�Ɏ��s���܂����B\n"
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
	�����̃\�P�b�g�n���h���Ɠ���̃\�P�b�g�����肷��B

	@param[in] sock		����Ώۂ̃\�P�b�g�n���h��
	@return ��v�����Ƃ���TRUE��Ԃ��B
	��v���Ȃ������Ƃ���FALSE��Ԃ��B
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
	�G���[���b�Z�[�W�̕\�������܂��B
*/
void cUdpSock::ErrMsgBox(){

	if(strlen(m_szErrMsg) > 0){
		MessageBox(NULL, m_szErrMsg, "Error[cUdpSock]", MB_OK | MB_ICONERROR);
	}

	return;

}