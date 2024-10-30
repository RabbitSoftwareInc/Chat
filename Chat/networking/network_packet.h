#pragma once
#include "message.h"

#include "../client_state.h"
#include "../util/util.h"

struct NetworkPacket_Data_t
{
	void Assemble(std::wstring filepath, std::wstring filename);
	void Disassemble();

	bool data_packet = false;

	char file_name[256];
};

struct NetworkPacket_Message_t
{
	void Assemble(Message_t& message);
	Message_t Disassemble();

	//bool data_packet = false;

	char user_name[128];
	char user_message[1152];

	char send_time[32];
};