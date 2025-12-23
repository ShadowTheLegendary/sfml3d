////////////////////////
//    camera3d.cpp    //
////////////////////////

#include <SFML/Graphics/Sprite.hpp>

#include <iostream>
#include <algorithm>

#include "camera3d.hpp"
#include "object3d.hpp"

namespace sf {
	namespace _3D {

		namespace {
			Vector3<double> calculate_mean_center(const std::vector<Object3D*>& objects) {
				if (objects.empty()) {
					return Vector3 <double>(0.0f, 0.0f, 0.0f);
				}

				Vector3 <double> sum(0.0f, 0.0f, 0.0f);

				for (int i = 0; i < static_cast<int>(objects.size()); i++) {
					sum += objects[i]->get_center();
				}

				double count = static_cast<double>(objects.size());
				Vector3<double> mean = sum / count;

				return mean;
			}
		}

		Camera3D::Camera3D(Vector3f position, Vector3<Angle> rotation, Vector2u size)
			: position(position),
			rotation(rotation),
			size(size) {
			if (not rt.resize(size)) {
				// idk >:)
			}
		}

		Vector3f Camera3D::get_position() {
			return position;
		}

		Vector3<Angle> Camera3D::get_rotation() {
			return rotation;
		}

		void Camera3D::set_position(Vector3f position) {
			this->position = position;
		}

		void Camera3D::set_rotation(Vector3<Angle> rotation) {
			this->rotation = rotation;
		}

		void Camera3D::rotate(Vector3<Angle> rotation) {
			this->rotation += rotation;
		}

		void Camera3D::move(Vector3f offset) {
			position += offset;
		}

		void Camera3D::draw(RenderTarget& target, RenderStates states) const {
			target.draw(Sprite(rt.getTexture()), states);
		}

		void Camera3D::update(std::vector<Object3D*>& objects) {
			rt.clear(Color::Transparent);
			std::vector<std::pair<double, Object3D*>> depth_sorted_objects;
			depth_sorted_objects.reserve(objects.size());

			sf::Vector3<double> mean = calculate_mean_center(objects);
			mean -= Vector3<double>(position);

			for (Object3D* object : objects) {
				object->update(Vector3<double>(position), rotation, mean);
				depth_sorted_objects.emplace_back(object->perceived_z, object);
			}

			std::sort(depth_sorted_objects.begin(), depth_sorted_objects.end(),
				[](const auto& a, const auto& b) {
					return a.first > b.first;
			});

			for (auto& object : depth_sorted_objects) {
				object.second->draw(rt);
			}
			rt.display();
		}
	}
}