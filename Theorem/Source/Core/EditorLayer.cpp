#include "EditorLayer.h"

EditorLayer::EditorLayer() : Layer("EditorLayer") {
}

void EditorLayer::onAttach() {
    Axiom::AX_LOG_INFO("EditorLayer atached");
    Axiom::RenderAttachment colorAttachment;
    colorAttachment.loadOp = Axiom::LoadOp::Clear;
    colorAttachment.storeOp = Axiom::StoreOp::Store;
    colorAttachment.clearColor = Axiom::Color::transparent();
    viewportSize = Axiom::Application::getRenderer()->getCurrentRenderTargetSize();

    renderPass.colorAttachments[0] = colorAttachment;
    renderPass.colorAttachmentCount = 1;
    renderPass.width = viewportSize.x();
    renderPass.height = viewportSize.y();

    Axiom::UUID textureHandle = Axiom::AssetManager::loadTexture("Assets/Textures/redstone_block.png");
    textureAsset = Axiom::AssetManager::getAsset<Axiom::TextureAsset>(textureHandle);

    scene = std::make_shared<Axiom::Scene>();
    entity0 = scene->createEntity();
    Axiom::TransformComponent transform = {
        .position = Math::Vec3(500.0f, 50.0f, 0.0f),
        .rotation = Math::Vec3(0.0f, 0.0f, 0.0f),
        .scale = Math::Vec3(100.0f, 100.0f, 1.0f),
    };
    entity0.addComponent<Axiom::TransformComponent>(transform);
    Axiom::Sprite2DComponent sprite = {.textureId = textureHandle, .color = Axiom::Color::white()};
    entity0.addComponent<Axiom::Sprite2DComponent>(sprite);

    entity1 = scene->createEntity();
    transform.position = Math::Vec3(500.0f, 200.0f, 0.0f);
    entity1.addComponent<Axiom::TransformComponent>(transform);
    sprite.textureId = Axiom::AssetManager::loadTexture("Assets/Textures/amethyst_block.png");
    sprite.color = Axiom::Color::white();
    entity1.addComponent<Axiom::Sprite2DComponent>(sprite);

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
    sceneTexture = Axiom::Application::getRenderer()->createTexture(createInfo);
}

void EditorLayer::onDetach() {
    Axiom::AX_LOG_INFO("EditorLayer detached");
}

void EditorLayer::onUpdate() {
    scene->onUpdate(0.125f);
}

void EditorLayer::onUIRender() {
    float winWidth = Axiom::Application::getWindow()->getWidth();
    float winHeight = Axiom::Application::getWindow()->getHeight();
    float sideWidth = winWidth * 0.20f;
    float bottomHeight = winHeight * 0.25f;
    float mainHeight = winHeight - bottomHeight;
    float centerWidth = winWidth - (sideWidth * 2.0f);

    Axiom::UI::beginPanel("Hierarchy", Math::Vec2(0.0f, 0.0f), Math::Vec2(sideWidth, mainHeight));
    if (Axiom::UI::treeNode("Scene Root")) {
        Axiom::UI::text("Entity 1", Axiom::Color::white(), 8);
        Axiom::UI::text("Entity 2", Axiom::Color::white(), 8);
        Axiom::UI::treePop();
    }
    Axiom::UI::endPanel();

    Axiom::UI::beginPanel("Inspector", Math::Vec2(winWidth - sideWidth, 0.0f), Math::Vec2(sideWidth, mainHeight));
    float fullWidth = Axiom::UI::getAvaibleWidth();
    float halfWidth = (fullWidth - Axiom::UI::getCurrentStyle().itemSpacing) / 2.0f;
    Axiom::UI::button("Button 1", Math::Vec2(halfWidth, 30.0f));
    Axiom::UI::sameLine();
    Axiom::UI::button("Button 2", Math::Vec2(halfWidth, 30.0f));
    Axiom::UI::dragFloat("Slider", sliderValue);
    Axiom::UI::endPanel();

    Axiom::UI::beginPanel("Viewport", Math::Vec2(sideWidth, 0.0f), Math::Vec2(centerWidth, mainHeight));
    float targetAspect = 1920.0f / 1080.0f;
    float availableHeight = mainHeight - 24.0f;
    float panelAspect = centerWidth / availableHeight;
    float imageWidth = 0.0f;
    float imageHeight = 0.0f;
    if (panelAspect > targetAspect) {
        imageHeight = availableHeight;
        imageWidth = imageHeight * targetAspect;
    } else {
        imageWidth = centerWidth;
        imageHeight = imageWidth / targetAspect;
    }
    float offsetX = (centerWidth - imageWidth) / 2.0f;
    float offsetY = (availableHeight - imageHeight) / 2.0f;
    Axiom::UI::image(sceneTexture.get(), Math::Vec2(imageWidth, imageHeight));
    Axiom::UI::endPanel();
    Axiom::UI::beginPanel("Console", Math::Vec2(0.0f, mainHeight), Math::Vec2(winWidth, bottomHeight));
    Axiom::UI::text("Theorem Editor Initialized.", Axiom::Color::white(), 8);
    Axiom::UI::text("Batch Renderer: Ready.", Axiom::Color::white(), 8);
    Axiom::UI::endPanel();
}

void EditorLayer::onEvent(Axiom::Event& event) {
}

void EditorLayer::onRender(Axiom::CommandBuffer* commandBuffer) {
    scene->onRender(commandBuffer, sceneTexture.get());
}
