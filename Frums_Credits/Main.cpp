/*
Inspired by: https://www.youtube.com/watch?v=o3cKQzrtFgQ
Text animation from: https://github.com/plaaosert/credits_public/blob/main
Feel free to reach out to us with any questions you may have, we'd be happy to explain! (website in github bio)
*/
#define SDL_MAIN_HANDLED
#pragma warning(push)
#pragma warning(disable: 4244 26819)
#include "SDL.h"
#pragma warning(pop)
//#define test_for_debug_removals
#include "SceneManager.h"
int main() {
	return SceneManager::Begin();
}

