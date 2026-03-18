#include "TheoremApplication.h"

TheoremApplication::TheoremApplication(const Axiom::ApplicationInfo& appInfo) : Axiom::Application(appInfo) {
	editorLayer = new EditorLayer();
	pushLayer(editorLayer);
}

TheoremApplication::~TheoremApplication() {
	popLayer(editorLayer);
	delete editorLayer;
}

Axiom::Application* Axiom::createApplication(const Axiom::ApplicationInfo& appInfo) {
	return new TheoremApplication(appInfo);
}
