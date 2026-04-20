#pragma once
#include "Axiom.h"

class EditorCamera : public Axiom::Camera {
  public:
    EditorCamera(const Math::Vec3& position = Math::Vec3::zero(), float pitch = 0.0f, float yaw = 0.0f);
    ~EditorCamera() override = default;

    void onUpdate(float deltaTime);

    inline const Math::Mat4& getView() const { return view; }

  private:
    void updateView();

  private:
    Math::Mat4 view;
    Math::Vec3 position;
    float pitch = 0.0f;
    float yaw = 0.0f;
};