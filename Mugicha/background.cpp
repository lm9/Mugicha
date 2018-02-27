#include "background.h"

Background::Background(LPDIRECT3DTEXTURE9 _tex, D3DXVECTOR2 &_camera, float _u, float _v, float _uw, float _vh)
	: ScalableObject(BACKGROUND_X, BACKGROUND_Y, BACKGROUND_WIDTH, BACKGROUND_HEIGHT, _tex, INT_MAX, _camera, _u, _v, _uw, _vh)
{
}

Background::Background(float _x, float _y, float _w, float _h, LPDIRECT3DTEXTURE9 _tex, D3DXVECTOR2 &_camera, float _u, float _v, float _uw, float _vh)
	: ScalableObject(_x, _y, _w, _h, _tex, INT_MAX, _camera, _u, _v, _uw, _vh)
{
}

Background::~Background()
{
	// 無
}

void Background::update()
{
	if (!status) return;
	
	auto current = std::chrono::system_clock::now();


	// 操作
	if (std::chrono::duration_cast<std::chrono::milliseconds>(current - latest_update).count() > 1) // 1ms間隔で
	{
		// uv値の変更などをする

		latest_update = current;
	}
}