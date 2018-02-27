#include "player.h"
#include "collision_checker.h"
#include "thorn.h"

bool Player::collision_for_enemies()
{
	// 敵との当たり判定
	for (const auto& enemy : polygons[SquarePolygonBase::PolygonTypes::ENEMY])
	{
		if (is_collision(enemy->get_square(), get_square()))
		{
			if (zoom_level.w <= 1.0f)
			{
				// プレイヤの負け
				kill(DeadReason::HitEnemy);
				return true;
			}
			else
			{
				// 敵の負け
				enemy->off();
			}
		}
	}
	return false;
}

bool Player::collision_for_thorns()
{
	// トゲとの当たり判定
	// 通常状態より自分がデカい状態で接触死の判定を取る
	if (zoom_level.w > 1.0f) return false;
	for (const auto& _thorn : polygons[SquarePolygonBase::PolygonTypes::THORN])
	{
		auto thorn = static_cast<Thorn*>(_thorn);
		char result = where_collision(this, thorn, 1.0f);		
		if (
			(result & HitLine::BOTTOM && thorn->get_vec() == Thorn::Vec::UP)
			|| (result & HitLine::TOP && thorn->get_vec() == Thorn::Vec::DOWN)
			|| (result & HitLine::LEFT && thorn->get_vec() == Thorn::Vec::RIGHT)
			|| (result &  HitLine::RIGHT && thorn->get_vec() == Thorn::Vec::LEFT)
			)
		{
			kill(DeadReason::HitThorn);
			return true;
		}
	}

	return false;
}

bool Player::collision_for_magmas()
{
	// マグマとの当たり判定
	for (const auto& magma : polygons[SquarePolygonBase::PolygonTypes::MAGMA])
	{
		if (is_collision(this, magma))
		{
			kill(DeadReason::HitMagma);
			return true;
		}
	}
	return false;
}

// コンストラクタ 座標とかをセットしていく
Player::Player(LPDIRECT3DTEXTURE9 _tex, D3DXVECTOR2 &_camera, std::map<SquarePolygonBase::PolygonTypes, std::vector<SquarePolygonBase*>>& _polygons, int _layer, float _x, float _y, float _w, float _h, float _u, float _v, float _uw, float _vh)
	: polygons(_polygons), PlainSquarePolygon(_x, _y, _w, _h, _tex, _layer, _camera, _u, _v, _uw, _vh), before_zoom_level(1, 1), alive(true)
{
	init();
}

// デストラクタ
Player::~Player()
{
	// 特になし
}

void Player::init()
{
	speed = PLAYER_SPEED;
	ground = false;
	controll_lock = false;
}

void Player::zoom(POLSIZE _zoom_level)
{
	zoom_level = _zoom_level;
}

void Player::update()
{
	if (!status) return; // statusみて切る

	auto current = std::chrono::system_clock::now();

	// 操作
	if (std::chrono::duration_cast<std::chrono::milliseconds>(current - latest_update).count() > 1) // 1ms間隔で
	{
		if (collision_for_enemies())
		{
			// 死
			return;
		}

		if (collision_for_thorns())
		{
			// 死
			return;
		}

		if (collision_for_magmas())
		{
			// 死
			return;
		}
		
		// 当たり精査
		char result = 0x00;
		for (const auto& type : { SquarePolygonBase::PolygonTypes::SCALABLE_OBJECT, SquarePolygonBase::PolygonTypes::RAGGED_FLOOR, SquarePolygonBase::PolygonTypes::THORN })
			for (const auto& polygon : polygons[type])
				result |= where_collision(this, polygon);

		if (controll_lock)
		{
			// 移動前の座標と拡縮する前のズームレベルと現在のズームレベルから割り出したモノをかけていく．
			x = when_locked_coords.x * (zoom_level.w / before_zoom_level.w);
			y = when_locked_coords.y * (zoom_level.h / before_zoom_level.h);
		}
		else
		{
			auto vector = D3DXVECTOR2(0, 0); // いくら移動したかをここに

			// 挟まれ判定
			if ((result & HitLine::BOTTOM && result & HitLine::TOP) || (result & HitLine::LEFT && result & HitLine::RIGHT))
			{
				kill(DeadReason::Sandwiched);
			}

			// 枠外落下判定
			if (y < -50)
			{
				kill(DeadReason::Falling);
			}

			// 接地判定
			if (result & HitLine::BOTTOM)
			{
				ground = true;
			}
			else
			{
				ground = false;
			}

			if (!(result & HitLine::LEFT) && (GetKeyboardPress(DIK_A) || GetKeyboardPress(DIK_LEFTARROW))) // 左方向への移動
			{
				vector.x -= speed;
			}
			if (!(result & HitLine::RIGHT) && (GetKeyboardPress(DIK_D) || GetKeyboardPress(DIK_RIGHTARROW))) // 右方向への移動
			{
				vector.x += speed;
			}

			// TODO: ジャンプ量とジャンプしている時間を調整する必要アリ
			if (!(result & HitLine::TOP) && jumping)
			{
				if (timeGetTime() - jumped_at > PLAYER_JUMP_TIME) jumping = false;
				vector.y += PLAYER_JUMP_POWER;
			}

			if(result & HitLine::TOP)
			{
				jumping = false;
			}

			// TODO: 同様に落下速度も調整する必要がある
			unless(ground)
			{
				vector.y -= PLAYER_FALLING;
			}

			// 変更を加算して終了
			x += vector.x;
			y += vector.y;
		}
		
		latest_update = current;
	}
}

bool Player::jump()
{
	unless(ground) return false;
	if (controll_lock) return false;
	ground = false;
	jumped_at = timeGetTime();
	jumping = true;
	return true;
}

void Player::lock()
{
	controll_lock = true;
	when_locked_coords = { x, y };
	before_zoom_level = zoom_level;
}

void Player::unlock()
{
	controll_lock = false;
}

void Player::kill()
{
	alive = false;
}

void Player::kill(const std::string & reason)
{
	kill();
	std::cout << reason.c_str() << std::endl;
}

void Player::kill(const DeadReason & reason)
{
	kill();
}

bool Player::dead()
{
	return !alive;
}

// === Player END ===