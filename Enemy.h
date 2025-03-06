#pragma once
#include "./Library/GameObject.h"
#include "./globals.h"

class Enemy :
    public GameObject
{
    Point pos_;
    Point move_;

    enum DIR
    {
        UP,
        DOWN,
        LEFT,
        RIGHT,
    };

    DIR dir;

    float ChengeTimer_;
    int algonum_;

    void MoveAlgo();

    //移動方向決め
    
    void RandomMove();      //ランダム移動
    void RightHandMove();   //右手法
    void LeftHandMove();    //左手法

    void BFS();
    void Dijkstra();
public:
    Enemy();
    ~Enemy();
    void Update() override;
    void Draw() override;
    bool CheckHit(const Rect& me, const Rect& other);
};

