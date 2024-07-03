//
//  client.cpp
//  Socket
//
//  Created by Mick Maciel on 13/02/2024.
//

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

constexpr int PORT = 8080;
constexpr int BUFFER_SIZE = 1024;

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    // Criando o descritor de arquivo do socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "ERROR!" << std::endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convertendo o endereço IP de string para binário
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "INVALID ADDRESS" << std::endl;
        return -1;
    }

    // Conectando ao servidor
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "CONNECTION FAILURE" << std::endl;
        return -1;
    }

    std::cout << "CONNECTED" << std::endl;

    while (true) {
        std::cout << "MESSAGE (OR 'exit' TO LEAVE): ";
        std::string message;
        std::getline(std::cin, message);

        // Envia a mensagem para o servidor
        send(sock, message.c_str(), message.length(), 0);

        // Verifica se o usuário quer sair
        if (message == "exit")
            break;

        // Limpa o buffer antes de receber a resposta
        memset(buffer, 0, sizeof(buffer));

        // Recebe a resposta do servidor
        ssize_t bytesRead = recv(sock, buffer, BUFFER_SIZE, 0);

        // Verifica se houve erro na recepção
        if (bytesRead < 0) {
            std::cerr << "ERROR! NO DATA RECIEPED" << std::endl;
            break;
        }

        // Verifica se o servidor desconectou
        if (bytesRead == 0) {
            std::cout << "SERVER DOWN" << std::endl;
            break;
        }

        std::cout << "SERVER: " << buffer << std::endl;
    }

    // Fecha o socket
    close(sock);

    return 0;
}
