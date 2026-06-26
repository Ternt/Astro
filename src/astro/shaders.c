// 2026-06-19

const char r_geo2d_vs_shader_src[] = 
{
#if defined(GRAPHICS_API_OPENGL_33)
"#version 330 core                  \n"

"in vec2  in_pos;                   \n"
"in vec2  in_tr;                    \n"
"in float in_rt;                    \n"
"in vec2  in_sc;                    \n"

"uniform mat4 u_view;               \n"
"uniform mat4 u_proj;               \n"
#endif

#if defined(GRAPHICS_API_OPENGL_ES3)
"#version 300 es                    \n"
"precision mediump float;           \n"

"in vec2  in_pos;                   \n"
"in vec2  in_tr;                    \n"
"in float in_rt;                    \n"
"in vec2  in_sc;                    \n"

"uniform mat4 u_view;               \n"
"uniform mat4 u_proj;               \n"
#endif

"mat4 translation2d(vec2 tr)        \n"
"{                                  \n"
"  return mat4(                     \n"
"    vec4(1.0, 0.0, 0.0, 0.0),      \n"
"    vec4(0.0, 1.0, 0.0, 0.0),      \n"
"    vec4(0.0, 0.0, 1.0, 0.0),      \n"
"    vec4(tr.x, tr.y, 0.0, 1.0)     \n"
"  );                               \n"
"}                                  \n"

"mat4 rotation2d(float angle)       \n"
"{                                  \n"
"  float c = cos(angle);            \n"
"  float s = sin(angle);            \n"
"  return mat4(                     \n"
"    vec4(  c,    s,  0.0, 0.0),    \n"
"    vec4( -s,    c,  0.0, 0.0),    \n"
"    vec4( 0.0,  0.0, 1.0, 0.0),    \n"
"    vec4( 0.0,  0.0, 0.0, 1.0)     \n"
"  );                               \n"
"}                                  \n"

"mat4 scale2d(vec2 sc)              \n"
"{                                  \n"
"  return mat4(                     \n"
"    vec4(sc.x, 0.0,  0.0, 0.0),    \n"
"    vec4(0.0,  sc.y, 0.0, 0.0),    \n"
"    vec4(0.0,  0.0,  1.0, 0.0),    \n"
"    vec4(0.0,  0.0,  0.0, 1.0)     \n"
"  );                               \n"
"}                                  \n"

"void main(void)                    \n"
"{                                  \n"
"  mat4 model = translation2d(in_tr) * rotation2d(in_rt) * scale2d(in_sc);\n"
"  gl_Position = u_proj*u_view*model*vec4(in_pos, 0.0, 1.0);\n"
"}                                  \n"
};

const char r_geo2d_ps_shader_src[] = 
{
#if defined(GRAPHICS_API_OPENGL_33)
"#version 330 core                  \n"
"out vec4 frag_color;               \n"
#endif

#if defined(GRAPHICS_API_OPENGL_ES3)
"#version 300 es                    \n"
"precision mediump float;           \n"
"out vec4 frag_color;               \n"
#endif

"void main()                        \n"
"{                                  \n"
"  frag_color = vec4(1.0, 1.0, 1.0, 1.0);\n"
"}                                  \n"
};

const struct { const char *data; u32 size; } r_vs_shader_src[R_PassType_COUNT] = {
  { r_geo2d_vs_shader_src, sizeof(r_geo2d_vs_shader_src) },
};

const struct { const char *data; u32 size; } r_ps_shader_src[R_PassType_COUNT] = {
  { r_geo2d_ps_shader_src, sizeof(r_geo2d_ps_shader_src) },
};
