#include "Input.h"

/*void handleKeys(float delta, Camera* cam)
{
	const Uint8 *keys = SDL_GetKeyboardState(nullptr);
	if (keys[SDL_SCANCODE_W])
	{
		cam->forward(delta * velocity);
	}
	if (keys[SDL_SCANCODE_S])
	{
		cam->forward(delta * -velocity);
	}
	if (keys[SDL_SCANCODE_A])
	{
		cam->pan(delta * velocity);
	}
	if (keys[SDL_SCANCODE_D])
	{
		cam->pan(delta * -velocity);
	}
	if (keys[SDL_SCANCODE_SPACE])
	{
		cam->rise(delta * velocity);
	}
	if (keys[SDL_SCANCODE_LSHIFT])
	{
		cam->rise(delta * -velocity);
	}
	if (keys[SDL_SCANCODE_TAB] && !keyTabLastDownState)
	{
		keyTabLastDownState = true;
	}
	else if (!keys[SDL_SCANCODE_TAB])
	{
		keyTabLastDownState = false;
	}
	if (keys[SDL_SCANCODE_F] && !keyFLastDownState)
	{
		keyFLastDownState = true;
		cam->floating = !cam->floating;
	}
	else if (!keys[SDL_SCANCODE_F])
	{
		keyFLastDownState = false;
	}
	if (keys[SDL_SCANCODE_K] && !keyKLastDownState)
	{
		keyKLastDownState = true;
	}
	else if (!keys[SDL_SCANCODE_K])
	{
		keyKLastDownState = false;
	}
	if (keys[SDL_SCANCODE_M] && !keyMLastDownState)
	{
		worldMap->visible = !worldMap->visible;
		keyMLastDownState = true;
	}
	else if (!keys[SDL_SCANCODE_M])
	{
		keyMLastDownState = false;
	}
	if (keys[SDL_SCANCODE_N] && !keyNLastDownState)
	{
		worldMap->cycleActiveMapType();
		keyNLastDownState = true;
	}
	else if (!keys[SDL_SCANCODE_N])
	{
		keyNLastDownState = false;
	}
	if (keys[SDL_SCANCODE_O] && !keyOLastDownState)
	{
		debugMode = !debugMode;
		keyOLastDownState = true;
	}
	else if (!keys[SDL_SCANCODE_O])
	{
		keyOLastDownState = false;
	}
	if (keys[SDL_SCANCODE_R] && !keyRLastDownState)
	{
		velocity = velocity == walkingSpeed ? boostSpeed : walkingSpeed;
		keyRLastDownState = true;
	}
	else if (!keys[SDL_SCANCODE_R])
	{
		keyRLastDownState = false;
	}
	if (keys[SDL_SCANCODE_ESCAPE])
	{
		exiting = true;
	}
}

void handleMouse(float delta, SDL_Event event, Camera* cam)
{
	if (newEvent)
	{
		if (event.type == SDL_MOUSEMOTION)
		{
			cam->updateDirection((float)-event.motion.xrel, (float)-event.motion.yrel);
		}

		if (event.type == SDL_MOUSEBUTTONDOWN)
		{
			if (event.button.button == SDL_BUTTON_LEFT)
			{

			}
			if (event.button.button == SDL_BUTTON_RIGHT)
			{

			}
		}
		if (event.type == SDL_MOUSEBUTTONUP)
		{
			if (event.button.button == SDL_BUTTON_LEFT)
			{

			}
			if (event.button.button == SDL_BUTTON_RIGHT)
			{

			}
		}
	}
}*/