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

    //�ړ���������
    
    void RandomMove();      //�����_���ړ�
    void RightHandMove();   //�E��@
    void LeftHandMove();    //����@

    void BFS();
    void Dijkstra();
public:
    Enemy();
    ~Enemy();
    void Update() override;
    void Draw() override;
    bool CheckHit(const Rect& me, const Rect& other);
};

