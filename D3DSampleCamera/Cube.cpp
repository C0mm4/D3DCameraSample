#include "Cube.h"



HRESULT Cube::Init(LPDIRECT3DDEVICE9 device) {

    // 큐브의 정점 초기화
    CUSTOMVERTEX vertices[] =
    {
        {-0.5f, -0.5f, -0.5f, D3DCOLOR_ARGB(255,255, 0, 0)},
        {0.5f, -0.5f, -0.5f, D3DCOLOR_ARGB(255,0, 255, 0)},
        {0.5f, 0.5f, -0.5f, D3DCOLOR_ARGB(255,0, 0, 255)},
        {-0.5f, 0.5f, -0.5f, D3DCOLOR_ARGB(255,255, 255, 255)},

        {-0.5f, -0.5f, 0.5f, D3DCOLOR_ARGB(255,255, 255, 0)},
        {0.5f, -0.5f, 0.5f, D3DCOLOR_ARGB(255,0, 255, 255)},
        {0.5f, 0.5f, 0.5f, D3DCOLOR_ARGB(255,255, 0, 255)},
        {-0.5f, 0.5f, 0.5f, D3DCOLOR_ARGB(255,0,0,0)},

    };

    // Vertex Buffer 생성
    if (FAILED(device->CreateVertexBuffer(sizeof(vertices), 0, CUSTOMFVF, D3DPOOL_DEFAULT, &_vertexes, NULL))) {
        return E_FAIL;  // 버텍스 버퍼 생성 실패 시 실패 반환
    }

    VOID* pVertices;

    if (SUCCEEDED(_vertexes->Lock(0, sizeof(vertices), (void**)&pVertices, 0))) {
        memcpy(pVertices, vertices, sizeof(vertices));
        _vertexes->Unlock();
    }
    else return E_FAIL;

    // 큐브의 버텍스 인덱스 초기화
    WORD indices[] = {
        // 왼쪽 면
        4, 0, 3, 4, 3, 7,
        // 아래쪽 면
        4,5,1, 4,1,0,
        // 오른쪽 면
        1,5,2, 2,5,6,
        // 위쪽 면
        3,6,7, 3,2,6,
        // 앞면
        0,1,2, 0,2,3,
        // 뒷면
        5, 7, 6, 5, 4, 7,
    };


    // Index Buffer 생성
    if (FAILED(device->CreateIndexBuffer(sizeof(indices), 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &_indicies, NULL))) {
        return E_FAIL;  // 인덱스 버퍼 생성 실패 시 실패 반환
    }

    // Index Buffer 데이터 채우기
    VOID* pIndices;
    if (SUCCEEDED(_indicies->Lock(0, sizeof(indices), (void**)&pIndices, 0))) {
        memcpy(pIndices, indices, sizeof(indices));
        _indicies->Unlock();
    }
    else return E_FAIL;

    // 정점의 개수와 폴리고 개수 계산
    D3DVERTEXBUFFER_DESC vbDesc;
    _vertexes->GetDesc(&vbDesc);
    vertexCount = vbDesc.Size / sizeof(CUSTOMVERTEX);

    // Index Buffer의 정보를 받음
    D3DINDEXBUFFER_DESC ibDesc;
    _indicies->GetDesc(&ibDesc);
    // Index Buffer의 Format이 2바이트인지 4바이트인지 검사
    int indexSize = (ibDesc.Format == D3DFMT_INDEX16) ? sizeof(WORD) : sizeof(DWORD);
    // Index Buffer의 사이즈와 Format 크기로 폴리곤의 개수를 계산
    int indexCount = ibDesc.Size / indexSize;
    polygonCount = indexCount / 3;

    return S_OK;

}


void Cube::KeyInput(float deltaT) {

    if (GetAsyncKeyState(VK_LEFT)) {
        Move(-1.f * deltaT, 0, 0);
    }

    if (GetAsyncKeyState(VK_RIGHT)) {
        Move(1.f * deltaT, 0, 0);
    }

    if (GetAsyncKeyState(VK_UP)) {
        Move(0, 1.f * deltaT, 0);
    }

    if (GetAsyncKeyState(VK_DOWN)) {
        Move(0, -1.f * deltaT, 0);
    }

    if (GetAsyncKeyState('Z')) {
        Move(0, 0, -1.f * deltaT);
    }

    if (GetAsyncKeyState('X')) {
        Move(0, 0, 1.f * deltaT);
    }



    if (GetAsyncKeyState('D')) {
        Rotate(0, 1.f * deltaT, 0);
    }

    if (GetAsyncKeyState('A')) {
        Rotate(0, -1.f * deltaT, 0);
    }
}