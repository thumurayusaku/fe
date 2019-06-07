#include"GLFWEW.h"
#include"TitleScene.h"
#include"MainGameScene.h"
#include<vector>

MeshList meshList;

/**
*@file Main.cpp
*/

/**
*�G���g���[�|�C���g.
*/

int main() {

	GLFWEW::Window& window = GLFWEW::Window::Instance();

	if (!window.Init(1920, 1080, "OpenGLTutorial")) {
		return 1;
	}

	Scene* pScene = new TitleScene;
	if(!pScene || !pScene->Initialize()){
		delete pScene;
		return 1;
	}
	
	//���C�����[�v.

	window.IniTimer();
	while (!window.ShouldClose()) {
		window.UpDateTimer();

		pScene->ProcessInput();
		pScene->Update();
		pScene->Render();
		//NextScene���ݒ肳��Ă�����A�V�[����؂�ւ���.
		if(!pScene->NextScene().empty()){
			const std::string SceneName =
				pScene->NextScene();
			pScene->Finalize();
			delete pScene;
			pScene = nullptr;
			if (SceneName == "TitleScene") {
				pScene = new TitleScene;
			}
			else if (SceneName == "MainGameScene") {
				pScene = new MainGameScene;

			}
			if (!pScene || !pScene->Initialize()) {
				break;
			}
		}

		window.SwapBuffers();

	}
	if (pScene) {
		pScene->Finalize();
		delete pScene;
	}

	return 0;
}