#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "DetectionDecoder.h"
#include "CRC.h"

bool verify_frame(const FrameDynamic *frame, uint8_t crc_type, size_t total_size) {
    if (frame == NULL || total_size < sizeof(FrameDynamic)) {
        return false;
    }

    if (sizeof(FrameDynamic) + frame->len != total_size) {
        return false;
    }

    FrameDynamic *temp_frame = (FrameDynamic *)malloc(total_size);
    if (temp_frame == NULL) {
        return false;
    }

    memcpy(temp_frame, frame, total_size);
    uint32_t received_crc = temp_frame->CRC;
    temp_frame->CRC = 0;

    uint32_t calculated_crc = 0;
    switch (crc_type) {
        case CRC_TYPE_8:
            calculated_crc = calculate_crc8((uint8_t *)temp_frame, total_size);
            break;
        case CRC_TYPE_16:
            calculated_crc = calculate_crc16((uint8_t *)temp_frame, total_size);
            break;
        case CRC_TYPE_32:
        default:
            calculated_crc = calculate_crc32((uint8_t *)temp_frame, total_size);
            break;
    }

    free(temp_frame);
    return (calculated_crc == received_crc);
}

bool receiver_verify_once(const FrameDynamic *incoming, size_t total_size, uint16_t expected_seq, FILE *out_file, uint8_t crc_type) {
    if (!verify_frame(incoming, crc_type, total_size)) {
        printf("  [ODBIORNIK] Ramka seq=%u ma blad CRC!\n", expected_seq);
        return false;
    }

    if (out_file != NULL && incoming->len > 0) {
        fwrite(incoming->payload, 1, incoming->len, out_file);
    }
    printf("  [ODBIORNIK] Ramka seq=%u poprawna (%u B)\n", expected_seq, incoming->len);
    return true;
}