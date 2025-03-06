#include "Enemy.h"
#include "Stage.h"
#include "ImGui/imgui.h"
#include "Player.h"
#include <queue>

namespace {
	const Point MDir[4] = { {0,-1},{0,1},{-1,0},{1,0} };
	const float CHENGETIME{ 10.0f };
}

void Enemy::MoveAlgo()
{

	//switch (algonum_)
	//{
	//case 0:
	//	RandomMove();
	//	break;
	//case 1:
	//	RightHandMove();
	//	break;
	//case 2:
	//	LeftHandMove();
	//	break;
	//default:
	//	break;
	//}
	BFS();
}

void Enemy::RandomMove()
{
	dir = (DIR)(rand() % 4);
}

void Enemy::RightHandMove()
{
	//向いている方向から左右を取得
	DIR myRight[4] = { RIGHT, LEFT, UP, DOWN };
	DIR myLeft[4] = { LEFT, RIGHT, DOWN, UP };
	//前に移動したときと右に移動したときの当たり判定を作る
	Rect FrontMoveRect = { pos_.x + MDir[dir].x,		  pos_.y + MDir[dir].y,			 CHA_WIDTH, CHA_HEIGHT };
	Rect RightMoveRect = { pos_.x + MDir[myRight[dir]].x, pos_.y + MDir[myRight[dir]].y, CHA_WIDTH, CHA_HEIGHT };

	bool isRightOpen = true;
	bool isForwardOpen = true;

	Stage* stage = FindGameObject<Stage>();
	for (auto& obj : stage->GetStageRects()) {
		//前が当たっていたら前は壁である
		if (CheckHit(FrontMoveRect, obj)) {
			isForwardOpen = false;
		}
		//右に当たっていたら右は壁である
		if (CheckHit(RightMoveRect, obj)) {
			isRightOpen = false;
		}
	}

	//右が開いていたら右を向く
	if (isRightOpen)
	{
		dir = myRight[dir];
	}
	//右も前も空いていないときは左を向く
	else if (isRightOpen == false && isForwardOpen == false)
	{
		dir = myLeft[dir];
	}
}

void Enemy::LeftHandMove()

{	//向いている方向から左右を取得
	DIR myRight[4] = { RIGHT, LEFT, UP, DOWN };
	DIR myLeft[4] = { LEFT, RIGHT, DOWN, UP };

	//前に移動したときと左に移動したときの当たり判定を作る
	Rect FrontMoveRect = { pos_.x + MDir[dir].x,		  pos_.y + MDir[dir].y,			 CHA_WIDTH, CHA_HEIGHT };
	Rect LeftMoveRect = { pos_.x + MDir[myLeft[dir]].x, pos_.y + MDir[myLeft[dir]].y, CHA_WIDTH, CHA_HEIGHT };

	bool isLeftOpen = true;
	bool isForwardOpen = true;

	Stage* stage = FindGameObject<Stage>();
	for (auto& obj : stage->GetStageRects()) {
		//前が当たっていたら前は壁である
		if (CheckHit(FrontMoveRect, obj)) {
			isForwardOpen = false;
		}
		//左に当たっていたら左は壁である
		if (CheckHit(LeftMoveRect, obj)) {
			isLeftOpen = false;
		}
	}

	//左が開いていたら左を向く
	if (isLeftOpen)
	{
		dir = myLeft[dir];
	}
	//左も前も空いていないときは右を向く
	else if (isLeftOpen == false && isForwardOpen == false)
	{
		dir = myRight[dir];
	}
}

void Enemy::BFS()
{

	Player* player = FindGameObject<Player>();
	Point TargetGridPos = { player->GetPositionPoint().x / CHA_WIDTH,player->GetPositionPoint().y / CHA_HEIGHT };	//グリッド座標
	Stage* stage = FindGameObject<Stage>();
	stage->GridNumReset();
	int step = 0;
	stage->SetStageNum(pos_.x / CHA_WIDTH, pos_.y / CHA_HEIGHT, step);
	while (true)
	{
		bool Search = false;
		for (int i = 1; i < STAGE_HEIGHT - 1; i++) {
			for (int j = 1; j < STAGE_WIDTH - 1; j++) {

				if (stage->GetStageData(j, i).num == step) {
					for (int k = 0; k < 4; k++) {
						if (stage->GetStageData(j + MDir[k].x, i + MDir[k].y).obj != STAGE_OBJ::WALL) {
							if (stage->GetStageData(j + MDir[k].x, i + MDir[k].y).num == -1)
								stage->SetStageNum(j + MDir[k].x, i + MDir[k].y, step + 1);
							Search = true;
						}
					}
				}
			}
		}
		step++;

		if (!Search) {
			break;
		}
	}
	int num = stage->GetStageData(TargetGridPos.x, TargetGridPos.y).num;

	struct Info
	{
		Point Nextpos;
		DIR dir;
	};

	std::queue<Info> root_;

	root_.push({ TargetGridPos.x,TargetGridPos.y });

	DIR SearchDir[4] = { DOWN,UP,RIGHT,LEFT };
	
	for (int i = num - 1; i > 0; i--) {

		for (int j = 0; j < 4; j++) {
			Point BWpos = { root_.back().Nextpos.x + MDir[j].x,root_.back().Nextpos.y + MDir[j].y };
			if (stage->GetStageData(BWpos.x, BWpos.y).num == i) {
				root_.push({ BWpos,(DIR)SearchDir[j] });
				break;
			}
		}

	}

	dir = root_.back().dir;
}

void Enemy::Dijkstra()
{
}

Enemy::Enemy()
	:pos_({ CHA_WIDTH,CHA_HEIGHT }), move_({ -1,0 }), dir(DIR::LEFT)
{
	Stage* stage = FindGameObject<Stage>();
	while (true)
	{
		POINT pos = { GetRand(STAGE_WIDTH - 1),GetRand(STAGE_HEIGHT - 1) };
		if (stage->GetStageData(pos.x, pos.y).obj == STAGE_OBJ::EMPTY) {
			pos_ = { pos.x * CHA_WIDTH,pos.y * CHA_HEIGHT };
			break;
		}
	}

	for (int i = 0; i < 4; i++) {
		bool open = true;
		Rect rect = { pos_.x + MDir[i].x,pos_.y + MDir[i].y,CHA_WIDTH,CHA_HEIGHT };
		for (auto& obj : stage->GetStageRects()) {
			if (CheckHit(rect, obj)) {
				open = false;
			}
		}
		if (open) {
			dir = (DIR)i;
			break;
		}
	}
	move_ = MDir[dir];
	ChengeTimer_ = CHENGETIME;
	algonum_ = 1;

}

Enemy::~Enemy()
{
}

void Enemy::Update()
{
	Point oldPos = pos_;    //移動前


	move_ = MDir[dir];
	Stage* stage = FindGameObject<Stage>();
	//移動後
	Rect moveRect = { pos_.x,pos_.y,CHA_WIDTH,CHA_HEIGHT };
	pos_ = { pos_.x + move_.x,pos_.y + move_.y };
	//当たり判定
	for (auto& obj : stage->GetStageRects()) {
		if (CheckHit(moveRect, obj)) {
			Rect RectX = { oldPos.x,pos_.y,CHA_WIDTH,CHA_HEIGHT };
			Rect RectY = { pos_.x,oldPos.y,CHA_WIDTH,CHA_HEIGHT };

			//移動前のXが当たっていなかったら戻す
			if (!CheckHit(RectX, obj)) {
				pos_.x = oldPos.x;
			}
			//移動前のYが当たっていなかったら戻す
			else if (!CheckHit(RectY, obj)) {
				pos_.y = oldPos.y;
			}
			else {
				pos_ = oldPos;
			}
			RightHandMove();
			break;
		}
	}

	if (ChengeTimer_ < 0) {
		algonum_ = rand() % 3;
		ChengeTimer_ = CHENGETIME;
	}
	else {
		ChengeTimer_ -= Time::DeltaTime();
	}

	//4方向に移動できるとき
	if (pos_.x / CHA_WIDTH % 2 == 1 && pos_.x % CHA_WIDTH == 0 &&
		pos_.y / CHA_HEIGHT % 2 == 1 && pos_.y % CHA_HEIGHT == 0) {
		MoveAlgo();
	}
}

void Enemy::Draw()
{
	DrawBox(pos_.x, pos_.y, pos_.x + CHA_WIDTH, pos_.y + CHA_HEIGHT, GetColor(10, 200, 10), TRUE);

	Point tp[4][3] = {
	{{pos_.x + CHA_WIDTH / 2, pos_.y}, {pos_.x, pos_.y + CHA_HEIGHT / 2}, {pos_.x + CHA_WIDTH, pos_.y + CHA_HEIGHT / 2}},
	{{pos_.x + CHA_WIDTH / 2, pos_.y + CHA_HEIGHT}, {pos_.x, pos_.y + CHA_HEIGHT / 2}, {pos_.x + CHA_WIDTH, pos_.y + CHA_HEIGHT / 2}},
	{{pos_.x            , pos_.y + CHA_HEIGHT / 2}, {pos_.x + CHA_WIDTH / 2, pos_.y}, {pos_.x + CHA_WIDTH / 2, pos_.y + CHA_HEIGHT}},
	{{pos_.x + CHA_WIDTH, pos_.y + CHA_HEIGHT / 2}, {pos_.x + CHA_WIDTH / 2, pos_.y}, {pos_.x + CHA_WIDTH / 2, pos_.y + CHA_HEIGHT}}
	};

	DrawTriangle(tp[dir][0].x, tp[dir][0].y, tp[dir][1].x, tp[dir][1].y, tp[dir][2].x, tp[dir][2].y, GetColor(255, 255, 255), TRUE);
}

bool Enemy::CheckHit(const Rect& me, const Rect& other)
{
	if (me.x < other.x + other.w &&
		me.x + me.w > other.x &&
		me.y < other.y + other.h &&
		me.y + me.h > other.y)
	{
		return true;
	}
	return false;
}
