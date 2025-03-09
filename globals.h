#pragma once


	struct Point
	{
		int x, y;

		const bool operator < (const Point& other) const {
			if (x < other.x && y < other.y)
				return true;
			return false;
		}

		const bool operator == (const Point& other) const {
			if (x == other.x && y == other.y)
				return true;
			return false;
		}
	};

	struct Pointf
	{
		float x, y;
	};

	struct Rect
	{
		int x, y, w, h;
		Rect(int x, int y, int w, int h) : x(x), y(y), w(w), h(h) {}
		Point GetCenter() { return Point{ x + w / 2, y + h / 2 }; }
	};


	enum STAGE_OBJ {
		EMPTY,
		WALL,
		GOAL,
		MAX_OBJECT
	};

	struct StageObj
	{
		STAGE_OBJ obj;
		float weight;
		int num;
	};

	const int CHA_WIDTH = 32;
	const int CHA_HEIGHT = 32;

