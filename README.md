# Mugicha
麦茶美味しい

## パスを通して
`DXSDK_DIR=C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\`

## テクスチャのCSV仕様

TEXTURE_LABEL,TEXTURE_PATH
TEXTURE_LABEL,TEXTURE_PATH

## ステージのCSVの仕様

MAP_WIDTH,MAP_HEIGHT,PLAYER_COORD_X, PLAYER_COORD_Y
BLOCK_NUMBER,BLOCK_NUMBER,BLOCK_NUMBER,BLOCK_NUMBER,BLOCK_NUMBER,BLOCK_NUMBER,BLOCK_NUMBER,BLOCK_NUMBER,...
BLOCK_NUMBER,BLOCK_NUMBER,BLOCK_NUMBER,BLOCK_NUMBER,BLOCK_NUMBER,BLOCK_NUMBER,BLOCK_NUMBER,BLOCK_NUMBER,...
BLOCK_NUMBER,
BLOCK_NUMBER,
BLOCK_NUMBER,...

### CELL_SIZE

WIDTH = 50, HEIGHT = 50

### BLOCK_NUMBER

特に記述がなければ幅と高さはCELL_SIZE

0. NONE
1. PLAYER
2. ENEMY
3. GOAL
4. WALL
5. WALL(上半分)
6. WALL(下半分)
7. WALL(switchで出現)
11. THORNS(下)
12. THORNS(上)
13. THORNS(右)
14. THORNS(左)
15. THORNS(下ハーフ，上壁)
16. THORNS(上ハーフ,下壁)
21. AIRCANNON(上)
22. AIRCANNON(下)
23. AIRCANNON(右)
24. AIRCANNON(左)
31. GIMMICK(持てるやつ)
32. SWITCH
33. MAGMA

## キー操作

### Title
- Enterで次の画面へ

### Stage Select
- A,Dで左右選択
- Enterで決定，次の画面へ

### Game
- A,Dまたは左右矢印で左右移動
- Spaceでジャンプ