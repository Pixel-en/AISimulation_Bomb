#include "Enemy.h"
#include "Stage.h"

namespace {
	const Point MDir[4] = { {0,-1},{0,1},{-1,0},{1,0} };
}

void Enemy::RandomMove()
{
	//4�����Ɉړ��ł���Ƃ�
	if (pos_.x / CHA_WIDTH % 2 == 1 && pos_.x % CHA_WIDTH == 0 &&
		pos_.y / CHA_HEIGHT % 2 == 1 && pos_.y % CHA_HEIGHT == 0) {
		move_ = MDir[rand() % 4];
	}
	//�ړ�
	pos_.x += move_.x;
	pos_.y += move_.y;
}

void Enemy::RightHandMove()
{

}

void Enemy::LeftHandMove()
{
}

Enemy::Enemy()
	:pos_({ CHA_WIDTH,CHA_HEIGHT })
{
	Stage* stage = FindGameObject<Stage>();
	while (true)
	{
		POINT pos = { GetRand(STAGE_WIDTH - 1),GetRand(STAGE_HEIGHT - 1) };
		if (stage->GetStageData(pos.x, pos.y) == STAGE_OBJ::EMPTY) {
			pos_ = { pos.x * CHA_WIDTH,pos.y * CHA_HEIGHT };
			break;
		}
	}

}

Enemy::~Enemy()
{
}

void Enemy::Update()
{
	Point oldPos = pos_;    //�ړ��O
	//�ړ�
	RandomMove();
	//�ړ�
	Stage* stage = FindGameObject<Stage>();
	//�ړ���
	Rect moveRect = { pos_.x,pos_.y,CHA_WIDTH,CHA_HEIGHT };

	//�����蔻��
	for (auto& obj : stage->GetStageRects()) {
		if (CheckHit(moveRect, obj)) {
			Rect RectX = { oldPos.x,pos_.y,CHA_WIDTH,CHA_HEIGHT };
			Rect RectY = { pos_.x,oldPos.y,CHA_WIDTH,CHA_HEIGHT };

			//�ړ��O��X���������Ă��Ȃ�������߂�
			if (!CheckHit(RectX, obj)) {
				pos_.x = oldPos.x;
			}
			//�ړ��O��Y���������Ă��Ȃ�������߂�
			else if (!CheckHit(RectY, obj)) {
				pos_.y = oldPos.y;
			}
			else {
				pos_ = oldPos;
			}
		}
	}
}

void Enemy::Draw()
{
	DrawBox(pos_.x, pos_.y, pos_.x + CHA_WIDTH, pos_.y + CHA_HEIGHT, GetColor(10, 200, 10), TRUE);
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
