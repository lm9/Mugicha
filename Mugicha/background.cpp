#include "background.h"

Background::Background(LPDIRECT3DTEXTURE9 _tex, D3DXVECTOR2 *_camera, float _u, float _v, float _uw, float _vh)
	: ScalableObject(BACKGROUND_X, BACKGROUND_Y, BACKGROUND_WIDTH, BACKGROUND_HEIGHT, _tex, INT_MAX, _camera, _u, _v, _uw, _vh)
{
}

Background::~Background()
{
	// 無
}

void Background::update()
{
	if (!status) return;
	
	DWORD current = timeGetTime();

	// 操作
	if (current - latest_update > 1) // 1ms間隔で
	{
		// uv値の変更などをする

		latest_update = current;
	}
}