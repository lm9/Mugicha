#include "collision_checker.h"
#include "player.h"
#include "thorn.h"
#include "bullet.h"
#include "gimmick_floor.h"
#include "keyconf.h"

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
				if (!unrivaled)
				{

					if (life <= 1)
					{
						// プレイヤの負け
						kill(DeadReason::HitEnemy);
						life = 0;
						return true;
					}
					else
					{
						--life;
						unrivaled = true; // 無敵にする
						unrivaled_time = SCNOW;
					}
				}
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
				kill(DeadReason::HitThorn);
				return true;
			}
			break;
		case Thorn::Vec::DOWN:
			if (hit_top(self, another))
			{
				kill(DeadReason::HitThorn);
				return true;
			}
			break;
		case Thorn::Vec::RIGHT:
			if (hit_left(self, another))
			{
				kill(DeadReason::HitThorn);
				return true;
			}
			break;
		case Thorn::Vec::LEFT:
			if (hit_right(self, another))
			{
				kill(DeadReason::HitThorn);
				return true;
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
			kill(DeadReason::HitMagma);
			return true;
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
			kill(DeadReason::Shot);
			return true;
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
	self.y += h / 2; // 甘さ
	if (
		polygons[PlainSquarePolygon::PolygonTypes::HELLGATE].front()->is_drawing()
		&& is_collision(self, polygons[PlainSquarePolygon::PolygonTypes::HELLGATE].front()->get_square())
		)
	{
		// プレイヤの負け
		kill(DeadReason::FallenHellGate);
		return true;
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

	// GIMMICK_FLOORだけappearingを見る
	for (const auto& polygon : polygons[SquarePolygonBase::PolygonTypes::GIMMICK_FLOOR])
		if(static_cast<GimmickFloor*>(polygon)->is_appearing())
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
		v = PLAYER_NORMAL_UV_V;
		ground = true;
	}
	else
	{
		v = PLAYER_JUMPING_UV_V;
		ground = false;
	}
}

void Player::head_check(char & result)
{
	if (result & HitLine::TOP)
	{
		jumping = false;
		audiocontroller->stop(SE_JUMP);
	}
}

void Player::controlls(D3DXVECTOR2 & vector, char & result)
{

	unless(dead_reason == DeadReason::ALIVE) return;

	vec = Player::Vec::CENTER;

	bool walking = false;

	// 左方向への移動
	if (!(result & HitLine::LEFT) && PLAYER_MOVE_LEFT)
	{
		vector.x -= speed;
		vec = Player::Vec::LEFT;
		old_vec = vec;
		walking = true;
	}
	
	// 右方向への移動
	if (!(result & HitLine::RIGHT) && PLAYER_MOVE_RIGHT)
	{
		vector.x += speed;
		vec = Player::Vec::RIGHT;
		old_vec = vec;
		walking = true;
	}

	if (walking)
	{
		audiocontroller->play(SE_WALK);
	}
	else
	{
		audiocontroller->stop(SE_WALK);
	}

#if defined(_DEBUG) || defined(_STAGE_DEBUG) // あちこち行っちゃうぜデバッグモード
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
		auto diff = y - jumped_at;
		if (diff < PLAYER_JUMP_HEIGHT || (PLAYER_JUMP_HOLD && diff < PLAYER_HOLD_JUMP_HEIGHT)) vector.y += PLAYER_JUMP_POWER;
		else
		{
			jumping = false;
		}
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

void Player::update_unrivaled()
{
	unless(unrivaled) return;

	// 点滅
	if (time_diff(unrivaled_time) % 100 == 0)
	{
		is_drawing() ? hide() : show();
	}

	// 無敵時間のチェック
	if (time_diff(unrivaled_time) > UNRIVALED_TIME)
	{
		unrivaled = false;
		show();
	}
}

void Player::generate_vertexes()
{
	// u値を変えてやりたいんだが
	if (vec != Player::Vec::CENTER && ground && dead_reason == DeadReason::ALIVE)
	{
		u += uw * (vec == Player::Vec::RIGHT ? 1 : -1);
	}

	PlainSquarePolygon::generate_vertexes();

	if ((vec == Player::Vec::CENTER && old_vec == Player::Vec::LEFT) || vec == Player::Vec::LEFT)
	{
		if (old_vec == Player::Vec::LEFT)
		{
			for (auto i = 0; i < 4; ++i)
			{
				vertexes[i].u = u + (i % 3 == 0 ? uw : 0);
			}
		}
	}
}

Player::Player(LPDIRECT3DTEXTURE9 _tex, D3DXVECTOR2 &_camera, PolygonsContainer & _polygons, int _layer, float _x, float _y, float _w, float _h, float _u, float _v, float _uw, float _vh)
	: PlainSquarePolygon(_x, _y, _w, _h, _tex, _layer, _camera, _u, _v, _uw, _vh),
	polygons(_polygons), before_zoom_level(1.0f), dead_reason(DeadReason::ALIVE),
	vec(Player::Vec::CENTER), item(nullptr), jumping(false), jumped_at(_y),
	old_vec(Player::Vec::CENTER), dead_falling_speed(0.1f), life(PLAYER_LIFE_COUNT_MAX),
	unrivaled(false), unrivaled_time(SCNOW)
{
	init();
}

Player::~Player()
{
	delete audiocontroller;
}

void Player::init()
{
	speed = PLAYER_SPEED;
	ground = false;
	controll_lock = false;

	audiocontroller = new AudioController(
		PLAYER_AUDIO_PARAMS
	);
}

void Player::zoom(float _zoom_level)
{
	zoom_level = _zoom_level;
}

void Player::update()
{
	// statusみて切る
	unless (status) return; 

	if (
		collision_for_enemies() // 敵との当たり判定
		|| collision_for_thorns() // トゲとの当たり判定
		|| collision_for_magmas() // マグマとの当たり判定
		|| collision_for_bullets() // 弾丸との当たり判定
		|| is_fallen_hellgate() // 下から上がってくるアレとの当たり判定
		)
	{
#ifndef _NEVER_DIE
		return;
#endif
	}
		
	if (controll_lock)
	{
		// 移動前の座標と拡縮する前のズームレベルと現在のズームレベルから割り出したモノをかけていく．
		x = when_locked_coords.x * (zoom_level / before_zoom_level);
		y = when_locked_coords.y * (zoom_level / before_zoom_level);

		// 当たり精査
		auto self = get_square();
		bool run = true;
		for (const auto& type : COLLISION_CHECK_POLYGONTYPES)
		{
			for (const auto& polygon : polygons[type])
			{
				auto sq = polygon->get_square();
				if (hit_bottom(self, sq))
				{
					y += CELL_HEIGHT * zoom_level; // ふわっとあげてやる
					run = false;
					break;
				}
			}
			unless(run) break;
		}

		// GIMMICK_FLOORだけappearingを見る
		for (const auto& polygon : polygons[SquarePolygonBase::PolygonTypes::GIMMICK_FLOOR])
		{
			if (static_cast<GimmickFloor*>(polygon)->is_appearing())
			{
				auto sq = polygon->get_square();
				if (hit_bottom(self, sq))
				{
					y += CELL_HEIGHT * zoom_level; // ふわっとあげてやる
					run = false;
					break;
				}
			}
		}
		
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

		// 時間管理します
		auto current = SCNOW;
		if (time_diff(latest_update, current) > UPDATE_INTERVAL)
		{
			latest_update = current;

			if (dead_reason == DeadReason::ALIVE)
			{
				// 操作
				controlls(vector, result);

				// ジャンプ処理
				jump(vector, result);

				// 浮いている状態
				drifting(vector);
			}
			else
			{
				y -= dead_falling_speed;
				dead_falling_speed *= 1.01f; // 加速させる
			}
			
		}

		// 死んでたらとにかくこのv
		unless (dead_reason == DeadReason::ALIVE) v = PLAYER_DIE_UV_V;
		
		// 変更を加算して終了
		x += vector.x;
		y += vector.y;
	}
		
	// itemを持っているならitemの位置を修正してあげる
	if (is_holding_item()) item->move(D3DXVECTOR2(x + w / ( (vec == Player::Vec::CENTER ? old_vec : vec) == Player::Vec::RIGHT ? 2 : -2), y));

	// 音の更新
	audiocontroller->reload();

	// 無敵について更新
	update_unrivaled();
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
#ifdef _DEBUG
	switch (_dead_reason)
	{
	case DeadReason::ALIVE:
		puts(STRING(DeadReason::ALIVE));
		break;
	case DeadReason::Sandwiched:
		puts(STRING(DeadReason::Sandwiched));
		break;
	case DeadReason::HitEnemy:
		puts(STRING(DeadReason::HitEnemy));
		break;
	case DeadReason::HitThorn:
		puts(STRING(DeadReason::HitThorn));
		break;
	case DeadReason::Falling:
		puts(STRING(DeadReason::Falling));
		break;
	case DeadReason::HitMagma:
		puts(STRING(DeadReason::HitMagma));
		break;
	case DeadReason::Shot:
		puts(STRING(DeadReason::Shot));
		break;
	case DeadReason::FallenHellGate:
		puts(STRING(DeadReason::FallenHellGate));
		break;
	}

#endif
#ifndef _NEVER_DIE
	// 初死のみ
	if(dead_reason == DeadReason::ALIVE) death_timing = SCNOW;
	dead_reason = _dead_reason;
#endif
	v = PLAYER_DIE_UV_V;
}

bool Player::is_jumping()
{
	return jumping;
}

void Player::trigger_controlls()
{
	unless(dead_reason == DeadReason::ALIVE)
	{
#ifdef _DEBUG
		puts("hogee");
		return;
#endif
	}

	// プレイヤをジャンプさせる
	if (!controll_lock && ground && PLAYER_JUMP)
	{
		ground = false;
		jumped_at = y;
		jumping = true;
		v = PLAYER_JUMPING_UV_V;
		audiocontroller->play(SE_JUMP);
	}

	// プレイヤに掴ませたりする
	if (PLAYER_ITEM_USE)
	{
		if (is_holding_item()) release_item();
		else catch_item();
	}
}

Player::DeadReason Player::dead()
{
	return (time_diff(death_timing) > DEATH_HOLD_TIME ? dead_reason : DeadReason::ALIVE);
}

Player::Vec Player::get_vec()
{
	return vec;
}

SQUARE Player::get_square()
{
	// 辺り半分にします
	auto sq = PlainSquarePolygon::get_square();
	sq.w *= 0.8;
	return sq;
}

char& Player::get_life()
{
	return life;
}

// === Player END ===