#include "Object.h"


Object::Object() {
    pos.x = pos.y = pos.z = 0;
	upVec = { 0, 1, 0 };
	lookAt = { 0, 0, 1 };
	right = { 1, 0, 0 };
	D3DXMatrixIdentity(&rotateMatrix);
}

Object::Object(float x, float y, float z) {
	this->pos.x = x; this->pos.y = y; this->pos.z = z;
	upVec = { 0, 1, 0 };
	lookAt = { 0, 0, 1 };
	right = { 1, 0, 0 };
	D3DXMatrixIdentity(&rotateMatrix);
};

void Object::Render(LPDIRECT3DDEVICE9 device) {
    device->SetStreamSource(0, _vertexes, 0, sizeof(CUSTOMVERTEX));
    device->SetFVF(CUSTOMFVF);
    device->SetIndices(_indicies);

    D3DXMATRIX trans;
    D3DXMatrixTranslation(&trans, pos.x, pos.y, pos.z);

    D3DXMATRIX worldMatrix = rotateMatrix * trans;
    device->SetTransform(D3DTS_WORLD, &worldMatrix);

    device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, vertexCount, 0, polygonCount);


    LineVertex lines[6]; // 3 Lines, 2 points each lines = 6 points

    float lineLength = 2.0f;

    // Calculate End point each vector
    D3DXVECTOR3 lookEnd = pos + lookAt * lineLength;
    D3DXVECTOR3 upEnd = pos + upVec * lineLength;
    D3DXVECTOR3 rightEnd = pos + right * lineLength;

    // lookAt (R)
    lines[0] = { pos, D3DCOLOR_ARGB(255, 255, 0, 0) };
    lines[1] = { lookEnd, D3DCOLOR_ARGB(255, 255, 0, 0) };

    // upVec (G)
    lines[2] = { pos, D3DCOLOR_ARGB(255, 0, 255, 0) };
    lines[3] = { upEnd, D3DCOLOR_ARGB(255, 0, 255, 0) };

    // right (B)
    lines[4] = { pos, D3DCOLOR_ARGB(255, 0, 0, 255) };
    lines[5] = { rightEnd, D3DCOLOR_ARGB(255, 0, 0, 255) };
    D3DXMATRIX identity;
    D3DXMatrixIdentity(&identity);
    device->SetTransform(D3DTS_WORLD, &identity);

    // Set FVF and Draw Primitive
    device->SetFVF(CUSTOMFVF_LINEVERTEX);
    device->DrawPrimitiveUP(D3DPT_LINELIST, 3, lines, sizeof(LineVertex));
}

void Object::Move(float x, float y, float z) {

	D3DXVECTOR3 movement = right * x + upVec * y + lookAt * z;

	pos += movement;
}

void Object::MoveInWorld(float x, float y, float z) {

    D3DXVECTOR3 movement = { x,y,z };

    pos += movement;
}

void Object::Rotate(float x, float y, float z) {

    // ReCalculate right vector, Renormalize direction vector
    D3DXVec3Cross(&right, &upVec, &lookAt);
    D3DXVec3Normalize(&right, &right);
    D3DXVec3Normalize(&upVec, &upVec);
    D3DXVec3Normalize(&lookAt, &lookAt);


    // Set Axis matrix Identity
    D3DXMATRIX rotX, rotY, rotZ;
    D3DXMatrixIdentity(&rotX);
    D3DXMatrixIdentity(&rotY);
    D3DXMatrixIdentity(&rotZ);

    // Local Z Axis Rotation
    if (z != 0.0f)
    {
        // calculate matrix, vector
        D3DXMatrixRotationAxis(&rotZ, &lookAt, z);
        D3DXVec3TransformNormal(&upVec, &upVec, &rotZ);
    }

    if (x != 0.0f)
    {
        // calculate matrix, vector
        D3DXMatrixRotationAxis(&rotX, &right, x);
        D3DXVec3TransformNormal(&lookAt, &lookAt, &rotX);
        D3DXVec3TransformNormal(&upVec, &upVec, &rotX);
    }

    if (y != 0.0f)
    {
        D3DXMatrixRotationAxis(&rotY, &upVec, y);
        D3DXVec3TransformNormal(&lookAt, &lookAt, &rotY);
        D3DXVec3TransformNormal(&upVec, &upVec, &rotY);
    }

    // calculate rotated matrix
    rotateMatrix *= rotZ * rotY * rotX;


    // Normalize Direction Vector
    D3DXVec3Normalize(&lookAt, &lookAt);
    D3DXVec3Normalize(&upVec, &upVec);
    D3DXVec3Cross(&right, &upVec, &lookAt);
}
