#pragma once
#include "./Library/GameObject.h"
#include "./globals.h"
#include <vector>
#include <queue>

using std::vector;

class Enemy :
    public GameObject
{
    Point pos_;
    Point move_;
    Point PlayerOrigion_;

    enum DIR
    {
        UP,
        DOWN,
        LEFT,
        RIGHT,
        NONE
    };
    DIR dir;

    enum MOVESWITCH
    {
        RANDOM,
        RIGHTHAND,
        LEFTHAND,
        MIX,
        BFS,
        DIJKSTRA,
        ASTAR,
        MAX
    };

    int MoveSwitch_;

    float ChengeTimer_;
    int algonum_;
    bool isDisplayRoute_;
    bool isDisplayDist_;

    struct Info
    {
        Point Nextpos;
        DIR dir;
    };

    void MoveAlgo();

    //移動方向決め
    void RandomMove();      //ランダム移動
    void RightHandMove();   //右手法
    void LeftHandMove();    //左手法
    void BFSAlgo();
    void Dijkstra();
    void AStar();

    vector<vector<int>> dist;
    vector<vector<Point>> pre;
    std::deque<Info> route_;

    void ImGuiOperation();
public:
    Enemy();
    ~Enemy();
    void Update() override;
    void Draw() override;
    bool CheckHit(const Rect& me, const Rect& other);
};

