#pragma once
#include "./Library/GameObject.h"
#include "./globals.h"

class Enemy :
    public GameObject
{
    Point pos_;
    Point move_;

    enum DIRECTION
    {
        UP,
        DOWN,
        RIGHT,
        LEFT,
    };

    void RandomMove();      //�����_���ړ�
    void RightHandMove();   //�E��@
    void LeftHandMove();    //����@
public:
    Enemy();
    ~Enemy();
    void Update() override;
    void Draw() override;
    bool CheckHit(const Rect& me, const Rect& other);
};

