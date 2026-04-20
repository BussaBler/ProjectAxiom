#pragma once
#include "Axiom.h"
#include "EditorCamera.h"

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
    Math::uVec2 viewportSize{0, 0};
    float sliderValue = 0.0f;
    std::shared_ptr<Axiom::TextureAsset> textureAsset;
    std::shared_ptr<Axiom::Scene> scene;
    std::shared_ptr<Axiom::Texture> sceneTexture;
    std::unique_ptr<EditorCamera> editorCamera;
    Axiom::Entity entity0;
    Axiom::Entity entity1;
};
