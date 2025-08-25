#pragma once
#include "Axiom.h"

class EditorLayer : public Axiom::Layer {
public:
	EditorLayer();
	~EditorLayer() = default;

	void onAttach() override;
	void onDetach() override;
	void onUpdate() override;
	void onEvent(Axiom::Event& event) override;

private:
	Math::Vec3 cameraPosition;
	Math::Vec3 cameraRotation;
};

