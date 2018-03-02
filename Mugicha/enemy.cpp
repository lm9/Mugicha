#include "enemy.h"
#include "collision_checker.h"

Enemy::Enemy(LPDIRECT3DTEXTURE9 _tex, D3DXVECTOR2 &_camera, int _layer, float _x, float _y, Vec _vec, PolygonsContainer &_polygons, float _w, float _h, float _u, float _v, float _uw, float _vh)
	: ScalableObject(_x, _y, _w, _h, _tex, _layer, _camera, _u, _v, _uw, _vh), vec(_vec), polygons(_polygons), moving(false), jumping(false), jumped_at(SCNOW), on_ground(true)
{
	speed = ENEMY_SPEED;
}

void Enemy::update()
{
	unless (status) return;

	// 画面外かどうかでmovingを切り分ける
	moving = (vertexes[0].x <= SCREEN_WIDTH * 1.25 && vertexes[1].x >= 0 && vertexes[0].y <= SCREEN_HEIGHT * 1.25 && vertexes[2].y >= 0) ? true : false;

	if (moving) // 1ms間隔で
	{
		// 当たり精査
		char result = 0x00;
		for (const auto& type : { SquarePolygonBase::PolygonTypes::SCALABLE_OBJECT, SquarePolygonBase::PolygonTypes::RAGGED_FLOOR, SquarePolygonBase::PolygonTypes::THORN, SquarePolygonBase::PolygonTypes::AIRCANNON })
			for (const auto& polygon : polygons[type])
				result |= where_collision(this, polygon, 1.0f);

		auto vector = D3DXVECTOR2(0, 0); // いくら移動したかをここに

		// アタマぶつけとる
		if (result & TOP) jumping = false;

		// 足がついている
		if (result & BOTTOM) on_ground = true;

		// 左がぶつかっている
		if (result & LEFT) vec = Vec::RIGHT;

		// 右がぶつかっている
		if (result & RIGHT) vec = Vec::LEFT;
		
		vector.x += speed * (vec == Vec::RIGHT ? 1 : -1);

		// ジャンプ開始から時間が経過
		if (time_diff(jumped_at) > ENEMY_JUMP_TIME) jumping = false;
		
		// ジャンプなう
		if (jumping) vector.y += ENEMY_JUMP_POWER;
		
		// 空中に居る間は落下し続ける
		if (!on_ground) vector.y -= ENEMY_FALLING;

		// 次々ジャンプしていけ
		if (on_ground && !jumping)
		{
			jumping = true;
			on_ground = false;
			jumped_at = SCNOW;
		}

		// 反映
		x += vector.x;
		y += vector.y;
	}
}
