#pragma once

#include "scalable_object.h"
#include "helpers.h"

// 初期値など
#define ENEMY_COLLISION_CHECK_POLYGONTYPES { \
	SquarePolygonBase::PolygonTypes::SCALABLE_OBJECT, \
	SquarePolygonBase::PolygonTypes::RAGGED_FLOOR, \
	SquarePolygonBase::PolygonTypes::THORN, \
	SquarePolygonBase::PolygonTypes::AIRCANNON, \
	SquarePolygonBase::PolygonTypes::GIMMICK_SWITCH, \
	SquarePolygonBase::PolygonTypes::MAGMA \
	} // 当たり判定を取るポリゴンのタイプまとめ
#define ENEMY_WIDTH (CELL_WIDTH * 0.8) // 幅
#define ENEMY_HEIGHT (CELL_HEIGHT * 0.8) // 高さ
#define ENEMY_SPEED (0.3f)
#define ENEMY_JUMP_POWER (2.0f)
#define ENEMY_JUMP_HEIGHT (CELL_HEIGHT * 2.0)
#define ENEMY_FALLING (1.0f)
#define RESPAWN_TIME (5000) // リスポーンにかかる時間（初期値）
#define ACTIVE_AREA_WIDTH (SCREEN_WIDTH * 2)
#define ACTIVE_AREA_HEIGHT (SCREEN_HEIGHT * 2)

/*
* Enemy
* 敵さんのクラス
*/

class Enemy : public ScalableObject
{
public:
	enum class Vec{ LEFT, RIGHT}; // 進む向き
private:
	D3DXVECTOR2 init_coords; // 初期位置
	bool alive; // 生きているか
	time_point dead_time; // 死んだタイミング
	const float respawn_time; // リスポーンにかかる時間
	bool on_ground; // 接地
	bool jumping; // ジャンプしている
	bool moving; // 行動
	float jumped_at; // ジャンプした時点の時刻
	Vec vec;
	PolygonsContainer &polygons;

	void init(); // 初期化
	void generate_vertexes();
public:
	Enemy(LPDIRECT3DTEXTURE9 _tex, D3DXVECTOR2 &_camera, int _layer, float _x, float _y, Vec _vec, PolygonsContainer &_polygons, float _w = ENEMY_WIDTH, float _h = ENEMY_HEIGHT, float _respawn_time = RESPAWN_TIME, float _u = 0.0f, float _v = 0.0f, float _uw = 1.0f, float _vh = 1.0f);
	void update();
	bool is_alive();
	void kill();
};