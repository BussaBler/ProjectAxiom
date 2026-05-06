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
    void refreshHierarchyPanel();
    void refreshInspectorPanel();
    void spawnHierarchyContextMenu();

    std::shared_ptr<Axiom::UIElement> createFieldUI(const Axiom::FieldInfo& field, void* fieldPtr);
    void buildFloatUI(std::shared_ptr<Axiom::UIHorizontalBox> horizontalBox, const Axiom::FieldInfo& field, void* fieldPtr);
    void buildIntUI(std::shared_ptr<Axiom::UIHorizontalBox> horizontalBox, const Axiom::FieldInfo& field, void* fieldPtr);
    void buildBoolUI(std::shared_ptr<Axiom::UIHorizontalBox> horizontalBox, const Axiom::FieldInfo& field, void* fieldPtr);
    void buildStringUI(std::shared_ptr<Axiom::UIHorizontalBox> horizontalBox, const Axiom::FieldInfo& field, void* fieldPtr);
    void buildVec2UI(std::shared_ptr<Axiom::UIHorizontalBox> horizontalBox, const Axiom::FieldInfo& field, void* fieldPtr);
    void buildVec3UI(std::shared_ptr<Axiom::UIHorizontalBox> horizontalBox, const Axiom::FieldInfo& field, void* fieldPtr);
    void buildVec4UI(std::shared_ptr<Axiom::UIHorizontalBox> horizontalBox, const Axiom::FieldInfo& field, void* fieldPtr);
    void buildColorUI(std::shared_ptr<Axiom::UIHorizontalBox> horizontalBox, const Axiom::FieldInfo& field, void* fieldPtr);

  private:
    Math::uVec2 viewportSize{0, 0};
    std::shared_ptr<Axiom::TextureAsset> textureAsset;
    std::shared_ptr<Axiom::Scene> scene;
    std::unique_ptr<Axiom::SceneRenderer> sceneRenderer;
    std::vector<std::shared_ptr<Axiom::Texture>> sceneTextures;
    std::vector<std::shared_ptr<Axiom::Texture>> depthTextures;
    std::unique_ptr<EditorCamera> editorCamera;

    Axiom::UIContext mainUiContext;
    std::shared_ptr<Axiom::UIElement> uiRoot;
    std::shared_ptr<Axiom::UIImage> viewportImage;
    std::shared_ptr<Axiom::UIVerticalBox> hierarchyPanel;
    std::shared_ptr<Axiom::UIVerticalBox> inspectorPanel;
    bool shouldRefreshHierarchy = false;
    bool shouldRefreshInspector = false;
    bool shouldDeleteContextMenu = false;

    std::shared_ptr<Axiom::UIPanel> contextMenu = nullptr;
    Math::Vec2 lastMousePos = Math::Vec2::zero();

    Axiom::Entity selectedEntity = Axiom::Entity();
};
