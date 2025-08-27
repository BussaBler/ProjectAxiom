#include "EditorLayer.h"

EditorLayer::EditorLayer() : Layer("EditorLayer"), cameraPosition(0.0f, 0.0f, 30.0f) {

}

void EditorLayer::onAttach() {
	Axiom::AX_CORE_LOG_INFO("EditorLayer attached");
}

void EditorLayer::onDetach() {
	Axiom::AX_CORE_LOG_INFO("EditorLayer detached");
}

void EditorLayer::onUpdate() {
	Math::Mat4 translation = Math::Mat4::translate(Math::Vec3(cameraPosition.x(), cameraPosition.y(), cameraPosition.z()));
	Math::Mat4 view = translation.inverse();
	Axiom::RendererSystem::setViewMatrix(view);
	if (Axiom::Input::isKeyPressed(Axiom::KeyCode::W)) {
		cameraPosition += view.getForward() * 0.01f;
	}
	if (Axiom::Input::isKeyPressed(Axiom::KeyCode::S)) {
		cameraPosition += view.getBackward() * 0.01f;
	}
	if (Axiom::Input::isKeyPressed(Axiom::KeyCode::A)) {
		cameraPosition += view.getLeft() * 0.01f;
	}
	if (Axiom::Input::isKeyPressed(Axiom::KeyCode::D)) {
		cameraPosition += view.getRight() * 0.01f;
	}
}

void EditorLayer::onEvent(Axiom::Event& event) {
	
}

