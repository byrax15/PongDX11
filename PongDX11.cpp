#include "framework.h"
#include <bit>

using Microsoft::WRL::ComPtr;

ComPtr<IDXGISwapChain> swapchain;
ComPtr<ID3D11Device> device;
ComPtr<ID3D11DeviceContext> context;
ComPtr<ID3D11RenderTargetView> backbuffer;

void InitD3D(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC scd{
        .BufferDesc = {.Format = DXGI_FORMAT_R8G8B8A8_UNORM},
        .SampleDesc = {.Count = 4},
        .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
        .BufferCount = 1,
        .OutputWindow = hWnd,
        .Windowed = TRUE,
    };
    D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL, D3D11_SDK_VERSION, &scd,
                                  swapchain.GetAddressOf(), device.GetAddressOf(), NULL, context.GetAddressOf());

    ComPtr<ID3D11Texture2D> pbackbuf;
    swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), std::bit_cast<LPVOID *>(pbackbuf.GetAddressOf()));
    device->CreateRenderTargetView(pbackbuf.Get(), NULL, backbuffer.GetAddressOf());
    context->OMSetRenderTargets(1, backbuffer.GetAddressOf(), NULL);

    D3D11_VIEWPORT viewport{
        .TopLeftX = 0,
        .TopLeftY = 0,
        .Width = 800,
        .Height = 600,
    };
    context->RSSetViewports(1, &viewport);
}

void RenderFrame()
{
    context->ClearRenderTargetView(backbuffer.Get(), DirectX::Colors::CornflowerBlue);
    swapchain->Present(0, 0);
}

static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // sort through and find what code to run for the message given
    switch (message)
    {
        // this message is read when the window is closed
    case WM_DESTROY: {
        // close the application entirely
        PostQuitMessage(0);
        return 0;
    }
    break;
    }

    // Handle any messages the switch statement didn't
    return DefWindowProc(hWnd, message, wParam, lParam);
}

// the entry point for any Windows program
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    // the handle for the window, filled by a function
    HWND hWnd;
    // this struct holds information for the window class
    WNDCLASSEX wc{
        .cbSize = sizeof(WNDCLASSEX),
        .style = CS_HREDRAW | CS_VREDRAW,
        .lpfnWndProc = WindowProc,
        .hInstance = hInstance,
        .hCursor = LoadCursor(NULL, IDC_ARROW),
        .hbrBackground = (HBRUSH)COLOR_WINDOW,
        .lpszClassName = L"WindowClass1",
    };

    // register the window class
    RegisterClassEx(&wc);

    RECT winRect{0, 0, 500, 400};
    AdjustWindowRect(&winRect, WS_OVERLAPPEDWINDOW, FALSE);

    // create the window and use the result as the handle
    hWnd = CreateWindowEx(NULL,
                          L"WindowClass1",               // name of the window class
                          L"Our First Windowed Program", // title of the window
                          WS_OVERLAPPEDWINDOW,           // window style
                          300,                           // x-position of the window
                          300,                           // y-position of the window
                          winRect.right - winRect.left,  // width of the window
                          winRect.bottom - winRect.top,  // height of the window
                          NULL,                          // we have no parent window, NULL
                          NULL,                          // we aren't using menus, NULL
                          hInstance,                     // application handle
                          NULL);                         // used with multiple windows, NULL

    // display the window on the screen
    ShowWindow(hWnd, nCmdShow);
    InitD3D(hWnd);

    // enter the main loop:

    // this struct holds Windows event messages
    MSG msg;

    // wait for the next message in the queue, store the result in 'msg'
    while (TRUE)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                break;
        }
        RenderFrame();
    }

    // return this part of the WM_QUIT message to Windows
    return static_cast<int>(msg.wParam);
}