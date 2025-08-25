#include "EditorLayer.h"

EditorLayer::EditorLayer() : Layer("EditorLayer"), cameraPosition(0.0f, 0.0f, -30.0f) {

}

void EditorLayer::onAttach() {
	Axiom::AX_CORE_LOG_INFO("EditorLayer attached");
}

void EditorLayer::onDetach() {
	Axiom::AX_CORE_LOG_INFO("EditorLayer detached");
}

void EditorLayer::onUpdate() {
	if (Axiom::Input::isKeyPressed(Axiom::KeyCode::W)) {
		cameraPosition.z() += 0.01f;
	}
	if (Axiom::Input::isKeyPressed(Axiom::KeyCode::S)) {
		cameraPosition.z() -= 0.01f;
	}
	if (Axiom::Input::isKeyPressed(Axiom::KeyCode::A)) {
		cameraPosition.x() += 0.01f;
	}
	if (Axiom::Input::isKeyPressed(Axiom::KeyCode::D)) {
		cameraPosition.x() -= 0.01f;
	}
	Math::Mat4 translation = Math::Mat4::translate(Math::Vec3(-cameraPosition.x(), -cameraPosition.y(), -cameraPosition.z()));
	Math::Mat4 view = translation.inverse();
	// This will not stay here, we will have a camera class later
	Axiom::RendererSystem::setViewMatrix(view);
}

void EditorLayer::onEvent(Axiom::Event& event) {
	
}

