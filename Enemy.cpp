#include "Enemy.h"
#include "Stage.h"
#include "ImGui/imgui.h"

namespace {
	const Point MDir[4] = { {0,-1},{0,1},{-1,0},{1,0} };
	const float CHENGETIME{ 10.0f };
}

void Enemy::MoveAlgo()
{

	switch (algonum_)
	{
	case 0:
		RandomMove();
		break;
	case 1:
		RightHandMove();
		break;
	case 2:
		LeftHandMove();
		break;
	default:
		break;
	}
}

void Enemy::RandomMove()
{
	dir = (DIR)(rand() % 4);
}

void Enemy::RightHandMove()
{
	//�����Ă���������獶�E���擾
	DIR myRight[4] = { RIGHT, LEFT, UP, DOWN };
	DIR myLeft[4] = { LEFT, RIGHT, DOWN, UP };
	//�O�Ɉړ������Ƃ��ƉE�Ɉړ������Ƃ��̓����蔻������
	Rect FrontMoveRect = { pos_.x + MDir[dir].x,		  pos_.y + MDir[dir].y,			 CHA_WIDTH, CHA_HEIGHT };
	Rect RightMoveRect = { pos_.x + MDir[myRight[dir]].x, pos_.y + MDir[myRight[dir]].y, CHA_WIDTH, CHA_HEIGHT };

	bool isRightOpen = true;
	bool isForwardOpen = true;

	Stage* stage = FindGameObject<Stage>();
	for (auto& obj : stage->GetStageRects()) {
		//�O���������Ă�����O�͕ǂł���
		if (CheckHit(FrontMoveRect, obj)) {
			isForwardOpen = false;
		}
		//�E�ɓ������Ă�����E�͕ǂł���
		if (CheckHit(RightMoveRect, obj)) {
			isRightOpen = false;
		}
	}

	//�E���J���Ă�����E������
	if (isRightOpen)
	{
		dir = myRight[dir];
	}
	//�E���O���󂢂Ă��Ȃ��Ƃ��͍�������
	else if (isRightOpen == false && isForwardOpen == false)
	{
		dir = myLeft[dir];
	}
}

void Enemy::LeftHandMove()

{	//�����Ă���������獶�E���擾
	DIR myRight[4] = { RIGHT, LEFT, UP, DOWN };
	DIR myLeft[4] = { LEFT, RIGHT, DOWN, UP };
	
	//�O�Ɉړ������Ƃ��ƍ��Ɉړ������Ƃ��̓����蔻������
	Rect FrontMoveRect = { pos_.x + MDir[dir].x,		  pos_.y + MDir[dir].y,			 CHA_WIDTH, CHA_HEIGHT };
	Rect LeftMoveRect = { pos_.x + MDir[myLeft[dir]].x, pos_.y + MDir[myLeft[dir]].y, CHA_WIDTH, CHA_HEIGHT };

	bool isLeftOpen = true;
	bool isForwardOpen = true;

	Stage* stage = FindGameObject<Stage>();
	for (auto& obj : stage->GetStageRects()) {
		//�O���������Ă�����O�͕ǂł���
		if (CheckHit(FrontMoveRect, obj)) {
			isForwardOpen = false;
		}
		//���ɓ������Ă����獶�͕ǂł���
		if (CheckHit(LeftMoveRect, obj)) {
			isLeftOpen = false;
		}
	}

	//�����J���Ă����獶������
	if (isLeftOpen)
	{
		dir = myLeft[dir];
	}
	//�����O���󂢂Ă��Ȃ��Ƃ��͉E������
	else if (isLeftOpen == false && isForwardOpen == false)
	{
		dir = myRight[dir];
	}
}

Enemy::Enemy()
	:pos_({ CHA_WIDTH,CHA_HEIGHT }), move_({ 0,-1 }), dir(DIR::UP)
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
	ChengeTimer_ = CHENGETIME;
	algonum_ = 1;
}

Enemy::~Enemy()
{
}

void Enemy::Update()
{
	Point oldPos = pos_;    //�ړ��O

	pos_ = { pos_.x + move_.x,pos_.y + move_.y };

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

	//4�����Ɉړ��ł���Ƃ�
	if (pos_.x / CHA_WIDTH % 2 == 1 && pos_.x % CHA_WIDTH == 0 &&
		pos_.y / CHA_HEIGHT % 2 == 1 && pos_.y % CHA_HEIGHT == 0) {
		MoveAlgo();
	}

	move_ = MDir[dir];

	ImGui::Begin("enemy");
	ImGui::InputInt("num", &algonum_);
	ImGui::End();
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
