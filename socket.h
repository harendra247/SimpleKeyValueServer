#pragma once

#include <string>

namespace macrometa {

	class ClientSocket {
		public:
			ClientSocket();
			ClientSocket(int fd);
			~ClientSocket();
			int Connect(const char* ip, unsigned short port);
			int Write(const void* buffer, int bufferSize);
			int WriteLine(const std::string& line);
			int Read(void* buffer, int bufferSize);
			int ReadLine(std::string& line);
			int Close();

		private:
			int fd;
	};

	class ServerSocket {
		public:
			ServerSocket();
			~ServerSocket();
			int Listen(const char* ip, unsigned short port);
			ClientSocket* Accept();
			int Close();

		private:
			int fd;
	};

} // namespace macrometa
