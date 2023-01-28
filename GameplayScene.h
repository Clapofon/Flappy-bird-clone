#pragma once

#include "../Engine/IGameScene.h"
#include "../Engine/Camera.h"
#include "../Engine/LineRenderer.h"
#include "../Engine/FrameBuffer.h"
#include "../Engine/ParticleSystem2D.h"

#include "Player.h"
#include "Floor.h"
#include "Pipes.h"
#include "ContactListener.h"
#include "GameData.h"

#include <Box2D/Box2D.h>

#include <sstream>
#include <vector>


class GameplayScene : public Engine::IGameScene
{
public:
	GameplayScene();
	~GameplayScene();

	int getNextScreenIndex() const override;
	int getPreviousScreenIndex() const override;

	void build() override;
	void destroy() override;

	void onEntry() override;
	void onExit() override;

	void update(double deltaTime) override;
	void draw() override;

	void handleInput(double deltaTime);
private:

	void setupStyle();
	void setupFloorAndCeiling();
	void reset();

	Engine::Camera m_camera;
	Engine::ParticleSystem2D m_particleSystem;

	GameplayState m_gameplayState = GameplayState::MENU;

	glm::vec2 m_screenDims = glm::vec2(0);

	std::stringstream m_stream;

	std::unique_ptr<b2World> m_b2World;
	
	Engine::LineRenderer m_lineRenderer;

	Player m_player;
	Floor m_floor;
	Engine::Timer m_timer;

	float m_time = 0.0f;
	uint32_t m_score = 0;
	uint32_t m_highScore = 0;
	int32_t m_groundPosition = 0;
	
	std::vector<Pipes> m_pipes;

	ContactListener m_contactListener;
};