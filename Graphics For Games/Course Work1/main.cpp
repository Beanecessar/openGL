#pragma comment(lib, "nclgl.lib")

#include "../../nclgl/window.h"
#include "Renderer.h"

int main() {

	Window w("Course Work", 1600, 900, true);
	if (!w.HasInitialised()) {
		return -1;
	}

	Renderer renderer(w);//This handles all the boring OGL 3.2 initialisation stuff, and sets up our tutorial!
	if (!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_P)) {
			renderer.pause = !renderer.pause;
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_B)) {
			renderer.postProcessEnable = !renderer.postProcessEnable;
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_LEFT)) {
			renderer.SetCurrentScene((renderer.GetCurrentScene() + renderer.GetNumOfScene() - 1) % renderer.GetNumOfScene());
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_RIGHT)) {
			renderer.SetCurrentScene((renderer.GetCurrentScene() + 1) % renderer.GetNumOfScene());
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_UP)) {
			renderer.autoChangeScene = !renderer.autoChangeScene;
		}

		renderer.UpdateScene(w.GetTimer()->GetTimedMS());
		renderer.RenderScene();
	}

	return 0;
}