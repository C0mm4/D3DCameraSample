#include "CCamera.h"
#include "math.h"

HRESULT CCamera::Init(LPDIRECT3DDEVICE9 device) {
    // 최초 방향 벡터 초기화
	lookAt = { 0, 0, 1 };
	upVec = { 0, 1, 0 };
    right = { 1, 0, 0 };
	return S_OK;
}

void CCamera::SetViewTransform(LPDIRECT3DDEVICE9 device) {
    // 현재 위치와 lookAt 벡터, upVec을 기반으로 View Matrix 계산 및 설정
	D3DXMATRIX viewTM;
	D3DXVECTOR3 lookPos = lookAt + pos;
	D3DXMatrixLookAtLH(&viewTM, &pos, &lookPos, &upVec);
	device->SetTransform(D3DTS_VIEW, &viewTM);
}

void CCamera::Rotate(float x, float y, float z)
{
    // right 벡터 계산 및 각 방향벡터 정규화
    D3DXVec3Cross(&right, &upVec, &lookAt);
    D3DXVec3Normalize(&right, &right);
    D3DXVec3Normalize(&upVec, &upVec);
    D3DXVec3Normalize(&lookAt, &lookAt);

    if (x != 0.0f)
    {
        // X축 회전
        D3DXMATRIX rotX;
        D3DXMatrixRotationAxis(&rotX, &right, x);
        D3DXVec3TransformNormal(&upVec, &upVec, &rotX);
    }

    if (y != 0.0f)
    {
        // Y축 회전
        D3DXMATRIX rotY;
        D3DXMatrixRotationAxis(&rotY, &upVec, y);
        D3DXVec3TransformNormal(&lookAt, &lookAt, &rotY);
    }

    if (z != 0.0f)
    {
        // Z축 회전
        D3DXMATRIX rotZ;
        D3DXMatrixRotationAxis(&rotZ, &lookAt, z);
        D3DXVec3TransformNormal(&upVec, &upVec, &rotZ);
    }

    // 방향 벡터 정규화 및 right 벡터 재계산
    D3DXVec3Normalize(&lookAt, &lookAt);
    D3DXVec3Normalize(&upVec, &upVec);
    D3DXVec3Cross(&right, &upVec, &lookAt);
}

void CCamera::SetTarget(Object& target) {
    CameraCenterTarget = &target;

    // 목표 위치 설정
    targetPos = CalculateNewPosAtSetTarget(target);

    // 위치 보간 시작
    PositionLerp(0);

    // 목표 방향 벡터 설정
    CalculateNewTargetVector();

    // 방향 벡터 보간 시작
    RotateLerp(0);
}

D3DXVECTOR3 CCamera::CalculateNewPosAtSetTarget(Object& target) {
    D3DXVECTOR3 ret;

    // option의 타겟과의 Y 거리를 기반으로 객체의 XZ 평면 위 Y 거리만큼 떨어진 XZ평면의 원의 반지름 계산
    float plainCircleRadiusSquare = sqrt(pow(option.TargetDistance, 2) + pow(option.TargetDistanceY, 2));

    // 계산된 반지름을 객체의 방향 벡터를 곱해줌. (카메라가 해당 위치에서 바라볼 수 있도록)
    float x = -plainCircleRadiusSquare * target.GetLookAtVector().x;
    float z = -plainCircleRadiusSquare * target.GetLookAtVector().z;

    // 계산된 원의 좌표에 객체의 위치를 더해 객체 주변의 원으로 계산
    auto targetPos = target.GetPos();
    ret.x = targetPos.x + x;
    ret.z = targetPos.z + z;
    ret.y = targetPos.y + option.TargetDistanceY;

    return ret;
}

void CCamera::CalculateNewTargetVector() {
    // 목표 lookAt 계산
    targetLookAt = CameraCenterTarget->GetPos() - targetPos;
    D3DXVec3Normalize(&targetLookAt, &targetLookAt);

    // 임시 upVec (World Coordinate)
    targetUpVec = { 0,1,0 };

    // lookAt, 임시 upVec으로 right 계산
    D3DXVec3Cross(&right, &targetUpVec, &targetLookAt);

    // lookAt, right으로 upVec 계산
    D3DXVec3Cross(&targetUpVec, &targetLookAt, &right);
}

void CCamera::PositionLerp(float range){
    // pos 값 lerp 계산
    D3DXVec3Lerp(&pos, &pos, &targetPos, range * 2);
    if (lerpValue >= 0.5f) {
        isLerp = false;
    }
    else isLerp = true;
}

void CCamera::RotateLerp(float range) {
    // lookAt 값 lerp 계산
    D3DXVec3Lerp(&lookAt, &lookAt, &targetLookAt, range);
    D3DXVec3Normalize(&lookAt, &lookAt);

    // 임시 upVec (World Coordionate)
    targetUpVec = { 0,1,0 };
    
    // lookAt, 임시 upVec으로 right 계산
    D3DXVec3Cross(&right, &upVec, &lookAt);
    D3DXVec3Normalize(&right, &right);

    // lookAt, right으로 upVec 계산
    D3DXVec3Cross(&targetUpVec, &targetLookAt, &right);
}

void CCamera::Update(float deltaT) {
    // 카메라 타겟 설정시 목표 지점까지 이동
    if (isLerp) {
        lerpValue += deltaT;
        TargetPosLerp(lerpValue);
        TargetLookAtLerp(lerpValue);
    }
    // 카메라 타겟 설정 이후 타겟 이동에 따른 카메라 위치, 방향 벡터 설정
    else if (CameraCenterTarget != NULL) {
        // 목표 지점과의 거리, 방향 벡터 정확도 계산
        D3DXVECTOR3 vectorToTarget = CameraCenterTarget->GetPos() - pos;
        float distance = D3DXVec3Length(&vectorToTarget);

        auto targetLookVec = CameraCenterTarget->GetLookAtVector();
        float dot = D3DXVec3Dot(&lookAt, &targetLookVec);

        // 거리 조건 or 방향 벡터 정확도로 추적 활성화
        if (distance >= option.TargetTraceMaxDistance || distance <= option.TargetTraceMinDistance ||
            dot <= 0.95f) {
            isTargetTraceLerp = true;
        }

        // 추적 상태일 때 위치와 카메라 각도 추적
        if (isTargetTraceLerp) {
            TargetPosLerp(deltaT);
            TargetLookAtLerp(deltaT);
        }
        // 추적 종료 시 부드럽게 정지
        else if(isTargetTraceLerpEnd){

            // 추적 멈춤 후 감속 처리
            option.stopLerpTimer += deltaT;
            float fade = 1.0f - (option.stopLerpTimer / option.StopLerpDuration);
            fade = max(fade, 0.0f); // 음수 방지


            TargetPosLerp(deltaT * fade);  // 부드럽게 멈춤
            TargetLookAtLerp(deltaT * fade);

            if (fade <= 0.01f) {
                isTargetTraceLerpEnd = false; // 완전히 멈춤
            }
        }

    }
}

void CCamera::TargetPosLerp(float deltaT) {
    // 목표 위치 계산
    targetPos = CalculateNewPosAtSetTarget(*CameraCenterTarget);

    // 부드럽게 이동
    PositionLerp(deltaT * option.LerpSpeed);

    // 타겟 객체와 카메라 사이 거리 계산
    D3DXVECTOR3 toRealTarget = CameraCenterTarget->GetPos() - pos;
    float realTargetDistance = D3DXVec3Length(&toRealTarget);

    if (realTargetDistance <= option.TargetTraceMinDistance) {
        option.stopLerpTimer = 0.0f;
        isTargetTraceLerp = false;
        isTargetTraceLerpEnd = true;
    }

}

void CCamera::TargetLookAtLerp(float range) {
    // 목표 방향 벡터 계산
    CalculateNewTargetVector();

    // 현재 바라보는 방향과 목표 방향의 각도 차이를 계산
    float dot = D3DXVec3Dot(&lookAt, &targetLookAt);
    dot = max(-1.0f, min(1.0f, dot)); // 안전하게 클램프

    float angle = acosf(dot); // 라디안 단위로 각도 차이

    // 빠르게 회전해야 할 경우를 고려해서 회전 속도 스케일 조정
    float rotationSpeedScale = 1.0f + (angle / D3DX_PI) * option.additionalRotationSpeed;
    float adjustedDelta = range * option.baseRotationSpeed * rotationSpeedScale;

    // 보정 값으로 Lerp
    RotateLerp(adjustedDelta);
}