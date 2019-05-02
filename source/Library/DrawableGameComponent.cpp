#include "DrawableGameComponent.h"
#include "Game.h"
namespace Library
{
	RTTI_DEFINITIONS(DrawableGameComponent)
	DrawableGameComponent::DrawableGameComponent()
	{
		mCamera = NULL;
	}
	DrawableGameComponent::DrawableGameComponent(Game& game) : GameComponent(game), mCamera(NULL)
	{
		// DrawableGameComponent();
	}
	DrawableGameComponent::DrawableGameComponent(Game& game, Camera& camera) : GameComponent(game), mCamera(&camera)
	{

	}
	DrawableGameComponent::~DrawableGameComponent()
	{
	
	}

	bool DrawableGameComponent::Visible() const
	{
		return mVisible;
	}

	void DrawableGameComponent::SetVisible(bool visible)
	{
		mVisible = visible;
	}

	Camera* DrawableGameComponent::GetCamera()
	{
		return mCamera;
	}

	void DrawableGameComponent::SetCamera(Camera* camera)
	{
		mCamera = camera;
	}

	void DrawableGameComponent::Draw(const GameTime& gameTime)
	{

	}
	DrawableGameComponent& DrawableGameComponent::operator=(const DrawableGameComponent&
		rhs)
	{
		return *this;
	}
}