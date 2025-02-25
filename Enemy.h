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

    void RandomMove();      //ランダム移動
    void RightHandMove();   //右手法
    void LeftHandMove();    //左手法
public:
    Enemy();
    ~Enemy();
    void Update() override;
    void Draw() override;
    bool CheckHit(const Rect& me, const Rect& other);
};

