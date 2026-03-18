#pragma once
#include "Axiom.h"

class EditorLayer : public Axiom::Layer {
public:
	EditorLayer();
	~EditorLayer() = default;

	void onAttach() override;
	void onDetach() override;
	void onUpdate() override;
	void onUIRender() override;
	void onEvent(Axiom::Event& event) override;
	void onRender(Axiom::CommandBuffer* commandBuffer) override;

private:
	Axiom::RenderPass renderPass{};
	Math::uVec2 viewportSize{ 0, 0 };
};

