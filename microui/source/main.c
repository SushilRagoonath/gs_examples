/*================================================================
    * Copyright: 2020 John Jackson
    * nuklear_gui

    The purpose of this example is to

    Included:
        *

    Press `esc` to exit the application.
================================================================*/

// Nuklear will define these, so we will not implement them ourselves
#define GS_IMPL

#define GS_IMMEDIATE_DRAW_IMPL
#include <gs/gs.h>
#include <gs/util/gs_idraw.h>
#include <microui/microui.h>
#include <microui/microui.c>
#include "atlas.inl"

gs_command_buffer_t gcb = {0};
/* Platform */
mu_Context mu = {0};
gs_immediate_draw_t gsi = {0};
#include "gs_microui.h"
static   int logbuf_updated = 0;
static  char logbuf[64000];
gs_handle(gs_graphics_texture_t)  atlas_tex = {0};
static char * str = "";
static float bg[3] = { 90, 95, 100 };

void mu_char_callback(uint32_t handle, char codepoint)
{
    uint8_t txt[2] ={codepoint ,0};
    gs_printf("codepoint %c %i\n",codepoint,codepoint);
    if(codepoint>31&& codepoint<127){
        printf("reached \n");
        mu_input_text(&mu, txt);
    }
}
static int text_width(mu_Font font, const char *text, int len) {
    int res = 0;
    for (const char* p = text; *p && len--; p++) {
        res += atlas[ATLAS_FONT + (unsigned char)*p].w;
    }
    return res;

}



static int text_height(mu_Font font) {
    return 18;
}

static void write_log(const char* text) {
    /* FIXME: THIS IS UNSAFE! */
    if (logbuf[0]) {
        strcat(logbuf, "\n");
    }
    strcat(logbuf, text);
    logbuf_updated = 1;
}
static void log_window(mu_Context *ctx) {
    if (mu_begin_window(ctx, "Log Window", mu_rect(350, 40, 300, 200))) {
        /* output text panel */
        mu_layout_row(ctx, 1, (int[]) { -1 }, -25);
        mu_begin_panel(ctx, "Log Output");
        mu_Container *panel = mu_get_current_container(ctx);
        mu_layout_row(ctx, 1, (int[]) { -1 }, -1);
        mu_text(ctx, logbuf);
        mu_end_panel(ctx);
        if (logbuf_updated) {
            panel->scroll.y = panel->content_size.y;
            logbuf_updated = 0;
        }

        /* input textbox + submit button */
        static char buf[128];
        int submitted = 0;
        mu_layout_row(ctx, 2, (int[]) { -70, -1 }, 0);
        if (mu_textbox(ctx, buf, sizeof(buf)) & MU_RES_SUBMIT) {
            mu_set_focus(ctx, ctx->last_id);
            submitted = 1;
        }
        if (mu_button(ctx, "Submit")) { submitted = 1; }
        if (submitted) {
            write_log(buf);
            buf[0] = '\0';
        }

        mu_end_window(ctx);
    }
}

static void test_window(mu_Context *ctx) {
    /* do window */
    if (mu_begin_window(ctx, "Demo Window", mu_rect(40, 40, 300, 450))) {
        mu_Container *win = mu_get_current_container(ctx);
        win->rect.w = mu_max(win->rect.w, 240);
        win->rect.h = mu_max(win->rect.h, 300);

        /* window info */
        if (mu_header(ctx, "Window Info")) {
            mu_Container *win = mu_get_current_container(ctx);
            char buf[64];
            mu_layout_row(ctx, 2, (int[]) { 54, -1 }, 0);
            mu_label(ctx,"Position:");
            sprintf(buf, "%d, %d", win->rect.x, win->rect.y); mu_label(ctx, buf);
            mu_label(ctx, "Size:");
            sprintf(buf, "%d, %d", win->rect.w, win->rect.h); mu_label(ctx, buf);
        }

        /* labels + buttons */
        if (mu_header_ex(ctx, "Test Buttons", MU_OPT_EXPANDED)) {
            mu_layout_row(ctx, 3, (int[]) { 86, -110, -1 }, 0);
            mu_label(ctx, "Test buttons 1:");
            if (mu_button(ctx, "Button 1")) { write_log("Pressed button 1"); }
            if (mu_button(ctx, "Button 2")) { write_log("Pressed button 2"); }
            mu_label(ctx, "Test buttons 2:");
            if (mu_button(ctx, "Button 3")) { write_log("Pressed button 3"); }
            if (mu_button(ctx, "Popup")) { mu_open_popup(ctx, "Test Popup"); }
            if (mu_begin_popup(ctx, "Test Popup")) {
                mu_button(ctx, "Hello");
                mu_button(ctx, "World");
                mu_end_popup(ctx);
            }
        }

        /* tree */
        if (mu_header_ex(ctx, "Tree and Text", MU_OPT_EXPANDED)) {
            mu_layout_row(ctx, 2, (int[]) { 140, -1 }, 0);
            mu_layout_begin_column(ctx);
            if (mu_begin_treenode(ctx, "Test 1")) {
                if (mu_begin_treenode(ctx, "Test 1a")) {
                    mu_label(ctx, "Hello");
                    mu_label(ctx, "world");
                    mu_end_treenode(ctx);
                }
                if (mu_begin_treenode(ctx, "Test 1b")) {
                    if (mu_button(ctx, "Button 1")) { write_log("Pressed button 1"); }
                    if (mu_button(ctx, "Button 2")) { write_log("Pressed button 2"); }
                    mu_end_treenode(ctx);
                }
                mu_end_treenode(ctx);
            }
            if (mu_begin_treenode(ctx, "Test 2")) {
                mu_layout_row(ctx, 2, (int[]) { 54, 54 }, 0);
                if (mu_button(ctx, "Button 3")) { write_log("Pressed button 3"); }
                if (mu_button(ctx, "Button 4")) { write_log("Pressed button 4"); }
                if (mu_button(ctx, "Button 5")) { write_log("Pressed button 5"); }
                if (mu_button(ctx, "Button 6")) { write_log("Pressed button 6"); }
                mu_end_treenode(ctx);
            }
            if (mu_begin_treenode(ctx, "Test 3")) {
                static int checks[3] = { 1, 0, 1 };
                mu_checkbox(ctx, "Checkbox 1", &checks[0]);
                mu_checkbox(ctx, "Checkbox 2", &checks[1]);
                mu_checkbox(ctx, "Checkbox 3", &checks[2]);
                mu_end_treenode(ctx);
            }
            mu_layout_end_column(ctx);

            mu_layout_begin_column(ctx);
            mu_layout_row(ctx, 1, (int[]) { -1 }, 0);
            mu_text(ctx, "Lorem ipsum dolor sit amet, consectetur adipiscing "
                         "elit. Maecenas lacinia, sem eu lacinia molestie, mi risus faucibus "
                         "ipsum, eu varius magna felis a nulla.");
            mu_layout_end_column(ctx);
        }

        /* background color sliders */
        if (mu_header_ex(ctx, "Background Color", MU_OPT_EXPANDED)) {
            mu_layout_row(ctx, 2, (int[]) { -78, -1 }, 74);
            /* sliders */
            mu_layout_begin_column(ctx);
            mu_layout_row(ctx, 2, (int[]) { 46, -1 }, 0);
            mu_label(ctx, "Red:");   mu_slider(ctx, &bg[0], 0, 255);
            mu_label(ctx, "Green:"); mu_slider(ctx, &bg[1], 0, 255);
            mu_label(ctx, "Blue:");  mu_slider(ctx, &bg[2], 0, 255);
            mu_layout_end_column(ctx);
            /* color preview */
            mu_Rect r = mu_layout_next(ctx);
            mu_draw_rect(ctx, r, mu_color(bg[0], bg[1], bg[2], 255));
            char buf[32];
            sprintf(buf, "#%02X%02X%02X", (int) bg[0], (int) bg[1], (int) bg[2]);
            mu_draw_control_text(ctx, buf, r, MU_COLOR_TEXT, MU_OPT_ALIGNCENTER);
        }

        mu_end_window(ctx);
    }
}


void init()
{
    gcb = gs_command_buffer_new();
    gsi = gs_immediate_draw_new();

    mu_init(&mu);
    mu.text_width = text_width;
    mu.text_height = text_height;
    uint32_t rgba8_size = ATLAS_WIDTH * ATLAS_HEIGHT * 4;
    uint32_t* rgba8_pixels = (uint32_t*) malloc(rgba8_size);
    for (int y = 0; y < ATLAS_HEIGHT; y++) {
        for (int x = 0; x < ATLAS_WIDTH; x++) {
            int index = y*ATLAS_WIDTH + x;
            rgba8_pixels[index] = 0x00FFFFFF | ((uint32_t)atlas_texture[index]<<24);
        }
    }
    atlas_tex  = gs_graphics_texture_create (
            &(gs_graphics_texture_desc_t) {
                .width = ATLAS_WIDTH,
                .height = ATLAS_HEIGHT,
                .wrap_s = GS_GRAPHICS_TEXTURE_WRAP_REPEAT,
                .wrap_t = GS_GRAPHICS_TEXTURE_WRAP_REPEAT,
                .min_filter = GS_GRAPHICS_TEXTURE_FILTER_NEAREST,
                .mag_filter = GS_GRAPHICS_TEXTURE_FILTER_NEAREST,
                .format = GS_GRAPHICS_TEXTURE_FORMAT_RGBA8,
                .data = rgba8_pixels
            }
    );
    gs_platform_set_character_callback(0,mu_char_callback);
}


void update()
{
    if (gs_platform_key_pressed(GS_KEYCODE_ESC)) gs_engine_quit();
    gs_vec2 fbs = gs_platform_framebuffer_sizev(gs_platform_main_window());
    gs_vec2 mouse_pos = gs_platform_mouse_positionv();
    gsi_texture(&gsi,atlas_tex);
    gsi_camera2D(&gsi);
        // Poll all events that occured this frame
        gs_platform_event_t evt = gs_default_val();
        while (gs_platform_poll_events(&evt, false))
        {
            switch (evt.type)
            {
                case GS_PLATFORM_EVENT_MOUSE:
                {
                    switch (evt.mouse.action)
                    {
                        case GS_PLATFORM_MOUSE_MOVE:
                        {
                            mu_input_mousemove(&mu,(int)evt.mouse.move.x,(int)evt.mouse.move.y);
                        } break;

                        case GS_PLATFORM_MOUSE_WHEEL:
                        {
                            mu_input_scroll(&mu, 0, (int)evt.mouse.wheel.y* -30);
                        } break;

                        case GS_PLATFORM_MOUSE_BUTTON_PRESSED:
                        {
                            int code = 1<<evt.mouse.button;
                            gs_printf(" pressed %i shifted code %i\n",evt.mouse.button,code);
                            mu_input_mousedown(&mu,(int) mouse_pos.x,(int) mouse_pos.y,code);
                        } break;

                        case GS_PLATFORM_MOUSE_BUTTON_RELEASED:
                        {
                            int code = 1<<evt.mouse.button;
                            gs_printf(" released %i shifted code %i\n",evt.mouse.button,code);
                            mu_input_mouseup(&mu,(int) mouse_pos.x,(int) mouse_pos.y,code);
                        } break;

                        case GS_PLATFORM_MOUSE_BUTTON_DOWN:
                        {
                            int code = 1<<evt.mouse.button;
                            gs_printf(" down %i shifted code %i\n",evt.mouse.button,code);
                            mu_input_mousedown(&mu,(int) mouse_pos.x,(int) mouse_pos.y,code);
                        } break;

                        case GS_PLATFORM_MOUSE_ENTER:
                        {
                            // If there are user callbacks, could trigger them here
                        } break;

                        case GS_PLATFORM_MOUSE_LEAVE:
                        {
                            // If there are user callbacks, could trigger them here
                        } break;
                    }

                } break;

                case GS_PLATFORM_EVENT_KEY:
                {
                    switch (evt.key.action)
                    {
//                        uint8_t txt[2] ={( char)evt.key.codepoint,0};
//                        txt[0] = ( char)evt.key.codepoint+32;
//                        mu_input_text(&mu, txt);
                        case GS_PLATFORM_KEY_PRESSED:
                        case GS_PLATFORM_KEY_DOWN:
                        {
                            mu_input_keydown(&mu,evt.key.codepoint);
                        } break;

                        case GS_PLATFORM_KEY_RELEASED:
                        {
                            mu_input_keyup(&mu,evt.key.codepoint);
                        } break;
                    }

                } break;

                case GS_PLATFORM_EVENT_WINDOW:
                {
                    switch (evt.window.action)
                    {
                        default: break;
                    }

                } break;


                default: break;
            }
        }
    mu_begin(&mu);
    test_window(&mu);
    log_window(&mu);
    mu_end(&mu);
    mu_Command *cmd = NULL;
    while (mu_next_command(&mu, &cmd)) {
      switch (cmd->type) {
        case MU_COMMAND_TEXT:
            r_draw_text(cmd->text.str,cmd->text.pos,cmd->text.color);
              break;
        case MU_COMMAND_RECT:

            r_draw_rect(cmd->rect.rect, cmd->rect.color);

            break;
        case MU_COMMAND_ICON:


            gsi_texture(&gsi,atlas_tex);
            r_draw_icon(cmd->icon.id, cmd->icon.rect, cmd->icon.color);
            break;
        case MU_COMMAND_CLIP:
            gs_graphics_set_view_scissor(&gcb,cmd->clip.rect.x,cmd->clip.rect.y,cmd->clip.rect.w,cmd->clip.rect.h);
            break;
      }
    }
    gsi_render_pass_submit(&gsi, &gcb, gs_color(bg[0],bg[1],bg[2], 255.f));
    gs_graphics_submit_command_buffer(&gcb);
}

gs_app_desc_t gs_main(int32_t argc, char** argv)
{
    return (gs_app_desc_t){
        .init = init,
        .update = update,
        .window_width = 1024,
        .window_height = 760,
        .frame_rate = 100000
    };
}
