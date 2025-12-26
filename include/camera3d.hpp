////////////////////////
//    camera3d.hpp    //
////////////////////////

#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTexture.hpp>

#include <SFML/System/Vector3.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Angle.hpp>

#include <vector>

#include "voxel.hpp"
#include "object3d.hpp"

namespace sf {
	namespace _3D {

		namespace {
			Vector3<double> calculate_mean_center(const std::vector<Object3D*>& objects);
		}

		class Camera3D : public Drawable {
		public:
			Camera3D() = default;

			Camera3D(Vector3f position, Vector3<Angle> rotation, Vector2u size);

			Vector3f get_position();

			Vector3<Angle> get_rotation();

			void rotate(Vector3<Angle> rotation);

			void move(Vector3f offset);

			void set_position(Vector3f position);

			void set_rotation(Vector3<Angle> rotation);

			void draw(RenderTarget& target, RenderStates states) const override;

			void update(std::vector<Object3D*>& objects);

			template<typename Container>
			void update(Container& objects) {
				std::vector<Object3D*> ptrs;
				ptrs.reserve(objects.size());
				for (auto& o : objects) ptrs.push_back(static_cast<Object3D*>(&o));
				update(ptrs);
			}

		private:
			Vector3f position;
			Vector3<Angle> rotation;
			Vector2u size;
			RenderTexture rt;

		};
	}
}