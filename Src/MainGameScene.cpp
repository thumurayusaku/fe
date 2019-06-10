/**
*@file MainGameScene.cpp
*/

#include "MainGameScene.h"
#include<glm/gtc/matrix_transform.hpp>
#include <iostream>

/**
*プレイヤーの状態を更新する.
*
*@param deltaTime	経過時間.
*/

void PlayerActor::Update(float deltaTime) {
	
	//プレイヤーが死んでいたら、地面に倒れる(-90度になる)まで「倒れ具合」を大きくしていく.
	if (health <= 0) {
		rotation.x -= glm::radians(45.0f) * deltaTime;
		if (rotation.x < glm::radians(-90.0f)) {
			rotation.x = glm::radians(-90.0f);
		}
	}
	Actor::Update(deltaTime);
}

/**
*プレイヤーの弾の状態を更新する.
*/

void BulletActor::Update(float deltaTime) {

	const float mapSize = 20;
	for (size_t i = 0; i < 3; ++i) {
		if (position[i] < -mapSize || position[i] > mapSize) {

			health = 0;
			break;
		}
	}
	Actor::Update(deltaTime);
}

/**
*ゾンビの状態を更新.
*/

void ZombieActor::Update(float deltaTime) {

	if (!target) {
		return;
	}
	const float moveSpeed = baseSpeed * 2.0f;
	const float rotationSpeed = baseSpeed * glm::radians(60.0f);
	const float frontRange = glm::radians(15.0f);

	//ターゲット(プレイヤー)の方向を調べる.
	const glm::vec3 v = target->position - position;
	const glm::vec3 vTarget = glm::normalize(v);
	float radian = std::atan2(-vTarget.z, vTarget.x) - glm::radians(90.0f);
	
	if (radian <= 0) {
		radian += glm::radians(360.0f);
	}
	const glm::vec3 vZombieFront = glm::rotate(
		glm::mat4(1), rotation.y, glm::vec3(0, 1, 0)) * glm::vec4(0, 0, -1, 1);

	//ターゲットが正面にいなかったら、正面にとらえるような左右に旋回.
	if (std::abs(radian - rotation.y) > frontRange) {
		const glm::vec3 vRotDir = glm::cross(vZombieFront, vTarget);
		if (vRotDir.y >= 0) {
			rotation.y += rotationSpeed * deltaTime;
			if (rotation.y >= glm::radians(360.0f)) {
				rotation.y -= glm::radians(360.0f);
			}
		}
		else {
			rotation.y -= rotationSpeed * deltaTime;
			if (rotation.y < 0) {
				rotation.y += glm::radians(360.0f);
			}
		}
	}
	//十分に接近していなければ移動する。接近していれば攻撃する.
	if (glm::length(v) > 0.5f) {
		velocity = vZombieFront * moveSpeed;
	}
	else {
		velocity = glm::vec3(0);	//接近しているのでもう移動しない.
		//定期的に攻撃状態になる.
		if (isAttacking) {
			isAttacking = false;
			attackingTimer = 5.0f;	//次の攻撃は５秒後.
		}
		else {
			attackingTimer -= deltaTime;
			if (attackingTimer <= 0) {
				isAttacking = true;
			}
		}
	}
	Actor::Update(deltaTime);
}

void TreeActor::Update(float deltaTime)
{
}

/**
*初期化.
*/

bool MainGameScene::Initialize() {

	random.seed(std::random_device()());

	//MeshList meshList;

	if (!meshList.Allocate()) {
		return false;
	}

	progSimple.Reset(Shader::BuildFromFile("Res/Simple.vert", "Res/Simple.frag"));
	progLighting.Reset(Shader::BuildFromFile("Res/FragmentLighting.vert", "Res/FragmentLighting.frag"));

	//OpenGLの情報をコンソールウィンドウへ出力する.

	const GLubyte* renderer = glGetString(GL_RENDERER);
	std::cout << "Renderer:" << renderer << std::endl;
	const GLubyte* version = glGetString(GL_VERSION);
	std::cout << "Version:" << version << std::endl;

	const GLubyte* extensions = glGetString(GL_EXTENSIONS);
	std::cout << "Extensions:" << extensions << std::endl;

	//家のテクスチャを作成する.

	const int imageWidth = 8;
	const int imageHeight = 8;
	const uint32_t B = 0xff'00'00'00;
	const uint32_t W = 0xff'ff'ff'ff;
	const uint32_t imageData[imageWidth * imageHeight] = {

		W,W,B,W,W,W,W,W,
		W,W,B,W,W,W,W,W,
		W,W,B,W,W,W,W,W,
		B,B,B,B,B,B,B,B,
		W,W,W,W,W,W,B,W,
		W,W,W,W,W,W,B,W,
		W,W,W,W,W,W,B,W,
		B,B,B,B,B,B,B,B,


	};
	texId.Reset(Texture::CreateImage2D(imageWidth, imageHeight, imageData, GL_RGBA, GL_UNSIGNED_BYTE));
	texHouse.Reset(Texture::LoadImage2D("Res/House.tga"));
	texRock.Reset(Texture::LoadImage2D("Res/Iwa.tga"));
	texHuman.Reset(Texture::LoadImage2D("Res/Human2.tga"));
	texBullet.Reset(Texture::LoadImage2D("Res/Bullet.tga"));
	texGround.Reset(Texture::LoadImage2D("Res/ground.tga"));
	
	//情報表示用テクスチャを読み込む.
	texScore.Reset(Texture::LoadImage2D("Res/Score.tga"));
	for (int i = 0; i < 10; ++i) {
		const std::string filename =
			std::string("Res/Number_") + (char)('0' + i) + ".tga";
		texNumber[i].Reset(Texture::LoadImage2D(filename.c_str()));
	}
	texHP.Reset(Texture::LoadImage2D("Res/HP.tga"));
	texStageClear.Reset(Texture::LoadImage2D("Res/StageClear.tga"));
	texGameOver.Reset(Texture::LoadImage2D("Res/GameOver.tga"));

	//木のテクスチャを作成する.

	/*const uint32_t imageData2[imageWidth * imageHeight] = {

		W,W,W,W,W,W,W,W,
		B,W,B,W,B,W,B,W,
		W,B,W,B,W,B,W,B,
		W,W,W,W,W,W,W,W,
		B,W,B,W,B,W,B,W,
		W,B,W,B,W,B,W,B,
		W,W,W,W,W,W,W,W,
		W,W,W,W,W,W,W,W,


	};*/
	texTree.Reset(Texture::LoadImage2D("Res/tree.tga"));

	//ライトの設定.

	lights.ambient.color = glm::vec3(0.05f, 0.1f, 0.1f);			//環境光の色.
	lights.directional.direction = glm::normalize(glm::vec3(1, -1, -1));	//指向性ライトの方向.
	lights.directional.color = glm::vec3(1.0f, 1.0f, 1.0f);			//指向性ライトの色.
	lights.point.position[0] = glm::vec3(5, 4, 0);
	lights.point.color[0] = glm::vec3(1.0f, 0.8f, 0.4f) * 100.0f;
	const glm::vec3 tmpSpotLightDir = glm::normalize(glm::vec3(-1, -2, -2));
	lights.spot.dirAndCutOff[0] = glm::vec4(tmpSpotLightDir, std::cos(glm::radians(20.0f)));
	lights.spot.posAndInnerCutOff[0] = glm::vec4(-6, 6, 8, std::cos(glm::radians(15.0f)));
	lights.spot.color[0] = glm::vec3(0.4f, 0.8f, 1.0f) * 200.0f;

	player.Initialize(5, texHuman.Get(), 10,
		glm::vec3(8, 0, 8), glm::vec3(0), glm::vec3(1));
	player.colLocal =
	{ glm::vec3(-0.5f,0.0f,-0.5f),glm::vec3(1.0f,1.7f,1.0f) };

	////木のアクター初期化
	//tree.Initialize(0, texTree.Get(), 10,
	//	glm::vec3(8, 0, 8), glm::vec3(0), glm::vec3(1));
	//tree.colLocal = { glm::vec3(-0.5f,0.0f,-0.5f),glm::vec3(1.0f,1.7f,1.0f) };

	playerBullets.resize(128);
	for (auto& bullet : playerBullets) {
		bullet = new BulletActor;
	}
	enemies.resize(128);
	for (auto& zombie : enemies) {
		zombie = new ZombieActor;
	}

	treeA.resize(10);
	for (auto& tree1 : treeA){
		tree1 = new TreeActor;
	}

	return true;
}

/**
*入力の反映.
*/

void MainGameScene::ProcessInput() {

	GLFWEW::Window& window = GLFWEW::Window::Instance();

	if (state == State::play) {

		//プレイヤーを移動する.
		player.velocity = glm::vec3(0);
		if (window.IsKeyPressed(GLFW_KEY_A)) {
			player.velocity.x = -1;
		}
		else if (window.IsKeyPressed(GLFW_KEY_D)) {
			player.velocity.x += 1;
		}
		if (window.IsKeyPressed(GLFW_KEY_W)) {
			player.velocity.z = -1;
		}
		else if (window.IsKeyPressed(GLFW_KEY_S)) {
			player.velocity.z = 1;
		}
		if (player.velocity.x || player.velocity.z) {
			player.velocity = glm::normalize(player.velocity);

			//ショットボタンが押されていなければ方向転換.
			if (!window.IsKeyPressed(GLFW_KEY_SPACE)) {
				player.rotation.y = std::atan2(-player.velocity.z, player.velocity.x);
				player.rotation.y -= glm::radians(90.0f);
			}
			const float speed = 10.0f;
			player.velocity *= speed;
		}
		//ショットボタンが押されていたら弾を発射.
		if (window.IsKeyPressed(GLFW_KEY_SPACE)) {
			if (playerBulletTimer <= 0) {
				Actor* bullet = FindAvailableActor(playerBullets);
				if (bullet) {

					const int meshId = 6;		//弾のメッシュID.
					const float speed = 40.0f;	//弾の移動速度(m/秒).
					const glm::mat4 matRotY =
						glm::rotate(glm::mat4(1), player.rotation.y, glm::vec3(0, 1, 0));
					bullet->Initialize(meshId, texBullet.Get(), 1,
						player.position, player.rotation, glm::vec3(1));
					bullet->velocity = matRotY * glm::vec4(0, 0, -speed, 1);
					bullet->colLocal =
					{ glm::vec3(-0.25f,-0.25f,-0.25f),glm::vec3(1,1,1) };

					playerBulletTimer = 1.0f / 8.0f;	//秒間８連射.
				}
			}
		}
		else if (window.IsKeyPressed(GLFW_KEY_Q)) {
			if (playerBulletTimer <= 0) {
				Actor* bullet = FindAvailableActor(playerBullets);
				if (bullet) {
					const int meshId = 6;		//弾のメッシュID.
					const float speed = 100.0f;	//弾の移動速度(m/秒).
					const glm::mat4 matRotY =
						glm::rotate(glm::mat4(1), player.rotation.y, glm::vec3(0, 1, 0));
					bullet->Initialize(meshId,texTree.Get(), 1,player.position, player.rotation, glm::vec3(1));
					bullet->velocity = matRotY * glm::vec4(0, 0, -speed, 1);
					bullet->colLocal =
					{ glm::vec3(-0.25f,-0.25f,-0.25f),glm::vec3(1,1,1) };
					playerBulletTimer = 1.0f / 128.0f;	//秒間８連射.
				}
			}
		}
		else {
			playerBulletTimer = 0.0f;
		}
		if (window.IsKeyPressed(GLFW_KEY_ENTER)) {

			if (player.health >= 0 && enemyKilled <= enemyTotal) {
				player.velocity.x = player.velocity.z = 0;
				state = State::stopScene;
			}
		}
	}
	else if (state == State::stageClear) {
		player.velocity.x = player.velocity.z = 0;
		if (window.IsKeyPressed(GLFW_KEY_P)) {
			++stageNo;
			player.position = glm::vec3(8, 0, 8);
			state = State::play;

			//より多くの敵を、より早く出現させる.
			enemyTotal = 10 + stageNo * 10;
			enemyLeft = enemyTotal;
			enemyKilled = 0;
			enemyPoppingInterval = 15.0f - (float)(stageNo - 1) * 2.0f;
			if (enemyPoppingInterval < 5.0f) {
				enemyPoppingInterval = 5.0f;
			}
			enemyPoppingTimer = 0;

			//敵の移動速度を上げる.
			enemySpeed = 1.0f + (float)(stageNo - 1) * 0.2f;
		}
	}
	else if (state == State::stopScene && window.IsKeyPressed(GLFW_KEY_ENTER)) {
		state = State::play;
	}
	else if(state == State::gameOver){
		player.velocity.x = player.velocity.z = 0;
		if (window.IsKeyPressed(GLFW_KEY_ENTER)) {
			NextScene("TitleScene");
		}
	}
}

/**
*状態の更新.
*/

void MainGameScene::Update() {

	const float deltaTime = (float)GLFWEW::Window::Instance().DeltaTime();

	//モデルのY軸回転角を更新.
	angleY += glm::radians(1.0f);
	if (angleY > glm::radians(360.0f)) {
		angleY -= glm::radians(360.0f);
	}

	//視点の位置を更新.
	const glm::vec3 viewOffset(0, 20, 10);
	viewPos = player.position + viewOffset;

	//プレイヤーの状態を更新.
	player.Update(deltaTime);

	//プレイヤーの弾の発射タイマーを更新.
	if (playerBulletTimer > 0) {
		playerBulletTimer -= deltaTime;
	}

	//プレイヤーの弾の状態を更新.
	UpdateActorList(playerBullets, deltaTime);

	UpdateActorList(treeA,deltaTime);

	//ゾンビの状態を更新.
	if (state != State::stopScene) {
		UpdateActorList(enemies, deltaTime);
	}

	//ゾンビの発生.
	if (enemyLeft > 0 && state != State::stopScene) {
		if (enemyPoppingTimer >= 0) {
			enemyPoppingTimer -= deltaTime;
		}
		else {
			enemyPoppingTimer += enemyPoppingInterval;

			int popCount = 10;	//同時に発生させるゾンビの最大数.
			if (enemyLeft < popCount) {
				popCount = enemyLeft;
			}
			enemyLeft -= popCount;

			//マップの前後左右15ｍの範囲で出現ポイントを決め、そこを中心として前後左右5ｍの範囲に出現.
			std::uniform_int_distribution<int> rangeBase(-15, 15);
			std::uniform_int_distribution<int> range(-5, 5);

			glm::vec3 posBase(rangeBase(random), 0, rangeBase(random));
			for (int i = 0; i < popCount; ++i) {
				ZombieActor* zombie = (ZombieActor*)FindAvailableActor(enemies);
				if (zombie) {
					glm::vec3 pos = posBase + glm::vec3(range(random), 0, range(random));
					zombie->Initialize(8, texHuman.Get(), 5, pos, glm::vec3(0), glm::vec3(1));

					zombie->colLocal =
					{ glm::vec3(-0.5f,0,-0.5f),glm::vec3(1,1.8f,1) };
					zombie->target = &player;
					zombie->baseSpeed = enemySpeed;
				}
				else {
					++enemyLeft;	//出現できなかった場合、残り数を増やして次回の出現を待つ.
				}
			}
		}
	}

	//プレイヤーの弾と敵の衝突判定.
	DetectCollision(playerBullets, enemies, [&](Actor& bullet, Actor& zombie) {
		zombie.health -= 1;
		bullet.health = 0;
		if (zombie.health <= 0) {
			score += 200;
			++enemyKilled;
		}
		else {
			score += 10;
		}
	});


	//ステージクリア判定.
	if (state == State::play && enemyKilled >= enemyTotal) {
		state = State::stageClear;
	}

	//ゾンビの攻撃.
	for (auto& actor : enemies) {
		if (actor->health <= 0) {
			continue;
		}
		//ターゲットの体力が０以下なら攻撃しない.
		ZombieActor* zombie = (ZombieActor*)actor;
		if (zombie->target->health <= 0) {
			continue;
		}
		if (zombie->isAttacking) {
			//ゾンビの正面左右45度の範囲にターゲットがいたら、ターゲットの体力を１減らす.
			const glm::vec3 vFront =
				glm::rotate(glm::mat4(1), zombie->rotation.y, glm::vec3(0, 1, 0))* glm::vec4(0, 0, -1, 1);
			const glm::vec3 vTarget = zombie->target->position - zombie->position;
			const float angle = std::acos(glm::dot(vFront, glm::normalize(vTarget)));
			if (std::abs(angle) < glm::radians(45.0f) && glm::length(vTarget) < 1.5f) {
				--zombie->target->health;
			}
		}
	}
	//ゲームオーバー判定.
	if (state == State::play && player.health <= 0) {
		state = State::gameOver;
	}
}

/**
*描画.
*/

void MainGameScene::Render(){
	GLFWEW::Window& window = GLFWEW::Window::Instance();

	const float deltaTime = (float)window.DeltaTime();

	glEnable(GL_DEPTH_TEST);

	glClearColor(0.1f, 0.3f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	progLighting.Use();

	//座標変換行列を作成してシェーダーに転送する.

	const glm::mat4x4 matProj =
		glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 500.0f);
	const glm::mat4x4 matView =
		glm::lookAt(viewPos, player.position, glm::vec3(0, 1, 0));
	progLighting.SetViewProjectionMatrix(matProj * matView);

	meshList.BindVertexArraay();

	//光源を設定する.
	progLighting.SetLightList(lights);


	progLighting.BindTexture(0, player.texture);
	progLighting.Draw(meshList.Get(player.mesh),
		player.position, player.rotation, player.scale);

	RenderActorList(playerBullets, progLighting, meshList);
	RenderActorList(enemies, progLighting, meshList);

	RenderActorList(treeA, progLighting, meshList);

	/*progLighting.BindTexture(0, texTree.Get());*/

	const float treeCount = 10;		//木の本数.
	const float radius = 15;		//木を植える円の半径.

	/*progLighting.BindTexture(0, tree.texture);*/

	for (float i = 0; i < treeCount; ++i) {

		
		//const float x = std::cos(3.14f * 2 / treeCount * i) * radius;
		//const float z = std::sin(3.14f * 2 / treeCount * i) * radius;
		//const glm::mat4x4 matModel = glm::translate(glm::mat4x4(1), glm::vec3(x, 0, z));
		//const glm::mat4x4 matMVP = matProj * matView * matModel;

		const float theta = 3.14f * 2 / treeCount * i;
		const float x = std::cos(theta) * radius;
		const float z = std::sin(theta) * radius;

		Actor* tree = FindAvailableActor(treeA);
		if (tree) {
			tree->Initialize(0,texTree.Get(),6, glm::vec3(x, 0, z), glm::vec3(0, theta * 5, 0), glm::vec3(1));
			tree->colLocal = { glm::vec3(-0.5f,0.0f,-0.5f),glm::vec3(1.0f,1.7f,1.0f) };
		}

		/*progLighting.Draw(meshList.Get(tree.mesh),
			glm::vec3(x, 0, z), glm::vec3(0, theta * 5, 0), glm::vec3(1));*/
	}

	//progLighting.BindTexture(0, texId.Get());

	const float houseCount = 4;		//家の数.
	const float houseRadius = 8;	//家を建てる半径.

	progLighting.BindTexture(0, texHouse.Get());

	for (float i = 0; i < houseCount; ++i)
	{

		const float x = std::cos(3.14f * 2 / houseCount * i) * houseRadius;
		const float z = std::sin(3.14f * 2 / houseCount * i) * houseRadius;
		//const glm::mat4x4 matModel = glm::translate(glm::mat4(1), glm::vec3(x, 0, z));
		//const glm::mat4x4 matMVP = matProj * matView * matModel;
		//glUniformMatrix4fv(matMVPLoc, 1, GL_FALSE, &matMVP[0][0]);
		//glDrawElementsBaseVertex(meshList[1].mode, meshList[1].count,
		//GL_UNSIGNED_SHORT, meshList[1].indices, meshList[1].baseVertex);

		progLighting.Draw(meshList.Get(1),
			glm::vec3(x, 0, z), glm::vec3(0, 0, 0), glm::vec3(1));
	}

	const float wallCount = 1;		//壁の数.
	const float wallRadius = 0;	//壁を建てる半径.

	progLighting.BindTexture(0, texHouse.Get());

	for (float i = 0; i < wallCount; ++i)
	{
		

		const float x = std::cos(3.14f * 2 / wallCount * i) * wallRadius;
		const float z = std::sin(3.14f * 2 / wallCount * i) * wallRadius;
		//const glm::mat4x4 matModel = glm::translate(glm::mat4(1), glm::vec3(x, 0, z));
		//const glm::mat4x4 matMVP = matProj * matView * matModel;
		//glUniformMatrix4fv(matMVPLoc, 1, GL_FALSE, &matMVP[0][0]);
		//glDrawElementsBaseVertex(meshList[2].mode, meshList[2].count,
		//GL_UNSIGNED_SHORT, meshList[2].indices, meshList[2].baseVertex);

		progLighting.Draw(meshList.Get(2),
			glm::vec3(x, 0, z), glm::vec3(0, 0, 0), glm::vec3(1));
	}

	//progLighting.BindTexture(0, texId.Get());

	//{

	//	//正八面体のデータ.

	//	//const glm::mat4x4 matModel = glm::translate(glm::mat4(1), glm::vec3(0, 10, 0));
	//	//const glm::mat4x4 matMVP = matProj * matView * matModel;
	//	//glUniformMatrix4fv(matMVPLoc, 1, GL_FALSE, &matMVP[0][0]);
	//	//glDrawElementsBaseVertex(meshList[3].mode, meshList[3].count,
	//	//GL_UNSIGNED_SHORT, meshList[3].indices, meshList[3].baseVertex);

	//	progLighting.Draw(meshList.Get(3),
	//		glm::vec3(0, 10, 0), glm::vec3(0, 0, 0), glm::vec3(1));
	//}

	const float HumanCount = 3;		//の数.
	const float HumanRadius = 8;	//家を建てる半径.

	progLighting.BindTexture(0, texHouse.Get());

	for (float i = 0; i < HumanCount; ++i)
	{

		const float x = std::cos(3.14f * 2 / HumanCount * i) * HumanRadius;
		const float z = std::sin(3.14f * 2 / HumanCount * i) * HumanRadius;

		progLighting.Draw(meshList.Get(5),
			glm::vec3(x, 10, z), glm::vec3(0, 0, 0), glm::vec3(1));
	}
	progLighting.BindTexture(0, texGround.Get());

	progLighting.Draw(meshList.Get(7),
		glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1));

	//ポイント・ライトの位置が分かるように適当なモデルを表示.
	{

		//ライトの位置に木のモデルを表示.
		progLighting.BindTexture(0, texId.Get());
		for (int i = 0; i < 8; ++i) {

			const glm::mat4 matModelT = glm::translate(glm::mat4(1), lights.point.position[i]);
			const glm::mat4 matModelR = glm::rotate(glm::mat4(1), angleY, glm::vec3(0, 1, 0));
			const glm::mat4 matModelS = glm::scale(glm::mat4(1), glm::vec3(1, -0.25f, 1));
			progLighting.Draw(meshList.Get(0),
				lights.point.position[i], glm::vec3(0, angleY, 0), glm::vec3(1, -0.25f, 1));
		}

		//情報を表示.
		{
			glDisable(GL_CULL_FACE);
			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			progSimple.Use();

			//正射影で描画するように行列を設定.
			const glm::mat4x4 matProj = glm::ortho(
				-400.0f, 400.0f, -300.0f, 300.0f, 1.0f, 500.0f);
			const glm::mat4x4 matView = glm::lookAt(
				glm::vec3(0, 0, 100), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
			progSimple.SetViewProjectionMatrix(matProj * matView);

			const Mesh& planeMeshId = meshList.Get(4);

			//スコアを表示.
			progSimple.BindTexture(0, texScore.Get());
			progSimple.Draw(planeMeshId,
				glm::vec3(-120, 270, 0), glm::vec3(0), glm::vec3(96, 32, 1));

			const int maxScoreDigits = 8;	//表示する桁数.
			int tmpScore = score;
			for (int i = 0; i < maxScoreDigits; ++i) {
				const int posX = -32 + 32 * (maxScoreDigits - i);
				const int number = tmpScore % 10;
				tmpScore /= 10;
				progSimple.BindTexture(0, texNumber[number].Get());
				progSimple.Draw(planeMeshId,
					glm::vec3(posX, 270, 0), glm::vec3(0), glm::vec3(32, 32, 1));
			}

			//プレイヤーの体力を表示.
			progSimple.BindTexture(0, texHP.Get());
			progSimple.Draw(planeMeshId,
				glm::vec3(-336, -270, 0), glm::vec3(0), glm::vec3(48, 32, 1));

			const int maxHealthDigits = 2;	//表示する桁数.
			int tmpHealth = player.health;
			for (int i = 0; i < maxHealthDigits; ++i) {
				const int posX = -300 + 32 * (maxHealthDigits - i);
				const int number = tmpHealth % 10;
				tmpHealth /= 10;
				progSimple.BindTexture(0, texNumber[number].Get());
				progSimple.Draw(planeMeshId,
					glm::vec3(posX, -270, 0), glm::vec3(0), glm::vec3(32, 32, 1));
			}
			//ステージ番号を表示.
			const int maxStageDigits = 1;	//表示する桁数.
			for (int i = 0; i < maxStageDigits; ++i) {
				const int posX = -400 + 32 * (maxStageDigits - i);
				const int number = stageNo;
				progSimple.BindTexture(0, texNumber[number].Get());
				progSimple.Draw(planeMeshId,
					glm::vec3(posX, 270, 0), glm::vec3(0), glm::vec3(32, 32, 1));
			}
			//ステージクリア・ゲームオーバー表示.
			if (state == State::stageClear) {
				progSimple.BindTexture(0, texStageClear.Get());
				progSimple.Draw(planeMeshId,
					glm::vec3(0), glm::vec3(0), glm::vec3(350, 60, 1));
			}
			else if (state == State::gameOver) {
				progSimple.BindTexture(0, texGameOver.Get());
				progSimple.Draw(planeMeshId,
					glm::vec3(0), glm::vec3(0), glm::vec3(300, 60, 1));
			}
			else if (state == State::stopScene){
				progSimple.BindTexture(0, texGameOver.Get());
				progSimple.Draw(planeMeshId,
					glm::vec3(0), glm::vec3(0), glm::vec3(300, 60, 1));
			}
		}
	}
}

/**
*終了.
*/
void MainGameScene::Finalize() {
	ClearActorList(playerBullets);
	ClearActorList(enemies);
	ClearActorList(treeA);
}


