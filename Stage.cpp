
#include "Stage.h"
#include "./globals.h"
#include <stack>
#include "Enemy.h"
#include <queue>
#include "ImGui/imgui.h"

namespace {
	std::stack<Point> prStack;

	void DigDug(int x, int y, vector<vector<StageObj>>& _stage)
	{
		_stage[y][x].obj = STAGE_OBJ::EMPTY;

		Point Dir[]{ {0,-1},{1, 0},{0, 1},{-1,0} };
		std::vector<int> dList;
		for (int i = 0; i < 4; i++) {
			//nextを0~3まで回してでたーを取得
			Point next = Point{ x + Dir[i].x, y + Dir[i].y };
			Point nextNext = { next.x + Dir[i].x, next.y + Dir[i].y };
			if (nextNext.x < 0 || nextNext.y < 0 || nextNext.x > STAGE_WIDTH - 1 || nextNext.y > STAGE_HEIGHT - 1)
				continue;

			if (_stage[nextNext.y][nextNext.x].obj == STAGE_OBJ::WALL)
			{
				dList.push_back(i);
			}
			//ここにelseつけて、もう一個掘っちゃうといいかも！（ループのやつ）
		}
		if (dList.empty())
		{
			return;
		}
		int nrand = rand() % dList.size();
		int tmp = dList[nrand];

		Point next = { x + Dir[tmp].x, y + Dir[tmp].y };
		Point nextNext = { next.x + Dir[tmp].x, next.y + Dir[tmp].y };

		_stage[next.y][next.x].obj = STAGE_OBJ::EMPTY;
		_stage[nextNext.y][nextNext.x].obj = STAGE_OBJ::EMPTY;

		prStack.push(nextNext);
		DigDug(nextNext.x, nextNext.y, _stage);
	}


	void AllWall(int w, int h, vector<vector<StageObj>>& _stage)
	{
		for (int j = 0; j < h; j++)
		{
			for (int i = 0; i < w; i++) {
				if (i == 0 || j == 0 || i == w - 1 || j == h - 1)
					_stage[j][i].obj = STAGE_OBJ::EMPTY;
				else
					_stage[j][i].obj = STAGE_OBJ::WALL;
				_stage[j][i].weight = 0.0;
				_stage[j][i].num = -1;
			}
		}
	}

	void MakeMazeDigDug(int w, int h, vector<vector<StageObj>>& _stage)
	{
		AllWall(w, h, _stage);
		Point sp{ 1, 1 };
		prStack.push(sp);
		while (!prStack.empty())
		{
			sp = prStack.top();
			prStack.pop();
			DigDug(sp.x, sp.y, _stage);
		}


		for (int j = 0; j < h; j++)
		{
			for (int i = 0; i < w; i++)
			{
				if (i == 0 || j == 0 || i == w - 1 || j == h - 1)
					_stage[j][i].obj = STAGE_OBJ::WALL;
				continue;
			}
		}
	}

	void PillarMaze(int w, int h, vector<vector<StageObj>>& _stage)
	{
		AllWall(w, h, _stage);
		for (int y = 0; y < h; y++)
		{
			for (int x = 0; x < w; x++)
			{
				if (y == 0 || y == h - 1 || x == 0 || x == w - 1)
				{
					_stage[y][x].obj = STAGE_OBJ::WALL;
				}
				else
				{
					if (x % 2 == 0 && y % 2 == 0)
						_stage[y][x].obj = STAGE_OBJ::WALL;
					else {
						_stage[y][x].obj = STAGE_OBJ::EMPTY;
						_stage[y][x].weight = rand() % 5;
					}
				}

			}
		}
	}

	void DijkstraMaze(int w, int h, vector<vector<StageObj>>& _stage) {
		AllWall(w, h, _stage);
		int temp = w / 3;
		for (int i = 0; i < h; i++) {
			for (int j = 0; j < w; j++) {
				if (i == 0 || i == h - 1 || j == 0 || j == w - 1)
				{
					_stage[i][j].obj = STAGE_OBJ::WALL;
				}
				else {
					if (j == temp) {
						if (i >= (h / 2 - h / 4)) {
							_stage[i][j].obj = STAGE_OBJ::WALL;
						}
						else {
							_stage[i][j].obj = STAGE_OBJ::EMPTY;
							_stage[i][j].weight = rand() % 5;
						}
					}
					else if (j == temp * 2) {
						if (i <= (h / 2 + h / 4)) {
							_stage[i][j].obj = STAGE_OBJ::WALL;
						}
						else {
							_stage[i][j].obj = STAGE_OBJ::EMPTY;
							_stage[i][j].weight = rand() % 5;
						}
					}
					else {
						_stage[i][j].obj = STAGE_OBJ::EMPTY;
						_stage[i][j].weight = rand() % 5;
					}
				}
			}
		}

	}

}




Stage::Stage()
{
	stageData = vector(STAGE_HEIGHT, vector<StageObj>(STAGE_WIDTH, { STAGE_OBJ::EMPTY, 1.0f,1 }));

	//MakeMazeDigDug(STAGE_WIDTH, STAGE_HEIGHT, stageData);

	//PillarMaze(STAGE_WIDTH, STAGE_HEIGHT, stageData);

	DijkstraMaze(STAGE_WIDTH, STAGE_HEIGHT, stageData);
	setStageRects();
	type_ = DIJKSTRA;
}

Stage::~Stage()
{
}

void Stage::Update()
{
	ImGui::Begin("Config");
	if (ImGui::TreeNode("Stage")) {
		if (ImGui::Button("Pillar")) {
			PillarMaze(STAGE_WIDTH, STAGE_HEIGHT, stageData);
			setStageRects();
			type_ = PILLAR;
		}
		if (ImGui::Button("DigDug")) {
			MakeMazeDigDug(STAGE_WIDTH, STAGE_HEIGHT, stageData);
			setStageRects();
			type_ = DIGDUG;
		}
		if (ImGui::Button("Dijkstra")) {
			DijkstraMaze(STAGE_WIDTH, STAGE_HEIGHT, stageData);
			setStageRects();
			type_ = DIJKSTRA;
		}
		if (ImGui::Button("WeightReset")) {
			GridWeightReset();
		}
		ImGui::TreePop();
	}

	ImGui::End();
}

void Stage::Draw()
{


	for (int y = 0; y < STAGE_HEIGHT; y++)
	{
		for (int x = 0; x < STAGE_WIDTH; x++)
		{
			switch (stageData[y][x].obj)
			{
			case STAGE_OBJ::EMPTY:
				DrawBox(x * CHA_WIDTH, y * CHA_HEIGHT, x * CHA_WIDTH + CHA_WIDTH, y * CHA_HEIGHT + CHA_HEIGHT, GetColor(stageData[y][x].weight * 60, 205, 170), TRUE);
				DrawBox(x * CHA_WIDTH, y * CHA_HEIGHT, x * CHA_WIDTH + CHA_WIDTH, y * CHA_HEIGHT + CHA_HEIGHT, GetColor(0, 0, 0), false);
				break;
			case STAGE_OBJ::WALL:
				DrawBox(x * CHA_WIDTH, y * CHA_HEIGHT, x * CHA_WIDTH + CHA_WIDTH, y * CHA_HEIGHT + CHA_HEIGHT, GetColor(69, 86, 103), TRUE);
				DrawBox(x * CHA_WIDTH, y * CHA_HEIGHT, x * CHA_WIDTH + CHA_WIDTH, y * CHA_HEIGHT + CHA_HEIGHT, GetColor(0, 0, 0), false);
				break;
			case STAGE_OBJ::GOAL:

				break;
			default:
				break;
			}
		}
	}

}

void Stage::setStageRects()
{
	stageRects.clear();


	for (int y = 0; y < STAGE_HEIGHT; y++)
	{
		for (int x = 0; x < STAGE_WIDTH; x++)
		{
			if (stageData[y][x].obj == STAGE_OBJ::WALL)
			{
				stageRects.push_back(Rect(x * CHA_WIDTH, y * CHA_HEIGHT, CHA_WIDTH, CHA_HEIGHT));
			}
		}
	}

}

void Stage::GridNumReset()
{
	for (int y = 0; y < STAGE_HEIGHT; y++)
	{
		for (int x = 0; x < STAGE_WIDTH; x++)
		{
			stageData[y][x].num = -1;
		}
	}
}

void Stage::GridWeightReset()
{
	for (int y = 0; y < STAGE_HEIGHT; y++)
	{
		for (int x = 0; x < STAGE_WIDTH; x++)
		{
			stageData[y][x].weight = 0;
		}
	}
}
