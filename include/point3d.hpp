#include <SFML/System/Vector3.hpp>
#include <SFML/System/Vector2.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Vertex.hpp>

#include "object3d.hpp"

typedef sf::Vector3<double> Vector3d;
typedef sf::Vector2<double> Vector2d;

namespace sf {
    namespace _3D {
        struct Point3D : public Object3D {
            Point3D() = default;

            Point3D(Vector3d position) {
                pos = position;
            }

            Vector2d get_2d_position() {
                return Vector2d(pos.x, pos.y);
            }

            void draw(RenderTarget& target) const;

            Vector3d pos;
        };
    }
}