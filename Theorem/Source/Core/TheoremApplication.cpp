#include "TheoremApplication.h"

TheoremApplication::TheoremApplication(const Axiom::ApplicationInfo& appInfo) : Axiom::Application(appInfo) {
	pushLayer<EditorLayer>();
}

TheoremApplication::~TheoremApplication() {

}

Axiom::Application* Axiom::createApplication(const Axiom::ApplicationInfo& appInfo) {
	return new TheoremApplication(appInfo);
}
