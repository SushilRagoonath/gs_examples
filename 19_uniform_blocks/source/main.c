/*================================================================
    * Copyright: 2020 John Jackson
    * uniforms_buffers

    The purpose of this example is to demonstrate how to bind and upload 
    uniform data to the GPU using pipelines and uniform buffers.

    Included: 
        * Construct vertex/index buffers from user defined declarations
        * Construct shaders from source
        * Construct uniforms and bind data to pass to GPU
        * Construct pipelines
        * Rendering via command buffers

    Press `esc` to exit the application.
================================================================*/

#define GS_IMPL
#include <gs/gs.h>

gs_command_buffer_t                      cb          = {0};
gs_camera_t                              cam         = {0};
gs_handle(gs_graphics_vertex_buffer_t)   vbo         = {0};
gs_handle(gs_graphics_pipeline_t)        pips[4]     = {0};
gs_handle(gs_graphics_shader_t)          shaders[4]  = {0};
gs_handle(gs_graphics_uniform_t)         u_model     = {0};
gs_handle(gs_graphics_uniform_buffer_t)  u_vp        = {0};

const char* v_src =
    "#version 330\n"
    "layout(location = 0) in vec3 a_pos;\n"
    "layout (std140) uniform u_vp {\n"
    "   mat4 projection;\n"
    "   mat4 view;\n"
    "};\n"
    "uniform mat4 u_model;\n"
    "void main() {\n"
    "   gl_Position = projection * view * u_model * vec4(a_pos, 1.0);\n"
    "}\n";
    
const char* f_red_src =
    "#version 330\n"
    "layout(location = 0) out vec4 frag_color;\n"
    "void main() {\n"
    "   frag_color = vec4(1.0, 0.0, 0.0, 1.0);\n"
    "}\n";

const char* f_blue_src =
    "#version 330\n"
    "layout(location = 0) out vec4 frag_color;\n"
    "void main() {\n"
    "   frag_color = vec4(0.0, 0.0, 1.0, 1.0);\n"
    "}\n";

const char* f_green_src =
    "#version 330\n"
    "layout(location = 0) out vec4 frag_color;\n"
    "void main() {\n"
    "   frag_color = vec4(0.0, 1.0, 0.0, 1.0);\n"
    "}\n";

const char* f_yellow_src =
    "#version 330\n"
    "layout(location = 0) out vec4 frag_color;\n"
    "void main() {\n"
    "   frag_color = vec4(0.0, 1.0, 1.0, 1.0);\n"
    "}\n";

typedef struct vparams_t
{
    gs_mat4 projection;
    gs_mat4 view;
} vparams_t;

void app_init()
{
    // Construct new command buffer
    cb = gs_command_buffer_new(); 

    // Set up camera
    cam = gs_camera_perspective();
    cam.transform.position = gs_v3(0.f, 0.f, 3.f);

    // Cube positions
    float v_data[] = {
        // positions         
        -0.5f, -0.5f, -0.5f, 
         0.5f, -0.5f, -0.5f,  
         0.5f,  0.5f, -0.5f,  
         0.5f,  0.5f, -0.5f,  
        -0.5f,  0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 

        -0.5f, -0.5f,  0.5f, 
         0.5f, -0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f, 
        -0.5f, -0.5f,  0.5f, 

        -0.5f,  0.5f,  0.5f, 
        -0.5f,  0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 
        -0.5f, -0.5f,  0.5f, 
        -0.5f,  0.5f,  0.5f, 

         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f, -0.5f,  
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  

        -0.5f, -0.5f, -0.5f, 
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f,  0.5f,  
         0.5f, -0.5f,  0.5f,  
        -0.5f, -0.5f,  0.5f, 
        -0.5f, -0.5f, -0.5f, 

        -0.5f,  0.5f, -0.5f, 
         0.5f,  0.5f, -0.5f,  
         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f, 
        -0.5f,  0.5f, -0.5f, 
    };

    // Construct vertex buffer
    vbo = gs_graphics_vertex_buffer_create(
        &(gs_graphics_vertex_buffer_desc_t) {
            .data = v_data,
            .size = sizeof(v_data)
        }
    );

    // Create uniform buffer
    u_vp = gs_graphics_uniform_buffer_create(
        &(gs_graphics_uniform_buffer_desc_t){
            .data = NULL,
            .size = sizeof(vparams_t),
            .name = "u_vp"
        } 
    );

    // Upload projection matrix into buffer
    gs_vec2 ws = gs_platform_window_sizev(gs_platform_main_window());
    gs_mat4 proj = gs_camera_get_projection(&cam, (int32_t)ws.x, (int32_t)ws.y);

    // Update sub region of uniform buffer data with projection (won't change during runtime) 
    gs_graphics_uniform_buffer_request_update(
        &cb, u_vp, 
        &(gs_graphics_uniform_buffer_desc_t){
            .data = &proj,
            .size = sizeof(proj),
            .update = {
                .type = GS_GRAPHICS_BUFFER_UPDATE_SUBDATA,
                .offset = 0
            }
        }
    );

    u_model = gs_graphics_uniform_create (
        &(gs_graphics_uniform_desc_t) {
            .name = "u_model",
            .layout = (gs_graphics_uniform_layout_desc_t[]){{.type = GS_GRAPHICS_UNIFORM_MAT4}}
        }
    );

    const char* fshaders[4] = {
        f_red_src,
        f_blue_src,
        f_green_src,
        f_yellow_src
    };

    // Create shaders and pipelines
    for (uint32_t i = 0; i < 4; ++i)
    {
        shaders[i] = gs_graphics_shader_create (
            &(gs_graphics_shader_desc_t) {
                .sources = (gs_graphics_shader_source_desc_t[]){
                    {.type = GS_GRAPHICS_SHADER_STAGE_VERTEX, .source = v_src},
                    {.type = GS_GRAPHICS_SHADER_STAGE_FRAGMENT, .source = fshaders[i]}
                }, 
                .size = 2 * sizeof(gs_graphics_shader_source_desc_t),
                .name = "color_shader"
            }
        );

        pips[i] = gs_graphics_pipeline_create (
            &(gs_graphics_pipeline_desc_t) {
                .raster = {
                    .shader = shaders[i]
                },
                .blend = {
                    .func = GS_GRAPHICS_BLEND_EQUATION_ADD,
                    .src = GS_GRAPHICS_BLEND_MODE_SRC_ALPHA,
                    .dst = GS_GRAPHICS_BLEND_MODE_ONE_MINUS_SRC_ALPHA              
                },
                .depth = {
                    .func = GS_GRAPHICS_DEPTH_FUNC_LESS
                },
                .layout = {
                    .attrs = (gs_graphics_vertex_attribute_desc_t[]) {
                        {.format = GS_GRAPHICS_VERTEX_ATTRIBUTE_FLOAT3}        // Position
                    },
                    .size = sizeof(gs_graphics_vertex_attribute_desc_t)
                }
            }
        );
    }
}

void app_update()
{
    if (gs_platform_key_pressed(GS_KEYCODE_ESC)) gs_engine_quit();

    gs_vec2 fs = gs_platform_framebuffer_sizev(gs_platform_main_window());
    gs_vec2 ws = gs_platform_window_sizev(gs_platform_main_window());

    // Action for clearing the screen
    gs_graphics_clear_desc_t clear = (gs_graphics_clear_desc_t){
        .actions = &(gs_graphics_clear_action_t){.color = {0.1f, 0.1f, 0.1f, 1.f}}
    };

    gs_mat4 view = gs_camera_get_view(&cam);

    // Request buffer upate for view once per frame to be shared across pipelines
    gs_graphics_uniform_buffer_request_update(
        &cb, u_vp, 
        &(gs_graphics_uniform_buffer_desc_t){
            .data = &view,
            .size = sizeof(view),
            .update = {
                .type = GS_GRAPHICS_BUFFER_UPDATE_SUBDATA,
                .offset = sizeof(gs_mat4)
            }
        }
    );

    const float t = gs_platform_elapsed_time() * 0.001f;

    gs_mat4 models[] = {
        gs_mat4_mul(gs_mat4_translate(-0.75f, 0.75f, 0.0f), gs_mat4_rotatev(t, GS_XAXIS)),
        gs_mat4_mul(gs_mat4_translate(0.75f, 0.75f, 0.0f), gs_mat4_rotatev(t, GS_YAXIS)),
        gs_mat4_mul(gs_mat4_translate(-0.75f, -0.75f, 0.0f), gs_mat4_rotatev(t, GS_ZAXIS)),
        gs_mat4_mul(gs_mat4_translate(0.75f, -0.75f, 0.0f), gs_mat4_rotatev(t, gs_v3(1.f, 0.f, 1.f)))
    };

    /* Render */
    gs_graphics_begin_render_pass(&cb, GS_GRAPHICS_RENDER_PASS_DEFAULT);
    {
        // Set viewport
        gs_graphics_set_viewport(&cb, 0, 0, (uint32_t)fs.x, (uint32_t)fs.y);

        // Clear screen
        gs_graphics_clear(&cb, &clear);

        // For each pipeline
        for (uint32_t i = 0; i < 4; ++i)
        {
            gs_graphics_bind_pipeline(&cb, pips[i]);

            // Binding descriptor for uniform buffer, uniform, and vertex buffer
            gs_graphics_bind_desc_t binds = {
                .vertex_buffers = {&(gs_graphics_vertex_buffer_bind_desc_t){.buffer = vbo}},
                .uniform_buffers = {&(gs_graphics_uniform_buffer_bind_desc_t){.buffer = u_vp, .binding = 0}},
                .uniforms = {&(gs_graphics_uniform_bind_desc_t){.uniform = u_model, .data = &models[i]}}
            };

            // Set bindings to be bound
            gs_graphics_set_bindings(&cb, &binds);

            // Draw to screen
            gs_graphics_draw(&cb, &(gs_graphics_draw_desc_t){.start = 0, .count = 36});
        }
    }
    gs_graphics_end_render_pass(&cb);

    // Submit command buffer (syncs to GPU, MUST be done on main thread where you have your GPU context created)
    gs_graphics_submit_command_buffer(&cb);
}

gs_app_desc_t gs_main(int32_t argc, char** argv)
{
    return (gs_app_desc_t){
        .init = app_init,
        .update = app_update
    };
}   






