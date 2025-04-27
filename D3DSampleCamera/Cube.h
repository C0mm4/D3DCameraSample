#pragma once
#include "Object.h"

class Cube : public Object {
public:
    Cube() : Object() {};
    Cube(float x, float y, float z) : Object(x,y,z) {};

    HRESULT Init(LPDIRECT3DDEVICE9 device);


    void KeyInput(float deltaT) override;
};