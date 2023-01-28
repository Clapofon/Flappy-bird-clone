#include "App.h"

App::App()
{

}

App::~App()
{
	m_sceneList->destroy();
}

void App::onInit()
{
	
}

void App::addScreens()
{
	m_gameplayScene = std::make_unique<GameplayScene>();
	m_sceneList->addScene(m_gameplayScene.get());
	m_sceneList->setScene(m_gameplayScene->getSceneIndex());
}

void App::onExit()
{

}