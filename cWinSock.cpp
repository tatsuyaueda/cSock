#include "cWinSock.h"

/**	@file
	@brief	Windows�\�P�b�g
	
	@author	TATSUYA
	@date	2007/02/28
*/

/******************************************************************************/
/**	@brief	�R���X�g���N�^

	�N���X�̏��������s���܂��B
*/
cWinSock::cWinSock(){

	// WinSock�̖�������
	m_bStartUp = FALSE;

}

/******************************************************************************/
/**	@brief	�f�X�g���N�^

	�N���X�̉�����s���܂��B
*/
cWinSock::~cWinSock(){

	// ���
	Release();

}

/******************************************************************************/
/** 
	Windows�\�P�b�g�̏����������܂��B
	
	@return ���������Ƃ���TRUE��Ԃ��B
	���s�����Ƃ���FALSE��Ԃ��B
*/
BOOL cWinSock::Startup(){

	if(m_bStartUp){
		if(!Release()){
			return FALSE;
		}
	}

	if(WSAStartup(MAKEWORD(2, 0), &m_wsadata) != 0){
		wsprintf(m_szErrMsg, "Windows Socket�̏������Ɏ��s���܂����B\n"
							 "ErrCode:%d",
									WSAGetLastError());
		return FALSE;
	}

	if(m_wsadata.wVersion != MAKEWORD(2, 0)){
		wsprintf(m_szErrMsg, "Windows Socket�̃o�[�W�������v�������o�[�W�����ƈقȂ�܂��B\n"
							 "ErrCode:%d",
									WSAGetLastError());
		return FALSE;
	}

	m_bStartUp = TRUE;

	// ����I��
	ZeroMemory(m_szErrMsg, sizeof(m_szErrMsg));
	return TRUE;

}

/******************************************************************************/
/** 
	Windows�\�P�b�g�̉�������܂��B
	
	@return ���������Ƃ���TRUE��Ԃ��B
	���s�����Ƃ���FALSE��Ԃ��B
*/
BOOL cWinSock::Release(){

	if(m_bStartUp){
		if(WSACleanup() != 0){
			wsprintf(m_szErrMsg, "Windows Socket�̉�����܂����B\n"
								 "ErrCode:%d",
									WSAGetLastError());
			return FALSE;
		}
	}

	m_bStartUp = FALSE;

	// ����I��
	ZeroMemory(m_szErrMsg, sizeof(m_szErrMsg));
	return TRUE;

}

/******************************************************************************/
/** 
	�G���[���b�Z�[�W�̕\�������܂��B
*/
void cWinSock::ErrMsgBox(){

	if(strlen(m_szErrMsg) > 0){
		MessageBox(NULL, m_szErrMsg, "Error[cWinSock]", MB_OK | MB_ICONERROR);
	}

	return;

}
