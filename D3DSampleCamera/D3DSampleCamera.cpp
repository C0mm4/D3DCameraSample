#include <d3d9.h>
#include <d3dx9math.h>
#include "Cube.h"
#include "GameTimer.h"
#include "CCamera.h"


LPDIRECT3D9             g_pD3D       = NULL;   
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL;    
CCamera* camera;
Cube* cube[10];
GameTimer* timer;


// D3D ��ü�� Device ��ü ����
HRESULT InitD3D( HWND hWnd )
{
    // D3D ��ü ����
    D3DPRESENT_PARAMETERS d3dpp;
    if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return E_FAIL;
    ZeroMemory( &d3dpp, sizeof(d3dpp) );               
    d3dpp.Windowed = TRUE;                                 
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;    
    d3dpp.BackBufferCount = 2;

    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;


    // D3D Device ��ü ����
    if (g_pd3dDevice != NULL) g_pd3dDevice->Release();
	if( FAILED( g_pD3D->CreateDevice (D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                        &d3dpp, &g_pd3dDevice))) {
        return E_FAIL;
    }

    // ī�޶� ��ü ����
    camera = new CCamera(0, 0, 0);
    camera->Init(g_pd3dDevice);


    // �� ť���� ��ġ ���� �迭
    int ix[10] = { 0, -2, -2, 2, 2, -3, -5, 5, 7, 0 };
    int iy[10] = { 0, -2, 2, -2, 2, -5, -7, -1, -8, 6 };
    int iz[10] = { 2, 3, 4, 5, 3, 7, 30, 10, 31, 4 };

    // �� ť�� ����
    for (int i = 0; i < 10; i++) {

        cube[i] = new Cube(ix[i], iy[i], iz[i]);
        HRESULT result = cube[i]->Init(g_pd3dDevice);
        if (SUCCEEDED(result)) {
            OutputDebugStringA("Cube Create Successfully\n");
        }
        else {
            delete cube;
            cube[i] = NULL;
        }
    }

    // ī�޶��� �ʱ� ��ǥ ����
    camera->SetTarget(*cube[0]);
    return S_OK;
}

VOID Cleanup()
{
    if( g_pd3dDevice != NULL) 
        g_pd3dDevice->Release();
    if( g_pD3D != NULL)
        g_pD3D->Release();
}

VOID Render()
{
    if( NULL == g_pd3dDevice )
        return;
    // Clear Device
    g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255,255,255), 1.0f, 1);    

    // Lock d3d Device
    if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
    {
        D3DXMATRIX tempTM;
        D3DXMatrixIdentity(&tempTM);

        // Projection Matrix ����
        D3DXMATRIX tempProjection;
        D3DXMatrixPerspectiveFovLH(&tempProjection, D3DXToRadian(90.f), 1.f, 0.01f, 100.f);
        g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &tempProjection);

        // View Matrix�� ī�޶��� ������ �°� ����
        camera->SetViewTransform(g_pd3dDevice);

        // Z ���� Ȱ��ȭ
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
        g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);  
        g_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);  


        // ������ ������� �������� vertex�� rgb ������ ��¿� ���.
        g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

        // ��� ť�� ������
        for (int i = 0; i < 10; i++) {
            if (cube[i])
                cube[i]->Render(g_pd3dDevice);
        }


        // Unlock d3d Device
        g_pd3dDevice->EndScene();

    }
    // Flip Device buffer
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}

int controlCube = 0;

void KeyInput(float deltaT) {

    cube[controlCube]->KeyInput(deltaT);

}


LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    static bool isActive;
    switch( msg )
    {
        case WM_DESTROY:
            // ������ ���� �� D3D ��ü �� ����̽� ����
            Cleanup();
            PostQuitMessage( 0 );
            return 0;
        case WM_KEYDOWN:
            switch (wParam) {
                // ���� Ű �Է½� ī�޶� ���� ��� ť�� ����
            case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                controlCube = (int)(wParam - '0');
                camera->SetTarget(*cube[controlCube]);
                break;
            }
        case WM_SIZE:
            InvalidateRect(hWnd, NULL, TRUE);
            break;
        case WM_PAINT:
            // WM_PAINT �߻� �� D3D Render Sequence ����
            // validate window rect = didn't loop WM_PAINT
            ValidateRect( hWnd, NULL );
            return 0;
    }
    return DefWindowProc( hWnd, msg, wParam, lParam );
}
 

INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    // ������ Ŭ���� ���
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L, 
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      L"D3D Tutorial", NULL };
    RegisterClassEx( &wc );
    // ������ ����
    HWND hWnd = CreateWindow( L"D3D Tutorial", L"D3D Tutorial 01: CreateDevice", 
                              WS_OVERLAPPEDWINDOW, 100, 100, 500, 500,
                              GetDesktopWindow(), NULL, wc.hInstance, NULL );
    timer = new GameTimer();
    // Direct3D �ʱ�ȭ
    // D3D �ʱ�ȭ ���� �� ������ ��� �� �޽��� ����
    if( SUCCEEDED( InitD3D( hWnd ) ) )
    { 
        // ������ ���
        ShowWindow( hWnd, SW_SHOWDEFAULT );
        UpdateWindow( hWnd );
        // �޽��� ����
        MSG msg; 
        float deltaT = 0;
        while (true) {
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) {
                    break;
                }
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else {
                // 60���������� �۵�
                deltaT += timer->getDeltaTime();
                if (deltaT >= 1.f / 60.f) {
                    KeyInput(deltaT);
                    camera->Update(deltaT);
                    Render();
                    deltaT = 0;
                }
            }
        }
    }
    // �޽��� ���� ���� = PostQuitMessage() or D3D �ʱ�ȭ ���� �� ��ϵ� WNDCLASSEX �Ұ� �� ���α׷� ����
    // ��ϵ� Ŭ���� �Ұ�
    UnregisterClass( L"D3D Tutorial", wc.hInstance );
    return 0;
}