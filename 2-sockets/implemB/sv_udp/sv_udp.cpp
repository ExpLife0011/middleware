// sv_udp.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.
//

#include "pch.h" //cabeçalho precompilado
#include <iostream> //trabalha com entrada e saída
#include <winsock2.h> //socks no windows
#include <Ws2tcpip.h> //tcp
#include <ctime>

#define PACOTES 1000000

#define DEFAULT_PORT_RX "27016" //"80" //"27015"
#define DEFAULT_IP "127.0.0.1" //"172.217.28.142" //"74.125.235.20"
#define DEFAULT_BUFLEN 1024

#pragma comment(lib,"ws2_32.lib") //biblioteca Winsock

//#define _WINSOCK_DEPRECATED_NO_WARNINGS

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
	struct addrinfo *result_rx = NULL, *result_tx = NULL, hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	hints.ai_flags = AI_PASSIVE;
	iResult = getaddrinfo(NULL, DEFAULT_PORT_RX, &hints, &result_rx);
	if (iResult) {
		std::cout << "Erro de atribuicao.";
		WSACleanup();
		return 2;
	}
	else {
		std::cout << "Atribuicao correta...";
	}

	std::cout << "\nCriando WinSocket...";
	SOCKET ls_sock = INVALID_SOCKET;
	ls_sock = socket(result_rx->ai_family, //ipv4
		result_rx->ai_socktype, //tcp
		result_rx->ai_protocol //protocolo
	);
	if (ls_sock == INVALID_SOCKET) {
		std::cout << "Nao foi possível criar socket: " << WSAGetLastError() << ".";
		WSACleanup();
		return 3;
	}
	else {
		std::cout << "...Winsocket criado! " << result_rx->ai_socktype;
	}

	std::cout << "\nConectando ao cliente...";
	iResult = bind(ls_sock, result_rx->ai_addr, (int) result_rx->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		std::cout << "Incapaz de se conectar ao cliente: " << WSAGetLastError() << ".";
		freeaddrinfo(result_rx);
		closesocket(ls_sock);
		ls_sock = INVALID_SOCKET;
		WSACleanup();
		return 4;
	}
	else {
		std::cout << "Conectado!";
	}
	/**
	std::cout << "\nOuvindo a conexao...";
	iResult = listen(ls_sock, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		std::cout << "Falha em ouvir a conexao: " << WSAGetLastError();
		closesocket(ls_sock);
		WSACleanup();
		return 1;
	}
	else {
		std::cout << "Conexao em alto e bom som!";
	}
	/**
	std::cout << "\nAceitando conexoes...";
	SOCKET cl_sock = INVALID_SOCKET;
	cl_sock = accept(ls_sock, NULL, NULL);
	if (cl_sock == INVALID_SOCKET) {
		std::cout << "Falha em aceitar conexoes: " << WSAGetLastError() << ".";
		closesocket(ls_sock);
		WSACleanup();
		return 1;
	}
	else {
		std::cout << "Conexao aceita!";
		closesocket(ls_sock);
	}
	/**/
	std::cout << "\nRecebendo Mensagem...";
	//int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;
	const char *sendbuf = "Mal feita!";
	int slen = sizeof(result_rx);
	int i = 0;
	bool ficar = TRUE;
	bool dispara = TRUE;
	int start_s;
	int stop_s;
	do {
		// memset(recvbuf, '\0', recvbuflen);
		iResult = recv(ls_sock, recvbuf, recvbuflen, FALSE);
		if (iResult > 0) {
			if (dispara) {
				start_s = clock();
				dispara = FALSE;
			}
			//recvbuf[iResult] = '\0';
			//std::cout << "Bytes Recebidos: " << iResult << "! Mensagem:\n\t\"" << recvbuf << "\"";
			/**
			std::cout << "\n\tRetornando mensagem ao cliente...";
			iSendResult = sendto(tx_sock, sendbuf, strlen(sendbuf), FALSE, result_tx->ai_addr, (int) result_tx->ai_addrlen);
			if (iSendResult == SOCKET_ERROR) {
				std::cout << "Falha ao enviar mensagem: " << WSAGetLastError() << ".";
				closesocket(tx_sock);
				WSACleanup();
				return 1;
			}
			else {
				std::cout << "Bytes retornados: " << iSendResult << "\n";
			}
			/**/
			i++;
			if (i > PACOTES * 0.85) {
				ficar = FALSE;
			}
		}
		else if (iResult == 0) {
			std::cout << "Conexao fechada.";
			ficar = FALSE;
		}
		else {
			std::cout << "Falha ao receber mensagem: " << WSAGetLastError() << ".";
			closesocket(ls_sock);
			WSACleanup();
			ficar = FALSE;
			//return 1;
		}
	} while (ficar);
	stop_s = clock();
	std::cout << "tempo: " << (stop_s - start_s) / double(CLOCKS_PER_SEC) * 1000 << ". Pacotes recebidos: " << i;

	std::cout << "\nNenhuma outra mensagem sera enviada...Fechando conexao...";
	iResult = shutdown(ls_sock, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		std::cout << "Falha em fechar a conexao: " << WSAGetLastError() << ".";
		closesocket(ls_sock);
		WSACleanup();
		return 6;
	}
	else {
		std::cout << "Conexao fechada!";
	}

	std::cout << "\nNenhuma outra mensagem sera enviada...Fechando socket...";
	iResult = closesocket(ls_sock);
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
