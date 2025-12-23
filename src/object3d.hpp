#pragma once

#include <SFML/Graphics/RenderTarget.hpp>

#include <SFML/System/Vector3.hpp>
#include <SFML/System/Angle.hpp>

#include <functional>
#include <vector>

typedef sf::Vector3<double> Vector3D;

namespace sf {
    namespace _3D {
        struct FaceDrawable {
            Vector3D centroid;
            std::function<void(RenderTarget&)> draw;
        };

        class Object3D {
            public:
            virtual ~Object3D() = default;

            virtual void draw(RenderTarget& target) const = 0;

            virtual void update(Vector3D position_offset, Vector3<Angle> rotation_offset, Vector3D rotation_axis) = 0;

            virtual Vector3D get_center() const = 0;

            double perceived_z{0.0};
        };
    }
}