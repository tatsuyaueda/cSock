#pragma once

/**	@file
	@brief	�N���X���C�u�������g�p���邽�߂̃C���N���[�h�t�@�C��
	
	���̃t�@�C���̓N���X���C�u�������g�p����ۂɁA�C���N���[�h����K�v�̂���t�@�C���ł��B
	
	@author	TATSUYA
	@date	2007/02/28
*/

/** \mainpage
 *
 * ���̃N���X���C�u������Windows�\�P�b�g�𗘗p�����ATCP/UDP/SSL�\�P�b�g�����������N���X���C�u�����ł��B<br>
 * SSL�\�P�b�g�̎����ɂ�OpenSSL�𗘗p���Ă��܂��B
 * \par
 * �N���X�ꗗ:
 * - cWinSock : Windows�\�P�b�g�̏������E����̃��b�p�[�N���X�ł��B
 * - cTcpSock : TCP�\�P�b�g�����������N���X�ł��B
 * - cTcpServerSock : TCP Server�\�P�b�g�����������N���X�ł��B
 * - cSslSock : SSL�\�P�b�g�����������N���X�ł��B
 * - cSslServerSock : SSL Server�\�P�b�g�����������N���X�ł��B
 * - cUdpSock : UDP�\�P�b�g�����������N���X�ł��B
 */

#include "cWinSock.h"
#include "cTcpSock.h"
#include "cTcpServerSock.h"
#include "cUdpSock.h"
#include "cSslSock.h"
#include "cSslServerSock.h"

#pragma comment(lib, "ws2_32.lib")

#ifdef _DEBUG
	#pragma comment(lib, "libeay32MTd.lib")
	#pragma comment(lib, "ssleay32MTd.lib")
#else
	#pragma comment(lib, "libeay32MT.lib")
	#pragma comment(lib, "ssleay32MT.lib")
#endif
