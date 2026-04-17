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
}

void EditorLayer::onDetach() {
    Axiom::AX_LOG_INFO("EditorLayer detached");
}

void EditorLayer::onUpdate() {
}

void EditorLayer::onUIRender() {
    Axiom::UI::beginPanel("Tool Bar", Math::Vec2(0.0f, 0.0f), Math::Vec2(Axiom::Application::getWindow()->getWidth() / 4.0f, 600.0f));
    float fullWidth = Axiom::UI::getAvaibleWidth();
    float halfWidth = (fullWidth - Axiom::UI::getCurrentStyle().itemSpacing) / 2.0f;
    Axiom::UI::button("Button 1", Math::Vec2(halfWidth, 30.0f));
    Axiom::UI::sameLine();
    Axiom::UI::button("Button 2", Math::Vec2(halfWidth, 30.0f));
    Axiom::UI::dragFloat("Slider", sliderValue);
    if (Axiom::UI::treeNode("Tree Node 1")) {
        Axiom::UI::text("Child 1", Axiom::Color::white(), 8);
        Axiom::UI::text("Child 2", Axiom::Color::white(), 8);
        Axiom::UI::treePop();
    }
    Axiom::UI::image(textureAsset->getTexture().get(), Math::Vec2(64.0f, 64.0f));
    Axiom::UI::endPanel();
}

void EditorLayer::onEvent(Axiom::Event& event) {
}

void EditorLayer::onRender(Axiom::CommandBuffer* commandBuffer) {
    renderPass.colorAttachments[0].texture = Axiom::Application::getRenderer()->getCurrentRenderTarget();
    viewportSize = Axiom::Application::getRenderer()->getCurrentRenderTargetSize();
    renderPass.width = static_cast<uint32_t>(viewportSize.x());
    renderPass.height = static_cast<uint32_t>(viewportSize.y());

    commandBuffer->beginRendering(renderPass);
    commandBuffer->setViewport(0.0f, 0.0f, viewportSize.x(), viewportSize.y());
    commandBuffer->setScissor(0, 0, viewportSize.x(), viewportSize.y());
    commandBuffer->endRendering();
}
