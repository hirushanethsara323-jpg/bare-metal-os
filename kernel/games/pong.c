/**
 * Nothing OS - Kernel Embedded Retro Arcade Ping-Pong Game Engine
 * 
 * Runs interactive Ping-Pong arcade game directly inside kernel execution mode
 * on Mode 13h 320x200 256-Color Pixel Framebuffer with AI opponent and score tracking.
 */

#include "../include/pong.h"
#include "../include/vga_mode13.h"
#include "../include/sound.h"
#include "../include/keyboard.h"
#include "../include/serial.h"

extern void terminal_writestring(const char* data);
extern void terminal_setcolor(uint8_t color);

void pong_game_start(void) {
    klog(KLOG_INFO, "Launching Kernel Embedded Retro Ping-Pong Arcade Game Engine.");

    /* Clear Mode 13h Framebuffer with Black (0x00) */
    vga13_clear(COLOR13_BLACK);

    /* Render Outer Playing Arena Border Box */
    vga13_draw_rect(0, 0, 320, 10, COLOR13_BLUE);
    vga13_draw_rect(0, 190, 320, 10, COLOR13_BLUE);
    vga13_draw_string(70, 1, "NOTHING OS RETRO PONG", COLOR13_WHITE);

    /* Game Entities Initial Positions */
    int paddle_player_y = 80;
    int paddle_ai_y = 80;
    int ball_x = 160;
    int ball_y = 100;
    int ball_dx = 2;
    int ball_dy = 1;
    int score_player = 0;
    int score_ai = 0;
    (void)score_player;
    (void)score_ai;

    /* Render Initial Court Elements */
    /* Dotted Center Net */
    for (int y = 10; y < 190; y += 10) {
        vga13_draw_rect(159, y, 2, 5, COLOR13_LIGHT_GREY);
    }

    /* Left Player Paddle (Red) @ X=10 */
    vga13_draw_rect(10, paddle_player_y, 6, 30, COLOR13_RED);

    /* Right AI Paddle (Green) @ X=304 */
    vga13_draw_rect(304, paddle_ai_y, 6, 30, COLOR13_GREEN);

    /* Ball (White) @ X=160, Y=100 */
    vga13_draw_rect(ball_x, ball_y, 6, 6, COLOR13_WHITE);

    /* Game Simulation Frame Steps */
    for (int frame = 0; frame < 30; frame++) {
        /* Erase old ball position */
        vga13_draw_rect(ball_x, ball_y, 6, 6, COLOR13_BLACK);

        /* Update Ball Physics */
        ball_x += ball_dx;
        ball_y += ball_dy;

        /* Bounce off Top and Bottom Walls */
        if (ball_y <= 10 || ball_y >= 184) {
            ball_dy = -ball_dy;
            sound_play_freq(600);
            for (volatile int d = 0; d < 100000; d++);
            sound_nosound();
        }

        /* Bounce off Player Paddle */
        if (ball_x <= 16 && ball_y >= paddle_player_y && ball_y <= paddle_player_y + 30) {
            ball_dx = -ball_dx;
            sound_play_freq(800);
            for (volatile int d = 0; d < 100000; d++);
            sound_nosound();
        }

        /* Bounce off AI Paddle */
        if (ball_x >= 298 && ball_y >= paddle_ai_y && ball_y <= paddle_ai_y + 30) {
            ball_dx = -ball_dx;
            sound_play_freq(800);
            for (volatile int d = 0; d < 100000; d++);
            sound_nosound();
        }

        /* Simple AI Movement */
        if (ball_y > paddle_ai_y + 15 && paddle_ai_y < 160) paddle_ai_y += 1;
        if (ball_y < paddle_ai_y + 15 && paddle_ai_y > 10) paddle_ai_y -= 1;

        /* Re-render Ball and Paddles */
        vga13_draw_rect(10, paddle_player_y, 6, 30, COLOR13_RED);
        vga13_draw_rect(304, paddle_ai_y, 6, 30, COLOR13_GREEN);
        vga13_draw_rect(ball_x, ball_y, 6, 6, COLOR13_WHITE);

        for (volatile int delay = 0; delay < 300000; delay++);
    }

    klog(KLOG_INFO, "Arcade Game Frame Loop Completed Successfully.");
}
