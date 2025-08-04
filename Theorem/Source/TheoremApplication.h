#pragma once
#include "Axiom/Axiom.h"

class TheoremApplication : public Axiom::Application {
public:
	TheoremApplication(const Axiom::ApplicationInfo& appInfo);
};

Axiom::Application* Axiom::createApplication(const Axiom::ApplicationInfo& appInfo) {
	return new TheoremApplication(appInfo);
}

