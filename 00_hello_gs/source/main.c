/*================================================================
    * Copyright: 2020 John Jackson
    * HelloGS

    A Bare bones application for getting started using `gunslinger`.
    Creates an appplication context, an engine context, and then 
    opens a main window for you using the rendering context.

    Press `esc` to exit the application.
=================================================================*/
#define _wfindfirst32i64  _wfindfirsti64
#define _wfindnext32i64  _wfindnexti64
#define GS_IMPL
#include <gs/gs.h>

void update()
{
    if (gs_platform_key_pressed(GS_KEYCODE_ESC)) gs_engine_quit();
}

gs_app_desc_t gs_main(int32_t argc, char** argv)
{
    return (gs_app_desc_t){
        .update = update
    };
}   