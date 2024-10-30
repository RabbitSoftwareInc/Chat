#include "network_packet.h"

#include "../security/security.h"

void NetworkPacket_Data_t::Assemble(std::wstring filepath, std::wstring filename)
{
	std::string encrypted_file_name = g_security->encode_message(filename, g_security->get_encryption_key("KEYBEGIN_DATA_FILETRANSFER", "KEYEND_DATA_FILETRANSFER"));

	for (int i = 0; i < 256; i++)
	{
		file_name[i] = '\0';

		if (i < encrypted_file_name.size())
			file_name[i] = encrypted_file_name[i];
	}

	data_packet = true;

	g_client_state->data_transfer = true;
	g_client_state->data_transfer_receive = false;

	g_client_state->data_transfer_file = filepath;
}

void NetworkPacket_Data_t::Disassemble()
{
	std::wstring decrypted_file_name = g_security->decode_message(file_name, g_security->get_encryption_key("KEYBEGIN_DATA_FILETRANSFER", "KEYEND_DATA_FILETRANSFER"));

	g_client_state->data_transfer = true;
	g_client_state->data_transfer_receive = true;

	g_client_state->data_transfer_file = decrypted_file_name;
}

void NetworkPacket_Message_t::Assemble(Message_t& message)
{
	std::string encrypted_name = g_security->encode_message(message.user_name, g_security->get_encryption_key("KEYBEGIN_NAME", "KEYEND_NAME"));
	std::string encrypted_message = g_security->encode_message(message.user_message, g_security->get_encryption_key("KEYBEGIN_MSG", "KEYEND_MSG"));
	std::string encrypted_time = g_security->encode_message(message.send_time, g_security->get_encryption_key("KEYBEGIN_TIME", "KEYEND_TIME"));

	for (int i = 0; i < 128; i++)
	{
		user_name[i] = '\0';

		if (i < encrypted_name.size())
			user_name[i] = encrypted_name[i];
	}

	for (int i = 0; i < 1152; i++)
	{
		user_message[i] = '\0';

		if (i < encrypted_message.size())
			user_message[i] = encrypted_message[i];
	}

	for (int i = 0; i < 32; i++)
	{
		send_time[i] = '\0';

		if (i < encrypted_time.size())
			send_time[i] = encrypted_time[i];
	}

	//data_packet = false;
}

Message_t NetworkPacket_Message_t::Disassemble()
{
	std::wstring decrypted_name = g_security->decode_message(user_name, g_security->get_encryption_key("KEYBEGIN_NAME", "KEYEND_NAME"));
	std::wstring decrypted_message = g_security->decode_message(user_message, g_security->get_encryption_key("KEYBEGIN_MSG", "KEYEND_MSG"));
	std::wstring decrypted_time = g_security->decode_message(send_time, g_security->get_encryption_key("KEYBEGIN_TIME", "KEYEND_TIME"));

	return Message_t(decrypted_name, decrypted_message, decrypted_time);
}