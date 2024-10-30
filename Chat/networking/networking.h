#pragma once
#include "../util/util.h"

struct NetworkPacket_Message_t;

class c_networking
{
	bool connected = false;
	bool server = false;

	std::wstring ip_address = L"127.0.0.1";
	int port = 5555;
public:
	bool should_close_receive_thread = false;
	bool should_close_send_thread = false;

	bool is_connected();
	bool is_server();

	void set_is_server(bool is_server);

	void set_ip_address(std::wstring ip);
	void set_port(int con_port);

	void initialize();

	void send_packet(SOCKET* socket);
	void receive_packet(NetworkPacket_Message_t* packet);
};

extern c_networking* g_networking;