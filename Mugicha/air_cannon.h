#pragma once

#include "scalable_object.h"
#include "bullet.h"
#include <map>

/*
* AirCannon, 空気砲のクラス
*/


class AirCannon : public ScalableObject
{
public:
	enum class Vec
	{
		UP,
		DOWN,
		RIGHT,
		LEFT,
	};

	AirCannon(float _x, float _y, float _w, float _h, LPDIRECT3DTEXTURE9 _tex, LPDIRECT3DTEXTURE9 _bullet_tex, int _layer, D3DXVECTOR2 &_camera, AirCannon::Vec _vec, PolygonsContainer &_polygons, float _u = 0.0f, float _v = 0.0f, float _uw = 1.0f, float _vh = 1.0f);
	void update();
	void draw();
	Bullet *get_bullet();
private:
	PolygonsContainer &polygons;
	Bullet *bullet;
	AirCannon::Vec vec;
};

