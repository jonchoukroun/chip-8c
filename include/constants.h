// CPU
#define RAM_SIZE 4096
#define REGISTER_COUNT 16
#define CARRY_FLAG_ADDRESS 0xF
#define KEYBOARD_SIZE 16
#define STACK_SIZE 16
#define FONTSET_SIZE 80
#define RAND_UPPER_BOUND 4096
#define PROGRAM_START 0x200

// Clock speed
#define CPU_RATE 500                // Hz
#define DELAY_RATE 60               // Hz
#define MS_CONVERSION 1000
#define CLOCK_SPEED MS_CONVERSION / CPU_RATE

// Display
#define DISPLAY_TITLE "Chip-8 Emulator by Jon Choukroun"
#define DISPLAY_HEIGHT 32
#define DISPLAY_WIDTH 64
#define DISPLAY_SIZE DISPLAY_HEIGHT * DISPLAY_WIDTH
#define SCALE 15
#define PIXEL_WIDTH 8

// Audio
#define SAMPLE_RATE 48000
#define SAMPLE_LENGTH 3             // seconds
#define AUDIO_BUFFER_SIZE 2048;     // number of samples
#define OUTPUT_CHANNELS 1           // mono
#define FREQUENCY 150               // between D3 & D3#
#define AMPLITUDE 8000
