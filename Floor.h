#pragma once

#include <Box2D/Box2D.h>

#include "../Engine/Constants.h"

#include "EntityData.h"

class Floor
{
public:
	Floor() {}
	~Floor() {}

	void init(b2World* world)
	{
		b2BodyDef bodyDef;
		bodyDef.type = b2_staticBody;
		bodyDef.position.Set(0.0f, -2.0f);
		m_body = world->CreateBody(&bodyDef);

		b2PolygonShape upperBoxShape;
		upperBoxShape.SetAsBox(2000 / 2 / PPM, 100 / 2 / PPM);


		b2FixtureDef upperFixtureDef;
		upperFixtureDef.shape = &upperBoxShape;
		upperFixtureDef.friction = 0.1f;
		m_body->CreateFixture(&upperFixtureDef);
	
		m_data = std::make_shared<EntityData>();
		m_data.get()->type = "floor";
		m_body->SetUserData(m_data.get());
	}

private:
	b2Body* m_body = nullptr;

	std::shared_ptr<EntityData> m_data;
};