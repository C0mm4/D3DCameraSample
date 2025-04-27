#pragma once
#include <d3d9.h>
#include <d3dx9math.h>

// 다이렉트3 폴리곤 정점 정보 구조체
struct CUSTOMVERTEX
{
	FLOAT x, y, z;
	DWORD color;
};

// 다이렉트3 선분 정점 정보 구조체
struct LineVertex {
	D3DXVECTOR3 pos;
	DWORD color;
};


// 폴리곤 정점 FVF 설정
#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_DIFFUSE)

// 선분 정점 FVF 설정
#define CUSTOMFVF_LINEVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE)


class Object {
protected:
	LPDIRECT3DVERTEXBUFFER9 _vertexes;	// 정점 버퍼
	LPDIRECT3DINDEXBUFFER9 _indicies;	// 인덱스 버퍼
	D3DXMATRIX worldMatrix;				// 객체 적용 월드 매트릭스
	D3DXMATRIX rotateMatrix;			// 객체 적용 회전 매트릭스
	D3DXVECTOR3 pos;					// 객체 위치 벡터
	int vertexCount;					// 정점 개수
	int polygonCount;					// 폴리곤 개수


	D3DXVECTOR3 upVec, lookAt, right;	// 객체의 up, front, right 벡터


public:
	Object();

	Object(float x, float y, float z);

	virtual ~Object() {
		// 객체 delete 시 정점 버퍼, 인덱스 버퍼 Release
		if (_vertexes != NULL) _vertexes->Release();
		if (_indicies != NULL) _indicies->Release();
	}

	/// <summary>
	/// 객체의 각 프레임별 동작 main의 메시지 루프에서 Update 호출
	/// </summary>
	/// <param name="deltaT">지난 프레임과의 시간</param>
	virtual void Update(float deltaT) {}; 

	/// <summary>
	/// 객체의 초기화 동작. 정점 버퍼 등 필요한 작업 객체 별 정리
	/// </summary>
	/// <param name="device">Direct3 디바이스 객체 포인터</param>
	/// <returns>초기화 성공 여부</returns>
	virtual HRESULT Init(LPDIRECT3DDEVICE9 device)=0;	

	/// <summary>
	/// 객체의 렌더링 동작. 
	/// </summary>
	/// <param name="device">Direct3 디바이스 객체 포인터</param>
	virtual void Render(LPDIRECT3DDEVICE9 device);

	/// <summary>
	/// 객체의 위치를 매개변수만큼 이동 이동은 객체의 local coordinate를 따름
	/// </summary>
	/// <param name="x">x value</param>
	/// <param name="y">y value</param>
	/// <param name="z">z value</param>
	virtual void Move(float x, float y, float z);

	/// <summary>
	/// 객체의 위치를 매개변수만큼 이동 이동은 world coordinate를 따름 
	/// </summary>
	/// <param name="x">x value</param>
	/// <param name="y">y value</param>
	/// <param name="z">z value</param>
	virtual void MoveInWorld(float x, float y, float z);

	/// <summary>
	/// 객체를 매개변수만큼 회전 회전은 객체의 local matrix를 따름
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <param name="z"></param>
	virtual void Rotate(float x, float y, float z);

	/// <summary>
	/// 객체의 위치를 반환
	/// </summary>
	/// <returns>위치 Vector</returns>
	D3DXVECTOR3 GetPos() { return pos; }

	/// <summary>
	/// 객체끼리의 위치를 계산
	/// </summary>
	/// <param name="obj">타겟 오브젝트</param>
	/// <returns>객체와의 거리 Vector</returns>
	D3DXVECTOR3 ComparePos(const Object& obj) { return obj.pos - pos; }

	/// <summary>
	/// 객체의 바라보는 방향을 반환
	/// </summary>
	/// <returns>객체의 LookAt Vector</returns>
	D3DXVECTOR3 GetLookAtVector() { return lookAt; }

	/// <summary>
	/// 객체의 키 입력을 정의
	/// </summary>
	/// <param name="deltaT">Frame Time</param>
	virtual void KeyInput(float deltaT) {};
};