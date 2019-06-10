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
*�v���C���[�����삷��A�N�^�[.
*/

class PlayerActor : public Actor {

public:
	virtual ~PlayerActor() = default;
	virtual void Update(float deltaTime)override;
};

/**
*�v���C���[���甭�˂����e�̃A�N�^�[.
*/
class BulletActor : public Actor {

public:
	virtual ~BulletActor() = default;
	virtual void Update(float deltaTime) override;
};

/**
*�G(�]���r)�̃A�N�^�[.
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
*�؂̃A�N�^�[.
*/
class TreeActor : public Actor {

public:
	virtual ~TreeActor() = default;
	virtual void Update(float deltaTime)override;
};

/**
*���C���Q�[�����.
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

	//���\���p�e�N�X�`��.
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

	//�Q�[�����.
	enum class State {
		play,		//�ʏ�v���C��.
		stageClear,	//�X�e�[�W�N���A.
		gameOver,	//�Q�[���I�[�o�[.
		stopScene,	//�ꎞ��~���.
	};
	State state = State::play;

	int stageNo = 1;
	int score = 0;
	float enemySpeed = 1.0f;			//�]���r�̈ړ����x.
	float enemyPoppingInterval = 15.0f;	//�]���r�̏o���Ԋu.
	float enemyPoppingTimer = 0.0f;		//�]���r�o���^�C�}�[.
	int enemyTotal = 20;				//�G�̑���.
	int enemyLeft = 20;				//���o��̓G�̐�.�G���o�������邽�тɌ������Ă���.
	int enemyKilled = 0;				//�E�����G�̐�.���̐��l��enemyTotal�Ɠ������Ȃ�����N���A.
};

#endif // ! MAINGAMESCENE_H_INCLUDED

