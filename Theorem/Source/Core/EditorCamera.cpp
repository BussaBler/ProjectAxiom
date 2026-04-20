#include "EditorCamera.h"

EditorCamera::EditorCamera(const Math::Vec3& position, float pitch, float yaw) : position(position), pitch(pitch), yaw(yaw) {
    updateView();
}

void EditorCamera::onUpdate(float deltaTime) {
    // TODO: I don;t know how to handle input since that should be done
    // by a layer
}

void EditorCamera::updateView() {
    Math::Mat4 rotation = Math::Mat4::rotateX(pitch) * Math::Mat4::rotateY(yaw);
    Math::Mat4 translation = Math::Mat4::translate(-position);
    view = rotation * translation;
}
