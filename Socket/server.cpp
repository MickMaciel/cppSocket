//
//  server.cpp
//  Socket
//
//  Created by Mick Maciel on 13/02/2024.
//

#include <iostream>
#include <cstring>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <chrono>

// definir porta, buffer size quantidade de clientes conectados e tempo de espera
constexpr int PORT = 8080;
constexpr int BUFFER_SIZE = 1024;
constexpr int MAX_PENDING_CONNECTIONS = 3;
constexpr int TIMEOUT_SECONDS = 60;

void handleClient(int clientSocket) {
    char buffer[BUFFER_SIZE] = {0};
    ssize_t bytesRead;
    
    while (true) {
        //limpar buffer antes de receber dados
        memset(buffer, 0, sizeof(buffer));
        
        //ler dados enviados pelo cliente
        bytesRead = read(clientSocket, buffer, sizeof(buffer));
        
        //veriicar leiruta dos dados
        if (bytesRead < 0) {
            std::cerr << "ERROR" << std::endl;
            break;
        }
        
        //verificar conexao com cliente
        if (bytesRead == false) {
            std::cout << "NO CONNECTED" <<  std::endl;
            break;
        }
        
        std::cout << "CLIENT" << buffer << std::endl;
        
        //Enviar resposta ao cliente
        if (send(clientSocket, buffer, bytesRead, 0) < 0) {
            std::cerr << "ERROR! NO SENT DATA" << std::endl;
            break;
        }
        
        if (strcmp(buffer, "exit") == 0) {
            std::cout << "CLOSING CONNECTION" << std::endl;
            break;
        }
    }
    
    //encerrar conexao
    close(clientSocket);
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in address;
    int opt = 1;

    // Criando o descritor de arquivo do socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("ERROR! SOCKET IS NOT BOUND");
        exit(EXIT_FAILURE);
    }

    // Definindo opções do socket para reutilizar endereços e portas
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("ERROR! CONFIGURE SOCKET OPTIONS");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Vinculando o socket ao endereço e à porta especificados
    if (bind(serverSocket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("SOCKET ERROR");
        exit(EXIT_FAILURE);
    }

    // Configurando o socket para ouvir conexões
    if (listen(serverSocket, MAX_PENDING_CONNECTIONS) < 0) {
        perror("ERROR! CONFIGURE SOCKET TIMEOUT");
        exit(EXIT_FAILURE);
    }

    while (true) {
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);

        // Aceitando conexões de clientes
        if ((clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen)) < 0) {
            perror("ERROR! CONNECTION NOT ACCEPT");
            exit(EXIT_FAILURE);
        }

        std::cout << "NEW CLIENT CONNECTED" << std::endl;

        // Configurando timeout para o socket do cliente
        struct timeval timeout;
        timeout.tv_sec = TIMEOUT_SECONDS;
        timeout.tv_usec = 0;
        if (setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) < 0) {
            perror("ERROR! CONFIGURE CLIENT SOCKET TIMEOUT");
            exit(EXIT_FAILURE);
        }

        // Inicia uma thread para lidar com o cliente
        std::thread(handleClient, clientSocket).detach();
    }
    
    // Fecha o socket do servidor (não alcançado neste exemplo)
    close(serverSocket);
    close(clientSocket);
    
    return 0;
}
