#include "EditorLayer.h"

EditorLayer::EditorLayer() : Layer("EditorLayer") {
}

void EditorLayer::onAttach() {
    Axiom::AX_LOG_INFO("EditorLayer atached");
    viewportSize = Axiom::Application::getRenderer()->getCurrentRenderTargetSize();

    uiRoot = std::make_shared<Axiom::UICanvas>();

    auto panel1 = std::make_shared<Axiom::UIPanel>();
    panel1->setID("Panel1");
    panel1->setHorizontalAlignment(Axiom::UIAlignment::Start);
    panel1->setFixedSize({300, -1});

    auto verticalBox1 = std::make_shared<Axiom::UIVerticalBox>();
    verticalBox1->setID("VerticalBox1");

    auto button1 = std::make_shared<Axiom::UIButton>("Button 1");
    button1->setID("Button1");
    button1->setVerticalAlignment(Axiom::UIAlignment::Start);
    verticalBox1->addChild(button1);
    auto button2 = std::make_shared<Axiom::UIButton>("Button 2");
    button2->setID("Button2");
    verticalBox1->addChild(button2);
    auto button3 = std::make_shared<Axiom::UIButton>("Button 3");
    button3->setID("Button3");
    verticalBox1->addChild(button3);

    auto text1 = std::make_shared<Axiom::UIText>("Hello, World!");
    text1->setID("Text1");
    text1->setHorizontalAlignment(Axiom::UIAlignment::Center);
    text1->setFontSize(8.0f);
    verticalBox1->addChild(text1);

    auto dragFloat1 = std::make_shared<Axiom::UIDragFloat>();
    dragFloat1->setID("DragFloat1");
    static float dragValue = 0.0f;
    dragFloat1->setValueGetter([&]() { return dragValue; });
    dragFloat1->setValueSetter([&](float newValue) { dragValue = newValue; });
    // dragFloat1->setFloatLimit({0.0f, 100.0f});
    verticalBox1->addChild(dragFloat1);

    panel1->addChild(verticalBox1);
    uiRoot->addChild(panel1);

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
}
