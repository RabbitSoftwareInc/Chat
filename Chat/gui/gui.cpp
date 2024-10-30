#include "gui.h"

#include "../libraries/imgui/imgui.h"

#include "../libraries/imgui/imgui_impl_win32.h"
#include "../libraries/imgui/imgui_impl_dx11.h"

#include "../networking/networking.h"
#include "../client_state.h"

c_gui* g_gui = new c_gui;

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void c_gui::initialize()
{
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"chat_window", NULL };
    RegisterClassEx(&wc);

    HDC hDCScreen = GetDC(NULL);
    int Horres = GetDeviceCaps(hDCScreen, HORZRES);
    int Vertres = GetDeviceCaps(hDCScreen, VERTRES);
    ReleaseDC(NULL, hDCScreen);

    HWND hwnd = CreateWindowW(L"chat_window", L"Chat", WS_POPUP, Horres / 2 - 200, Vertres / 2 - 150, 550, 365, NULL, NULL, wc.hInstance, NULL);

    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return;
    }

    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/Verdana.ttf", 14, NULL, io.Fonts->GetGlyphRangesCyrillic());

    ImGui::StyleColorsDark();

    ImGui::GetStyle().WindowPadding = ImVec2(10, 10);

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    bool done = true;

    while (done)
    {
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }

        if (!done)
            break;

        if (g_SwapChainOccluded && g_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
        {
            ::Sleep(10);
            continue;
        }

        g_SwapChainOccluded = false;

        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

            if (ImGui::Begin(ws2s(L"Chat v1.6 GUI (Chatting as " + g_client_state->current_user_name + L")").c_str(), &done, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_NoBringToFrontOnFocus))
            {
                if (g_networking->is_connected())
                {
                    ImGui::BeginListBox("##MESSAGE_LIST", ImVec2(ImGui::GetIO().DisplaySize.x - 20, ImGui::GetIO().DisplaySize.y - 64));

                    for (int i = 0; i < g_client_state->message_history.size(); i++)
                        ImGui::Selectable(ws2s(g_client_state->message_history.at(i)).c_str(), false);

                    ImGui::EndListBox();

                    static char message_buffer[1152];

                    ImGui::SetNextItemWidth(ImGui::GetIO().DisplaySize.x - 80);
                    ImGui::InputText("##MESSAGE_INPUT", message_buffer, 1152);

                    g_client_state->message_text = s2ws(message_buffer);

                    ImGui::SameLine();

                    if (ImGui::Button("Send", ImVec2(50, 19)) || ImGui::IsKeyPressed(ImGuiKey_Enter, false))
                    {
                        for (int i = 0; i < 1152; i++)
                            message_buffer[i] = '\0';

                        g_client_state->should_send_message = true;
                    }
                }
                else
                    ImGui::Text((g_networking->is_server() ? "Waiting for client..." : "Connecting..."));

                ImGui::End();
            }

        ImGui::Render();

        const float clear_color_with_alpha[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);

        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        HRESULT hr = g_pSwapChain->Present(1, 0);
        g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();

    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    TerminateProcess(GetCurrentProcess(), 0);
}

bool CreateDeviceD3D(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };

    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_gui->g_pSwapChain, &g_gui->g_pd3dDevice, &featureLevel, &g_gui->g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED)
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_gui->g_pSwapChain, &g_gui->g_pd3dDevice, &featureLevel, &g_gui->g_pd3dDeviceContext);
   
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_gui->g_pSwapChain) { g_gui->g_pSwapChain->Release(); g_gui->g_pSwapChain = nullptr; }
    if (g_gui->g_pd3dDeviceContext) { g_gui->g_pd3dDeviceContext->Release(); g_gui->g_pd3dDeviceContext = nullptr; }
    if (g_gui->g_pd3dDevice) { g_gui->g_pd3dDevice->Release(); g_gui->g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;

    g_gui->g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_gui->g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_gui->g_mainRenderTargetView);

    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_gui->g_mainRenderTargetView) { g_gui->g_mainRenderTargetView->Release(); g_gui->g_mainRenderTargetView = nullptr; }
}

bool can_drag(HWND hwnd)
{
    RECT rect;
    if (!GetWindowRect(hwnd, &rect))
        return false;

    POINT cur;
    GetCursorPos(&cur);

    LONG y = cur.y - rect.top;
    LONG x = cur.x - rect.left;
    return y <= 20 && x <= (rect.right - rect.left) - 20;
};

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_gui->g_ResizeWidth = (UINT)LOWORD(lParam);
        g_gui->g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    case WM_NCHITTEST:
        if (can_drag(hWnd))
            return HTCAPTION;
        break;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}