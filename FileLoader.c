#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include "ARQ.h"

void LoadingAndWriting(size_t L_payload, const char *filename, uint8_t crc_type, double ber) {
    FILE *ReadAnyToBinary = fopen(filename, "rb");
    if (ReadAnyToBinary == NULL) {
        perror("Blad otwarcia pliku wejściowego");
        return;
    }

    fseek(ReadAnyToBinary, 0, SEEK_END);
    long file_size = ftell(ReadAnyToBinary);
    rewind(ReadAnyToBinary);

    size_t frames = (file_size + L_payload - 1) / L_payload;
    printf("Rozmiar pliku: %ld bajtow\n", file_size);
    printf("Przewidywana liczba ramek: %zu\n\n", frames);

    FILE *WriteToBinary = fopen("wyjscie.txt", "wb");
    if (WriteToBinary == NULL) {
        perror("Blad tworzenia pliku wyjsciowego");
        fclose(ReadAnyToBinary);
        return;
    }

    TransmissionStats stats = Transmit_Once(ReadAnyToBinary, WriteToBinary, L_payload, crc_type, ber);

    printf("\nPODSUMOWANIE TRANSMISJI (Jednokrotna weryfikacja CRC)\n");
    printf("Wyslane ramki:              %zu\n", stats.frames_sent);
    printf("Poprawne ramki (CRC OK):    %zu\n", stats.frames_valid);
    printf("Uszkodzone ramki (CRC ERR): %zu\n", stats.frames_corrupted);

    fclose(ReadAnyToBinary);
    fclose(WriteToBinary);
}