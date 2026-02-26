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

}

void EditorLayer::onEvent(Axiom::Event& event) {
	
}

