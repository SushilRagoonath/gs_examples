//
// Created by Sushil on 2021/07/10.
//

#ifndef GS_EXAMPLES_GS_MICROUI_H
#define GS_EXAMPLES_GS_MICROUI_H
/*== micrui renderer =========================================================*/


static void r_push_quad(mu_Rect dst, mu_Rect src, mu_Color color) {
    float u0 = (float) src.x / (float) ATLAS_WIDTH;
    float v0 = (float) src.y / (float) ATLAS_HEIGHT;
    float u1 = (float) (src.x + src.w) / (float) ATLAS_WIDTH;
    float v1 = (float) (src.y + src.h) / (float) ATLAS_HEIGHT;

    float x0 = (float) dst.x;
    float y0 = (float) dst.y;
    float x1 = (float) (dst.x + dst.w);
    float y1 = (float) (dst.y + dst.h);
    gsi_begin(&gsi, GS_GRAPHICS_PRIMITIVE_TRIANGLES);
    gsi_c4ub(&gsi,color.r, color.g, color.b, color.a);

    // First triangle
    gsi_tc2f(&gsi, u0, v0); gsi_v2f(&gsi, x0, y0);
    gsi_tc2f(&gsi, u1, v0); gsi_v2f(&gsi, x1, y0);
    gsi_tc2f(&gsi, u0, v1); gsi_v2f(&gsi, x0, y1);

    // Second triangle
    gsi_tc2f(&gsi, u1, v0); gsi_v2f(&gsi, x1, y0);
    gsi_tc2f(&gsi, u1, v1); gsi_v2f(&gsi, x1, y1);
    gsi_tc2f(&gsi, u0, v1); gsi_v2f(&gsi, x0, y1);
    gsi_end(&gsi);
    //    sgl_v2f_t2f(x0, y0, u0, v0);
    //    sgl_v2f_t2f(x1, y0, u1, v0);
    //    sgl_v2f_t2f(x1, y1, u1, v1);
    //    sgl_v2f_t2f(x0, y1, u0, v1);
}
static void r_draw_icon(int id, mu_Rect rect, mu_Color color) {
    mu_Rect src = atlas[id];
    int x = rect.x + (rect.w - src.w) / 2;
    int y = rect.y + (rect.h - src.h) / 2;
    r_push_quad(mu_rect(x, y, src.w, src.h), src, color);
}
static void r_draw_rect(mu_Rect rect, mu_Color color) {
    r_push_quad(rect, atlas[ATLAS_WHITE], color);
}
void r_draw_text(const char *text, mu_Vec2 pos, mu_Color color) {
    mu_Rect dst = { pos.x, pos.y, 0, 0 };
    for (const char *p = text; *p; p++) {
        if ((*p & 0xc0) == 0x80) { continue; }
        int chr = mu_min((unsigned char) *p, 127);
        mu_Rect src = atlas[ATLAS_FONT + chr];
        dst.w = src.w;
        dst.h = src.h;
        r_push_quad(dst, src, color);
        dst.x += dst.w;
    }
}
#endif //GS_EXAMPLES_GS_MICROUI_H
