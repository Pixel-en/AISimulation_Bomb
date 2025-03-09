#include "Enemy.h"
#include "Stage.h"
#include "ImGui/imgui.h"
#include "Player.h"
#include <map>
#include <queue>

namespace {
	const Point MDir[5] = { {0,-1},{0,1},{-1,0},{1,0},{0,0 } };
	const float CHENGETIME{ 10.0f };
}

void Enemy::MoveAlgo()
{
	switch (MoveSwitch_)
	{
	case Enemy::RANDOM:
		RandomMove();
		break;
	case Enemy::RIGHTHAND:
		RightHandMove();
		break;
	case Enemy::LEFTHAND:
		LeftHandMove();
		break;
	case Enemy::MIX:
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
		break;
	case Enemy::BFS:
		BFSAlgo();
		break;
	case Enemy::DIJKSTRA:
		Dijkstra();
		break;
	case Enemy::ASTAR:
		AStar();
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
	DIR myRight[5] = { RIGHT, LEFT, UP, DOWN,NONE };
	DIR myLeft[5] = { LEFT, RIGHT, DOWN, UP,NONE };
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
	DIR myRight[5] = { RIGHT, LEFT, UP, DOWN,NONE };
	DIR myLeft[5] = { LEFT, RIGHT, DOWN, UP,NONE };

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

void Enemy::BFSAlgo()
{

	Player* player = FindGameObject<Player>();
	Point TargetGridPos = { player->GetPositionPoint().x / CHA_WIDTH,player->GetPositionPoint().y / CHA_HEIGHT };	//�O���b�h���W
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


	route_.clear();

	route_.push_back({ TargetGridPos.x,TargetGridPos.y ,DIR::NONE });

	DIR SearchDir[4] = { DOWN,UP,RIGHT,LEFT };

	for (int i = num - 1; i >= 0; i--) {

		for (int j = 0; j < 4; j++) {
			Point BWpos = { route_.back().Nextpos.x + MDir[j].x,route_.back().Nextpos.y + MDir[j].y };
			if (stage->GetStageData(BWpos.x, BWpos.y).num == i) {
				route_.push_back({ BWpos,(DIR)SearchDir[j] });
				break;
			}
		}

	}

	dir = route_.back().dir;
}

void Enemy::Dijkstra()
{
	Player* player = FindGameObject<Player>();
	Point sp = { pos_.x / CHA_WIDTH, pos_.y / CHA_HEIGHT };
	Point gp = { player->GetPositionPoint().x / CHA_WIDTH, player->GetPositionPoint().y / CHA_HEIGHT };

	if (gp == PlayerOrigion_ && !route_.empty()) {

		if (sp == route_.back().Nextpos) {
			//�ړI�n�ɒ������玟�̕������o��
			dir = route_.back().dir;
			//���̖ړI�n���o��
			route_.pop_back();
		}

		return;
	}

	using Mdat = std::pair<int, Point>;

	PlayerOrigion_ = gp;

	//�}�b�v���̏�����
	for (int i = 0; i < STAGE_HEIGHT; i++) {
		for (int j = 0; j < STAGE_WIDTH; j++) {
			dist[i][j] = INT_MAX;
			pre[i][j] = { -1,-1 };
		}
	}

	dist[sp.y][sp.x] = 0;
	//Mdat�^�Avector,����
	std::priority_queue<Mdat, std::vector<Mdat>, std::greater<Mdat>> pq;
	pq.push(Mdat(0, { sp.x, sp.y }));
	vector<vector<StageObj>> stageData = FindGameObject<Stage>()->GetStageGrid();

	//�}�b�v�̏����擾����
	while (!pq.empty())
	{
		Mdat p = pq.top();
		pq.pop();

		int c = p.first;
		Point v = p.second;

		for (int i = 0; i < 4; i++)
		{
			//4�����A�N�Z�X
			Point np = { v.x + (int)MDir[i].x, v.y + (int)MDir[i].y };

			//�X�e�[�W�̊O
			if (np.x < 0 || np.y < 0 || np.x >= STAGE_WIDTH || np.y >= STAGE_HEIGHT)
				continue;
			//��
			if (stageData[np.y][np.x].obj == STAGE_OBJ::WALL)
				continue;

			//��蓞�B�R�X�g�������ꍇ
			if (dist[np.y][np.x] <= stageData[np.y][np.x].weight + c)
				continue;

			//�R�X�g�X�V
			dist[np.y][np.x] = stageData[np.y][np.x].weight + c;

			//�ǂ����痈���̂��̏��
			pre[np.y][np.x] = Point({ v.x, v.y });

			//�������
			pq.push(Mdat(dist[np.y][np.x], np));
		}
	}

	route_.clear();

	route_.push_back({ gp ,DIR::NONE });

	while (route_.back().Nextpos.x != sp.x || route_.back().Nextpos.y != sp.y)
	{
		Info data = { pre[route_.back().Nextpos.y][route_.back().Nextpos.x],DIR::NONE };
		for (int i = 0; i < 4; i++) {
			Point mdata = { data.Nextpos.x + MDir[i].x,data.Nextpos.y + MDir[i].y };
			//�O�̏ꏊ����ړ�����������������
			if (mdata.x == route_.back().Nextpos.x && mdata.y == route_.back().Nextpos.y) {
				data.dir = (DIR)i;
				break;
			}
		}

		route_.push_back(data);
	}

	dir = route_.back().dir;
	route_.pop_back();
}

void Enemy::AStar()
{

	Player* player = FindGameObject<Player>();
	Point sp = { pos_.x / CHA_WIDTH, pos_.y / CHA_HEIGHT };
	Point gp = { player->GetPositionPoint().x / CHA_WIDTH, player->GetPositionPoint().y / CHA_HEIGHT };

	if (gp == PlayerOrigion_ && !route_.empty()) {

		if (sp == route_.back().Nextpos) {
			//�ړI�n�ɒ������玟�̕������o��
			dir = route_.back().dir;
			//���̖ړI�n���o��
			route_.pop_back();
		}

		return;
	}

	using Mdat = std::pair<int, Point>;

	PlayerOrigion_ = gp;

	//�}�b�v���̏�����
	for (int i = 0; i < STAGE_HEIGHT; i++) {
		for (int j = 0; j < STAGE_WIDTH; j++) {
			dist[i][j] = INT_MAX;
			pre[i][j] = { -1,-1 };
		}
	}

	dist[sp.y][sp.x] = 0;
	//Mdat�^�Avector,����
	std::priority_queue<Mdat, std::vector<Mdat>, std::greater<Mdat>> pq;
	pq.push(Mdat(0, { sp.x, sp.y }));
	vector<vector<StageObj>> stageData = FindGameObject<Stage>()->GetStageGrid();

	//�}�b�v�̏����擾����
	while (!pq.empty())
	{
		Mdat p = pq.top();
		pq.pop();

		int c = p.first;
		Point v = p.second;

		for (int i = 0; i < 4; i++)
		{
			//4�����A�N�Z�X
			Point np = { v.x + (int)MDir[i].x, v.y + (int)MDir[i].y };

			//�X�e�[�W�̊O
			if (np.x < 0 || np.y < 0 || np.x >= STAGE_WIDTH || np.y >= STAGE_HEIGHT)
				continue;
			//��
			if (stageData[np.y][np.x].obj == STAGE_OBJ::WALL)
				continue;

			int gdist = abs(np.x - gp.x) + abs(np.y - gp.y);

			//��蓞�B�R�X�g�������ꍇ
			if (dist[np.y][np.x] <= stageData[np.y][np.x].weight + c + gdist)
				continue;

			//�R�X�g�X�V
			dist[np.y][np.x] = stageData[np.y][np.x].weight + c + gdist;

			//�ǂ����痈���̂��̏��
			pre[np.y][np.x] = Point({ v.x, v.y });

			//�ړ��悪�S�[���Ȃ�
			if (np == gp) {
				pq = std::priority_queue<Mdat, std::vector<Mdat>, std::greater<Mdat>>();
				break;
			}
			//�ړ��悪�S�[���łȂ��Ȃ�
			//�������
			pq.push(Mdat(dist[np.y][np.x], np));
		}
	}

	route_.clear();

	route_.push_back({ gp ,DIR::NONE });

	while (route_.back().Nextpos.x != sp.x || route_.back().Nextpos.y != sp.y)
	{
		Info data = { pre[route_.back().Nextpos.y][route_.back().Nextpos.x],DIR::NONE };
		for (int i = 0; i < 4; i++) {
			Point mdata = { data.Nextpos.x + MDir[i].x,data.Nextpos.y + MDir[i].y };
			//�O�̏ꏊ����ړ�����������������
			if (mdata.x == route_.back().Nextpos.x && mdata.y == route_.back().Nextpos.y) {
				data.dir = (DIR)i;
				break;
			}
		}

		route_.push_back(data);
	}

	dir = route_.back().dir;
	route_.pop_back();
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

	MoveSwitch_ = 0;
	isDisplayRoute_ = false;
	isDisplayDist_ = false;

	dist = vector(STAGE_HEIGHT, vector<int>(STAGE_WIDTH, INT_MAX));
	pre = vector(STAGE_HEIGHT, vector<Point>(STAGE_WIDTH, { -1, -1 }));
	PlayerOrigion_ = { -1, -1 };
	route_.clear();
}

Enemy::~Enemy()
{
}

void Enemy::Update()
{
	Point oldPos = pos_;    //�ړ��O


	move_ = MDir[dir];
	Stage* stage = FindGameObject<Stage>();
	//�ړ���
	Rect moveRect = { pos_.x,pos_.y,CHA_WIDTH,CHA_HEIGHT };
	pos_ = { pos_.x + move_.x,pos_.y + move_.y };
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
			RightHandMove();
			break;
		}
	}
	ImGuiOperation();

	//�~�b�N�X�Ŏg��
	if (ChengeTimer_ < 0) {
		algonum_ = rand() % 3;
		ChengeTimer_ = CHENGETIME;
	}
	else {
		ChengeTimer_ -= Time::DeltaTime();
	}

	//4�����Ɉړ��ł���Ƃ�
	if (pos_.x % CHA_WIDTH == 0 && pos_.y % CHA_HEIGHT == 0) {
		if (stage->GetStageType() == Stage::PILLAR) {
			if (pos_.x / CHA_WIDTH % 2 == 1 && pos_.y / CHA_HEIGHT % 2 == 1) {
				MoveAlgo();
			}
		}
		else {
			MoveAlgo();
		}
	}

}

void Enemy::Draw()
{
	{	//�o�H�\��
		if (isDisplayRoute_) {
			for (int i = 0; i < route_.size(); i++) {
				Point rp[4][3] = {
					{{route_[i].Nextpos.x * CHA_WIDTH + CHA_WIDTH / 2, route_[i].Nextpos.y * CHA_HEIGHT}, {route_[i].Nextpos.x * CHA_WIDTH, route_[i].Nextpos.y * CHA_HEIGHT + CHA_HEIGHT / 2}, {route_[i].Nextpos.x * CHA_WIDTH + CHA_WIDTH, route_[i].Nextpos.y * CHA_HEIGHT + CHA_HEIGHT / 2}},
					{{route_[i].Nextpos.x * CHA_WIDTH + CHA_WIDTH / 2, route_[i].Nextpos.y * CHA_HEIGHT + CHA_HEIGHT}, {route_[i].Nextpos.x * CHA_WIDTH, route_[i].Nextpos.y * CHA_HEIGHT + CHA_HEIGHT / 2}, {route_[i].Nextpos.x * CHA_WIDTH + CHA_WIDTH, route_[i].Nextpos.y * CHA_HEIGHT + CHA_HEIGHT / 2}},
					{{route_[i].Nextpos.x * CHA_WIDTH            , route_[i].Nextpos.y * CHA_HEIGHT + CHA_HEIGHT / 2}, {route_[i].Nextpos.x * CHA_WIDTH + CHA_WIDTH / 2, route_[i].Nextpos.y * CHA_HEIGHT}, {route_[i].Nextpos.x * CHA_WIDTH + CHA_WIDTH / 2, route_[i].Nextpos.y * CHA_HEIGHT + CHA_HEIGHT}},
					{{route_[i].Nextpos.x * CHA_WIDTH + CHA_WIDTH, route_[i].Nextpos.y * CHA_HEIGHT + CHA_HEIGHT / 2}, {route_[i].Nextpos.x * CHA_WIDTH + CHA_WIDTH / 2, route_[i].Nextpos.y * CHA_HEIGHT}, {route_[i].Nextpos.x * CHA_WIDTH + CHA_WIDTH / 2, route_[i].Nextpos.y * CHA_HEIGHT + CHA_HEIGHT}}
				};
				DrawTriangle(rp[route_[i].dir][0].x, rp[route_[i].dir][0].y, rp[route_[i].dir][1].x, rp[route_[i].dir][1].y, rp[route_[i].dir][2].x, rp[route_[i].dir][2].y, GetColor(255, 255, 255), TRUE);
			}
		}
	}

	{	//�R�X�g�\��
		if (isDisplayDist_) {
			Stage* stage = FindGameObject<Stage>();
			for (int y = 0; y < STAGE_HEIGHT; y++)
			{
				for (int x = 0; x < STAGE_WIDTH; x++)
				{
					if (MoveSwitch_ == BFS) {
						if (stage->GetStageNum(x, y) != -1)
							DrawString(x * CHA_WIDTH + 3, y * CHA_HEIGHT + 17, std::to_string(stage->GetStageNum(x, y)).c_str(), GetColor(255, 255, 255), TRUE);
					}
					else {
						if (dist[y][x] < INT_MAX)
							DrawString(x * CHA_WIDTH + 3, y * CHA_HEIGHT + 17, std::to_string(dist[y][x]).c_str(), GetColor(255, 255, 255), TRUE);
					}
				}
			}
		}
	}


	{	//�G�\��
		DrawBox(pos_.x, pos_.y, pos_.x + CHA_WIDTH, pos_.y + CHA_HEIGHT, GetColor(10, 200, 10), TRUE);

		Point tp[4][3] = {
		{{pos_.x + CHA_WIDTH / 2, pos_.y}, {pos_.x, pos_.y + CHA_HEIGHT / 2}, {pos_.x + CHA_WIDTH, pos_.y + CHA_HEIGHT / 2}},
		{{pos_.x + CHA_WIDTH / 2, pos_.y + CHA_HEIGHT}, {pos_.x, pos_.y + CHA_HEIGHT / 2}, {pos_.x + CHA_WIDTH, pos_.y + CHA_HEIGHT / 2}},
		{{pos_.x            , pos_.y + CHA_HEIGHT / 2}, {pos_.x + CHA_WIDTH / 2, pos_.y}, {pos_.x + CHA_WIDTH / 2, pos_.y + CHA_HEIGHT}},
		{{pos_.x + CHA_WIDTH, pos_.y + CHA_HEIGHT / 2}, {pos_.x + CHA_WIDTH / 2, pos_.y}, {pos_.x + CHA_WIDTH / 2, pos_.y + CHA_HEIGHT}}
		};

		DrawTriangle(tp[dir][0].x, tp[dir][0].y, tp[dir][1].x, tp[dir][1].y, tp[dir][2].x, tp[dir][2].y, GetColor(255, 255, 255), TRUE);
	}
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

void Enemy::ImGuiOperation()
{
	ImGui::Begin("Config");
	if (ImGui::TreeNode("EnemyMove")) {
		ImGui::RadioButton("Random", &MoveSwitch_, MOVESWITCH::RANDOM);
		ImGui::RadioButton("RightHand", &MoveSwitch_, MOVESWITCH::RIGHTHAND);
		ImGui::RadioButton("LeftHand", &MoveSwitch_, MOVESWITCH::LEFTHAND);
		ImGui::RadioButton("Mix", &MoveSwitch_, MOVESWITCH::MIX);
		ImGui::RadioButton("BFS", &MoveSwitch_, MOVESWITCH::BFS);
		if (ImGui::RadioButton("Dijkstra", &MoveSwitch_, MOVESWITCH::DIJKSTRA)) {
			route_.clear();
		}
		if (ImGui::RadioButton("A*", &MoveSwitch_, MOVESWITCH::ASTAR)) {
			route_.clear();
		}

		if (MoveSwitch_ >= BFS) {
			if (ImGui::TreeNodeEx("Option", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::Checkbox("DiplayRoute", &isDisplayRoute_);
				ImGui::Checkbox("DiplayDist", &isDisplayDist_);
				ImGui::TreePop();
			}
		}
		else {
			isDisplayRoute_ = false;
			isDisplayDist_ = false;
			route_.clear();
		}

		ImGui::TreePop();
	}



	ImGui::End();

}