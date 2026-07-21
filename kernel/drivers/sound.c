/**
 * Nothing OS - PC Speaker Hardware Audio Driver & Frequency Synthesizer
 * 
 * Drives 8254 PIT Timer Channel 2 (Port 0x42/0x43) and PC Speaker Control Port 0x61
 * for square wave tone generation, audio telemetry, and system startup chimes.
 */

#include "../include/sound.h"
#include "../include/io.h"
#include "../include/serial.h"

/**
 * Synthesize audio frequency in Hz using PIT Channel 2 and PC Speaker Port 0x61.
 */
void sound_play_freq(uint32_t freq_hz) {
    if (freq_hz == 0) return;

    uint32_t divisor = 1193180 / freq_hz;

    /* Command Byte 0xB6: Channel 2, Access Mode LSB/MSB, Square Wave Mode */
    outb(PIT_COMMAND_PORT, 0xB6);
    outb(PIT_CHANNEL_2_DATA, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL_2_DATA, (uint8_t)((divisor >> 8) & 0xFF));

    /* Enable PC Speaker bits 0 and 1 */
    uint8_t tmp = inb(PC_SPEAKER_PORT);
    if (tmp != (tmp | 3)) {
        outb(PC_SPEAKER_PORT, tmp | 3);
    }
}

/**
 * Disable PC Speaker output.
 */
void sound_nosound(void) {
    uint8_t tmp = inb(PC_SPEAKER_PORT) & 0xFC;
    outb(PC_SPEAKER_PORT, tmp);
}

/**
 * Generate standard system telemetry notification beep (1000 Hz tone).
 */
void sound_beep(void) {
    klog(KLOG_INFO, "PC Speaker Hardware Beep Telemetry Triggered.");
    sound_play_freq(1000);
    for (volatile int i = 0; i < 5000000; i++);
    sound_nosound();
}

/**
 * Synthesize Nothing OS Startup Musical Audio Chime.
 */
void sound_play_chime(void) {
    klog(KLOG_INFO, "Playing Nothing OS Enterprise Startup Audio Chime.");
    uint32_t notes[] = {NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5};
    for (int i = 0; i < 4; i++) {
        sound_play_freq(notes[i]);
        for (volatile int d = 0; d < 3000000; d++);
    }
    sound_nosound();
}
