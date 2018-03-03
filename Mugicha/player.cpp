#include "collision_checker.h"
#include "player.h"
#include "thorn.h"
#include "bullet.h"

bool Player::collision_for_enemies()
{
	// 敵との当たり判定
	for (const auto& _enemy : polygons[SquarePolygonBase::PolygonTypes::ENEMY])
	{
		const auto& enemy = static_cast<Enemy*>(_enemy);
		if (enemy->is_alive() && is_collision(enemy->get_square(), get_square()))
		{
			if (zoom_level >= 1.0f)
			{
#ifndef _NEVER_DIE
				// プレイヤの負け
				kill(DeadReason::HitEnemy);
				return true;
#endif
			}
			else
			{
				// 敵の負け
				enemy->kill();
			}
		}
	}
	return false;
}

bool Player::collision_for_thorns()
{
	// トゲとの当たり判定
	// 通常状態より自分がデカい状態で接触死の判定を取る
	if (zoom_level > 1.0f) return false;
	for (const auto& thorn : polygons[SquarePolygonBase::PolygonTypes::THORN])
	{
		auto self = get_square();
		auto another = thorn->get_square();

		// トゲを落とす
		if (vec != Player::Vec::CENTER && static_cast<Thorn*>(thorn)->attack)
		{
			if (is_collision(SQUARE(get_square().x + get_square().w * (vec == Player::Vec::RIGHT ? 1 : -1), get_square().y + get_square().h * 2, w * 2, get_square().h * 10), another))
			{
				static_cast<Thorn*>(thorn)->trigger_falling();
			}
		}

		switch (static_cast<Thorn*>(thorn)->get_vec())
		{
		case Thorn::Vec::UP:
			if (hit_bottom(self, another))
			{
#ifndef _NEVER_DIE
				kill(DeadReason::HitThorn);
				return true;
#endif
			}
			break;
		case Thorn::Vec::DOWN:
			if (hit_top(self, another))
			{
#ifndef _NEVER_DIE
				kill(DeadReason::HitThorn);
				return true;
#endif
			}
			break;
		case Thorn::Vec::RIGHT:
			if (hit_left(self, another))
			{
#ifndef _NEVER_DIE
				kill(DeadReason::HitThorn);
				return true;
#endif
			}
			break;
		case Thorn::Vec::LEFT:
			if (hit_right(self, another))
			{
#ifndef _NEVER_DIE
				kill(DeadReason::HitThorn);
				return true;
#endif
			}
			break;
		}
	}

	return false;
}

bool Player::collision_for_magmas()
{
	// マグマとの当たり判定
	for (const auto& magma : polygons[SquarePolygonBase::PolygonTypes::MAGMA])
	{
		if (is_collision(get_square(), magma->get_square()))
		{
#ifndef _NEVER_DIE
			kill(DeadReason::HitMagma);
			return true;
#endif
		}
	}
	return false;
}

bool Player::collision_for_bullets()
{
	// 弾丸との当たり判定
	for (const auto& bullet : polygons[SquarePolygonBase::PolygonTypes::BULLET])
	{
		if (is_collision(get_square(), bullet->get_square()))
		{
			bullet->init();
#ifndef _NEVER_DIE
			kill(DeadReason::Shot);
			return true;
#endif
		}
	}
	return false;
}

D3DXVECTOR2 Player::collision_for_knockback_bullets()
{
	auto knockback = D3DXVECTOR2(0, 0);

	auto self = get_square();
	
	for (const auto& _bullet : polygons[SquarePolygonBase::PolygonTypes::KNOCKBACK_BULLET])
	{
		const auto& bullet = static_cast<Bullet*>(_bullet);

		// 発射されていなければ無視
		if (bullet->is_triggered())
		{
			auto another = bullet->get_square();
			auto vec = bullet->get_vec();
			auto result = where_collision(another, self, 0.0f);

			if (result & HitLine::TOP && vec == Bullet::Vec::UP)
			{
				knockback.y += KNOCKBACK_VOLUME;
				bullet->init();
			}
			else if (result & HitLine::BOTTOM && vec == Bullet::Vec::DOWN)
			{
				knockback.y -= KNOCKBACK_VOLUME;
				bullet->init();
			}
			else if (result & HitLine::LEFT && vec == Bullet::Vec::LEFT)
			{
				knockback.x -= KNOCKBACK_VOLUME;
				bullet->init();
			}
			else if (result & HitLine::RIGHT && vec == Bullet::Vec::RIGHT)
			{
				knockback.x += KNOCKBACK_VOLUME;
				bullet->init();
			}
		}
	}
	return knockback;
}

void Player::collision_for_knockback_bullets(D3DXVECTOR2 & vector, char & result)
{
	auto knockback = collision_for_knockback_bullets();

	if ((knockback.x < 0 && !(result & HitLine::LEFT)) || knockback.x > 0 && !(result & HitLine::RIGHT))
	{
		vector.x += knockback.x;
	}

	// 上下
	if ((knockback.y < 0 && !(result & HitLine::BOTTOM)) || knockback.y > 0 && !(result & HitLine::TOP))
	{
		vector.y += knockback.y;
	}
}

bool Player::is_fallen_hellgate()
{
	auto self = get_square();
	self.h *= 0.9; // 甘さ
	if (is_collision(self, polygons[PlainSquarePolygon::PolygonTypes::HELLGATE].front()->get_square()))
	{
#ifndef _NEVER_DIE
		// プレイヤの負け
		kill(DeadReason::FallenHellGate);
		return true;
#endif
	}
	return false;
}

char Player::collision_check()
{
	// 当たり精査
	char result = 0x00;
	for (const auto& type : COLLISION_CHECK_POLYGONTYPES)
		for (const auto& polygon : polygons[type])
			result |= where_collision(this, polygon);
	return result;
}

void Player::sandwich_check(char &result)
{
	// 挟まれ判定
	if (result & HitLine::BOTTOM && result & HitLine::TOP && result & HitLine::LEFT && result & HitLine::RIGHT)
	{
		kill(DeadReason::Sandwiched);
	}
}

void Player::falling_out_check(char &result)
{
	if (y < FALLING_OUT_Y)
	{
		kill(DeadReason::Falling);
	}

}

void Player::ground_check(char &result)
{
	if (result & HitLine::BOTTOM)
	{
		ground = true;
	}
	else
	{
		ground = false;
	}
}

void Player::head_check(char & result)
{
	if (result & HitLine::TOP) jumping = false;
}

void Player::controlls(D3DXVECTOR2 & vector, char & result)
{
	// 左方向への移動
	if (!(result & HitLine::LEFT) && (GetKeyboardPress(DIK_A)))
	{
		vector.x -= speed;
		vec = Player::Vec::LEFT;
	}
	
	// 右方向への移動
	if (!(result & HitLine::RIGHT) && (GetKeyboardPress(DIK_D)))
	{
		vector.x += speed;
		vec = Player::Vec::RIGHT;
	}

	// プレイヤをジャンプさせる
	if (!controll_lock && ground && GetKeyboardTrigger(DIK_SPACE))
	{
		ground = false;
		jumped_at = SCNOW;
		jumping = true;
	}

	// プレイヤに掴ませたりする
	if (GetKeyboardTrigger(DIK_U))
	{
		if (is_holding_item()) release_item();
		else catch_item();
	}

#ifdef _DEBUG // あちこち行っちゃうぜデバッグモード
	if (GetKeyboardPress(DIK_UPARROW)) vector.y += 5;
	if (GetKeyboardPress(DIK_DOWNARROW)) vector.y -= 5;
	if (GetKeyboardPress(DIK_LEFTARROW)) vector.x -= 5;
	if (GetKeyboardPress(DIK_RIGHTARROW)) vector.x += 5;
#endif
}

void Player::jump(D3DXVECTOR2 & vector, char & result)
{
	if (!(result & HitLine::TOP) && jumping)
	{
		auto spent = time_diff(jumped_at);
		if (spent < PLAYER_JUMP_TIME || (GetKeyboardPress(DIK_SPACE) && spent < PLAYER_HOLD_JUMP_TIME)) vector.y += PLAYER_JUMP_POWER;
		else jumping = false;
	}
}

void Player::drifting(D3DXVECTOR2 & vector)
{
	unless(ground) vector.y -= PLAYER_FALLING;
}

bool Player::catch_item()
{
	for (const auto& _item : polygons[SquarePolygonBase::PolygonTypes::ITEM])
	{
		if (static_cast<Item*>(_item)->hold(get_square()))
		{
			item = static_cast<Item*>(_item);
			return true;
		}
	}

	return false;
}

void Player::release_item()
{
	item->release();
	item = nullptr;
}

bool Player::is_holding_item()
{
	return item != nullptr;
}

Player::Player(LPDIRECT3DTEXTURE9 _tex, D3DXVECTOR2 &_camera, PolygonsContainer & _polygons, int _layer, float _x, float _y, float _w, float _h, float _u, float _v, float _uw, float _vh)
	: PlainSquarePolygon(_x, _y, _w, _h, _tex, _layer, _camera, _u, _v, _uw, _vh),
	polygons(_polygons), before_zoom_level(1.0f), dead_reason(DeadReason::ALIVE),
	vec(Player::Vec::CENTER), item(nullptr), jumped_at(SCNOW), jumping(false)
{
	init();
}

void Player::init()
{
	speed = PLAYER_SPEED;
	ground = false;
	controll_lock = false;
}

void Player::zoom(float _zoom_level)
{
	zoom_level = _zoom_level;
}

void Player::update()
{
	// statusみて切る
	unless (status) return; 

	// 敵との当たり判定
	if (collision_for_enemies())
	{
		// 死
		return;
	}

	// トゲとの当たり判定
	if (collision_for_thorns())
	{
		// 死
		return;
	}

	// マグマとの当たり判定
	if (collision_for_magmas())
	{
		// 死
		return;
	}

	// 弾丸との当たり判定
	if (collision_for_bullets())
	{
		// 死〜
		return;
	}

	// 下から上がってくるアレとの当たり判定
	if (is_fallen_hellgate())
	{
		// 死〜〜
		return;
	}
		
	if (controll_lock)
	{
		// 移動前の座標と拡縮する前のズームレベルと現在のズームレベルから割り出したモノをかけていく．
		x = when_locked_coords.x * (zoom_level / before_zoom_level);
		y = when_locked_coords.y * (zoom_level / before_zoom_level);
	}
	else
	{
		// いくら移動したかをここに
		auto vector = D3DXVECTOR2(0, 0);
		
		// 障害物などとの当たり判定								 
		auto result = collision_check(); 
		
		// ノックバックのある空気砲について当たり判定を見る
		collision_for_knockback_bullets(vector, result);

		// 挟まれているか判定
		sandwich_check(result);

		// 枠外落下判定
		falling_out_check(result);
		
		// 接地判定
		ground_check(result);

		// 頭をぶつけていないか
		head_check(result);

		// 操作
		controlls(vector, result);

		// ジャンプ処理
		jump(vector, result);

		// 浮いている状態
		drifting(vector);

		// 変更を加算して終了
		x += vector.x;
		y += vector.y;
	}
		
	// itemを持っているならitemの位置を修正してあげる
	if (is_holding_item()) item->move(D3DXVECTOR2(x + w / (vec == Player::Vec::RIGHT ? 2 : -2), y));
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

void Player::kill(const DeadReason & _dead_reason)
{
#ifndef _NEVER_DIE
	dead_reason = _dead_reason;
#endif
}

bool Player::is_jumping()
{
	return jumping;
}

Player::DeadReason Player::dead()
{
	return dead_reason;
}

Player::Vec Player::get_vec()
{
	return vec;
}

// === Player END ===