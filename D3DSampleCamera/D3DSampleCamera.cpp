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


// D3D 객체와 Device 객체 생성
HRESULT InitD3D( HWND hWnd )
{
    // D3D 객체 생성
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


    // D3D Device 객체 생성
    if (g_pd3dDevice != NULL) g_pd3dDevice->Release();
	if( FAILED( g_pD3D->CreateDevice (D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                        &d3dpp, &g_pd3dDevice))) {
        return E_FAIL;
    }

    // 카메라 객체 생성
    camera = new CCamera(0, 0, 0);
    camera->Init(g_pd3dDevice);


    // 각 큐브의 위치 정보 배열
    int ix[10] = { 0, -2, -2, 2, 2, -3, -5, 5, 7, 0 };
    int iy[10] = { 0, -2, 2, -2, 2, -5, -7, -1, -8, 6 };
    int iz[10] = { 2, 3, 4, 5, 3, 7, 30, 10, 31, 4 };

    // 각 큐브 생성
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

    // 카메라의 초기 목표 설정
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

        // Projection Matrix 설정
        D3DXMATRIX tempProjection;
        D3DXMatrixPerspectiveFovLH(&tempProjection, D3DXToRadian(90.f), 1.f, 0.01f, 100.f);
        g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &tempProjection);

        // View Matrix를 카메라의 설정에 맞게 설정
        camera->SetViewTransform(g_pd3dDevice);

        // Z 버퍼 활성화
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
        g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);  
        g_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);  


        // 광원을 사용하지 않음으로 vertex의 rgb 값만을 출력에 사용.
        g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

        // 모든 큐브 렌더링
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
            // 윈도우 종료 시 D3D 객체 및 디바이스 해제
            Cleanup();
            PostQuitMessage( 0 );
            return 0;
        case WM_KEYDOWN:
            switch (wParam) {
                // 숫자 키 입력시 카메라 추적 대상 큐브 변경
            case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                controlCube = (int)(wParam - '0');
                camera->SetTarget(*cube[controlCube]);
                break;
            }
        case WM_SIZE:
            InvalidateRect(hWnd, NULL, TRUE);
            break;
        case WM_PAINT:
            // WM_PAINT 발생 시 D3D Render Sequence 실행
            // validate window rect = didn't loop WM_PAINT
            ValidateRect( hWnd, NULL );
            return 0;
    }
    return DefWindowProc( hWnd, msg, wParam, lParam );
}
 

INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    // 윈도우 클래스 등록
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L, 
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      L"D3D Tutorial", NULL };
    RegisterClassEx( &wc );
    // 윈도우 생성
    HWND hWnd = CreateWindow( L"D3D Tutorial", L"D3D Tutorial 01: CreateDevice", 
                              WS_OVERLAPPEDWINDOW, 100, 100, 500, 500,
                              GetDesktopWindow(), NULL, wc.hInstance, NULL );
    timer = new GameTimer();
    // Direct3D 초기화
    // D3D 초기화 성공 시 윈도우 출력 및 메시지 루프
    if( SUCCEEDED( InitD3D( hWnd ) ) )
    { 
        // 윈도우 출력
        ShowWindow( hWnd, SW_SHOWDEFAULT );
        UpdateWindow( hWnd );
        // 메시지 루프
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
                // 60프레임으로 작동
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
    // 메시지 루프 종료 = PostQuitMessage() or D3D 초기화 실패 시 등록된 WNDCLASSEX 소거 및 프로그램 종료
    // 등록된 클래스 소거
    UnregisterClass( L"D3D Tutorial", wc.hInstance );
    return 0;
}