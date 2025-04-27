#pragma once
#include "Object.h"

class CCamera : public Object {
public:
	CCamera() : Object() {
		CameraCenterTarget = NULL;
		lerpValue = 0;

		// 옵션 값 설정
		option.LerpSpeed = 3;
		option.TargetDistance = 5;
		option.TargetLookDistance = 3;
		option.TargetTraceMaxDistance = 6;
		option.TargetTraceMinDistance = 4;
		option.TargetDistanceY = 2;
		option.stopLerpTimer = 0;
		option.StopLerpDuration = 0.5f;
		option.baseRotationSpeed = 1.f;
		option.additionalRotationSpeed = 2.f;

		// 트리거 초기화
		isLerp = false;
		isTargetTraceLerp = false;
		isTargetTraceLerpEnd = false; 
		option.stopLerpTimer = 0;
		option.StopLerpDuration = 0.5f;
	};

	CCamera(float x, float y, float z) : Object(x, y, z) {
		CameraCenterTarget = NULL;
		lerpValue = 0;

		// 옵션 값 설정
		option.LerpSpeed = 3;
		option.TargetDistance = 5;
		option.TargetLookDistance = 3;
		option.TargetTraceMaxDistance = 6;
		option.TargetTraceMinDistance = 4;
		option.TargetDistanceY = 2;
		option.stopLerpTimer = 0;
		option.StopLerpDuration = 0.5f;
		option.baseRotationSpeed = 1.f;
		option.additionalRotationSpeed = 2.f;

		// 트리거 초기화
		isLerp = false;
		isTargetTraceLerp = false;
		isTargetTraceLerpEnd = false;
	}

	/// <summary>
	/// 객체의 초기화 동작. 정점 버퍼 등 필요한 작업 객체 별 정리
	/// </summary>
	/// <param name="device">Direct3 디바이스 객체 포인터</param>
	/// <returns>초기화 성공 여부</returns>
	HRESULT Init(LPDIRECT3DDEVICE9 device) override;


	void Render(LPDIRECT3DDEVICE9 deivce) override 	{
		// 카메라 이펙트나 UI 등 추가 가능? 
	};


	/// <summary>
	/// 객체의 각 프레임별 동작 main의 메시지 루프에서 Update 호출
	/// </summary>
	/// <param name="deltaT">지난 프레임과의 시간</param>
	void Update(float deltaT) override;

	/// <summary>
	/// 카메라의 위치, 방향 벡터를 기반으로 View Transform 설정
	/// </summary>
	/// <param name="device">D3D device</param>
	void SetViewTransform(LPDIRECT3DDEVICE9 device);

	/// <summary>
	/// 객체를 매개변수만큼 회전 회전은 객체의 local matrix를 따름
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <param name="z"></param>
	void Rotate(float x, float y, float z) override;

	/// <summary>
	/// 카메라 중심 객체 설정
	/// </summary>
	/// <param name="target">중심 객체</param>
	void SetTarget(Object& target);

	/// <summary>
	/// 객체를 추적할 때 카메라의 목표 지점 계산
	/// </summary>
	/// <param name="target">목표 객체</param>
	/// <returns>목표 위치</returns>
	D3DXVECTOR3 CalculateNewPosAtSetTarget(Object& target);

	/// <summary>
	/// 객체를 추적할 때 목표 방향 벡터 계산 
	/// CalculateNewPosAtSetTarget 함수 호출 후 사용
	/// </summary>
	void CalculateNewTargetVector();

	/// <summary>
	/// pos를 targetPos 까지 range정도로 Lerp 계산 실시
	/// </summary>
	/// <param name="range">Lerp range</param>
	void PositionLerp(float range);

	/// <summary>
	/// lookAt Vector를 targetLookAt 까지 range 정도로 Lerp 계산 실시 이후 upVec, right Vector를 보정 계산
	/// </summary>
	/// <param name="range">Lerp range</param>
	void RotateLerp(float range);

	/// <summary>
	/// 카메라 센터 객체의 위치를 추적
	/// </summary>
	/// <param name="range">Lerp value</param>
	void TargetPosLerp(float range);

	/// <summary>
	/// 센터 객체의 방향 벡터를 추적 이 때 객체의 lookAt 벡터를 참조한다
	/// </summary>
	/// <param name="range">lerp Value</param>
	void TargetLookAtLerp(float range);

private:
	D3DXVECTOR3 targetLookAt, targetUpVec; // using for Lerp
	struct CameraOption {
		float TargetTraceMinDistance;	// 대상을 추적하는 최소 거리 (해당 거리 이하면 추적)
		float TargetTraceMaxDistance;	// 대상을 추적하는 최대 거리 (해당 거리 이상이면 추적)
		float TargetDistance;			// 대상과의 목표 거리
		float TargetLookDistance;		// 카메라 목표 지점 (대상의 Look At vector의 크기를 더함)
		float LerpSpeed ;				// 보간 속도
		float TargetDistanceY;			// 대상과의 Y 거리
		float stopLerpTimer;			// 추적 종료시 부드럽게 멈추는 시간 타이머 변수
		float StopLerpDuration;			// 추적 종료 후 감속 지속 시간
		float baseRotationSpeed;			// 카메라 회전 속도
		float additionalRotationSpeed;	// 객체의 회전 속도에 따른 추가 회전 속도 상수
	};
	CameraOption option;

	float lerpValue;					// 카메라 추적 객체 설정 시 해당 객체의 카메라 목표지점으로 이동할 때 사용하는 lerp 값
	bool isLerp;						// 카메라 추적 객체 설정 시 해당 객체의 lerp 진행 여부
	D3DXVECTOR3 targetPos;				// 카메라가 객체 추적 시 카메라의 목표 지점
	Object* CameraCenterTarget;			// 카메라가 추적하는 대상
	bool isTargetTraceLerp;				// 추적 대상이 움직임에 따라 카메라의 추적 여부
	bool isTargetTraceLerpEnd;			// 추적 대상이 추적 조건이 종료됨에 따라 추적을 마무리할 때 부드럽게 멈추기 위한 트리거

};