#include "Player.h"
#include "./Source/Input.h"
#include "./Stage.h"
#include "globals.h"

namespace {
	enum DIR
	{
		UP,
		DOWN,
		LEFT,
		RIGHT,
		NONE,
		MAXDIR
	};
	DIR inputDir = NONE;
}


Player::Player()
	:pos_({CHA_WIDTH,CHA_HEIGHT})
{
}

Player::~Player()
{
}

void Player::Update()
{
	int ox = pos_.x, oy = pos_.y;

	if (Input::IsKeepKeyDown(KEY_INPUT_UP))
	{
		pos_.y--;
		inputDir = UP;
	}
	else if (Input::IsKeepKeyDown(KEY_INPUT_DOWN))
	{
		pos_.y++;
		inputDir = DOWN;
	}
	else if (Input::IsKeepKeyDown(KEY_INPUT_LEFT))
	{
		pos_.x--;
		inputDir = LEFT;
	}
	else if (Input::IsKeepKeyDown(KEY_INPUT_RIGHT))
	{
		pos_.x++;
		inputDir = RIGHT;
	}

	Stage* stage = (Stage*)FindGameObject<Stage>();
	Rect playerRect = { pos_.x, pos_.y, CHA_WIDTH, CHA_HEIGHT };

	for (auto& obj : stage->GetStageRects())
	{
		if (CheckHit(playerRect, obj))
		{
			Rect tmpRectX = { ox, pos_.y, CHA_WIDTH, CHA_HEIGHT };
			Rect tmpRecty = { pos_.x, oy, CHA_WIDTH, CHA_HEIGHT };

			if (!CheckHit(tmpRectX, obj))
			{
				pos_.x = ox;
				Point centerMe = Rect{ pos_.x, pos_.y, CHA_WIDTH, CHA_HEIGHT }.GetCenter();
				Point centerObj = obj.GetCenter();
				if (centerMe.y > centerObj.y)
				{
					pos_.y++;
				}
				if (centerMe.y < centerObj.y)
				{
					pos_.y--;
				}
			}
			else if (!CheckHit(tmpRecty, obj))
			{
				pos_.y = oy;
				Point centerMe = Rect{ pos_.x, pos_.y, CHA_WIDTH, CHA_HEIGHT }.GetCenter();
				Point centerObj = obj.GetCenter();
				if (centerMe.x > centerObj.x)
				{
					pos_.x++;
				}
				if (centerMe.x < centerObj.x)
				{
					pos_.x--;
				}
			}
			else
			{
				pos_.x = ox;
				pos_.y = oy;
			}
		}
	}
}

void Player::Draw()
{
	DrawBox(pos_.x, pos_.y, pos_.x + CHA_WIDTH, pos_.y + CHA_HEIGHT, GetColor(255, 10, 10), TRUE);
}

bool Player::CheckHit(const Rect& me, const Rect& other)
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
