/**
*@file TitleScene.cpp
*/

#include "TitleScene.h"
#include<glm/gtc/matrix_transform.hpp>

/**
*������.
*/

bool TitleScene::Initialize() {

	if (!meshList.Allocate()) {
		return false;
	}

	progSimple.Reset(Shader::BuildFromFile("Res/Simple.vert", "Res/Simple.frag"));

	texLogo.Reset(Texture::LoadImage2D("Res/TitleLogo.tga"));
	texBackground.Reset(Texture::LoadImage2D("Res/TitleBack.tga"));

	timer = 1.0f;
	isFinish = false;

		return true;	
}

/**
*���͂̔��f.
*/

void TitleScene::ProcessInput() {
	//
	GLFWEW::Window& window = GLFWEW::Window::Instance();

	if (!isFinish && timer <= 0.0f) {
		if (window.IsKeyPressed(GLFW_KEY_ENTER)) {
			NextScene("MainGameScene");
			isFinish = true;
		}
	}
}

/**
*��Ԃ̍X�V.
*/

void TitleScene::Update() {

	GLFWEW::Window& window = GLFWEW::Window::Instance();

	if (timer > 0.0f) {
		timer -= window.DeltaTime();
	}

}

/**
*�`��.
*/

void TitleScene::Render() {

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//�����������@�\��L���ɂ���.

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.1f, 0.3f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	meshList.BindVertexArraay();
	progSimple.Use();

	//���ˉe�ŕ`�悷��悤�ɍs���ݒ�.

	const glm::mat4x4 matProj = glm::ortho(0.0f, 800.f, 0.0f, 600.0f, 1.0f, 500.0f);
	const glm::mat4x4 matView =
		glm::lookAt(glm::vec3(0, 0, 100), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	progSimple.SetViewProjectionMatrix(matProj * matView);

	//�w�i�ƃ��S���E�B���h�E�̒��S�ɕ`��.

	progSimple.BindTexture(0, texBackground.Get());
	progSimple.Draw(meshList.Get(4),
		glm::vec3(400, 300, -1), glm::vec3(0), glm::vec3(400, 300, 1));
	progSimple.BindTexture(0, texLogo.Get());
	progSimple.Draw(meshList.Get(4),
		glm::vec3(400, 300, 0), glm::vec3(0), glm::vec3(400, 300, 1));

}

/**
*�I��.
*/
void TitleScene::Finalize() {

}

/**
*�^�C�g����ʂ��I��������ǂ����𒲂ׂ�.
*
*@retval true	�I�����.
*@retval false	�I����Ă��Ȃ�.
*/

bool TitleScene::IsFinish()const {

	return isFinish;
}