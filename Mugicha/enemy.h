#pragma once

#include "scalable_object.h"

// 初期値など
#define COLLISION_CHECK_POLYGONTYPES { \
	SquarePolygonBase::PolygonTypes::SCALABLE_OBJECT, \
	SquarePolygonBase::PolygonTypes::RAGGED_FLOOR, \
	SquarePolygonBase::PolygonTypes::THORN, \
	SquarePolygonBase::PolygonTypes::AIRCANNON, \
	SquarePolygonBase::PolygonTypes::GIMMICK_SWITCH \
	} // 当たり判定を取るポリゴンのタイプまとめ
#define ENEMY_WIDTH (CELL_WIDTH * 0.8) // 幅
#define ENEMY_HEIGHT (CELL_HEIGHT * 0.8) // 高さ
#define ENEMY_SPEED (0.3f)
#define ENEMY_JUMP_POWER (1.01f)
#define ENEMY_FALLING PLAYER_FALLING
#define ENEMY_JUMP_TIME PLAYER_JUMP_TIME

/*
* Enemy
* 敵さんのクラス
*/

class Enemy : public ScalableObject
{
public:
	enum class Vec{ LEFT, RIGHT}; // 進む向き
private:
	bool on_ground; // 接地
	bool jumping; // ジャンプしている
	bool moving; // 行動
	DWORD jumped_at; // ジャンプした時点の時刻
	Vec vec;
	PolygonsContainer &polygons;
public:
	Enemy(LPDIRECT3DTEXTURE9 _tex, D3DXVECTOR2 &_camera, int _layer, float _x, float _y, Vec _vec, PolygonsContainer &_polygons, float _w = ENEMY_WIDTH, float _h = ENEMY_HEIGHT, float _u = 0.0f, float _v = 0.0f, float _uw = 1.0f, float _vh = 1.0f);
	void update();
};