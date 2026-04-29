#include "EditorLayer.h"

EditorLayer::EditorLayer() : Layer("EditorLayer") {
}

void EditorLayer::onAttach() {
    Axiom::AX_LOG_INFO("EditorLayer atached");
    viewportSize = Axiom::Application::getRenderer()->getCurrentRenderTargetSize();

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
}

void EditorLayer::onUIRender() {
    float winWidth = Axiom::Application::getWindow()->getWidth();
    float winHeight = Axiom::Application::getWindow()->getHeight();

    float sideWidth = winWidth * 0.20f;
    float mainHeight = winHeight;
    float centerWidth = winWidth - (sideWidth * 2.0f);

    Axiom::UI::beginPanel("Hierarchy", Math::Vec2(0.0f, 0.0f), Math::Vec2(sideWidth, mainHeight));

    if (scene) {
        auto view = scene->view<Axiom::TagComponent>();

        for (auto entityId : view) {
            Axiom::Entity entity = scene->getEntity(entityId);
            auto& tag = entity.getComponent<Axiom::TagComponent>();

            if (Axiom::UI::button(tag.tag, Math::Vec2(sideWidth - 10.0f, 24.0f))) {
                selectedEntity = entity;
            }
        }

        Axiom::UI::text("", Axiom::Color::white(), 4);
        if (Axiom::UI::button("Create Entity", Math::Vec2(sideWidth - 10.0f, 30.0f))) {
            Axiom::Entity newEntity = scene->createEntity();
            newEntity.addComponent<Axiom::TransformComponent>({});
            selectedEntity = newEntity;
        }
    }
    Axiom::UI::endPanel();

    Axiom::UI::beginPanel("Inspector", Math::Vec2(winWidth - sideWidth, 0.0f), Math::Vec2(sideWidth, mainHeight));

    if (selectedEntity) {
        auto& tag = selectedEntity.getComponent<Axiom::TagComponent>();
        Axiom::UI::inputText("Tag", tag.tag, 8);

        auto components = selectedEntity.getComponents();
        for (auto& [componentId, componentData] : components) {
            if (Axiom::UI::treeNode(Axiom::ComponentReflection::getComponentInfo(componentId)->name)) {
                Axiom::UI::component(componentId, componentData);
                Axiom::UI::treePop();
            }
        }

        static bool showAddComponentMenu = false;
        Axiom::UI::text("", Axiom::Color::white(), 8);

        if (Axiom::UI::button("Add Component", Math::Vec2(Axiom::UI::getAvaibleWidth(), 30.0f))) {
            showAddComponentMenu = !showAddComponentMenu;
        }

        if (showAddComponentMenu) {
            if (!selectedEntity.hasComponent<Axiom::Sprite2DComponent>()) {
                if (Axiom::UI::button("Sprite Renderer", Math::Vec2(Axiom::UI::getAvaibleWidth(), 24.0f))) {
                    selectedEntity.addComponent<Axiom::Sprite2DComponent>(Axiom::Sprite2DComponent{.textureId = 0, .color = Axiom::Color::white()});
                    showAddComponentMenu = false;
                }
            }

            if (!selectedEntity.hasComponent<Axiom::CameraComponent>()) {
                if (Axiom::UI::button("Camera", Math::Vec2(Axiom::UI::getAvaibleWidth(), 24.0f))) {
                    selectedEntity.addComponent<Axiom::CameraComponent>({});
                    showAddComponentMenu = false;
                }
            }
        }

    } else {
        Axiom::UI::text("No Entity Selected", Axiom::Color::gray(), 8);
    }

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

    Axiom::UI::image(sceneTextures[Axiom::Application::getRenderer()->getCurrentFrameIndex()].get(), Math::Vec2(imageWidth, imageHeight));

    Axiom::UI::endPanel();
}

void EditorLayer::onEvent(Axiom::Event& event) {
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
