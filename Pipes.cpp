#include "Pipes.h"

#include "../Engine/Constants.h"
#include "../Engine/Renderer2D.h"


Pipes::Pipes()
{

}

Pipes::~Pipes()
{

}

void Pipes::init(b2World* world, const glm::vec2& position)
{

	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
	bodyDef.position.Set(position.x / PPM, position.y / PPM);
	m_body = world->CreateBody(&bodyDef);

	b2PolygonShape upperBoxShape;
	upperBoxShape.SetAsBox(PIPE_WIDTH / 2 / PPM, 800 / 2 / PPM, { 0, 550 / PPM }, 0.0f);

	b2PolygonShape lowerBoxShape;
	lowerBoxShape.SetAsBox(PIPE_WIDTH / 2 / PPM, 800 / 2 / PPM, { 0, -550 / PPM }, 0.0f);


	b2FixtureDef upperFixtureDef;
	upperFixtureDef.shape = &upperBoxShape;
	upperFixtureDef.friction = 1.0f;
	m_body->CreateFixture(&upperFixtureDef);

	b2FixtureDef lowerFixtureDef;
	lowerFixtureDef.shape = &lowerBoxShape;
	lowerFixtureDef.friction = 1.0f;
	m_body->CreateFixture(&lowerFixtureDef);

	m_data = std::make_shared<EntityData>();
	m_data.get()->type = "pipe";
	m_body->SetUserData(m_data.get());

	// collider body

	b2BodyDef bodyDef2;
	bodyDef2.type = b2_staticBody;
	bodyDef2.position.Set(position.x / PPM, position.y / PPM);
	m_colliderBody = world->CreateBody(&bodyDef2);

	b2PolygonShape scoreCollider;
	scoreCollider.SetAsBox(100 / PPM, 100 / PPM, { 100 / PPM, 0 }, 0.0f);

	b2FixtureDef colliderFixtureDef;
	colliderFixtureDef.shape = &scoreCollider;
	colliderFixtureDef.isSensor = true;
	m_colliderBody->CreateFixture(&colliderFixtureDef);

	m_colliderData = std::make_shared<EntityData>();
	m_colliderData.get()->type = "pipe_score_collider";
	m_colliderBody->SetUserData(m_colliderData.get());
}

bool Pipes::update(float screenLeft, GameplayState state, b2World* world, uint32_t& score)
{
	m_body->SetTransform({ m_body->GetPosition().x - 0.05f, m_body->GetPosition().y }, 0.0f);

	if (!m_colliderDestroyed)
	{
		m_colliderBody->SetTransform({ m_colliderBody->GetPosition().x - 0.05f, m_colliderBody->GetPosition().y }, 0.0f);
	}

	if (m_colliderData.get()->contact)
	{
		if (m_colliderData.get()->with == "player")
		{
			world->DestroyBody(m_colliderBody);
			score++;
			m_colliderDestroyed = true;
		}
	}

	return m_body->GetPosition().x * PPM < screenLeft;
}

void Pipes::draw()
{
	glm::vec2 position = { m_body->GetPosition().x * PPM, m_body->GetPosition().y * PPM };

	Engine::Renderer2D::DrawTexturedQuad("pipe.png", { position.x - PIPE_WIDTH / 2, position.y + 150, PIPE_WIDTH, 1000 }, 3.14);
	Engine::Renderer2D::DrawTexturedQuad("pipe.png", { position.x - PIPE_WIDTH / 2, position.y - 1150, PIPE_WIDTH, 1000 });
}