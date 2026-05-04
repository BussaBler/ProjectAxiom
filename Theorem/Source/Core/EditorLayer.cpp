#include "EditorLayer.h"

EditorLayer::EditorLayer() : Layer("EditorLayer") {
}

void EditorLayer::onAttach() {
    Axiom::AX_LOG_INFO("EditorLayer attached");
    viewportSize = Axiom::Locator::getRenderer()->getCurrentRenderTargetSize();

    uiContext = {
        .renderer = Axiom::Locator::getUIRenderer(),
        .dpiScale = Axiom::Locator::getWindow()->getWindowDPI() / 72.0f,
    };

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

    inspectorPanel = std::make_shared<Axiom::UIVerticalBox>();
    inspectorPanel->setID("InspectorPanel");
    inspectorPanel->setPadding({5.0f, 5.0f, 5.0f, 5.0f});
    rightPanel->addChild(inspectorPanel);

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
    uint32_t frameCount = Axiom::Locator::getRenderer()->getFrameCount();

    sceneTextures.resize(frameCount);
    depthTextures.resize(frameCount);

    for (uint32_t i = 0; i < frameCount; ++i) {
        sceneTextures[i] = Axiom::Locator::getRenderer()->createTexture(createInfo);
        depthTextures[i] = Axiom::Locator::getRenderer()->createTexture(depthCreateInfo);
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
    float winWidth = Axiom::Locator::getWindow()->getWidth();
    float winHeight = Axiom::Locator::getWindow()->getHeight();
    uiRoot->arrange(uiContext, Math::Vec2(0, 0), Math::Vec2(winWidth, winHeight));
}

void EditorLayer::onUIRender() {
    uiRoot->onRender(uiContext);
}

void EditorLayer::onEvent(Axiom::Event& event) {
    uiRoot->onEvent(event);
}

void EditorLayer::onRender(Axiom::CommandBuffer* commandBuffer) {
    uint32_t currentFrameIndex = Axiom::Locator::getRenderer()->getCurrentFrameIndex();
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
        entityButton->setOnClick([this, entity]() {
            selectedEntity = entity;
            refreshInspectorPanel();
        });
        hierarchyPanel->addChild(entityButton);
    }
}

void EditorLayer::refreshInspectorPanel() {
    inspectorPanel->clearChildren();
    if (!selectedEntity)
        return;

    for (const auto& [typeIndex, dataPtr] : selectedEntity.getComponents()) {
        const Axiom::ComponentInfo* componentInfo = Axiom::ComponentReflection::getComponentInfo(typeIndex);
        if (!componentInfo) {
            continue;
        }
        auto componentBox = std::make_shared<Axiom::UIVerticalBox>();
        componentBox->setMargin({0.0f, 0.0f, 0.0f, 10.0f});

        auto headerText = std::make_shared<Axiom::UIText>(componentInfo->name);
        headerText->setMargin({0.0f, 0.0f, 0.0f, 5.0f});
        componentBox->addChild(headerText);
        for (const auto& field : componentInfo->fields) {
            void* fieldPtr = static_cast<char*>(dataPtr) + field.offset;

            auto fieldUI = createFieldUI(field, fieldPtr);
            if (fieldUI) {
                componentBox->addChild(fieldUI);
            }
        }
        inspectorPanel->addChild(componentBox);
    }
}

std::shared_ptr<Axiom::UIElement> EditorLayer::createFieldUI(const Axiom::FieldInfo& field, void* fieldPtr) {
    auto horizontalBox = std::make_shared<Axiom::UIHorizontalBox>();
    horizontalBox->setMargin({0.0f, 0.0f, 0.0f, 5.0f});

    std::string fieldName = field.name;
    fieldName[0] = std::toupper(fieldName[0]);
    auto label = std::make_shared<Axiom::UIText>(fieldName);
    label->setFontSize(6.0f);
    label->setFixedSize({80.0f, -1.0f});
    label->setVerticalAlignment(Axiom::UIAlignment::Center);
    horizontalBox->addChild(label);

    switch (field.type) {
    case Axiom::FieldType::Float:
        buildFloatUI(horizontalBox, field, fieldPtr);
        break;
    case Axiom::FieldType::Int:
        buildIntUI(horizontalBox, field, fieldPtr);
        break;
    case Axiom::FieldType::Bool:
        buildBoolUI(horizontalBox, field, fieldPtr);
        break;
    case Axiom::FieldType::String:
        buildStringUI(horizontalBox, field, fieldPtr);
        break;
    case Axiom::FieldType::Vec2:
        buildVec2UI(horizontalBox, field, fieldPtr);
        break;
    case Axiom::FieldType::Vec3:
        buildVec3UI(horizontalBox, field, fieldPtr);
        break;
    case Axiom::FieldType::Vec4:
        buildVec4UI(horizontalBox, field, fieldPtr);
        break;
    case Axiom::FieldType::Color:
        buildColorUI(horizontalBox, field, fieldPtr);
        break;
    case Axiom::FieldType::AssetHandle:
    default:
        break;
    }

    return horizontalBox;
}

void EditorLayer::buildFloatUI(std::shared_ptr<Axiom::UIHorizontalBox> horizontalBox, const Axiom::FieldInfo& field, void* fieldPtr) {
    float* valuePtr = static_cast<float*>(fieldPtr);
    auto drag = std::make_shared<Axiom::UIScalarField<float>>();
    drag->setHorizontalAlignment(Axiom::UIAlignment::Fill);
    drag->setValueGetter([valuePtr]() { return *valuePtr; });
    drag->setValueSetter([valuePtr](float v) { *valuePtr = v; });
    horizontalBox->addChild(drag);
}

void EditorLayer::buildIntUI(std::shared_ptr<Axiom::UIHorizontalBox> horizontalBox, const Axiom::FieldInfo& field, void* fieldPtr) {
    int* valuePtr = static_cast<int*>(fieldPtr);
    auto drag = std::make_shared<Axiom::UIScalarField<int>>();
    drag->setHorizontalAlignment(Axiom::UIAlignment::Fill);
    drag->setValueGetter([valuePtr]() { return *valuePtr; });
    drag->setValueSetter([valuePtr](int v) { *valuePtr = v; });
    horizontalBox->addChild(drag);
}

void EditorLayer::buildBoolUI(std::shared_ptr<Axiom::UIHorizontalBox> horizontalBox, const Axiom::FieldInfo& field, void* fieldPtr) {
    bool* valuePtr = static_cast<bool*>(fieldPtr);
    auto checkbox = std::make_shared<Axiom::UICheckbox>();
    checkbox->setHorizontalAlignment(Axiom::UIAlignment::Start);
    checkbox->setValueGetter([valuePtr]() { return *valuePtr; });
    checkbox->setValueSetter([valuePtr](bool v) { *valuePtr = v; });
    horizontalBox->addChild(checkbox);
}

void EditorLayer::buildStringUI(std::shared_ptr<Axiom::UIHorizontalBox> horizontalBox, const Axiom::FieldInfo& field, void* fieldPtr) {
    std::string* valuePtr = static_cast<std::string*>(fieldPtr);
    auto textInput = std::make_shared<Axiom::UITextInput>();
    textInput->setValueGetter([valuePtr]() { return *valuePtr; });
    textInput->setValueSetter([this, valuePtr](const std::string& v) {
        *valuePtr = v;
        refreshHierarchyPanel();
    });
    horizontalBox->addChild(textInput);
}

void EditorLayer::buildVec2UI(std::shared_ptr<Axiom::UIHorizontalBox> horizontalBox, const Axiom::FieldInfo& field, void* fieldPtr) {
    Math::Vec2* valuePtr = static_cast<Math::Vec2*>(fieldPtr);
    auto createAxis = [&](Axiom::Color color, auto getter, auto setter) {
        auto drag = std::make_shared<Axiom::UIScalarField<float>>();
        drag->setHorizontalAlignment(Axiom::UIAlignment::Fill);
        drag->setValueGetter(getter);
        drag->setValueSetter(setter);
        drag->setNormalColor(color);
        drag->setFontSize(6.0f);
        horizontalBox->addChild(drag);
    };

    createAxis(Axiom::Color(0.9f, 0.1f, 0.1f), [valuePtr]() { return valuePtr->x(); }, [valuePtr](float v) { valuePtr->x() = v; });
    createAxis(Axiom::Color(0.1f, 0.9f, 0.1f), [valuePtr]() { return valuePtr->y(); }, [valuePtr](float v) { valuePtr->y() = v; });
}

void EditorLayer::buildVec3UI(std::shared_ptr<Axiom::UIHorizontalBox> horizontalBox, const Axiom::FieldInfo& field, void* fieldPtr) {
    Math::Vec3* valuePtr = static_cast<Math::Vec3*>(fieldPtr);

    auto createAxis = [&](Axiom::Color color, auto getter, auto setter) {
        auto drag = std::make_shared<Axiom::UIScalarField<float>>();
        drag->setHorizontalAlignment(Axiom::UIAlignment::Fill);
        drag->setValueGetter(getter);
        drag->setValueSetter(setter);
        drag->setNormalColor(color);
        drag->setFontSize(6.0f);
        horizontalBox->addChild(drag);
    };

    createAxis(Axiom::Color(0.9f, 0.1f, 0.1f), [valuePtr]() { return valuePtr->x(); }, [valuePtr](float v) { valuePtr->x() = v; });
    createAxis(Axiom::Color(0.1f, 0.9f, 0.1f), [valuePtr]() { return valuePtr->y(); }, [valuePtr](float v) { valuePtr->y() = v; });
    createAxis(Axiom::Color(0.1f, 0.1f, 0.9f), [valuePtr]() { return valuePtr->z(); }, [valuePtr](float v) { valuePtr->z() = v; });
}

void EditorLayer::buildVec4UI(std::shared_ptr<Axiom::UIHorizontalBox> horizontalBox, const Axiom::FieldInfo& field, void* fieldPtr) {
    Math::Vec4* valuePtr = static_cast<Math::Vec4*>(fieldPtr);

    auto createAxis = [&](Axiom::Color color, auto getter, auto setter) {
        auto drag = std::make_shared<Axiom::UIScalarField<float>>();
        drag->setHorizontalAlignment(Axiom::UIAlignment::Fill);
        drag->setValueGetter(getter);
        drag->setValueSetter(setter);
        drag->setNormalColor(color);
        drag->setFontSize(6.0f);
        horizontalBox->addChild(drag);
    };

    createAxis(Axiom::Color(0.9f, 0.1f, 0.1f), [valuePtr]() { return valuePtr->x(); }, [valuePtr](float v) { valuePtr->x() = v; });
    createAxis(Axiom::Color(0.1f, 0.9f, 0.1f), [valuePtr]() { return valuePtr->y(); }, [valuePtr](float v) { valuePtr->y() = v; });
    createAxis(Axiom::Color(0.1f, 0.1f, 0.9f), [valuePtr]() { return valuePtr->z(); }, [valuePtr](float v) { valuePtr->z() = v; });
    createAxis(Axiom::Color(0.9f, 0.9f, 0.1f), [valuePtr]() { return valuePtr->w(); }, [valuePtr](float v) { valuePtr->w() = v; });
}

void EditorLayer::buildColorUI(std::shared_ptr<Axiom::UIHorizontalBox> horizontalBox, const Axiom::FieldInfo& field, void* fieldPtr) {
    Axiom::Color* valuePtr = static_cast<Axiom::Color*>(fieldPtr);
    // TODO: add a color picker UI element and use it here
    auto colorPreview = std::make_shared<Axiom::UIButton>("");
    colorPreview->setHorizontalAlignment(Axiom::UIAlignment::Start);
    colorPreview->setFixedSize({20.0f, 20.0f});
    colorPreview->setNormalColor(*valuePtr);
    colorPreview->setHoverColor(*valuePtr);
    colorPreview->setActiveColor(*valuePtr);
    horizontalBox->addChild(colorPreview);

    auto createChannelSlider = [&](const std::string& channelName, auto getter, auto setter) {
        auto slider = std::make_shared<Axiom::UIScalarField<float>>();
        slider->setHorizontalAlignment(Axiom::UIAlignment::Fill);
        slider->setValueGetter(getter);
        slider->setValueSetter(setter);
        slider->setNormalColor(Axiom::Color(0.5f, 0.5f, 0.5f));
        slider->setLimits(0.0f, 1.0f);
        slider->setFontSize(6.0f);
        horizontalBox->addChild(slider);
    };

    createChannelSlider(
        "R", [valuePtr]() { return valuePtr->r(); },
        [valuePtr, colorPreview](float v) {
            valuePtr->r() = v;
            colorPreview->setNormalColor(*valuePtr);
            colorPreview->setHoverColor(*valuePtr);
            colorPreview->setActiveColor(*valuePtr);
        });
    createChannelSlider(
        "G", [valuePtr]() { return valuePtr->g(); },
        [valuePtr, colorPreview](float v) {
            valuePtr->g() = v;
            colorPreview->setNormalColor(*valuePtr);
            colorPreview->setHoverColor(*valuePtr);
            colorPreview->setActiveColor(*valuePtr);
        });
    createChannelSlider(
        "B", [valuePtr]() { return valuePtr->b(); },
        [valuePtr, colorPreview](float v) {
            valuePtr->b() = v;
            colorPreview->setNormalColor(*valuePtr);
            colorPreview->setHoverColor(*valuePtr);
            colorPreview->setActiveColor(*valuePtr);
        });
    createChannelSlider(
        "A", [valuePtr]() { return valuePtr->a(); },
        [valuePtr, colorPreview](float v) {
            valuePtr->a() = v;
            colorPreview->setNormalColor(*valuePtr);
            colorPreview->setHoverColor(*valuePtr);
            colorPreview->setActiveColor(*valuePtr);
        });
}
