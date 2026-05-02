#include "EditorLayer.h"

EditorLayer::EditorLayer() : Layer("EditorLayer") {
}

void EditorLayer::onAttach() {
    Axiom::AX_LOG_INFO("EditorLayer attached");
    viewportSize = Axiom::Application::getRenderer()->getCurrentRenderTargetSize();

    uiRoot = std::make_shared<Axiom::UICanvas>();

    auto mainLayout = std::make_shared<Axiom::UIHorizontalBox>();
    mainLayout->setID("MainHBox");
    uiRoot->addChild(mainLayout);

    auto leftPanel = std::make_shared<Axiom::UIPanel>();
    leftPanel->setID("LeftPanel");
    leftPanel->setFixedSize({320.0f, -1.0f});
    leftPanel->setHorizontalAlignment(Axiom::UIAlignment::Start);
    leftPanel->setPadding({10.0f, 10.0f, 10.0f, 10.0f});
    mainLayout->addChild(leftPanel);

    hierarchyPanel = std::make_shared<Axiom::UIVerticalBox>();
    hierarchyPanel->setID("HierarchyPanel");
    hierarchyPanel->setPadding({5.0f, 5.0f, 5.0f, 5.0f});
    leftPanel->addChild(hierarchyPanel);

    viewportImage = std::make_shared<Axiom::UIImage>();
    viewportImage->setID("Viewport");
    viewportImage->setFixedSize({-1.0f, 360.0f});
    viewportImage->setVerticalAlignment(Axiom::UIAlignment::Start);
    mainLayout->addChild(viewportImage);

    auto rightPanel = std::make_shared<Axiom::UIPanel>();
    rightPanel->setID("RightPanel");
    rightPanel->setFixedSize({320.0f, -1.0f});
    rightPanel->setHorizontalAlignment(Axiom::UIAlignment::End);
    rightPanel->setPadding({10.0f, 10.0f, 10.0f, 10.0f});
    mainLayout->addChild(rightPanel);

    scene = std::make_shared<Axiom::Scene>();
    Axiom::SceneSerializer deserializer(scene.get());
    if (!deserializer.deserialize("Assets/Scenes/scene.json")) {
        Axiom::AX_LOG_ERROR("Failed to deserialize the scene");
    }

    Axiom::UUID textureHandle = Axiom::AssetManager::importAsset("Assets/Textures/redstone_block.png", Axiom::AssetType::Texture);
    textureAsset = Axiom::AssetManager::getAsset<Axiom::TextureAsset>(textureHandle);

    sceneRenderer = std::make_unique<Axiom::SceneRenderer>();

    Axiom::Texture::CreateInfo createInfo = {
        .width = 1920,
        .height = 1080,
        .mipLevels = 1,
        .arrayLayers = 1,
        .format = Axiom::Format::B8G8R8A8Unorm,
        .usage = Axiom::TextureUsage::ColorAttachment | Axiom::TextureUsage::Sampled,
        .aspect = Axiom::TextureAspect::Color,
        .initialState = Axiom::TextureState::Undefined,
        .memoryUsage = Axiom::MemoryUsage::GPUOnly,
    };
    Axiom::Texture::CreateInfo depthCreateInfo = {
        .width = 1920,
        .height = 1080,
        .mipLevels = 1,
        .arrayLayers = 1,
        .format = Axiom::Format::D32sFloat,
        .usage = Axiom::TextureUsage::DepthStencilAttachment,
        .aspect = Axiom::TextureAspect::Depth,
        .initialState = Axiom::TextureState::Undefined,
        .memoryUsage = Axiom::MemoryUsage::GPUOnly,
    };
    uint32_t frameCount = Axiom::Application::getRenderer()->getFrameCount();

    sceneTextures.resize(frameCount);
    depthTextures.resize(frameCount);

    for (uint32_t i = 0; i < frameCount; ++i) {
        sceneTextures[i] = Axiom::Application::getRenderer()->createTexture(createInfo);
        depthTextures[i] = Axiom::Application::getRenderer()->createTexture(depthCreateInfo);
    }

    editorCamera = std::make_unique<EditorCamera>(Math::Vec3(0.0f, 0.0f, 500.0f));
    editorCamera->setPerspective(45.0f, static_cast<float>(viewportSize.x()) / static_cast<float>(viewportSize.y()), 0.1f, 1000.0f);

    refreshHierarchyPanel();
}

void EditorLayer::onDetach() {
    Axiom::SceneSerializer serializer(scene.get());
    serializer.serialize("Assets/Scenes/scene.json");
    Axiom::AX_LOG_INFO("EditorLayer detached");
}

void EditorLayer::onUpdate() {
    editorCamera->onUpdate(0.125f);
    scene->onUpdate(0.125f);
    float winWidth = Axiom::Application::getWindow()->getWidth();
    float winHeight = Axiom::Application::getWindow()->getHeight();
    uiRoot->arrange(Math::Vec2(0, 0), Math::Vec2(winWidth, winHeight));
}

void EditorLayer::onUIRender(Axiom::UIRenderer* uiRenderer) {
    uiRoot->onRender(uiRenderer);
}

void EditorLayer::onEvent(Axiom::Event& event) {
    uiRoot->onEvent(event);
}

void EditorLayer::onRender(Axiom::CommandBuffer* commandBuffer) {
    uint32_t currentFrameIndex = Axiom::Application::getRenderer()->getCurrentFrameIndex();
    std::shared_ptr<Axiom::Texture> renderTarget = sceneTextures[currentFrameIndex];
    std::shared_ptr<Axiom::Texture> depthTexture = depthTextures[currentFrameIndex];
    Math::Mat4 view = editorCamera->getView();
    Math::Mat4 projection = editorCamera->getProjection();
    Axiom::SceneRenderPassData data = {
        .scene = scene.get(),
        .commandBuffer = commandBuffer,
        .renderTarget = renderTarget.get(),
        .depthTarget = depthTexture.get(),
        .projection = projection,
        .view = view,
    };

    sceneRenderer->geometryPass(data);
    if (selectedEntity && selectedEntity.hasComponent<Axiom::TransformComponent>()) {
        auto& transform = selectedEntity.getComponent<Axiom::TransformComponent>();
        sceneRenderer->gizmoPass(data, transform.position);
    }

    viewportImage->setTexture(renderTarget);
}

void EditorLayer::refreshHierarchyPanel() {
    hierarchyPanel->clearChildren();

    auto headerText = std::make_shared<Axiom::UIText>("Hierarchy");
    headerText->setID("HierarchyHeader");
    headerText->setMargin({0.0f, 0.0f, 0.0f, 10.0f});
    hierarchyPanel->addChild(headerText);

    Axiom::View entityView = scene->view();
    for (uint32_t entityId : entityView) {
        Axiom::Entity entity = scene->getEntity(entityId);
        auto& tag = entity.getComponent<Axiom::TagComponent>();
        auto entityButton = std::make_shared<Axiom::UIButton>(tag.tag);
        entityButton->setID("Entity_" + std::to_string(entityId));
        entityButton->setMargin({0.0f, 0.0f, 0.0f, 5.0f});
        entityButton->setPadding({5.0f, 5.0f, 5.0f, 5.0f});
        entityButton->setVerticalAlignment(Axiom::UIAlignment::Start);
        entityButton->setOnClick([this, entity]() { selectedEntity = entity; });
        hierarchyPanel->addChild(entityButton);
    }
}
