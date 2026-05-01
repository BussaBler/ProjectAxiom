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
    void onUIRender(Axiom::UIRenderer* uiRenderer) override;
    void onEvent(Axiom::Event& event) override;
    void onRender(Axiom::CommandBuffer* commandBuffer) override;

  private:
    Math::uVec2 viewportSize{0, 0};
    std::shared_ptr<Axiom::TextureAsset> textureAsset;
    std::shared_ptr<Axiom::Scene> scene;
    std::unique_ptr<Axiom::SceneRenderer> sceneRenderer;
    std::vector<std::shared_ptr<Axiom::Texture>> sceneTextures;
    std::vector<std::shared_ptr<Axiom::Texture>> depthTextures;
    std::unique_ptr<EditorCamera> editorCamera;

    std::shared_ptr<Axiom::UIElement> uiRoot;

    Axiom::Entity selectedEntity;
};
