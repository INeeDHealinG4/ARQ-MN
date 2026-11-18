#ifndef ARQ_SIM_ARQ_H
#define ARQ_SIM_ARQ_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define FRAME_PREAMBLE 0x7E

typedef enum {
    FRAME_TYPE_DATA = 0x01
} FrameType;

typedef enum {
    CRC_TYPE_8  = 8,
    CRC_TYPE_16 = 16,
    CRC_TYPE_32 = 32
} CrcType;

#pragma pack(push, 1)
typedef struct {
    uint8_t  preamble;   // Preambuła
    uint8_t  type;       // DATA
    uint16_t len;        // Długość pola payload w bajtach
    uint16_t seq_number; // Numer ramki
    uint32_t CRC;        // Obliczona suma kontrolna
    uint8_t  payload[];  // Dynamiczne dane o rozmiarze 'len'
} FrameDynamic;
#pragma pack(pop)

typedef struct {
    size_t frames_sent;
    size_t frames_valid;
    size_t frames_corrupted;
} TransmissionStats;

FrameDynamic* create_frame(uint8_t type, uint16_t seq_number, const uint8_t *data, uint16_t len, uint8_t crc_type);

static inline size_t get_frame_total_size(uint16_t payload_len) {
    return sizeof(FrameDynamic) + payload_len;
}

TransmissionStats Transmit_Once(FILE *in_file, FILE *out_file, size_t payload_size, uint8_t crc_type, double ber);

#endif // ARQ_SIM_ARQ_H