#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/aes.h>

#define MAX_PACKET_SIZE 1408
#define AES_KEY_SIZE 128

void decrypt(const unsigned char *key, const unsigned char *ciphertext, unsigned char *plaintext) {
    AES_KEY aes_key;
    AES_set_decrypt_key(key, AES_KEY_SIZE, &aes_key);

    size_t i;
    for (i = 0; i < MAX_PACKET_SIZE; i += AES_BLOCK_SIZE) {
        AES_decrypt(ciphertext + i, plaintext + i, &aes_key);
    }
}


int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <number_of_packets> <decrypt|no-decrypt> <receiver IP>\n", argv[0]);
        return 1;
    }

    int numPackets = atoi(argv[1]);
    int doDecrypt = strcmp(argv[2], "decrypt") == 0 ? 1 : 0;

    int sockfd;
    struct sockaddr_in serverAddr, senderAddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Error in socket");
        return 1;
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(60000);
    serverAddr.sin_addr.s_addr = argc < 4 ? inet_addr("192.168.40.40") : inet_addr(argv[3]);// htonl(INADDR_ANY); // Receiver IP

    if (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Error in binding");
        return 1;
    }

    unsigned char receivedPacket[MAX_PACKET_SIZE];
    unsigned char decryptedPacket[MAX_PACKET_SIZE];
    unsigned char key[AES_KEY_SIZE / 8];
    for (int i = 0; i < AES_KEY_SIZE / 8; i++) {
                key[i] = i;
    }

    socklen_t senderLen;
    int bytesRead;

    for (int i = 0; i < numPackets; i++) {
        senderLen = sizeof(senderAddr);

        // Receive packet
        bytesRead = recvfrom(sockfd, receivedPacket, sizeof(receivedPacket), 0, (struct sockaddr *)&senderAddr, &senderLen);
	if (bytesRead < 0) {
            perror("Error in receiving packet");
            return 1;
        }
        // Decrypt packet if required
        if (doDecrypt) {
            decrypt(key, receivedPacket, decryptedPacket);
        } else {
            memcpy(decryptedPacket, receivedPacket, bytesRead);
        }
	decryptedPacket[bytesRead] = '\0';
	//printf("Received packet %d from %s:%d: %s\n", i + 1, inet_ntoa(senderAddr.sin_addr), ntohs(senderAddr.sin_port), decryptedPacket);
    	printf("%s", decryptedPacket);
    }
    printf("\n");	
    close(sockfd);
    return 0;
}
