#include "Player.h"

#include "../Engine/Renderer2D.h"

Player::Player()
{

}

Player::~Player()
{

}

void Player::init(b2World* world, const glm::vec2& position)
{
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(position.x / PPM, position.y / PPM);
	m_body = world->CreateBody(&bodyDef);


	b2CircleShape circleShape;
	circleShape.m_radius = PLAYER_WIDTH / 2 / PPM;

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &circleShape;
	fixtureDef.density = 30.0f;
	fixtureDef.friction = 1.0f;
	m_body->CreateFixture(&fixtureDef);

	m_data = std::make_shared<EntityData>();
	m_data.get()->type = "player";
	m_body->SetUserData(m_data.get());

	m_body->SetAwake(false);
}

bool Player::update(Engine::InputManager& inputManager, b2World* world)
{
	if (inputManager.isKeyPressed(SDLK_SPACE))
	{
		m_body->ApplyLinearImpulse({ 0.0f, 25.0f }, m_body->GetWorldCenter(), true);
	}

	if (m_data.get()->contact)
	{
		return m_data.get()->with == "floor" || m_data.get()->with == "pipe";
	}

	return false;
}

void Player::draw()
{
	Engine::Renderer2D::DrawTexturedQuad("bird.png", { getPosition() - PLAYER_WIDTH / 2, 200, 100 });
}

