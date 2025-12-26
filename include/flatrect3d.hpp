#include <SFML/System/Vector3.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/RenderTarget.hpp>   

#include <array>

namespace sf {
    namespace _3D {
        // idk what to call this thing cuz its not quite a rectangle but its a flat shape in 2d space
        class Rect2D {
            public:
            Rect2D(Vector2<double> start, Vector2<double> end) {
                this->vertices = {start, Vector2<double>(end.x, start.y), Vector2<double>(start.x, end.y), end};
            }

            Rect2D(std::array<Vector2<double>, 4> vertices) {
                this->vertices = vertices;
            }

            void draw(RenderTarget& target) const {
                VertexArray quad(PrimitiveType::TriangleStrip, 5);
                for (int i = 0; i < 4; i++) {
                    quad[i].position = Vector2f(vertices[i].x, vertices[i].y);
                    quad[i].color = color;
                }
                target.draw(quad);
            }

            Color color;

            private:
            std::array<sf::Vector2<double>, 4> vertices;
        };
    }
}