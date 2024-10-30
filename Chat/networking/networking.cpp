#include "networking.h"

#include "../client_state.h"

#include "../security/security.h"
#include "../util/filter.h"

#include "../notifications/sounds/new_message.h"

#include "network_packet.h"

c_networking* g_networking = new c_networking;

DWORD WINAPI server_receive(LPVOID lpParam)
{
	SOCKET client = *(SOCKET*)lpParam;

	while (!g_networking->should_close_receive_thread)
	{
		NetworkPacket_Message_t message_packet;

		int received_size = recv(client, (char*)&message_packet, sizeof(NetworkPacket_Message_t), 0);
		if (received_size == SOCKET_ERROR)
		{
			std::wcout << L"ReceivePacket: Error " << WSAGetLastError() << std::endl;
			g_networking->should_close_receive_thread = true;

			return -1;
		}

		g_networking->receive_packet(&message_packet);
		Sleep(1);
	}
	return 1;
}

DWORD WINAPI server_send(LPVOID lpParam)
{
	SOCKET client = *(SOCKET*)lpParam;

	while (!g_networking->should_close_send_thread)
	{
		if (g_client_state->should_send_message)
		{
			if (g_client_state->message_text != L"" && g_client_state->message_text != L" ")
				g_networking->send_packet(&client);

			g_client_state->should_send_message = false;
			g_client_state->message_text = L"";
		}
		Sleep(1);
	}
	return 1;
}

bool c_networking::is_connected()
{
	return connected;
}

bool c_networking::is_server()
{
	return server;
}

void c_networking::set_is_server(bool is_server)
{
	server = is_server;
}

void c_networking::set_ip_address(std::wstring ip)
{
	ip_address = ip;
}

void c_networking::set_port(int con_port)
{
	port = con_port;
}

void c_networking::initialize()
{
	if (server)
	{
		WSADATA WSAData;
		SOCKET server, client;
		SOCKADDR_IN serverAddr, clientAddr;

		WSAStartup(MAKEWORD(2, 0), &WSAData);

		server = socket(AF_INET, SOCK_STREAM, 0);
		if (server == INVALID_SOCKET)
		{
			std::wcout << L"socket: Error " << WSAGetLastError() << std::endl;
			return;
		}

		serverAddr.sin_addr.s_addr = INADDR_ANY;
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(port);

		if (bind(server, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		{
			std::cout << L"bind: Error " << WSAGetLastError() << std::endl;
			return;
		}

		if (listen(server, 0) == SOCKET_ERROR)
		{
			std::wcout << L"listen: Error " << WSAGetLastError() << std::endl;
			return;
		}

		int clientAddrSize = sizeof(clientAddr);

		if ((client = accept(server, (SOCKADDR*)&clientAddr, &clientAddrSize)) != INVALID_SOCKET)
		{
			DWORD tid;
			HANDLE t1 = CreateThread(NULL, 0, server_receive, &client, 0, &tid);

			if (t1 == NULL)
				std::wcout << L"CreateThread: Error " << WSAGetLastError() << std::endl;

			HANDLE t2 = CreateThread(NULL, 0, server_send, &client, 0, &tid);

			if (t2 == NULL)
				std::wcout << L"CreateThread: Error " << WSAGetLastError() << std::endl;

			connected = true;

			WaitForSingleObject(t1, INFINITE);
			WaitForSingleObject(t2, INFINITE);

			closesocket(client);

			if (closesocket(server) == SOCKET_ERROR)
			{
				std::wcout << "closesocket: Error " << WSAGetLastError() << std::endl;
				return;
			}

			WSACleanup();
		}
	}
	else
	{
		WSADATA WSAData;
		SOCKET server;
		SOCKADDR_IN addr;

		WSAStartup(MAKEWORD(2, 0), &WSAData);

		if ((server = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
		{
			std::wcout << L"socket: Error " << WSAGetLastError() << std::endl;
			return;
		}

		addr.sin_addr.s_addr = inet_addr(ws2s(ip_address).c_str());
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);

		if (connect(server, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR)
		{
			std::wcout << "connect: Error " << WSAGetLastError() << std::endl;
			return;
		}

		DWORD tid;
		HANDLE t1 = CreateThread(NULL, 0, server_receive, &server, 0, &tid);

		if (t1 == NULL)
			std::wcout << L"CreateThread: Error " << WSAGetLastError() << std::endl;

		HANDLE t2 = CreateThread(NULL, 0, server_send, &server, 0, &tid);

		if (t2 == NULL)
			std::wcout << L"CreateThread: Error " << WSAGetLastError() << std::endl;

		connected = true;

		WaitForSingleObject(t1, INFINITE);
		WaitForSingleObject(t2, INFINITE);

		closesocket(server);
		WSACleanup();
	}
}

void c_networking::send_packet(SOCKET* socket)
{
	SOCKET client = *socket;

	if (g_client_state->censorship && g_text_filter->filter_message_text(g_client_state->message_text) != g_client_state->message_text)
		MessageBoxW(GetConsoleWindow(), L"This message contains bad words. Please rewrite it.", L"Error", 0);
	else
	{
		Message_t message;
		message = Message_t(g_client_state->message_text);

		NetworkPacket_Message_t packet;
		packet.Assemble(message);

		if (send(client, (char*)&packet, sizeof(NetworkPacket_Message_t), 0) == SOCKET_ERROR)
		{
			std::wcout << L"SendPacket: Error " << WSAGetLastError() << std::endl;
			should_close_send_thread = true;

			return;
		}

		g_client_state->message_history.push_back(L"* [" + message.send_time + L"] " + message.user_name + L": " + message.user_message);
	}
}

void c_networking::receive_packet(NetworkPacket_Message_t* packet)
{
	Message_t message = packet->Disassemble();

	if (g_client_state->censorship)
		message.user_message = g_text_filter->filter_message_text(message.user_message);

	g_client_state->message_history.push_back(L"[" + message.send_time + L"] " + message.user_name + L": " + message.user_message);

	if (message.user_name != L"" && message.user_message != L"" && g_client_state->sound_on_new_message && GetForegroundWindow() != GetConsoleWindow())
		stream_music(new_message, sizeof(new_message), g_client_state->player_stream);
}