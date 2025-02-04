#include <string>
#include <windows.h>

#include <GL/Gl.h>
#include "glext.h"
#include "wglext.h"

// API: implemented in user code
void draw();
void setup();

inline void *GetAnyGLFuncAddress(const char *name) {
  void *p = (void *)wglGetProcAddress(name);
  if (p == 0 || (p == (void *)0x1) || (p == (void *)0x2) ||
      (p == (void *)0x3) || (p == (void *)-1)) {
    HMODULE module = LoadLibraryA("opengl32.dll");
    p = (void *)GetProcAddress(module, name);
  }
  return p;
}

// generated code beg
#include "def_glext.h"
// generated code end

namespace brocgl {

std::pair<HWND, HDC> createWindow(HINSTANCE hInstance, int nCmdShow,
                                  WNDPROC windowProc, boolean show) {
  const char *className = "className";
  WNDCLASSA wc = {.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
                  .lpfnWndProc = windowProc,
                  .hInstance = hInstance,
                  .lpszClassName = className};
  RegisterClassA(&wc);
  HWND hwnd = CreateWindowExA(
      0, className, "windowTitle",
      WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, CW_USEDEFAULT,
      CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, hInstance, 0);
  if (show) {
    ShowWindow(hwnd, nCmdShow);
  }

  return {hwnd, GetDC(hwnd)};
}

bool initialized = false;

std::pair<HWND, HDC> createOpenGLWindow(HINSTANCE hInstance, int nCmdShow,
                                        WNDPROC windowProc) {
  auto [dummyWindow, dummyHdc] =
      createWindow(hInstance, nCmdShow, windowProc, false);

  PIXELFORMATDESCRIPTOR pfd = {.nSize = sizeof(PIXELFORMATDESCRIPTOR),
                               .nVersion = 1,
                               .dwFlags =
                                   PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL,
                               .iPixelType = PFD_TYPE_RGBA,
                               .cColorBits = 32,
                               .cAlphaBits = 8,
                               .cDepthBits = 24};

  int dummyPixelFormat = ChoosePixelFormat(dummyHdc, &pfd);
  DescribePixelFormat(dummyHdc, dummyPixelFormat, sizeof(PIXELFORMATDESCRIPTOR),
                      &pfd);
  SetPixelFormat(dummyHdc, dummyPixelFormat, &pfd);

  // #################### create dummy gl context ####################
  HGLRC dummyHglrc = wglCreateContext(dummyHdc);
  wglMakeCurrent(dummyHdc, dummyHglrc);

  // ### get extensions ###
  auto wglGetExtensionsStringARB =
      (PFNWGLGETEXTENSIONSSTRINGARBPROC)GetAnyGLFuncAddress(
          "wglGetExtensionsStringARB");

  std::string extensionsString(wglGetExtensionsStringARB(dummyHdc));

  _ASSERTE(extensionsString.find("WGL_ARB_pixel_format") != std::string::npos);

  auto wglChoosePixelFormatARB =
      (PFNWGLCHOOSEPIXELFORMATARBPROC)GetAnyGLFuncAddress(
          "wglChoosePixelFormatARB");

  const int attribList[] = {
      WGL_DRAW_TO_WINDOW_ARB,
      GL_TRUE,
      WGL_SUPPORT_OPENGL_ARB,
      GL_TRUE,
      WGL_DOUBLE_BUFFER_ARB,
      GL_TRUE,
      WGL_PIXEL_TYPE_ARB,
      WGL_TYPE_RGBA_ARB,
      WGL_COLOR_BITS_ARB,
      32,
      WGL_DEPTH_BITS_ARB,
      24,
      WGL_STENCIL_BITS_ARB,
      8,
      0, // End
  };
  auto [hwnd, hdc] = createWindow(hInstance, nCmdShow, windowProc, true);

  int pixelFormat;
  UINT numFormats;
  wglChoosePixelFormatARB(hdc, attribList, 0, 1, &pixelFormat, &numFormats);

  PIXELFORMATDESCRIPTOR realPFD = {.nSize = sizeof(PIXELFORMATDESCRIPTOR)};
  DescribePixelFormat(hdc, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR),
                      &realPFD);
  SetPixelFormat(hdc, pixelFormat, &realPFD);

  _ASSERTE(extensionsString.find("WGL_ARB_create_context") !=
           std::string::npos);
  auto wglCreateContextAttribsARB =
      (PFNWGLCREATECONTEXTATTRIBSARBPROC)GetAnyGLFuncAddress(
          "wglCreateContextAttribsARB");

  const int contextAttribList[] = {
      WGL_CONTEXT_MAJOR_VERSION_ARB,
      4,
      WGL_CONTEXT_MINOR_VERSION_ARB,
      6,
      WGL_CONTEXT_FLAGS_ARB,
      0,
      0, // end
  };
  HGLRC hglrc = wglCreateContextAttribsARB(hdc, 0, contextAttribList);
  wglMakeCurrent(0, 0);
  wglDeleteContext(dummyHglrc);
  wglMakeCurrent(hdc, hglrc);

  DestroyWindow(dummyWindow);

  loadGlFunctions();

  initialized = true;

  return {hwnd, hdc};
}

int running = 2;

LRESULT CALLBACK windowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
                            LPARAM lParam) {
  switch (uMsg) {
  case WM_DESTROY: {
    --running;
  } break;
  case WM_SIZE: {
    glViewport(0, 0, LOWORD(lParam), HIWORD(lParam));
  } break;
  case WM_PAINT: {
    if (initialized) {
      draw();
      SwapBuffers(GetDC(hwnd));
    }
  } break;
  default:
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
  }
  return 0;
}

void run(HINSTANCE hInstance, int nCmdShow, WNDPROC windowProc) {
  AllocConsole();
  freopen("CONIN$", "r", stdin);
  freopen("CONOUT$", "w", stdout);

  auto [hwnd, hdc] = createOpenGLWindow(hInstance, nCmdShow, windowProc);

  setup();

  auto processWindowsMessages = []() {
    MSG message;
    while (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
      switch (message.message) {
      case WM_QUIT: {
        running = 0;
      } break;
      default: {
        TranslateMessage(&message);
        DispatchMessage(&message);
      }
      }
    }
  };
  while (running) {
    processWindowsMessages();
    draw();
    SwapBuffers(hdc);
  }
}
} // namespace

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PWSTR pCmdLine, int nCmdShow) {
  brocgl::run(hInstance, nCmdShow, brocgl::windowProc);
  return 0;
}