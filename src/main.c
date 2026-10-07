#include <vita2d.h>
#include <psp2/ctrl.h>
#include <psp2/kernel/processmgr.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PRESSED(b) ((pad.buttons & (b)) && !(prev.buttons & (b)))

int main(void) {
    vita2d_init();
    vita2d_set_clear_color(RGBA8(20, 20, 45, 255));
    vita2d_pgf *pgf = vita2d_load_default_pgf();

    SceCtrlData pad, prev;
    memset(&pad, 0, sizeof(pad));
    memset(&prev, 0, sizeof(prev));
    srand(sceKernelGetProcessTimeLow());

    int hp = 50, mp = 20, enemy_hp = 60, sel = 0;
    int state = 0; /* 0 = tu turno, 1 = ganaste, 2 = perdiste */
    char msg1[64] = "Un demonio aparece!";
    char msg2[64] = "";
    const char *menu[3] = { "Atacar", "Fuego (5 MP)", "Curar (4 MP)" };

    while (1) {
        sceCtrlPeekBufferPositive(0, &pad, 1);

        if (pad.buttons & SCE_CTRL_SELECT) break;

        if (state == 0) {
            if (PRESSED(SCE_CTRL_UP))   sel = (sel + 2) % 3;
            if (PRESSED(SCE_CTRL_DOWN)) sel = (sel + 1) % 3;

            if (PRESSED(SCE_CTRL_CROSS)) {
                int acted = 1, extra = 0, d;
                msg2[0] = '\0';

                if (sel == 0) {
                    d = 8 + rand() % 5;
                    enemy_hp -= d;
                    snprintf(msg1, sizeof(msg1), "Atacas: %d de dano", d);
                } else if (sel == 1) {
                    if (mp < 5) {
                        snprintf(msg1, sizeof(msg1), "No tienes MP suficiente");
                        acted = 0;
                    } else {
                        mp -= 5;
                        d = (14 + rand() % 5) * 2; /* el demonio es debil al fuego */
                        enemy_hp -= d;
                        extra = 1;
                        snprintf(msg1, sizeof(msg1), "Fuego! Punto debil: %d. Turno extra!", d);
                    }
                } else {
                    if (mp < 4) {
                        snprintf(msg1, sizeof(msg1), "No tienes MP suficiente");
                        acted = 0;
                    } else {
                        mp -= 4;
                        hp += 15;
                        if (hp > 50) hp = 50;
                        snprintf(msg1, sizeof(msg1), "Te curas 15 HP");
                    }
                }

                if (acted) {
                    if (enemy_hp <= 0) {
                        enemy_hp = 0;
                        state = 1;
                        snprintf(msg2, sizeof(msg2), "Ganaste! START para reiniciar");
                    } else if (!extra) {
                        d = 6 + rand() % 5;
                        hp -= d;
                        snprintf(msg2, sizeof(msg2), "El demonio te hace %d de dano", d);
                        if (hp <= 0) {
                            hp = 0;
                            state = 2;
                            snprintf(msg2, sizeof(msg2), "Caiste... START para reiniciar");
                        }
                    }
                }
            }
        } else if (PRESSED(SCE_CTRL_START)) {
            hp = 50; mp = 20; enemy_hp = 60; sel = 0; state = 0;
            snprintf(msg1, sizeof(msg1), "Un demonio aparece!");
            msg2[0] = '\0';
        }

        vita2d_start_drawing();
        vita2d_clear_screen();

        /* Demonio */
        vita2d_draw_rectangle(380, 100, 160, 160, RGBA8(160, 40, 60, 255));
        vita2d_draw_rectangle(410, 140, 20, 20, RGBA8(255, 255, 0, 255));
        vita2d_draw_rectangle(490, 140, 20, 20, RGBA8(255, 255, 0, 255));
        vita2d_pgf_draw_text(pgf, 380, 80, RGBA8(255, 255, 255, 255), 1.0f, "Sombra (debil al fuego)");
        vita2d_draw_rectangle(380, 275, 160, 10, RGBA8(60, 60, 60, 255));
        vita2d_draw_rectangle(380, 275, (enemy_hp * 160) / 60, 10, RGBA8(220, 50, 50, 255));

        /* Jugador */
        vita2d_pgf_draw_textf(pgf, 40, 60, RGBA8(255, 255, 255, 255), 1.0f, "HP: %d / 50", hp);
        vita2d_pgf_draw_textf(pgf, 40, 90, RGBA8(120, 180, 255, 255), 1.0f, "MP: %d / 20", mp);

        /* Menu */
        for (int i = 0; i < 3; i++) {
            unsigned int c = (i == sel) ? RGBA8(255, 220, 80, 255) : RGBA8(200, 200, 200, 255);
            vita2d_pgf_draw_textf(pgf, 40, 170 + i * 30, c, 1.0f, "%s%s", (i == sel) ? "> " : "  ", menu[i]);
        }

        /* Mensajes */
        vita2d_pgf_draw_text(pgf, 40, 440, RGBA8(255, 255, 255, 255), 1.0f, msg1);
        vita2d_pgf_draw_text(pgf, 40, 480, RGBA8(255, 255, 255, 255), 1.0f, msg2);
        vita2d_pgf_draw_text(pgf, 600, 530, RGBA8(150, 150, 150, 255), 0.8f, "X: elegir   SELECT: salir");

        vita2d_end_drawing();
        vita2d_swap_buffers();
        prev = pad;
    }

    vita2d_free_pgf(pgf);
    vita2d_fini();
    sceKernelExitProcess(0);
    return 0;
}
