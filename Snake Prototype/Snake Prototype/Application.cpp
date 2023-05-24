#include "raylib.h"
#include "Application.h"

Application::Application() 
{

}

Application::~Application() 
{

}

void Application::Run() 
{
	InitWindow(m_windowWidth, m_windowHeight, "Snake Prototype");
	SetTargetFPS(60);

	Load();

	while (!WindowShouldClose()) 
	{
		Update(GetFrameTime());
		Draw(); 
	}

	Unload();
}

void Application::Load()
{

}

void Application::Unload()
{

}

void Application::Update(float deltaTime)
{
	//Enter game logic 
}

void Application::Draw()
{
	BeginDrawing();
	ClearBackground(RAYWHITE);



}
