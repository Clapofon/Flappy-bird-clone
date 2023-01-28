#pragma once

#include "../Engine/InputManager.h"
#include "../Engine/Constants.h"

#include "EntityData.h"

#include <Box2D/Box2D.h>
#include <SDL.h>
#include <glm.hpp>

const float PLAYER_WIDTH = 100;

class Player
{
public:
	Player();
	~Player();

	void init(b2World* world, const glm::vec2& position);
	bool update(Engine::InputManager& inputManager, b2World* world);
	void draw();

	void startGame() { m_body->SetAwake(true); m_body->ApplyLinearImpulse({ 0.0f, 25.0f }, m_body->GetWorldCenter(), true); }
	void setAwake(bool val) { m_body->SetAwake(val); }
	void setPositionWithoutWake(const glm::vec2& position) { m_body->SetTransform({ position.x / PPM, position.y / PPM }, 0.0f); }

	glm::vec2 getPosition() { return { m_body->GetPosition().x * PPM, m_body->GetPosition().y * PPM }; }

	void setPosition(const glm::vec2& position) { m_body->SetTransform({ position.x / PPM, position.y / PPM }, 0.0f); m_body->SetAwake(true); }

private:
	b2Body* m_body = nullptr;

	std::shared_ptr<EntityData> m_data;
};

