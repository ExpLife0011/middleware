// cl_udp.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.
//

#include "pch.h" //cabeçalho precompilado
#include <iostream> //trabalha com entrada e saída
#include <winsock2.h> //socks no windows
#include <Ws2tcpip.h> //tcp
#include <ctime>

#define PACOTES 1000000

#define DEFAULT_PORT_TX "27016"
#define DEFAULT_IP "127.0.0.1"
#define DEFAULT_BUFLEN 512

#pragma comment(lib,"ws2_32.lib") //biblioteca Winsock

int main(int argc, char *argv[])
{
	setlocale(LC_ALL, "portuguese-brazilian");

	std::cout << "\nInicializando Winsock...";
	WSADATA wsa; //recebe detalhe do socket
	int iResult; //recebe retornos de funções int
	iResult = WSAStartup(MAKEWORD(2, 2), //versão do socket
		&wsa);
	if (iResult != 0) {
		std::cout << "Falha ao inicializar, erro: " << WSAGetLastError() << ".";
		return 1;
	}
	else {
		std::cout << "Inicializado!";
	}

	std::cout << "\nAtribuindo endereco...";
	struct addrinfo *result = NULL, hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	iResult = getaddrinfo(DEFAULT_IP, DEFAULT_PORT_TX, &hints, &result);
	if (iResult) {
		std::cout << "Erro de atribuicao.";
		WSACleanup();
		return 2;
	}
	else {
		std::cout << "Atribuicao correta!";
	}

	std::cout << "\nCriando WinSocket...";
	SOCKET sock = INVALID_SOCKET;
	sock = socket(result->ai_family, //ipv4
		result->ai_socktype, //udp
		result->ai_protocol //protocolo
	);
	if (sock == INVALID_SOCKET) {
		std::cout << "Nao foi possível criar socket: " << WSAGetLastError() << ".";
		WSACleanup();
		return 3;
	}
	else {
		std::cout << "...Winsocket criado!";
	}
	/**
	std::cout << "\nConectando ao servidor...";
	iResult = bind(sock, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		std::cout << "Incapaz de se conectar ao servidor: " << WSAGetLastError() << ".";
		freeaddrinfo(result);
		closesocket(sock);
		sock = INVALID_SOCKET;
		WSACleanup();
		return 4;
	}
	else {
		std::cout << "Conectado!";
	}
	/**
	std::cout << "\nConectando ao servidor...";
	iResult = connect(sock, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		std::cout << "Incapaz de se conectar ao servidor: " << WSAGetLastError() << ".";
		freeaddrinfo(result);
		closesocket(sock);
		sock = INVALID_SOCKET;
		WSACleanup();
		return 4;
	}
	else {
		std::cout << "Conectado!";
	}
	/**/
	std::cout << "\nEnviando Mensagem...";
	int recvbuflen = DEFAULT_BUFLEN;
	const char *sendbuf = "Mais uma escolha mau feita! Ou é mal feita?";
	char recvbuf[DEFAULT_BUFLEN];
	int slen = result->ai_addrlen;
	int i = PACOTES;
	int start_s = clock();
	do {
		/**
		memset(recvbuf, '\0', recvbuflen);
		iResult = sendto(sock, sendbuf, strlen(sendbuf), FALSE, result->ai_addr, result->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			std::cout << "Falha no envio de mensagem: " << WSAGetLastError() << ".";
			closesocket(sock);
			WSACleanup();
			return 5;
		}
		else {
			recvbuf[iResult] = '\0';
			std::cout << "Mensagem enviada com sucesso! Bytes: " << iResult << ".Mensagem: \n\t\"" << recvbuf << "\"\n";
		}

		//std::cout << "\nRecebendo Mensagem...";
		memset(recvbuf, '\0', recvbuflen);
		iResult = recvfrom(sock, recvbuf, recvbuflen, FALSE, result->ai_addr, &slen);
		recvbuf[iResult] = '\0';
		if (iResult == SOCKET_ERROR) {
			std::cout << "Falha no recebimento de mensagem: " << WSAGetLastError() << ".";
			closesocket(sock);
			WSACleanup();
			return 5;
		}
		else {
			std::cout << "Mensagem recebida com sucesso com sucesso! Bytes: " << iResult << ".Mensagem: \n\t\"" << recvbuf << "\"\n";
		}
		/**/
		iResult = sendto(sock, sendbuf, strlen(sendbuf), FALSE, result->ai_addr, result->ai_addrlen);
		if (iResult > 0) {
			//recvbuf[iResult] = '\0';
			//std::cout << "Bytes Enviados: " << iResult << "! Mensagem:\n\t\"" << sendbuf << "\"";
			/**
			std::cout << "\n\tRecebendo mensagem do servidor...";
			iResult = recv(sock, recvbuf, recvbuflen, FALSE);
			if (iResult == SOCKET_ERROR) {
				std::cout << "Falha ao enviar mensagem: " << WSAGetLastError() << ".";
				closesocket(sock);
				WSACleanup();
				return 1;
			}
			else {
				std::cout << "Bytes retornados: " << iResult << "\n";
			}
			/**/
			i--;
		}
		else if (iResult == 0)
			std::cout << "Conexao fechada.";
		else {
			std::cout << "Falha ao receber mensagem: " << WSAGetLastError() << ".";
			closesocket(sock);
			WSACleanup();
			return 1;
		}
	} while (i > 0);
	int stop_s = clock();
	std::cout << "tempo: " << (stop_s - start_s) / double(CLOCKS_PER_SEC) * 1000 << ". Pacotes faltantes: " << i;

	std::cout << "\nNenhuma outra mensagem sera enviada...Fechando conexao...";
	iResult = shutdown(sock, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		std::cout << "Falha em fechar a conexao: " << WSAGetLastError() << ".";
		closesocket(sock);
		WSACleanup();
		return 6;
	}
	else {
		std::cout << "Conexao fechada!";
	}

	std::cout << "\nNenhuma outra mensagem sera enviada...Fechando socket...";
	iResult = closesocket(sock);
	if (iResult != 0) {
		std::cout << "Falha no fechamento do socket:" << iResult << ".";
		return 7;
	}
	else {
		std::cout << "Socket fechado!";
		WSACleanup();
	}
	std::cout << "\n";
	return 0;
}

// Executar programa: Ctrl + F5 ou Menu Depurar > Iniciar Sem Depuração
// Depurar programa: F5 ou menu Depurar > Iniciar Depuração

// Dicas para Começar: 
//   1. Use a janela do Gerenciador de Soluções para adicionar/gerenciar arquivos
//   2. Use a janela do Team Explorer para conectar-se ao controle do código-fonte
//   3. Use a janela de Saída para ver mensagens de saída do build e outras mensagens
//   4. Use a janela Lista de Erros para exibir erros
//   5. Ir Para o Projeto > Adicionar Novo Item para criar novos arquivos de código, ou Projeto > Adicionar Item Existente para adicionar arquivos de código existentes ao projeto
//   6. No futuro, para abrir este projeto novamente, vá para Arquivo > Abrir > Projeto e selecione o arquivo. sln
