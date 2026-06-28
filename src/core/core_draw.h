// 2026-06-19

#ifndef DRAW_H
#define DRAW_H

////////////////////////////
//- Draw Type Constants

#define DR_FONT_SIZE 20

////////////////////////////
//- Draw Raylib Functions

static void DR_DrawLine2D_(Vector2 start_pos, Vector2 dir, f32 length, Color color);

#define DR_DrawText(text, x, y, color)           DrawTextEx(GAME.default_font, (text), (Vector2){(x),(y)}, DR_FONT_SIZE, 1, (color))
#define DR_DrawTextEx(text, x, y, size, color)   DrawTextEx(GAME.default_font, (text), (Vector2){(x),(y)}, (size), 1, (color))
#define DR_DrawRectangle(x, y, w, h, color)      DrawRectangle(x, y, w, h, color)
#define DR_DrawRectangleLines(x, y, w, h, color) DrawRectangleLines(x, y, w, h, color)
#define DR_DrawLine2D(start, dir, len, color)    DR_DrawLine2D_(start, dir, len, color)

#endif // DRAW_H
