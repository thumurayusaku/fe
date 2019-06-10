/**
*@file MainGameScene.h
*/

#ifndef MAINGAMESCENE_H_INCLUDED
#define MAINGAMESCENE_H_INCLUDED
#include"GLFWEW.h"
#include"Texture.h"
#include"Shader.h"
#include"MeshList.h"
#include"Scene.h"
#include"Actor.h"
#include<vector>
#include<random>

/**
*プレイヤーが操作するアクター.
*/

class PlayerActor : public Actor {

public:
	virtual ~PlayerActor() = default;
	virtual void Update(float deltaTime)override;
};

/**
*プレイヤーから発射される弾のアクター.
*/
class BulletActor : public Actor {

public:
	virtual ~BulletActor() = default;
	virtual void Update(float deltaTime) override;
};

/**
*敵(ゾンビ)のアクター.
*/
class ZombieActor : public Actor {

public:
	virtual ~ZombieActor() = default;
	virtual void Update(float deltaTime)override;

	Actor* target = nullptr;
	float attackingTimer = 5.0f;
	bool isAttacking = false;
	float baseSpeed = 1.0f;
};

/**
*木のアクター.
*/
class TreeActor : public Actor {

public:
	virtual ~TreeActor() = default;
	virtual void Update(float deltaTime)override;
};

/**
*メインゲーム画面.
*/

class MainGameScene : public Scene{

public:
	MainGameScene() = default;
	virtual ~MainGameScene() = default;

	virtual bool Initialize()override;
	virtual void ProcessInput()override;
	virtual void Update()override;
	virtual void Render()override;
	virtual void Finalize()override;

private:
	MeshList meshList;

	Texture::Image2D texId;
	Texture::Image2D texTree;
	Texture::Image2D texHouse;
	Texture::Image2D texRock;
	Texture::Image2D texHuman;
	Texture::Image2D texBullet;
	Texture::Image2D texPlane;
	Texture::Image2D texGround;

	//情報表示用テクスチャ.
	Texture::Image2D texScore;
	Texture::Image2D texNumber[10];
	Texture::Image2D texHP;
	Texture::Image2D texStageClear;
	Texture::Image2D texGameOver;

	Shader::Program progSimple;
	Shader::Program progLighting;
	Shader::LightList lights;

	float angleY = 0;
	glm::vec3 viewPos;

	PlayerActor player;
	std::vector<Actor*>playerBullets;
	float playerBulletTimer = 0;

	std::vector<Actor*>enemies;


	std::mt19937 random;

	std::vector<Actor*>treeA;

	//ゲーム状態.
	enum class State {
		play,		//通常プレイ中.
		stageClear,	//ステージクリア.
		gameOver,	//ゲームオーバー.
		stopScene,	//一時停止画面.
	};
	State state = State::play;

	int stageNo = 1;
	int score = 0;
	float enemySpeed = 1.0f;			//ゾンビの移動速度.
	float enemyPoppingInterval = 15.0f;	//ゾンビの出現間隔.
	float enemyPoppingTimer = 0.0f;		//ゾンビ出現タイマー.
	int enemyTotal = 20;				//敵の総数.
	int enemyLeft = 20;				//未登場の敵の数.敵を出現させるたびに減少していく.
	int enemyKilled = 0;				//殺した敵の数.この数値がenemyTotalと等しくなったらクリア.
};

#endif // ! MAINGAMESCENE_H_INCLUDED

