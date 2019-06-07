/**
*@file Actor.cpp
*/
#include"Actor.h"
#include<algorithm>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/quaternion.hpp>

/**
*�A�N�^�[������������.
*
*@param mesh	�A�N�^�[�Ƃ��ĕ\�����郁�b�V��ID.
*@param tex		���b�V���ɓ\��e�N�X�`��ID.
*@param hp		�A�N�^�[�̑ϋv��.
*@param pos		���b�V����\��������W.
*@param rot		���b�V����\�������]�p�x.
*@param scale	���b�V����\������傫��.
*/

void Actor::Initialize(int mesh, GLuint tex, int hp,
	const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scale) {

	this->mesh = mesh;
	texture = tex;
	position = pos;
	rotation = rot;
	this->scale = scale;
	health = hp;
}

	/**
	*�A�N�^�[��j������.
	*/
	void Actor::Finalize() {

		health = 0;
	}
	
	/**
	*�A�N�^�[���X�V.
	*
	*@param deltaTime	�o�ߎ���.
	*/

	void Actor::Update(float deltaTime) {

		position += velocity * deltaTime;
		colWorld.origin = colLocal.origin + position;
		colWorld.size = colLocal.size;
	}

	/**
	*���p�\�ȃA�N�^�[���擾����.
	*
	*@param actorList	�����Ώۂ̃A�N�^�[���X�g.
	*
	*@retval			�����Ώۂ̃A�N�^�[�̃|�C���^.
	*					�����Ώۂ̃A�N�^�[��������Ȃ����nullptr.
	*/

	Actor* FindAvailableActor(std::vector<Actor*>& actorList) {

		for (auto& actor : actorList) {
			if (actor && actor->health <= 0) {

				return actor;
			}
		}
		return nullptr;
	}

	/**
	*�A�N�^�[�̏�Ԃ��X�V����.
	*
	*@param actorList	�X�V����A�N�^�[���X�g.
	*@param deltaTime	�O��̍X�V����̌o�ߎ���.
	*/

	void UpdateActorList(std::vector<Actor*>& actorList, float deltaTime) {

		for (auto& actor : actorList) {
			if (actor && actor->health > 0) {

				actor->Update(deltaTime);
			}
		}
	}

	/**
	*�A�N�^�[��`�悷��.
	*
	*@param actorList	�`�悷��A�N�^�[�̃��X�g.
	*@param shader		�`��Ɏg�p����V�F�[�_�E�I�u�W�F�N�g.
	*@param meshList	�`��Ɏg�p���郁�b�V�����X�g.
	*
	*���O��Shader::Use(),Shader::SetViewProjectionMatrix()�Ȃǂ̊֐������s���Ă�������.
	*/

	void RenderActorList(std::vector<Actor*>& actorList,
		Shader::Program& shader, MeshList& meshList) {

		for (auto& actor : actorList) {
			if (actor && actor->health > 0) {
				shader.BindTexture(0, actor->texture);
				shader.Draw(meshList.Get(actor->mesh),
					actor->position, actor->rotation, actor->scale);
			}
		}
	}

	/**
	*�A�N�^�[���X�g����ɂ���.
	*
	*@param actorList	��ɂ���A�N�^�[���X�g.
	*/
	void ClearActorList(std::vector<Actor*>& actorList) {

		for (auto& actor : actorList) {
			delete actor;
		}
		actorList.clear();
	}

	/**
	*2�̒����̂̏Փ˔���𒲂ׂ�.
	*
	*@param lhs	�����̂��̂P.
	*@param rhs	�����̂��̂Q.
	*
	*@retval true	�Փ˂��Ă���.
	*@retval false	�Փ˂��Ă��Ȃ�.
	*/

	bool DetectCollision(const Actor& lhs, const Actor& rhs) {

		return
			lhs.colWorld.origin.x < rhs.colWorld.origin.x + rhs.colWorld.size.x &&
			lhs.colWorld.origin.x + lhs.colWorld.size.x > rhs.colWorld.origin.x &&
			lhs.colWorld.origin.y < rhs.colWorld.origin.y + rhs.colWorld.size.y &&
			lhs.colWorld.origin.y + lhs.colWorld.size.y > rhs.colWorld.origin.y &&
			lhs.colWorld.origin.z < rhs.colWorld.origin.z + rhs.colWorld.size.z &&
			lhs.colWorld.origin.z + lhs.colWorld.size.z > rhs.colWorld.origin.z;
	}

	/**
	*2�̃O���[�v�ԂŏՓ˔�����s��.
	*
	*@param va		�O���[�vA.
	*@param vb		�O���[�vB.
	*@param func	A-B�Ԃ̏Փ˂���������֐�.
	*/

	void DetectCollision(std::vector<Actor*>& va, std::vector<Actor*>& vb,
		CollisionHandlerType func) {

		for (auto& a : va) {
			if (a->health <= 0) {
				continue;
			}
			for (auto& b : vb) {
				if (b->health <= 0) {
					continue;
				}
				if (DetectCollision(*a, *b)) {
					func(*a, *b);
					if (a->health <= 0) {
						break;
					}
				}
			}
		}
	}