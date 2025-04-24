#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <arpa/inet.h> //Para funções de socket como socket(), bind(), listen(), accept(), etc


# define PORT 12345
# define BUFFER_SIZE 64

struct Employee{
    int EmployeeID;
    char * username;
    char* password;
}Employee;

int main(){

    int server_socket, client_socket;   // cliente e servidor
    struct sockaddr_in server_addr, client_addr; // Endereços do cliente e servidor

    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // 1 Criar o socket
    // Create UDP socket:
    server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (server_socket == -1) {
        perror("Erro ao criar socket");
        exit(EXIT_FAILURE);
    }
    
    // make port reusable
    int enable = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));

    // 2 configurar o endereço do servidor
    server_addr.sin_family = AF_INET;   // Define a família de endereços como sendo IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY;   // permite que o servidor aceite conexões em qualquer interface de rede
    server_addr.sin_port = htons(PORT); //define  a porta 1300

    // 3 Associar socket ao endereço
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Erro ao associar socket");
        close(server_socket);
        exit(EXIT_FAILURE);
    }


    printf("Servidor rodando na porta %d...\n", PORT);

    while (1) {
        // 5. Aceitar conexões
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket == -1) {
            perror("Erro ao aceitar conexão");
            continue;
        }

        printf("Client conected...\n");

        // Receive client's message:
    if (recvfrom(client_socket, buffer, sizeof(BUFFER_SIZE), 0,
    (struct sockaddr*)&client_addr, &client_len) < 0){
        printf("Couldn't receive\n");
        return -1;
    }

        // Processo funcionando em paralelo
        int pid = fork();
        if (pid == -1) {
            perror("Erro ao criar processo filho");
            close(client_socket);
            continue;
        }
        // processos pai
        if (pid > 0) {
            close(client_socket);
            continue;
        }

        while (1)
        {
            // 5. Receive message from client
            int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
            if (bytes_received > 0) {
                buffer[bytes_received] = '\0';
                printf("Client says: %s\n", buffer);
            } else {
                printf("Client disconected...\n");
                break;
            }        

            // 6. Send a response
            char response[BUFFER_SIZE + 6]; // 6 extra bytes for "ECHO: " and null terminator
            sprintf(response, "ECHO: %s", buffer);

            send(client_socket, response, strlen(response), 0);

        }
        
       
    }

    // Fechar o socket do servidor
    close(server_socket);
    close(client_socket);

    return 0;
}