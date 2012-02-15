#pragma once

/**	@file
	@brief	クラスライブラリを使用するためのインクルードファイル
	
	このファイルはクラスライブラリを使用する際に、インクルードする必要のあるファイルです。
	
	@author	TATSUYA
	@date	2007/02/28
*/

/** \mainpage
 *
 * このクラスライブラリはWindowsソケットを利用した、TCP/UDP/SSLソケットを実装したクラスライブラリです。<br>
 * SSLソケットの実装にはOpenSSLを利用しています。
 * \par
 * クラス一覧:
 * - cWinSock : Windowsソケットの初期化・解放のラッパークラスです。
 * - cTcpSock : TCPソケットを実装したクラスです。
 * - cTcpServerSock : TCP Serverソケットを実装したクラスです。
 * - cSslSock : SSLソケットを実装したクラスです。
 * - cSslServerSock : SSL Serverソケットを実装したクラスです。
 * - cUdpSock : UDPソケットを実装したクラスです。
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
