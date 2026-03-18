#include "EditorLayer.h"

EditorLayer::EditorLayer() : Layer("EditorLayer") {

}

void EditorLayer::onAttach() {
	Axiom::AX_LOG_INFO("EditorLayer atached");
	Axiom::RenderAttachment colorAttachment;
	colorAttachment.loadOp = Axiom::LoadOp::Clear;
	colorAttachment.storeOp = Axiom::StoreOp::Store;
	colorAttachment.clearColor = Math::Vec4(0.0f, 0.0f, 1.0f, 1.0f);
	viewportSize = Axiom::Renderer::getCurrentRenderTargetSize();

	renderPass.colorAttachments[0] = colorAttachment;
	renderPass.colorAttachmentCount = 1;
	renderPass.width = viewportSize.x();
	renderPass.height = viewportSize.y();
}

void EditorLayer::onDetach() {
	Axiom::AX_LOG_INFO("EditorLayer detached");
}

void EditorLayer::onUpdate() {
	
}

void EditorLayer::onUIRender() {
	if (Axiom::UI::button("Test Button", Math::Vec2(0.0f, 0.0f), Math::Vec2(100.0f, 100.0f))) {
		Axiom::AX_LOG_INFO("Button clicked!");
	}
}

void EditorLayer::onEvent(Axiom::Event& event) {
	
}

void EditorLayer::onRender(Axiom::CommandBuffer* commandBuffer) {
	renderPass.colorAttachments[0].texture = Axiom::Renderer::getCurrentRenderTarget();
	viewportSize = Axiom::Renderer::getCurrentRenderTargetSize();
	renderPass.width = static_cast<uint32_t>(viewportSize.x());
	renderPass.height = static_cast<uint32_t>(viewportSize.y());

	commandBuffer->beginRendering(renderPass);
	commandBuffer->setViewport(0.0f, 0.0f, viewportSize.x(), viewportSize.y());
	commandBuffer->setScissor(0, 0, viewportSize.x(), viewportSize.y());
	commandBuffer->endRendering();
}

