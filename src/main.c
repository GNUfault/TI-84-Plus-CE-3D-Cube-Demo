#include <tice.h>
#include <graphx.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

float vertices[8][3] = {
    {-25,-25,-25}, {25,-25,-25}, {25,25,-25}, {-25,25,-25},
    {-25,-25, 25}, {25,-25, 25}, {25,25, 25}, {-25,25, 25}
};

int faces[6][4] = {
    {0, 3, 2, 1}, {4, 5, 6, 7}, {0, 1, 5, 4},
    {1, 2, 6, 5}, {2, 3, 7, 6}, {3, 0, 4, 7}
};

int main(void) {
    float cube_x = 160.0f, cube_y = 120.0f;
    float dx = 1.2f, dy = 0.9f;
    float rx = 0, ry = 0, rz = 0;
    
    const char *msg = "3D Cube Demo by GNUfault";
    int msg_len = strlen(msg);
    int total_width;
    int start_x;
    
    int active_char = 0;
    float char_time = 0;

    gfx_Begin();
    gfx_SetDrawBuffer();
    
    gfx_SetTextTransparentColor(0);
    gfx_SetTextBGColor(0);
    gfx_SetTextFGColor(255);

    for (int i = 0; i < 256; i++) {
        uint8_t gray = (uint8_t)i;
        uint16_t color = ((gray >> 3) << 11) | ((gray >> 2) << 5) | (gray >> 3);
        gfx_palette[i] = color;
    }

    total_width = gfx_GetStringWidth(msg);
    start_x = (GFX_LCD_WIDTH - total_width) / 2;

    while (!os_GetCSC()) {
        gfx_FillScreen(0);

        rx += 0.04f; 
        ry += 0.03f; 
        rz += 0.02f;
        cube_x += dx; 
        cube_y += dy;

        if (cube_x < 50 || cube_x > 270) dx *= -1;
        if (cube_y < 50 || cube_y > 190) dy *= -1;

        int projected_pts[8][2];
        float rot_z[8];

        for (int i = 0; i < 8; i++) {
            float x0 = vertices[i][0], y0 = vertices[i][1], z0 = vertices[i][2];
            float y1 = y0 * cosf(rx) - z0 * sinf(rx);
            float z1 = y0 * sinf(rx) + z0 * cosf(rx);
            float x2 = x0 * cosf(ry) + z1 * sinf(ry);
            float z2 = -x0 * sinf(ry) + z1 * cosf(ry);
            float x3 = x2 * cosf(rz) - y1 * sinf(rz);
            float y3 = x2 * sinf(rz) + y1 * cosf(rz);

            rot_z[i] = z2;
            float f = 200.0f / (200.0f + z2);
            projected_pts[i][0] = (int)(cube_x + x3 * f);
            projected_pts[i][1] = (int)(cube_y + y3 * f);
        }

        for (int i = 0; i < 6; i++) {
            int i0 = faces[i][0], i1 = faces[i][1], i2 = faces[i][2], i3 = faces[i][3];
            long cp = (long)(projected_pts[i1][0] - projected_pts[i0][0]) * (projected_pts[i2][1] - projected_pts[i0][1]) -
                      (long)(projected_pts[i1][1] - projected_pts[i0][1]) * (projected_pts[i2][0] - projected_pts[i0][0]);

            if (cp < 0) {
                float avg_z = (rot_z[i0] + rot_z[i1] + rot_z[i2] + rot_z[i3]) / 4.0f;
                int shade = 160 - (int)(avg_z * 3.0f);
                if (shade < 40) shade = 40;
                if (shade > 255) shade = 255;
                gfx_SetColor((uint8_t)shade);
                gfx_FillTriangle(projected_pts[i0][0], projected_pts[i0][1], projected_pts[i1][0], projected_pts[i1][1], projected_pts[i2][0], projected_pts[i2][1]);
                gfx_FillTriangle(projected_pts[i0][0], projected_pts[i0][1], projected_pts[i2][0], projected_pts[i2][1], projected_pts[i3][0], projected_pts[i3][1]);
            }
        }

        char_time += 0.8f;
        if (char_time >= 3.14159f) {
            char_time -= 3.14159f;
            active_char++;
            if (active_char >= msg_len) {
                active_char = 0;
            }
        }

        int current_draw_x = start_x;
        for (int i = 0; i < msg_len; i++) {
            int y_off = 0;
            if (i == active_char) {
                y_off = -(int)(25.0f * sinf(char_time));
            }
            gfx_SetTextXY(current_draw_x, 210 + y_off);
            gfx_PrintChar(msg[i]);
            current_draw_x += gfx_GetCharWidth(msg[i]);
        }

        gfx_SwapDraw();
    }

    gfx_End();
    return 0;
}
