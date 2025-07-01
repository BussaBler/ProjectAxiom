#pragma once
#include "Axiom/Axiom.h"

class TheoremApplication : public Axiom::Application {
public:
	TheoremApplication();
};

Axiom::Application* Axiom::createApplication() {
	return new TheoremApplication();
}

