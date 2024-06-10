#ifndef _TEST_ENGINES_H_
#define _TEST_ENGINES_H_

#define LMK_INCLUDE_ALL
#include "LMK/LMK.h"

using namespace lmk;

#pragma warning(disable : 4244)
class CollisionTest : public LMKEngine {
private: // Properties
	TTFont font;
	Text text;
	std::thread updateFPS;

	bool updated = false;
	bool overlapping = false;
	std::vector<uint32_t> overlappingBounds;

	Vector2 offset = Vector2::one() * 200;
	float rotation = 0;
	Vector2 scale = Vector2::one();

	PolygonCollider2D* playerCollider = nullptr;
	std::vector<BaseCollider2D*> staticColliders;

	Vector2 targetPos;
	float targetRotation;
	Vector2 forward;

	const float moveSpeed = 400;		// Unit of pixels per seconds
	const float rotationSpeed = 720;	// Unit of degrees per seconds
	const Vector2 scaleSpeed = Vector2(1.5f, 1.5f);

public: // Functions
	inline void OnUserStart() override {
		updateFPS = std::thread(&CollisionTest::UpdateText, this);

		font.LoadFont("Assets/Fonts/visitor2.ttf", 40);
		text.font = font;

		auto CreatePolygon = [&](uint8_t _corners, float _radius) {
			float rotationIter = 360.0f / _corners;
			std::vector<Vector2> vertices = { Vector2(0, _radius) };
			for (uint8_t i = 1; i < _corners; i++) {
				vertices.push_back(Vector2(
					-1 * _radius * std::sinf(LMK_DtoR(rotationIter * i)),
					_radius * std::cosf(LMK_DtoR(rotationIter * i)))
				);
			}
			return vertices;
		};

		playerCollider = new PolygonCollider2D(CreatePolygon(5, 30));
		playerCollider->Transform(offset, rotation, scale);
		forward = Vector2::up();

		for (int i = 0; i < 50; i++) {
			staticColliders.push_back(new BoxCollider2D(Vector2(30, 30), 0));
			staticColliders[i]->Transform(
				Vector2(
					Random::Range<float>(30, Screen::resolution().x - 30), 
					Random::Range<float>(30, Screen::resolution().y - 30)
				),
				Random::Range<float>(0, 359),
				Vector2(
					Random::Range<float>(0.5f, 5.0f), 
					Random::Range<float>(0.5f, 5.0f)
				)
			);

			overlappingBounds.push_back(false);
		}
	}

	inline void OnUserUpdate() override {
		// Transform player's collider.
		if (Input::GetMouseButton(MouseButton::Left)) {
			targetPos = Input::mousePosition();
			targetRotation = Vector2::SignedAngle(forward, targetPos - offset);

			if (std::abs(targetRotation - rotation) < rotationSpeed * Time::deltaTime()) {
				rotation = targetRotation;
			}
			else {
				rotation += rotationSpeed * Time::deltaTime() * ((targetRotation - rotation < 0) ? -1 : 1);
			}

			offset = Vector2::MoveTowards(offset, targetPos, moveSpeed * Time::deltaTime());
		}

		if (Input::GetKey(KeyCode::R)) {
			scale += scaleSpeed * Time::deltaTime();
		}
		else if (Input::GetKey(KeyCode::F)) {
			scale -= scaleSpeed * Time::deltaTime();
		}

		playerCollider->Transform(offset, rotation, scale);

		// Check collision.
		overlapping = false;

		for (size_t i = 0; i < staticColliders.size(); i++) {
			if (playerCollider->OverlapWith(*staticColliders[i])) {
				overlapping = true;
			}

			if (playerCollider->bounds().Intersects(staticColliders[i]->bounds())) {
				overlappingBounds[i] = true;
			}
			else {
				overlappingBounds[i] = false;
			}
		}
	}

	inline void OnDrawGizmos() override {
		// Draw player's collider.
		Gizmos::SetColor(Color::green() / 2.0f);
		Gizmos::DrawRect(playerCollider->bounds());

		if (overlapping) {
			Gizmos::SetColor(Color::red());
		}
		else {
			Gizmos::SetColor(Color::white());
		}

		Gizmos::DrawPolygon(playerCollider->vertices());
		Gizmos::DrawLine(offset, playerCollider->vertex(0));

		if (scale.x < 0 || scale.y < 0) {
			Gizmos::SetColor(Color::red());
			Gizmos::DrawLine(playerCollider->vertex(0), playerCollider->vertex(2));
			Gizmos::DrawLine(playerCollider->vertex(1), playerCollider->vertex(3));
		}

		// Draw static colliders.
		for (size_t i = 0; i < staticColliders.size(); i++) {
			Gizmos::SetColor(Color::green() / 2.0f);
			Gizmos::DrawRect(staticColliders[i]->bounds());

			if (overlappingBounds[i]) {
				Gizmos::SetColor(Color::yellow());
			}
			else {
				Gizmos::SetColor(Color::yellow() / 2.0f);
			}
			Gizmos::DrawPolygon(staticColliders[i]->vertices());
		}

		text.Display(m_renderer, Vector2Int(30, 30), Color::yellow() * 0.7f);
	}

	inline void OnUserExit() override {
		updateFPS.join();
	}

private:
	inline void UpdateText() {
		using namespace std::literals::chrono_literals;

		while (isRunning())
		{
			text.text = "FPS: " + std::to_string(Time::frameRate());
			std::this_thread::sleep_for(0.25s);
		}
	}
};



class RenderTest : public LMKEngine {
private:
	TTFont font;
	Text text;
	std::thread updateFPS;

	//Texture2D texture;
	//Sprite sprite;

	Camera2D camera{6};
	std::array<Vector2, 6> worldPoints {
		Vector2{0, 0},
		Vector2{1, 1},
		Vector2{1, 2},
		Vector2{0, 1},
		Vector2{-1, 0},
		Vector2{1, 0},
	};
	std::array<Vector2, 6> screenPoints { };

	Vector2 moveDirection;
	float rotateDirection;
	float moveSpeed = 100;
	float rotateSpeed = 45; // Degree

	float pixelsPerUnit;

public:
	inline void OnUserStart() override {
		//texture.LoadRawTextureData("Assets/Images/Koopa.png");
		//sprite.setTexture(texture);

		updateFPS = std::thread(&RenderTest::UpdateText, this);

		//Screen::SetFullScreenMode(FullScreenMode::Windowed);

		camera.transform = new Transform(Vector2::zero(), 0, Vector2::one());
		camera.ResetAspect();

		font.LoadFont("Assets/Fonts/visitor2.ttf", 40);
		text.font = font;
	}

	inline void OnUserUpdate() override {
		if (Input::GetKeyDown(KeyCode::F11)) {
			if (Screen::fullScreen()) {
				Screen::SetFullScreenMode(FullScreenMode::Windowed);
			}
			else {
				Screen::SetFullScreenMode(FullScreenMode::Borderless);
			}
		}

		moveDirection = Vector2::zero();
		if (Input::GetKey(KeyCode::W)) {
			moveDirection.y += 1;
		}
		if (Input::GetKey(KeyCode::S)) {
			moveDirection.y -= 1;
		}
		if (Input::GetKey(KeyCode::D)) {
			moveDirection.x += 1;
		}
		if (Input::GetKey(KeyCode::A)) {
			moveDirection.x -= 1;
		}

		rotateDirection = 0;
		if (Input::GetKey(KeyCode::Q)) {
			rotateDirection += 1;
		}
		if (Input::GetKey(KeyCode::E)) {
			rotateDirection -= 1;
		}

		if (Input::GetMouseButtonDown(MouseButton::Middle)) {
			std::cout << "Start panning...\n";
		}
		else if (Input::GetMouseButtonUp(MouseButton::Middle)) {
			std::cout << "Stop panning...\n";
		}

		// Update camera properties.
		camera.size -= Input::mouseScrollDelta().y * 200.0f;
		camera.transform->setPosition(camera.transform->position() + moveDirection.normalized() * moveSpeed * Time::deltaTime());
		camera.transform->setRotation(camera.transform->rotation() + rotateDirection * rotateSpeed * Time::deltaTime());

		// Update pixel per unit. 
		pixelsPerUnit = Screen::height() / (camera.size * 2);

		// Transform world points to screen points.
		for (size_t i = 0; i < 6; i++) {
			screenPoints[i] = camera.WorldToScreenPoint(worldPoints[i]);
		}
	}

	inline void OnDrawGizmos() override {
		Gizmos::SetColor(Color::green());
		for (auto screenPoint : screenPoints) {
			Gizmos::DrawRect(screenPoint, Vector2(pixelsPerUnit, pixelsPerUnit) / 2);
		}

		text.Display(m_renderer, Vector2Int(50, 50), Color::yellow());
	}

	inline void OnUserExit() override {
		updateFPS.join();
	}

private:
	inline void UpdateText() {
		using namespace std::literals::chrono_literals;

		while (isRunning()) {
			text.text = std::to_string(Time::frameRate());
			std::this_thread::sleep_for(0.1s);
		}
	}
};



class GameObjectTest : public LMKEngine {
private:
	Scene* scene = new Scene();
	GameObject playerObject;
	GameObject enemyObject;

public:
	inline void OnUserStart() override {
		playerObject = scene->CreateGameObject("Player");
		playerObject.AddComponent<Rigidbody2D>();
		playerObject.AddComponent<SpriteRenderer>();

		playerObject.transform->setPosition(Vector2(100, 100));

		std::cout << "Player's Position: " << (std::string)playerObject.transform->position() << "\n";
		std::cout << "Player's Rotation: " << playerObject.transform->rotation() << "\n";
	}

	inline void OnUserUpdate() override {
		if (Input::GetKeyDown(KeyCode::F11)) {
			if (Screen::fullScreen()) {
				Screen::SetFullScreenMode(FullScreenMode::Windowed);
			}
			else {
				Screen::SetFullScreenMode(FullScreenMode::Borderless);
			}
		}

		if (Input::GetKeyDown(KeyCode::P)) {

		}
	}

	inline void OnDrawGizmos() override {
		Gizmos::SetColor(Color::green());
		Gizmos::DrawRect(Bounds(playerObject.transform->position(), Vector2(10, 10)));
	}

	inline void OnUserExit() override {

	}
};
#pragma warning(default : 4244)

#endif // !_TEST_ENGINES_H_