#include "Cube.h"



HRESULT Cube::Init(LPDIRECT3DDEVICE9 device) {

    // ť���� ���� �ʱ�ȭ
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

    // Vertex Buffer ����
    if (FAILED(device->CreateVertexBuffer(sizeof(vertices), 0, CUSTOMFVF, D3DPOOL_DEFAULT, &_vertexes, NULL))) {
        return E_FAIL;  // ���ؽ� ���� ���� ���� �� ���� ��ȯ
    }

    VOID* pVertices;

    if (SUCCEEDED(_vertexes->Lock(0, sizeof(vertices), (void**)&pVertices, 0))) {
        memcpy(pVertices, vertices, sizeof(vertices));
        _vertexes->Unlock();
    }
    else return E_FAIL;

    // ť���� ���ؽ� �ε��� �ʱ�ȭ
    WORD indices[] = {
        // ���� ��
        4, 0, 3, 4, 3, 7,
        // �Ʒ��� ��
        4,5,1, 4,1,0,
        // ������ ��
        1,5,2, 2,5,6,
        // ���� ��
        3,6,7, 3,2,6,
        // �ո�
        0,1,2, 0,2,3,
        // �޸�
        5, 7, 6, 5, 4, 7,
    };


    // Index Buffer ����
    if (FAILED(device->CreateIndexBuffer(sizeof(indices), 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &_indicies, NULL))) {
        return E_FAIL;  // �ε��� ���� ���� ���� �� ���� ��ȯ
    }

    // Index Buffer ������ ä���
    VOID* pIndices;
    if (SUCCEEDED(_indicies->Lock(0, sizeof(indices), (void**)&pIndices, 0))) {
        memcpy(pIndices, indices, sizeof(indices));
        _indicies->Unlock();
    }
    else return E_FAIL;

    // ������ ������ ������ ���� ���
    D3DVERTEXBUFFER_DESC vbDesc;
    _vertexes->GetDesc(&vbDesc);
    vertexCount = vbDesc.Size / sizeof(CUSTOMVERTEX);

    // Index Buffer�� ������ ����
    D3DINDEXBUFFER_DESC ibDesc;
    _indicies->GetDesc(&ibDesc);
    // Index Buffer�� Format�� 2����Ʈ���� 4����Ʈ���� �˻�
    int indexSize = (ibDesc.Format == D3DFMT_INDEX16) ? sizeof(WORD) : sizeof(DWORD);
    // Index Buffer�� ������� Format ũ��� �������� ������ ���
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