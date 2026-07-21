#ifndef SOUND_H
#define SOUND_H

#include <stdint.h>

/* PC Speaker IO Ports */
#define PIT_CHANNEL_2_DATA 0x42
#define PIT_COMMAND_PORT   0x43
#define PC_SPEAKER_PORT    0x61

/* Common Musical Frequencies (Hz) */
#define NOTE_C4  261
#define NOTE_E4  329
#define NOTE_G4  392
#define NOTE_C5  523

/* Function Declarations */
void sound_play_freq(uint32_t freq_hz);
void sound_nosound(void);
void sound_beep(void);
void sound_play_chime(void);

#endif /* SOUND_H */
