#include <windows.h>

class GameTimer {
private:
    LARGE_INTEGER frequency;  // 초당 카운트 횟수
    LARGE_INTEGER lastTime;   // 이전 프레임 시간

public:
    GameTimer() {
        // 타이머 주파수 가져오기
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&lastTime); // 초기 시간 저장
    }

    float getDeltaTime() {
        LARGE_INTEGER currentTime;
        QueryPerformanceCounter(&currentTime); // 현재 시간 가져오기

        // 경과 시간 계산 (초 단위)
        float deltaTime = static_cast<float>(currentTime.QuadPart - lastTime.QuadPart) / frequency.QuadPart;

        // 이전 시간을 현재 시간으로 업데이트
        lastTime = currentTime;

        return deltaTime;
    }
};
#pragma once
