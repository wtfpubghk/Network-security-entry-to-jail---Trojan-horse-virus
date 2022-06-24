#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS // 避免報錯
#include <WinSock2.h>  //網絡庫
#pragma comment(lib, "ws2_32.lib")  //網絡庫
#include <Windows.h>
#include <iostream>
#include <thread>


namespace HalfPeopleNet
{

	namespace Server {
		struct HTCP_Data {
			SOCKET sockSrv;
			SOCKET sockConn;
			SOCKADDR_IN addrClieni;
			std::thread* Body_t;
			std::thread* MS_t;
		};

		bool init(int Port, HTCP_Data* Data)
		{
			WSADATA wsaData;

			if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
			{
				std::cout << "\n WSAStartup";
				return 0;
			}
			std::cout << "\n Now WSAStartup";
			Data->sockSrv = socket(AF_INET, SOCK_STREAM, 0);

			SOCKADDR_IN addrSrv;
			addrSrv.sin_family = AF_INET;
			addrSrv.sin_port = htons(Port); //1024以上的端口号
			addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

			int retVal = bind(Data->sockSrv, (LPSOCKADDR)&addrSrv, sizeof(SOCKADDR_IN));
			if (retVal == SOCKET_ERROR)
			{
				std::cout << "\n bind" << WSAGetLastError();
				return 0;
			}

			if (listen(Data->sockSrv, 10) == SOCKET_ERROR)
			{
				std::cout << "\n listen";
				return 0;
			}
			return 1;
		}

		void WConnect(void (*MS)(), HTCP_Data* Data)
		{
			int len = sizeof(SOCKADDR);
			while (true)
			{
				//Data->sockConn = new SOCKET;
				std::cout << "\n sockSrv : " << Data->sockSrv;
				Data->sockConn = accept(Data->sockSrv, (SOCKADDR*)&Data->addrClieni, &len);
				//std::cout << "\nWConnect2";
				//Data->sockConn = accept(Data->sockSrv, 0, 0);
				if (Data->sockConn == SOCKET_ERROR)
				{
					std::cout << "Create accept Error";
				}
				else
				{
					Data->MS_t = new std::thread(MS);
				}
			}
		}


		void WConnectT(void (*MS)(), HTCP_Data* Data)
		{
			Data->Body_t = new std::thread(WConnect, MS, Data);
		}

		bool Send(HTCP_Data* Data, const char* SendText)
		{
			int Error = send(Data->sockConn, SendText, strlen(SendText) + 1, 0);
			return (Error != SOCKET_ERROR);
		}


		bool Receive(HTCP_Data* Data,char* Text)
		{
			return (0 < recv(Data->sockConn, Text, 1024, 0));
		}

		void Disconnect(HTCP_Data* Data, bool CloseALL)
		{
			std::cout << "\n Remove ";
			if (CloseALL)
			{
				closesocket(Data->sockConn);
				try
				{
					Data->Body_t->detach();
					Data->MS_t->detach();
				}
				catch (const std::exception&)
				{
					std::cout << "\n Remove T Error!!!";
				}
				closesocket(Data->sockSrv);
				WSACleanup();

			}
			else
			{
				closesocket(Data->sockConn);
				try
				{
					//Data->Body_t->detach();
					Data->MS_t->detach();
				}
				catch (const std::exception&)
				{
					std::cout << "\n Remove T Error!!!";
				}

			}
		}
	}

	namespace Client
	{

		struct HTCP_Data {
			SOCKADDR_IN addrSrv;
			SOCKET sockClient;
		};

		bool initialization(int Port, const char* IP_Addr, HTCP_Data * Data)
		{
			WSADATA wsaData;
			//char buff[1024];
			//memset(buff, 0, sizeof(buff));

			if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
			{
				std::cout << "\n WSAStartup";
				return 0;
			}



			Data->addrSrv.sin_family = AF_INET;
			Data->addrSrv.sin_port = htons(Port);
			Data->addrSrv.sin_addr.S_un.S_addr = inet_addr(IP_Addr);

			Data->sockClient = socket(AF_INET, SOCK_STREAM, 0);
			if (SOCKET_ERROR == Data->sockClient) {
				//printf("\nSocket() error:%d", WSAGetLastError());
				std::cout << "\n socket";
				return 0 ;
			}
			if (connect(Data->sockClient, (struct  sockaddr*)&Data->addrSrv, sizeof(Data->addrSrv)) == INVALID_SOCKET) {
				std::cout << "\n connect  :  " << WSAGetLastError();
				return 0 ;
			}
			return 1;
		}

		void Send(const char* SendText, HTCP_Data* Data)
		{
			send(Data->sockClient, SendText, strlen(SendText) + 1, 0);
		}
		void Receive(char* ReceiveText,HTCP_Data* Data)
		{
			recv(Data->sockClient, ReceiveText, 1024, 0);
		}

		void Disconnect(bool CloseAll, HTCP_Data* Data)
		{
			closesocket(Data->sockClient);
			if (CloseAll)
				WSACleanup();
		}
	}
}
