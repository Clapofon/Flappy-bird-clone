#pragma once

#include <Box2D/Box2D.h>
#include <glm.hpp>

#include "EntityData.h"
#include "GameData.h"

const int PIPE_WIDTH = 100;

class Pipes
{
public:
	Pipes();
	~Pipes();

	void init(b2World* world, const glm::vec2& position);
	bool update(float screenLeft, GameplayState state, b2World* world, uint32_t& score);
	void draw();

	b2Body* getBody() { return m_body; }

private:
	b2Body* m_body = nullptr;
	b2Body* m_colliderBody = nullptr;

	std::shared_ptr<EntityData> m_data;
	std::shared_ptr<EntityData> m_colliderData;

	bool m_colliderDestroyed = false;
};

