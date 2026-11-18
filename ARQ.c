#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ARQ.h"
#include "BSC.h"
#include "CRC.h"
#include "DetectionDecoder.h"

FrameDynamic* create_frame(uint8_t type, uint16_t seq_number, const uint8_t *data, uint16_t len, uint8_t crc_type) {
    size_t total_size = get_frame_total_size(len);
    FrameDynamic *f1 = (FrameDynamic *)malloc(total_size);
    if (f1 == NULL) {
        return NULL;
    }

    f1->preamble = FRAME_PREAMBLE;
    f1->type = type;
    f1->len = len;
    f1->seq_number = seq_number;
    f1->CRC = 0;

    if (data != NULL && len > 0) {
        memcpy(f1->payload, data, len);
    }

    switch (crc_type) {
        case CRC_TYPE_8:
            f1->CRC = calculate_crc8((uint8_t *)f1, total_size);
            break;
        case CRC_TYPE_16:
            f1->CRC = calculate_crc16((uint8_t *)f1, total_size);
            break;
        case CRC_TYPE_32:
        default:
            f1->CRC = calculate_crc32((uint8_t *)f1, total_size);
            break;
    }

    return f1;
}

TransmissionStats Transmit_Once(FILE *in_file, FILE *out_file, size_t payload_size, uint8_t crc_type, double ber) {
    TransmissionStats stats = {0, 0, 0};

    unsigned char *buffer = (unsigned char *)malloc(payload_size);
    if (buffer == NULL) {
        printf("Blad alokacji bufora w Transmit_Once\n");
        return stats;
    }

    size_t bytes_read = 0;
    uint16_t tx_seq = 0;

    while ((bytes_read = fread(buffer, 1, payload_size, in_file)) > 0) {
        size_t data_frame_size = get_frame_total_size((uint16_t)bytes_read);

        FrameDynamic *tx_frame = create_frame(FRAME_TYPE_DATA, tx_seq, buffer, (uint16_t)bytes_read, crc_type);
        if (tx_frame == NULL) {
            printf("Blad alokacji ramki nr %u\n", tx_seq);
            free(buffer);
            return stats;
        }

        stats.frames_sent++;

        // 1. Transmisja ramki przez kanał BSC
        bsc(tx_frame, data_frame_size, ber);

        // 2. Jednokrotne sprawdzenie CRC po stronie odbiornika
        if (receiver_verify_once(tx_frame, data_frame_size, tx_seq, out_file, crc_type)) {
            stats.frames_valid++;
        } else {
            stats.frames_corrupted++;
        }

        free(tx_frame);
        tx_seq++;
    }

    free(buffer);
    return stats;
}