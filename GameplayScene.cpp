#include "GameplayScene.h"

#include "../Engine/IMainGame.h"
#include "../Engine/ResourceManager.h"
#include "../Engine/Random.h"
#include "../Engine/Renderer2D.h"
#include "../Engine/StringUtil.h"
#include "../Engine/Constants.h"
#include "../Engine/Timer.h"

#include "Indices.h"

#include <SDL.h>
#include <glew.h>

#include <iostream>
#include <filesystem>
#include <fstream>

#include <imgui.h>

GameplayScene::GameplayScene()
{
	m_sceneIndex = SCENE_INDEX_GAMEPLAY_SCENE;
}

GameplayScene::~GameplayScene()
{

}

int GameplayScene::getNextScreenIndex() const
{
	return SCENE_INDEX_NO_SCENE;
}

int GameplayScene::getPreviousScreenIndex() const
{
	return SCENE_INDEX_NO_SCENE;
}

void GameplayScene::build()
{

}

void GameplayScene::destroy()
{

}

void GameplayScene::onEntry()
{
	//world
	b2Vec2 gravity(0.0f, -9.81f);
	m_b2World = std::make_unique<b2World>(gravity);

	m_screenDims = m_game->getWindowDimentions();
	m_camera.init(m_screenDims);
	m_camera.setScale(1.0f);
	m_camera.setPosition(glm::vec2(0, 0));
	m_particleSystem.init(&m_camera);

	m_camera.update(m_screenDims);

	//m_player.setConfig(m_game->getPlayerConfig());

	/*
		Add all your batches firts!
		If not, the batches wont get inititialized!
		Then you can call Renderer2D::Init()
	*/
	Engine::Renderer2D::AddBatch("main");
	Engine::Renderer2D::AddBatch("fonts");

	Engine::Renderer2D::Init(&m_camera);

	m_player.init(m_b2World.get(), { 0, 0 });

	m_lineRenderer.init();

	m_game->setFrameLock(true);

	m_game->getAudioEngine().LoadSound("assets/audio/ambient/forest_ambient.wav", true, true, true);


	setupStyle();

	setupFloorAndCeiling();
	m_b2World->SetContactListener(&m_contactListener);
}

void GameplayScene::onExit()
{
	m_game->getAudioEngine().StopAllChannels();
	m_game->getAudioEngine().ResetPositionAllChannels();
}

void GameplayScene::update(double deltaTime)
{
	m_time += 0.05;

	m_screenDims = m_game->getWindowDimentions();

	if (m_game->getWindow().wasResized())
	{
		printf("dims: %f, %f\n", m_screenDims.x, m_screenDims.y);
		m_camera.init(m_screenDims);
		m_game->getWindow().setResize(false);
	}

	handleInput(deltaTime);

	glm::vec2 playerPos = m_player.getPosition();

	//m_camera.setPosition(glm::vec2(playerPos.x, playerPos.y));
	m_particleSystem.update(deltaTime);

	m_camera.update(m_game->getWindowDimentions());

	if (m_gameplayState == GameplayState::PLAY)
	{
		m_groundPosition -= 10;

		if (m_groundPosition <= -m_screenDims.x)
		{
			m_groundPosition = 0;
		}

		if (m_timer.ElapsedMillis() >= 1000)
		{
			int32_t y = Engine::Random::randInt((-m_screenDims.y / 2) + 300, (m_screenDims.y / 2) - 200);

			Pipes pipes;
			pipes.init(m_b2World.get(), { 1000, y });

			m_pipes.push_back(pipes);

			m_timer.Reset();
		}

		if (m_player.update(m_game->getInputManager(), m_b2World.get()))
		{
			if (m_gameplayState != GameplayState::GAMEOVER)
			{
				m_gameplayState = GameplayState::GAMEOVER;
			}
		}

		for (uint32_t i = 0; i < m_pipes.size(); i++)
		{
			if (m_pipes[i].update((-m_screenDims.x / 2) - 100, m_gameplayState, m_b2World.get(), m_score))
			{
				m_b2World.get()->DestroyBody(m_pipes[i].getBody());
				m_pipes[i] = m_pipes.back();
				m_pipes.pop_back();
			}
		}
	}

	if (m_gameplayState == GameplayState::MENU)
	{
		m_player.setPositionWithoutWake({ m_player.getPosition().x, glm::sin(m_time) * 100});

		if (m_game->getInputManager().isKeyPressed(SDLK_SPACE))
		{
			m_player.startGame();
			m_gameplayState = GameplayState::PLAY;
		}
	}

	m_b2World->Step(1.0f / 60.0f, 8, 3);
}

void GameplayScene::draw()
{
	Engine::Renderer2D::BeginScene("main", Engine::GlyphSortType::FRONT_TO_BACK);

	m_player.draw();
	for (auto pipe : m_pipes)
	{
		pipe.draw();
	}

	Engine::Renderer2D::DrawTexturedQuad("ground.png", { m_groundPosition + (-m_screenDims.x / 2), -m_screenDims.y / 2, 2000, 100 }, {0.0f, 0.0f, 1.0f, 1.0f}, 2.0f);
	Engine::Renderer2D::DrawTexturedQuad("ground.png", { m_groundPosition + (m_screenDims.x / 2), -m_screenDims.y / 2, 2000, 100 }, {0.0f, 0.0f, 1.0f, 1.0f}, 2.0f);

	Engine::Renderer2D::EndScene("main");
	Engine::Renderer2D::RenderScene("main");

	Engine::Renderer2D::BeginScene("fonts");

	m_stream.str(std::string());
	m_stream << m_score;
	Engine::Renderer2D::DrawText(m_stream.str(), { 0, 200 }, 200, Engine::Justification::MIDDLE);

	Engine::Renderer2D::EndScene("fonts");
	Engine::Renderer2D::RenderScene("fonts", Engine::ShaderType::SHADER_TEXT);

	if (m_gameplayState == GameplayState::GAMEOVER)
	{
		glm::vec2 dims = m_game->getWindowDimentions();
		float ratio = dims.x / 1920;

		glm::vec2 size = glm::vec2(330.0f, 320.0f);

		ImGui::SetNextWindowPos(ImVec2((dims.x / 2.0f) - size.x / 2.0f, (dims.y / 2) - size.y / 2), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(size.x, size.y));

		ImGui::Begin("main menu", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

		ImGui::Text("Score: %d Highscore: %d\n", m_score, m_highScore);

		if (ImGui::Button("Play Again", { size.x - 30.0f, 120 }))
		{
			m_gameplayState = GameplayState::MENU;
			reset();
		}

		if (ImGui::Button("Exit to Windows", { size.x - 30.0f, 120 }))
		{
			m_currentState = Engine::SceneState::EXIT_APPLICATION;
		}

		ImGui::End();
	}
}


void GameplayScene::handleInput(double deltaTime)
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		m_game->onSDLEvent(event);
	}
}

void GameplayScene::setupStyle()
{
	ImGuiStyle* style = &ImGui::GetStyle();

	style->WindowPadding = ImVec2(15, 15);
	style->WindowRounding = 0.0f;
	style->FramePadding = ImVec2(5, 5);
	style->FrameRounding = 1.0f;
	style->ItemSpacing = ImVec2(12, 8);
	style->ItemInnerSpacing = ImVec2(8, 2);
	style->IndentSpacing = 25.0f;
	style->ScrollbarSize = 15.0f;
	style->ScrollbarRounding = 9.0f;
	style->GrabMinSize = 5.0f;
	style->GrabRounding = 3.0f;

	style->Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);
	style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 0.05f);
	//style->Colors[ImGuiCol_ChildBg] = ImVec4(0.01f, 0.01f, 0.01f, 0.1f);
	style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_Border] = ImVec4(0.20f, 0.20f, 0.23f, 0.00f);
	style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.22f, 0.21f, 0.28f, 0.00f);
	style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 0.10f);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 0.10f);
	style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
}

void GameplayScene::setupFloorAndCeiling()
{
	
	m_floor.init(m_b2World.get());


	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
	bodyDef.position.Set(0.0f, 2.5f);
	b2Body* m_body = m_b2World->CreateBody(&bodyDef);

	b2PolygonShape upperBoxShape;
	upperBoxShape.SetAsBox(2000 / 2 / PPM, 100 / 2 / PPM);


	b2FixtureDef upperFixtureDef;
	upperFixtureDef.shape = &upperBoxShape;
	upperFixtureDef.friction = 1.0f;
	m_body->CreateFixture(&upperFixtureDef);

}

void GameplayScene::reset()
{
	for (auto& pipe : m_pipes)
	{
		m_b2World->DestroyBody(pipe.getBody());
	}

	m_pipes.clear();

	m_player.setPosition({ 0, 0 });
	m_player.setAwake(false);

	if (m_score > m_highScore)
	{
		m_highScore = m_score;
	}

	m_score = 0;
}

