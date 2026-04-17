#ifndef _TEST_ENGINES_H_
#define _TEST_ENGINES_H_

#define LMK_INCLUDE_ALL
#include "LMK.h"

using namespace lmk;

#if false
#pragma warning(disable : 4244)
class TileMapTest : public LMKEngine {
private:
	TTFont font;
	TextGUI text;

	// Camera 
	Camera2D camera{ 10 };
	bool lockRotation = true;
	bool panning;
	bool rotating;
	Vector2 moveDirection;
	float targetAngle;
	float zoomSpeed = 300;

	// Tilemap
	TileMapPalette palette;
	TileMap tilemap;

	// Controls
	Vector2 lastMousePos;

public:
	inline void OnUserStart() override {
		font.LoadFont("Assets/Fonts/visitor2.ttf", 30);
		text.font = font;
		text.text = "Tile Map Test";

		// Setup camera
		camera.transform = new Transform();
		camera.RecalculateAspect();

		// Tile map setup
		tilemap.GenerateTileMap("Assets/Tilemaps/Level1.txt");
	}

	inline void OnUserUpdate() override {
		if (Input::GetKeyDown(KeyCode::F11)) {
			std::cout << Screen::GetWidth() << " : " << Screen::GetHeight() << "\n";

			if (Screen::IsFullScreen()) {
				Screen::SetFullScreenMode(FullScreenMode::Windowed);
			}
			else {
				Screen::SetFullScreenMode(FullScreenMode::FullScreen);
			}
		}

		// Camera controls
		if (Input::GetKeyDown(KeyCode::P)) {
			camera.transform->SetLocalPosition(Vector2::zero);
			camera.transform->SetLocalRotation(0);
		}

		if (Input::GetKeyDown(KeyCode::O)) {
			camera.transform->SetParent(nullptr);
		}

		if (Input::GetMouseButtonDown(MouseButton::Middle)) {
			lastMousePos = Input::GetMousePosition();
			panning = true;
		}
		else if (Input::GetMouseButtonUp(MouseButton::Middle)) {
			panning = false;
		}

		if (Input::GetMouseButtonDown(MouseButton::Right)) {
			lastMousePos = Input::GetMousePosition();
			rotating = true;
		}
		else if (Input::GetMouseButtonUp(MouseButton::Right)) {
			rotating = false;
		}

		// Update camera properties
		if (panning) {
			moveDirection = camera.transform->InverseTransformDirection(lastMousePos - Input::GetMousePosition()) / camera.GetPixelsPerUnit();
			lastMousePos = Input::GetMousePosition();
		}
		if (rotating) {
			targetAngle = Vector2::SignedAngle(Input::GetMousePosition().GetNormalized(), lastMousePos.GetNormalized());
			lastMousePos = Input::GetMousePosition();
		}

		camera.SetSize(camera.GetSize() - Input::mouseScroll().y * Time::deltaTime() * zoomSpeed);
		camera.transform->SetLocalPosition(camera.transform->GetLocalPosition() + moveDirection);
		camera.transform->SetLocalRotation(camera.transform->GetLocalRotation() + targetAngle * 2);
		camera.RecalculateViewMatrix();
	}

	inline void OnDrawGizmos() override {
		Gizmos::DrawWorldGrid(camera);

		text.Display(m_renderer, Vector2Int(30, 30), Color::yellow);
	}
};
#endif

#if false
class RigidbodyTest : public LMKEngine {
private: // Properties
	TTFont font;
	TextGUI text;
	std::thread updateFPS;

	// Camera 
	Camera2D camera{10};
	bool lockRotation = true;
	bool panning;
	bool rotating;
	Vector2 lastMousePos;
	Vector2 moveDirection;
	float targetAngle;
	float zoomSpeed = 30.0f;

	// Colliders
	std::vector<PolygonCollider2D*> staticBodies;
	std::vector<PolygonCollider2D*> dynamicBodies;
	std::vector<bool> overlap;

public: // Functions
	inline void OnUserStart() override {
		updateFPS = std::thread(&RigidbodyTest::UpdateText, this);

		font.LoadFont("Assets/Fonts/visitor2.ttf", 40);
		text.font = font;

		camera.transform = new Transform();
	}

	inline void OnUserUpdate() override {
		// Camera controls
		if (Input::GetKeyDown(KeyCode::P)) {
			camera.transform->SetPosition(Vector2::zero);
			camera.transform->SetRotation(0);
		}

		if (Input::GetKeyDown(KeyCode::O)) {
			camera.transform->SetParent(nullptr);
		}

		if (Input::GetMouseButtonDown(MouseButton::Middle)) {
			lastMousePos = Input::GetMousePosition();
			panning = true;
		}
		else if (Input::GetMouseButtonUp(MouseButton::Middle)) {
			panning = false;
		}

		if (Input::GetMouseButtonDown(MouseButton::Right)) {
			lastMousePos = Input::GetMousePosition();
			rotating = true;
		}
		else if (Input::GetMouseButtonUp(MouseButton::Right)) {
			rotating = false;
		}

		// Update camera properties
		if (panning) {
			moveDirection = camera.transform->InverseTransformDirection(lastMousePos - Input::GetMousePosition()) / camera.GetPixelsPerUnit();
			lastMousePos = Input::GetMousePosition();
		}
		if (rotating && !lockRotation) {
			targetAngle = Vector2::SignedAngle(Input::GetMousePosition().GetNormalized(), lastMousePos.GetNormalized());
			lastMousePos = Input::GetMousePosition();
		}

		camera.SetSize(camera.GetSize() - (camera.GetSize() * Input::GetMouseScroll().y * Time::GetDeltaTime() * zoomSpeed));

		camera.transform->SetLocalPosition(camera.transform->GetLocalPosition() + moveDirection);
		camera.transform->SetLocalRotation(camera.transform->GetLocalRotation() + targetAngle * 2);
		camera.RecalculateViewMatrix();

		if (Input::GetMouseButtonDown(MouseButton::Left)) {
			staticBodies.push_back(CreateCollider(
				camera.ScreenToWorldPoint(Input::GetMousePosition()), 
				Random::Range(0.0f, 360.0f),
				Vector2::one,
				Random::Range(3, 6)));
		}

		if (Input::GetMouseButtonDown(MouseButton::Right)) {
			dynamicBodies.push_back(CreateCollider(
				camera.ScreenToWorldPoint(Input::GetMousePosition()),
				Random::Range(0.0f, 360.0f),
				Vector2::one,
				Random::Range(3, 6)));

			overlap.emplace_back();
		}

		for (size_t i = 0; i < dynamicBodies.size(); i++) {
			for (auto staticBody : staticBodies) {
				overlap[i] = dynamicBodies[i]->OverlapCollider(*staticBody);
			}
		}
	}

	inline void OnDrawGizmos() override {
		Gizmos::DrawWorldGrid(camera);

		// Display colliders
		for (size_t i = 0; i < dynamicBodies.size(); i++) {
			if (overlap[i]) {
				Gizmos::SetColor(Color::red);
			}
			else {
				Gizmos::SetColor(Color::white);
			}
			std::vector<Vector2> transformedVertices;
			for (auto vertex : dynamicBodies[i]->) {
				transformedVertices.push_back(camera.WorldToScreenPoint(vertex));
			}
			Gizmos::DrawPolygon(transformedVertices);
		}

		Gizmos::SetColor(Color::yellow);
		for (auto body : staticBodies) {
			std::vector<Vector2> transformedVertices;
			for (auto vertex : body->vertices()) {
				transformedVertices.push_back(camera.WorldToScreenPoint(vertex));
			}
			Gizmos::DrawPolygon(transformedVertices);
		}

		// Display UI
		text.Display(m_renderer, Vector2Int(30, 30), Color::yellow * 0.7f);
	}

	inline void OnUserExit() override {
		updateFPS.join();
	}

private:
	[[nodiscard]] inline PolygonCollider2D* CreateCollider(Vector2 _position, float _rotation, Vector2 _scale, size_t _edges) const {
		auto collider = new PolygonCollider2D(CreatePolygon(_edges, 2));
		collider->transform = new Transform(_position, _rotation, _scale);
		collider->RecalculateVertices();
		return collider;
	}

	inline std::vector<Vector2> CreatePolygon(size_t _corners, float _radius) const {
		float rotationIter = 360.0f / _corners;
		std::vector<Vector2> vertices = { Vector2(0, _radius) };
		for (size_t i = 1; i < _corners; i++) {
			vertices.push_back(Vector2(
				-1 * _radius * std::sinf(LMK_DtoR(rotationIter * i)),
				_radius * std::cosf(LMK_DtoR(rotationIter * i)))
			);
		}
		return vertices;
	}

	inline void UpdateText() {
		using namespace std::literals::chrono_literals;

		while (isRunning())
		{
			text.text = "FPS: " + std::to_string(Time::GetFrameRate());
			std::this_thread::sleep_for(0.15s);
		}
	}
};



class TriangulationTest : public LMKEngine {
private:
	TTFont font;

	TextGUI fpsText;

	Scene scene;

	Camera2D* camera;
	PolygonCollider2D* orgPolygon;

	bool polyChanged;

protected: // Functions
	inline void OnUserStart() override {
		ResourceLoad();

		fpsText.font = font;

		auto& cameraObject = scene.CreateGameObject("Camera");
		auto& colliderObject = scene.CreateGameObject("Collider");

		camera = cameraObject.AddComponent<Camera2D>();
		camera->setSize(10);
		camera->backgroundColor = Color(50, 50, 50, 255);

		orgPolygon = colliderObject.AddComponent<PolygonCollider2D>();
	}

	inline void OnUserUpdate() override {

	}

	inline void OnUserFixedUpdate() override {
		static int count = 0;
		count++;
		if (count == 10) {
			count = 0;
			std::stringstream ss;
			ss << std::fixed << std::setprecision(2) << Time::GetFrameRate();
			fpsText.text = "FPS: " + ss.str();
		}
	}

	inline void OnDrawGizmos() override {
		Gizmos::DrawWorldGrid(*camera, 0.8f);

		Gizmos::setColor(Color::green);

		fpsText.Display(m_renderer, Vector2(20, 40), Color::white);
	}

private:
	inline void ResourceLoad() {
		font.LoadFont("Assets/Fonts/visitor2.ttf", 40);
	}
};
#endif

#if false
#define TEST_CLOSEST 0
#define TEST_PROJECTION 0
#define TEST_DISTANCE 1
#define TEST_MOVEMENT 0
class ColliderTest : public LMKEngine {
private: // Properties
	// UI
	TTFont font;
	TextGUI fps;
	TextGUI cameraLockState;
	std::thread updateFPS;

	// Camera 
	Camera2D camera{ 15 };
	bool lockRotation = true;
	bool panning;
	bool rotating;
	Vector2 lastMousePos;
	Vector2 moveDirection;
	float targetAngle;
	float zoomSpeed = 50.0f;

	// Colliders
	std::unique_ptr<PolygonCollider2D> centerCollider;
	std::vector<std::unique_ptr<PolygonCollider2D>> staticBodies;
	std::vector<std::vector<Vector2>> movementTrace;
	std::vector<bool> overlapMouse;
	std::vector<Vector2> closest;
	std::vector<Collider2D::ProjectionResult> projections;
	std::vector<ColliderDistance2D> distances;

	// Project axis
	Vector2 mouseWorldPos;
	Vector2 projAxis;
	float rotateSpeed = 45;

public: // Functions
	inline void OnUserStart() override {
		updateFPS = std::thread(&ColliderTest::UpdateText, this);

		font.LoadFont("Assets/Fonts/ARCADE.TTF", 30);
		fps.font = font;
		cameraLockState.font = font;

		camera.transform = new Transform();

		centerCollider = CreateCollider(Vector2::zero, 0, Vector2::one, 6);
	}

	inline void OnUserUpdate() override {
		// Camera controls
		if (Input::GetKeyDown(KeyCode::P)) {
			camera.transform->SetPosition(Vector2::zero);
			camera.transform->SetRotation(0);
		}

		if (Input::GetKeyDown(KeyCode::O)) {
			camera.transform->SetParent(nullptr);
		}

		if (Input::GetMouseButtonDown(MouseButton::Middle)) {
			lastMousePos = Input::GetMousePosition();
			panning = true;
		}
		else if (Input::GetMouseButtonUp(MouseButton::Middle)) {
			panning = false;
		}

		if (Input::GetMouseButtonDown(MouseButton::Right)) {
			lastMousePos = Input::GetMousePosition();
			rotating = true;
		}
		else if (Input::GetMouseButtonUp(MouseButton::Right)) {
			rotating = false;
		}

		if (Input::GetKeyDown(KeyCode::L)) {
			lockRotation = !lockRotation;
		}

		// Update camera properties
		if (panning) {
			moveDirection = camera.transform->InverseTransformDirection(lastMousePos - Input::GetMousePosition()) / camera.GetPixelsPerUnit();
			lastMousePos = Input::GetMousePosition();
		}
		if (rotating && !lockRotation) {
			targetAngle = Vector2::SignedAngle(Input::GetMousePosition().GetNormalized(), lastMousePos.GetNormalized());
			lastMousePos = Input::GetMousePosition();
		}

		camera.SetSize(camera.GetSize() - (camera.GetSize() * Input::GetMouseScroll().y * Time::GetDeltaTime() * zoomSpeed));

		camera.transform->Translate(moveDirection);
		camera.transform->Rotate(targetAngle * 2);
		camera.RecalculateViewMatrix();

		// Get mouse world position
		mouseWorldPos = camera.ScreenToWorldPoint(Input::GetMousePosition());
		projAxis = (mouseWorldPos - Vector2::zero).GetNormalized();

		// Generate colliders
		if (Input::GetMouseButtonDown(MouseButton::Left)) {
			staticBodies.push_back(CreateCollider(
				camera.ScreenToWorldPoint(Input::GetMousePosition()),
				Random::Range(0.0f, 360.0f),
				Vector2::one,
				Random::Range(3, 6)));

			overlapMouse.emplace_back();
			closest.emplace_back();
			projections.emplace_back();
			distances.emplace_back();
			movementTrace.emplace_back();
		}

		// Remove colliders
		if (Input::GetMouseButtonDown(MouseButton::Right)) {
			auto iter = std::remove_if(staticBodies.begin(), staticBodies.end(),
				[&](auto& _body) { return _body->OverlapPoint(mouseWorldPos); });

			staticBodies.erase(iter, staticBodies.end());
		}

		if (Input::GetKeyDown(KeyCode::C)) {
			staticBodies.clear();
			overlapMouse.clear();
			closest.clear();
			projections.clear();
			distances.clear();
			movementTrace.clear();
		}

		// Update test properties
		for (size_t i = 0; i < staticBodies.size(); i++) {
			closest[i] = staticBodies[i]->ClosetPoint(mouseWorldPos);
			projections[i] = staticBodies[i]->ProjectPolygon(*staticBodies[i], projAxis);
			distances[i] = staticBodies[i]->Distance(*centerCollider);
			overlapMouse[i] = distances[i].isOverlapped;

			// Overlap resolve
			if (std::abs(distances[i].distance) < Physics2D::contactOffset()) continue;

			if (distances[i].isOverlapped) {
				staticBodies[i]->transform->Translate(distances[i].distance * distances[i].normal);
				staticBodies[i]->RecalculateVertices();
			}
			else {
				movementTrace[i].push_back(staticBodies[i]->transform->GetPosition());
				staticBodies[i]->transform->Translate(distances[i].distance * distances[i].normal * Time::GetDeltaTime());
				staticBodies[i]->RecalculateVertices();
			}
		}
	}

	inline void OnDrawGizmos() override {
		Gizmos::DrawWorldGrid(camera);

#if TEST_DISTANCE
		// Display center collider
		Gizmos::SetColor(Color::yellow);
		Gizmos::DrawPolygonWorld(camera, centerCollider->vertices());
#endif

		// Display colliders
		for (size_t i = 0; i < staticBodies.size(); i++) {
			if (overlapMouse[i]) {
				Gizmos::SetColor(Color::red);
			}
			else {
				Gizmos::SetColor(Color::white);
			}
			Gizmos::DrawPolygonWorld(camera, staticBodies[i]->vertices());

#if TEST_CLOSEST
			Gizmos::SetColor(Color::white);
			Gizmos::DrawLineWorld(camera, mouseWorldPos, closest[i]);
			Gizmos::SetColor(Color::cyan);
			Gizmos::DrawRectWorld(camera, closest[i], Vector2(0.1f, 0.1f));
#endif
#if TEST_PROJECTION
			// Display polygon projection
			Vector2 minProj = projections[i].minDis * projAxis;
			Vector2 maxProj = projections[i].maxDis * projAxis;
			Gizmos::SetColor(Color::cyan);
			Gizmos::DrawLineWorld(camera, projections[i].minPoint, minProj);
			Gizmos::DrawLineWorld(camera, projections[i].maxPoint, maxProj);
			Gizmos::SetColor(Color::magenta);
			Gizmos::DrawRectWorld(camera, projections[i].minPoint, Vector2(0.1f, 0.1f));
			Gizmos::DrawRectWorld(camera, projections[i].maxPoint, Vector2(0.1f, 0.1f));
			Gizmos::DrawRectWorld(camera, minProj, Vector2(0.1f, 0.1f));
			Gizmos::DrawRectWorld(camera, maxProj, Vector2(0.1f, 0.1f));
#endif
#if TEST_DISTANCE
			if (distances[i].isOverlapped) {
				Gizmos::SetColor(Color::red);
			}
			else {
				Gizmos::SetColor(Color::green);
			}
			Gizmos::DrawLineWorld(camera, distances[i].pointA, distances[i].pointB);
			Gizmos::SetColor(Color::cyan);
			Gizmos::DrawRectWorld(camera, distances[i].pointA, Vector2(0.1f, 0.1f));
			Gizmos::SetColor(Color::magenta);
			Gizmos::DrawRectWorld(camera, distances[i].pointB, Vector2(0.15f, 0.15f));
#endif
#if TEST_MOVEMENT
			Gizmos::SetColor(Color::white);
			for (auto point : movementTrace[i]) {
				Gizmos::DrawRect(camera.WorldToScreenPoint(point), Vector2(2, 2));
			}
#endif
		}

		// Display projecting axis
#if TEST_PROJECTION
		Gizmos::SetColor(Color::yellow);
		Gizmos::DrawLineWorld(camera, projAxis * -100, projAxis * 100);
#endif

		// Display UI
		fps.Display(m_renderer, Vector2Int(30, 30), Color::yellow * 0.7f);
		cameraLockState.text = (std::string)"Camera Rotation Lock [L]: " + (lockRotation ? "True" : "False");
		cameraLockState.Display(m_renderer, Vector2Int(30, 50), Color::white);
	}

	inline void OnUserExit() override {
		updateFPS.join();
	}

private:
	[[nodiscard]] 
	inline std::unique_ptr<PolygonCollider2D> CreateCollider(Vector2 _position, float _rotation, Vector2 _scale, size_t _edges) const {
		auto collider = std::make_unique<PolygonCollider2D>(CreatePolygon(_edges, 2));
		collider->transform = new Transform(_position, _rotation, _scale);
		collider->RecalculateVertices();
		return collider;
	}

	inline std::vector<Vector2> CreatePolygon(size_t _corners, float _radius) const {
		float rotationIter = 360.0f / _corners;
		std::vector<Vector2> vertices = { Vector2(0, _radius) };
		for (size_t i = 1; i < _corners; i++) {
			vertices.push_back(Vector2(
				-1 * _radius * std::sinf(LMK_DtoR(rotationIter * i)),
				_radius * std::cosf(LMK_DtoR(rotationIter * i)))
			);
		}
		return vertices;
	}

	inline void UpdateText() {
		using namespace std::literals::chrono_literals;

		while (IsRunning())
		{
			fps.text = "FPS: " + std::to_string(Time::GetFrameRate());
			std::this_thread::sleep_for(0.15s);
		}
	}
};
#endif

#if false
#define TEST_MINKOWSKI 0
#define TEST_BOUND 0
class GJKTest : public LMKEngine {
private: // Properties
	// UI
	TTFont font;
	TextGUI fps;
	TextGUI cameraLockState;
	std::thread updateFPS;

	// Camera 
	Camera2D camera{ 6 };
	bool lockRotation = true;
	bool panning;
	bool rotating;
	Vector2 lastMousePos;
	Vector2 moveDirection;
	float targetAngle;
	float zoomSpeed = 50.0f;

	// Colliders
	std::unique_ptr<BoxCollider2D> controllerCollider;
	std::unique_ptr<PolygonCollider2D> polyCollider;
	bool resolving;

	ColliderDistance2D* gjkResult;

	// GJK
	std::vector<Vector2> supports;
	Vector2 direction;
	size_t iterations = 360;

	// Collider movement
	float rotateDirection;
	float rotationSpeed = 45;

public: // Functions
	inline void OnUserStart() override {
		updateFPS = std::thread(&GJKTest::UpdateText, this);

		font.LoadFont("Assets/Fonts/ARCADE.TTF", 30);
		fps.font = font;
		cameraLockState.font = font;

		camera.transform = new Transform();

		controllerCollider = std::make_unique<BoxCollider2D>();
		controllerCollider->transform = new Transform();
		controllerCollider->transform->name = "Box Collider";
		controllerCollider->transform->SetPosition(Vector2(0.5f, 1.5f));
		controllerCollider->RecalculateVertices();

		polyCollider = CreateCollider(Vector2(1.5f, 0.5f), 0, Vector2::one, 5);
		polyCollider->transform->name = "Poly Collider";
		polyCollider->RecalculateVertices();
	}

	inline void OnUserUpdate() override {
		// Camera controls
		if (Input::GetKeyDown(KeyCode::P)) {
			camera.transform->setPosition(Vector2::zero);
			camera.transform->setRotation(0);
		}

		if (Input::GetKeyDown(KeyCode::O)) {
			camera.transform->setParent(nullptr);
		}

		if (Input::GetMouseButtonDown(MouseButton::Middle)) {
			lastMousePos = Input::GetMousePosition();
			panning = true;
		}
		else if (Input::GetMouseButtonUp(MouseButton::Middle)) {
			panning = false;
		}

		if (Input::GetMouseButtonDown(MouseButton::Right)) {
			lastMousePos = Input::GetMousePosition();
			rotating = true;
		}
		else if (Input::GetMouseButtonUp(MouseButton::Right)) {
			rotating = false;
		}

		if (Input::GetKeyDown(KeyCode::L)) {
			lockRotation = !lockRotation;
		}

		// Update camera properties
		if (panning) {
			moveDirection = camera.transform->InverseTransformDirection(lastMousePos - Input::GetMousePosition()) / camera.pixelsPerUnit();
			lastMousePos = Input::GetMousePosition();
		}
		if (rotating && !lockRotation) {
			targetAngle = Vector2::SignedAngle(Input::GetMousePosition().normalized(), lastMousePos.normalized());
			lastMousePos = Input::GetMousePosition();
		}

		camera.setSize(camera.size() - (camera.size() * Input::GetMouseScroll().y * Time::GetDeltaTime() * zoomSpeed));

		camera.transform->setLocalPosition(camera.transform->localPosition() + moveDirection);
		camera.transform->setLocalRotation(camera.transform->localRotation() + targetAngle * 2);
		camera.RecalculateViewMatrix();

		// Update box transform
		rotateDirection = 0;
		if (Input::GetKey(KeyCode::E)) {
			rotateDirection += -1;
		}
		if (Input::GetKey(KeyCode::Q)) {
			rotateDirection += 1;
		}

		if (Input::GetKeyDown(KeyCode::R)) {
			resolving = !resolving;
		}

		controllerCollider->transform->SetPosition(camera.ScreenToWorldPoint(Input::GetMousePosition()));
		controllerCollider->transform->Rotate(rotateDirection * rotationSpeed * Time::GetDeltaTime());
		controllerCollider->RecalculateVertices();

		// GJK result
		gjkResult = polyCollider->Distance(*controllerCollider);

		if (resolving) {
			polyCollider->transform->Translate((gjkResult->distance + Physics2D::contactOffset()) * gjkResult->normal);
			polyCollider->RecalculateVertices();
		}

#if TEST_MINKOWSKI
		// Get minkowski difference.
		supports.clear();
		float rotationIter = 360.0f / iterations;
		for (size_t i = 0; i < iterations; i++) {
			direction = Vector2(
				-1 * std::sinf(LMK_DtoR(rotationIter * i)),
				std::cosf(LMK_DtoR(rotationIter * i))
			);

			Vector2 support = Collider2D::Support(*polyCollider, *controllerCollider, direction);
			if (std::find(supports.begin(), supports.end(), support) == supports.end()) {
				supports.push_back(support);
			}
		}
#endif
	}

	inline void OnDrawGizmos() override {
		Gizmos::DrawWorldGrid(camera);

		// Display colliders
		Gizmos::setColor(Color::yellow);
		Gizmos::DrawPolygonWorld(camera, polyCollider->vertices());

		if (gjkResult.isOverlapped) {
			Gizmos::setColor(Color::red);
		}
		else {
			Gizmos::setColor(Color::white);
		}
		Gizmos::DrawPolygonWorld(camera, controllerCollider->vertices());
#if TEST_BOUND
		Gizmos::setColor(Color::green / 2);
		Gizmos::DrawRectWorld(camera, controllerCollider->bounds().center(), controllerCollider->bounds().extents());
#endif
#if TEST_MINKOWSKI
		// Display minkowski difference
		Gizmos::setColor(Color::cyan);
		size_t n = supports.size();
		for (size_t i = 0, j = n - 1; i < n; j = i++) {
			Gizmos::DrawLineWorld(camera, supports[j], supports[i]);
		}
#endif

		Gizmos::DrawCameraOrigin(camera);

		// Display UI
		fps.Display(m_renderer, Vector2Int(30, 30), Color::yellow * 0.7f);
		cameraLockState.text = (std::string)"Camera Rotation Lock [L]: " + (lockRotation ? "True" : "False");
		cameraLockState.Display(m_renderer, Vector2Int(30, 50), Color::white);
	}

	inline void OnUserExit() override {
		updateFPS.join();
	}

private:
	[[nodiscard]] inline std::unique_ptr<PolygonCollider2D> CreateCollider(Vector2 _position, float _rotation, Vector2 _scale, size_t _edges) const {
		auto collider = std::make_unique<PolygonCollider2D>(CreatePolygon(_edges, 0.5));
		collider->transform = new Transform(_position, _rotation, _scale);
		collider->RecalculateVertices();
		return collider;
	}

	inline std::vector<Vector2> CreatePolygon(size_t _corners, float _radius) const {
		float rotationIter = 360.0f / _corners;
		std::vector<Vector2> vertices = { Vector2(0, _radius) };
		for (size_t i = 1; i < _corners; i++) {
			vertices.push_back(Vector2(
				-1 * _radius * std::sinf(LMK_DtoR(rotationIter * i)),
				_radius * std::cosf(LMK_DtoR(rotationIter * i)))
			);
		}
		return vertices;
	}

	inline void UpdateText() {
		using namespace std::literals::chrono_literals;

		while (isRunning())
		{
			fps.text = "FPS: " + std::to_string(Time::GetFrameRate());
			std::this_thread::sleep_for(0.15s);
		}
	}
};
#endif

#if true
#include "Examples/flatctrl.h"

class CameraTest : public LMKEngine {
private:
	std::shared_ptr<Scene> scene;

	TTFont font;
	TextGUI text;
	std::thread updateFPS;

	// Sprite Renderer
	std::vector<Sprite*> sprites;
	std::vector<Transform*> transforms;

	Vector2 rendererScaleValue;
	float scaleSpeed = 5;
	float rendererRotateDirection;
	float rotateSpeed = 90;

	// Camera 
	Camera2D* camera;
	Vector2 moveDirection;
	float targetAngle;
	float zoomSpeed = 400; // Pixels
	Vector2 lastMousePos;

	float maxScale;
	float minScale;

	bool panning;
	bool rotating;

public:
	inline void OnUserStart() override {
		updateFPS = std::thread(&CameraTest::UpdateFPSText, this);

		font.LoadFont("Assets/Fonts/visitor2.ttf", 40);
		text.font = font;

		scene = std::make_shared<Scene>();

		// Setup camera
		auto camGO = scene->CreateGameObject("Main Camera");
		camGO->AddComponent<FlatControlBehaviour>();
		camera = camGO->AddComponent<Camera2D>();
		camera->backgroundColor = Color(120, 120, 120, 255);

		// Setup renderer
		Texture2D* texture;
		Sprite* sprite;

		texture = new Texture2D;
		texture->LoadRawTextureData("Assets/Images/Koopa.png");
		sprite = new Sprite;
		sprite->InitSprite(*texture, RectInt{ Vector2Int::zero, Vector2Int(32, 30) });
		sprite->SetPixelsPerUnit(30);
		sprites.push_back(sprite);

		sprite = new Sprite;
		sprite->InitSprite(*texture, RectInt{ Vector2Int(33, 0), Vector2Int(32, 30)});
		sprite->SetPixelsPerUnit(30);
		sprites.push_back(sprite);

		texture = new Texture2D;
		texture->LoadRawTextureData("Assets/Images/Mario.png");
		sprite = new Sprite;
		sprite->InitSprite(*texture, RectInt{ Vector2Int::zero, Vector2Int(31, 41) });
		sprite->SetPixelsPerUnit(30);
		sprites.push_back(sprite);

		texture = new Texture2D;
		texture->LoadRawTextureData("Assets/Images/Luigi.png");
		sprite = new Sprite;
		sprite->InitSprite(*texture, RectInt{ Vector2Int::zero, Vector2Int(31, 41) });
		sprite->SetPixelsPerUnit(30);
		sprites.push_back(sprite);
	}

	inline void OnUserUpdate() override {
		// Screen controls
		if (Input::GetKeyDown(KeyCode::F11)) {
			if (Screen::IsFullScreen()) {
				Screen::SetFullScreenMode(FullScreenMode::Windowed);
			}
			else {
				Screen::SetFullScreenMode(FullScreenMode::FullScreen);
			}
		}

		camera->SetSize(camera->GetSize() - Input::GetMouseScroll().y * Time::GetDeltaTime() * zoomSpeed);
		camera->RecalculateViewMatrix();

		if (Input::GetMouseButtonDown(MouseButton::Left)) {
			SpawnSpriteRenderer(camera->ScreenToWorldPoint(Input::GetMousePosition()));
		}
	}

	inline void OnDrawGizmos() override {
		Gizmos::DrawWorldGrid(*camera, 0.4f);

		// Render UI
		text.Display(m_renderer, Vector2Int(50, 50), Color::yellow);
	}

	inline void OnUserExit() override {
		updateFPS.join();
	}

private:
	inline void SpawnSpriteRenderer(Vector2 _worldPos) {
		auto go = scene->CreateGameObject("Renderer");
		go->transform->SetPosition(_worldPos);
		go->transform->SetRotation(Random::Range(0, 360));
		go->transform->SetLossyScale(Vector2::one * Random::Range(1.f, 2.f));
		auto renderer = go->AddComponent<SpriteRenderer>();
		renderer->SetSprite(*sprites.at(Random::Range((size_t)0, sprites.size())));
	}

	inline void UpdateFPSText() {
		using namespace std::literals::chrono_literals;

		while (IsRunning()) {
			text.text = std::to_string(Time::GetFrameRate());
			std::this_thread::sleep_for(0.1s);
		}
	}
};
#endif

#if false
class RenderTest : public LMKEngine {
private:
	Scene scene;

	TTFont font;
	TextGUI text;

	// Camera 
	Camera2D camera{ 10 };
	bool lockRotation = true;
	bool panning;
	bool rotating;
	Vector2 moveDirection;
	float targetAngle;
	float zoomSpeed = 300;

	// Sprite Renderer
	Texture2D texture;
	Sprite spriteA;
	Sprite spriteB;
	SpriteRenderer* rendererA;
	SpriteRenderer* rendererB;

	// Controls
	Vector2 lastMousePos;

	bool usingSpriteA = true;

	float scaleSpeed = 5;
	float moveSpeed = 10;
	float rotateSpeed = 45;

public:
	inline void OnUserStart() override {
		font.LoadFont("Assets/Fonts/visitor2.ttf", 40);
		text.font = font;	
		text.text = "What?";

		// Setup camera
		camera.transform = new Transform();
		camera.RecalculateAspect();

		// Setup texture
		texture.LoadRawTextureData("Assets/Images/NES - Mario Bros - General Sprites.png");

		spriteA.InitSprite(texture, RectInt{ Vector2Int(4, 603), Vector2Int(16, 21)});
		spriteA.setPivot(Vector2(0.1f, 0.0f));
		spriteA.setPixelsPerUnit(21);
		spriteB.InitSprite(texture, RectInt{ Vector2Int(23, 603), Vector2Int(12, 21) });
		spriteB.setPixelsPerUnit(21);

		// Setup renderers
		auto& objectA = scene.CreateGameObject("Renderer A");
		auto& objectB = scene.CreateGameObject("Renderer B");

		objectA.transform->setPosition(Vector2(1, 1));
		rendererA = objectA.AddComponent<SpriteRenderer>();
		rendererA->setSprite(spriteA);

		objectB.transform->setPosition(Vector2(2, 1));
		rendererB = objectB.AddComponent<SpriteRenderer>();
		rendererB->setSprite(spriteB);

		camera.transform->setParent(rendererA->transform);
	}

	inline void OnUserUpdate() override {
		if (Input::GetKeyDown(KeyCode::F11)) {
			std::cout << Screen::width() << " : " << Screen::height() << "\n";

			if (Screen::fullScreen()) {
				Screen::SetFullScreenMode(FullScreenMode::Windowed);
			}
			else {
				Screen::SetFullScreenMode(FullScreenMode::FullScreen);
			}
		}

		// Camera controls
		if (Input::GetKeyDown(KeyCode::P)) {
			camera.transform->setLocalPosition(Vector2::zero);
			camera.transform->setLocalRotation(0);
		}

		if (Input::GetKeyDown(KeyCode::O)) {
			camera.transform->setParent(nullptr);
		}

		if (Input::GetMouseButtonDown(MouseButton::Left)) {
			lastMousePos = Input::GetMousePosition();
			panning = true;
		}
		else if (Input::GetMouseButtonUp(MouseButton::Left)) {
			panning = false;
		}

		if (Input::GetMouseButtonDown(MouseButton::Right)) {
			lastMousePos = Input::GetMousePosition();
			rotating = true;
		}
		else if (Input::GetMouseButtonUp(MouseButton::Right)) {
			rotating = false;
		}

		// Update camera properties
		if (panning) {
			moveDirection = camera.transform->InverseTransformDirection(lastMousePos - Input::GetMousePosition()) / camera.pixelsPerUnit();
			lastMousePos = Input::GetMousePosition();
		}
		if (rotating) {
			targetAngle = Vector2::SignedAngle(Input::GetMousePosition().normalized(), lastMousePos.normalized());
			lastMousePos = Input::GetMousePosition();
		}

		camera.setSize(camera.size() - Input::mouseScroll().y * Time::deltaTime() * zoomSpeed);
		camera.transform->Translate(moveDirection);
		camera.transform->Rotate(targetAngle * 2);
		camera.RecalculateViewMatrix();

		// Renderers controls
		if (Input::GetKey(KeyCode::E)) {
			rendererA->transform->Rotate(-rotateSpeed * Time::deltaTime());
		}
		if (Input::GetKey(KeyCode::Q)) {
			rendererA->transform->Rotate(rotateSpeed * Time::deltaTime());
		}

		if (Input::GetKey(KeyCode::D)) {
			rendererA->transform->AddScale(Vector2::right * Time::deltaTime());
		}
		if (Input::GetKey(KeyCode::A)) {
			rendererA->transform->AddScale(Vector2::left * Time::deltaTime());
		}

		if (Input::GetKey(KeyCode::W)) {
			rendererA->transform->AddScale(Vector2::up * Time::deltaTime());
		}
		if (Input::GetKey(KeyCode::S)) {
			rendererA->transform->AddScale(Vector2::down * Time::deltaTime());
		}

		if (Input::GetKeyDown(KeyCode::R)) {
			rendererA->setSprite(usingSpriteA ? spriteB : spriteA);
			usingSpriteA = !usingSpriteA;
		}

		if (Input::GetKeyDown(KeyCode::T)) {
			rendererA->setSortingLayerID(rendererA->sortingLayerID() + 1);
		}
		if (Input::GetKeyDown(KeyCode::Y)) {
			rendererB->setSortingLayerID(rendererB->sortingLayerID() + 1);
		}

		if (Input::GetKeyDown(KeyCode::LeftArrow) || Input::GetKeyDown(KeyCode::RightArrow)) {
			rendererA->flipX = !rendererA->flipX;
		}
		if (Input::GetKeyDown(KeyCode::DownArrow) || Input::GetKeyDown(KeyCode::UpArrow)) {
			rendererA->flipY = !rendererA->flipY;
		}
	}

	inline void OnDrawGizmos() override {
		Gizmos::DrawWorldGrid(camera);

		Gizmos::setColor(Color::green);
		Gizmos::DrawRectWorld(camera, rendererA->transform->position(), Vector2(0.05f, 0.05f));
		Gizmos::DrawLineWorld(camera, rendererA->transform->position(), rendererA->transform->position() + rendererA->transform->up());

		Gizmos::DrawCameraOrigin(camera);

		text.Display(m_renderer, Vector2Int(50, 50), Color::yellow);
	}
};



class TransformTest : public LMKEngine {
private:
	TTFont worldFont;
	TTFont font;
	TextGUI worldText;
	TextGUI text;

	// Camera 
	Camera2D camera{ 10 };
	Vector2 lastMousePos;
	bool lockRotation = true;
	bool panning;
	bool rotating;
	Vector2 moveDirection;
	float targetAngle;
	float zoomSpeed = 300;

	// Game Objects
	Scene firstScene;
	GameObject root{ &firstScene, "Root" };
	GameObject branchA{ &firstScene, "Branch A" };
	GameObject branchB{ &firstScene, "Branch B" };
	GameObject leafA1{ &firstScene, "Leaf A 1" };
	GameObject leafA2{ &firstScene, "Leaf A 2" };
	Transform* currTransform;
	int curTransformSiblingIndex;

	// Controls
	float rotateSpeed = 45;
	float moveSpeed = 2;
	float scaleSpeed = 1;

public:
	inline void OnUserStart() override {
		font.LoadFont("Assets/Fonts/joystix monospace.otf", 15);
		worldFont.LoadFont("Assets/Fonts/visitor2.ttf", 24);
		text.font = font;
		worldText.font = worldFont;

		// Setup camera
		camera.transform = new Transform();
		camera.RecalculateAspect();
		camera.backgroundColor = Color{30, 30, 30, 255};

		// Setup hierarchy
		branchA.transform->setParent(root.transform);
		branchB.transform->setParent(root.transform);
		leafA1.transform->setParent(branchA.transform);
		leafA2.transform->setParent(branchA.transform);

		currTransform = root.transform;
	}

	inline void OnUserUpdate() override {
		UpdateCameraControl(camera);

		if (Input::GetKeyDown(KeyCode::F11)) {
			std::cout << Screen::width() << " : " << Screen::height() << "\n";

			if (Screen::fullScreen()) {
				Screen::SetFullScreenMode(FullScreenMode::Windowed);
			}
			else {
				Screen::SetFullScreenMode(FullScreenMode::FullScreen);
			}
		}

		// Transform controls
		if (Input::GetKey(KeyCode::Q)) {
			currTransform->Rotate(rotateSpeed * Time::deltaTime());
		}
		if (Input::GetKey(KeyCode::E)) {
			currTransform->Rotate(-rotateSpeed * Time::deltaTime());
		}

		if (Input::GetKey(KeyCode::W)) {
			currTransform->Translate(Vector2::up * moveSpeed * Time::deltaTime());
		}
		if (Input::GetKey(KeyCode::S)) {
			currTransform->Translate(Vector2::down * moveSpeed * Time::deltaTime());
		}
		if (Input::GetKey(KeyCode::D)) {
			currTransform->Translate(Vector2::right * moveSpeed * Time::deltaTime());
		}
		if (Input::GetKey(KeyCode::A)) {
			currTransform->Translate(Vector2::left * moveSpeed * Time::deltaTime());
		}

		if (Input::GetKey(KeyCode::R)) {
			currTransform->AddScale(Vector2::one * scaleSpeed * Time::deltaTime());
		}
		if (Input::GetKey(KeyCode::F)) {
			currTransform->AddScale(Vector2::one * -scaleSpeed * Time::deltaTime());
		}

		if (Input::GetKeyDown(KeyCode::T)) {
			currTransform->setLocalPosition(Vector2::zero);
			currTransform->setLocalRotation(0);
			currTransform->setLocalScale(Vector2::one);
		}

		if (Input::GetKeyDown(KeyCode::UpArrow)) {
			if (currTransform->parent() != nullptr) {
				currTransform = currTransform->parent();
			}
		}
		if (Input::GetKeyDown(KeyCode::DownArrow)) {
			if (currTransform->childCount() > 0) {
				currTransform = currTransform->GetChild(0);
				curTransformSiblingIndex = 0;
			}
		}
		if (Input::GetKeyDown(KeyCode::RightArrow)) {
			if (currTransform->parent() != nullptr) {
				curTransformSiblingIndex = (curTransformSiblingIndex + 1) % currTransform->parent()->childCount();
				currTransform = currTransform->parent()->GetChild(curTransformSiblingIndex);
			}
		}
		if (Input::GetKeyDown(KeyCode::LeftArrow)) {
			if (currTransform->parent() != nullptr) {
				curTransformSiblingIndex = (curTransformSiblingIndex + 1) % currTransform->parent()->childCount();
				currTransform = currTransform->parent()->GetChild(curTransformSiblingIndex);
			}
		}
	}

	inline void OnDrawGizmos() override {
		Gizmos::DrawWorldGrid(camera);

		// Display transform
		DrawTransform(camera, *root.transform, Color::white);
		DrawTransform(camera, *branchA.transform, Color::green);
		DrawTransform(camera, *branchB.transform, Color::yellow);
		DrawTransform(camera, *leafA1.transform, Color::cyan);
		DrawTransform(camera, *leafA2.transform, Color::magenta);

		// Display current transform indicator
		Gizmos::setColor(Color::grey);
		Vector2 oneWorldPoint = currTransform->TransformPoint(Vector2::one);
		Gizmos::DrawLineWorld(camera, currTransform->position(), oneWorldPoint);
		Gizmos::DrawRectWorld(camera, oneWorldPoint, Vector2(0.1f, 0.1f));
		worldText.text = currTransform->gameObject->name;
		worldText.Display(m_renderer, camera.WorldToScreenPoint(oneWorldPoint), Color::white);

		// Display UI
		Gizmos::DrawCameraOrigin(camera);

		// Display transform info
		text.text = "Name:  " + currTransform->gameObject->name;
		text.Display(m_renderer, Vector2Int(20, 40), Color::yellow);
		text.text = "World Position:  " + std::string(currTransform->position());
		text.Display(m_renderer, Vector2Int(20, 90), Color::white);
		text.text = "Position:  " + std::string(currTransform->localPosition());
		text.Display(m_renderer, Vector2Int(20, 120), Color::white);
		text.text = "Rotation:  " + std::to_string(currTransform->localRotation());
		text.Display(m_renderer, Vector2Int(20, 140), Color::white);
		text.text = "Scale:  " + std::string(currTransform->localScale());
		text.Display(m_renderer, Vector2Int(20, 160), Color::white);
	}

private:
	inline void DrawTransform(const Camera2D& _camera, const Transform& _transform, Color _color) {
		// World
		Gizmos::setColor(_color);
		Gizmos::DrawLineWorld(_camera, _transform.position(), _transform.position() + _transform.up());
		Gizmos::DrawRectWorld(_camera, _transform.position(), Vector2::Scale(Vector2(0.1f, 0.1f), _transform.lossyScale()));
		// Local
		Gizmos::setColor(_color / 2);
		Gizmos::DrawRectWorld(_camera, _transform.position(), Vector2::Scale(Vector2(0.1f, 0.1f), _transform.localScale()));
	}

	inline void UpdateCameraControl(Camera2D& _camera) {
		// Camera controls
		if (Input::GetKeyDown(KeyCode::P)) {
			_camera.transform->setPosition(currTransform->position());
			_camera.transform->setRotation(0);
		}

		if (Input::GetKeyDown(KeyCode::O)) {
			_camera.transform->setParent(nullptr);
		}

		if (Input::GetMouseButtonDown(MouseButton::Left)) {
			lastMousePos = Input::GetMousePosition();
			panning = true;
		}
		else if (Input::GetMouseButtonUp(MouseButton::Left)) {
			panning = false;
		}

		if (Input::GetMouseButtonDown(MouseButton::Right)) {
			lastMousePos = Input::GetMousePosition();
			rotating = true;
		}
		else if (Input::GetMouseButtonUp(MouseButton::Right)) {
			rotating = false;
		}

		// Update camera properties
		if (panning) {
			moveDirection = _camera.transform->InverseTransformDirection((lastMousePos - Input::GetMousePosition()) / _camera.pixelsPerUnit());
			lastMousePos = Input::GetMousePosition();
		}
		if (rotating) {
			targetAngle = Vector2::SignedAngle(Input::GetMousePosition().normalized(), lastMousePos.normalized());
			lastMousePos = Input::GetMousePosition();
		}

		_camera.setSize(_camera.size() - Input::mouseScroll().y * Time::deltaTime() * zoomSpeed);
		_camera.transform->Translate(_camera.transform->TransformDirection(moveDirection));
		_camera.transform->Rotate(targetAngle * 2);
		_camera.RecalculateViewMatrix();
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
		Gizmos::setColor(Color::green);
	}

	inline void OnUserExit() override {

	}
};



class NativeMemoryTest : public LMKEngine {

};



class SceneTest : public LMKEngine {
private:


private:

};



class SoundTest : public LMKEngine {
private:
	AudioClip clip;

public:
	inline void OnUserStart() override {
		clip.LoadAudioData("Assets/Audio/SFX/Jump.mp3");
	}

	inline void OnUserUpdate() override {
		if (Input::GetKeyDown(KeyCode::Space))
			AudioManager::PlayOneShot(clip);
	}
};
#endif

#if false
#define DEBUG_GIZMOS 0
class MarioTest : public LMKEngine {
public:
	~MarioTest() = default;

private:
	// Resource Setup
	std::thread updateFPS;
	TTFont font;
	TextGUI text;

	AudioClip jumpClip;
	AudioClip backgroundMusic;

	// Scene Setup
	Scene level1 { "Scene 1" };

	// Components
	Camera2D* camera;
	SpriteRenderer* playerRenderer;
	BoxCollider2D* playerColldier;
	SpriteRenderer* groundRenderer;
	BoxCollider2D* groundColldier;
	SpriteRenderer* groundRenderer2;
	BoxCollider2D* groundColldier2;
	BoxCollider2D* groundCheckCollider;

	// Textures
	Texture2D texture;
	Sprite playerSprite;
	Sprite groundSprite;

	// Player controls
	bool jumped;
	bool wasInAir;
	int horizontalDirection = 1;
	Vector2 velocity;
	float moveSpeedAccel = 10;
	float maxMoveSpeed = 7;
	float jumpPower = 12;
	float maxFallSpeed = -10;

	// Camera controls
	bool panning;
	bool rotating;
	Vector2 moveDirection;
	float targetAngle;
	float zoomSpeed = 300;
	Vector2 lastMousePos;

public:
	inline void OnUserStart() override {
		updateFPS = std::thread(&MarioTest::UpdateFPSText, this);

		font.LoadFont("Assets/Fonts/visitor2.ttf", 40);
		text.font = font;

		jumpClip.LoadAudioData("Assets/Audio/SFX/Jump.mp3");
		backgroundMusic.LoadAudioData("Assets/Audio/Music/Mario.mp3");

		// Setup textures.
		texture.LoadRawTextureData("Assets/Images/NES - Mario Bros - General Sprites.png");

		playerSprite.InitSprite(texture, RectInt{ Vector2Int(4, 603), Vector2Int(16, 21) });
		playerSprite.SetPivot(Vector2(0.5f, 0.0f));
		playerSprite.SetPixelsPerUnit(21);

		groundSprite.InitSprite(texture, RectInt{ Vector2Int(5, 293), Vector2Int(256, 15) });
		groundSprite.SetPivot(Vector2(0.5f, 1.0f));
		groundSprite.SetPixelsPerUnit(15);

		// Setup scene.
		auto& mainCamera = level1.CreateGameObject("MainCamera");
		auto& player = level1.CreateGameObject("Player");
		auto& ground = level1.CreateGameObject("Ground");
		auto& ground2 = level1.CreateGameObject("Ground");
		auto& groundCheck = level1.CreateGameObject("GroundCheck");

		// Setup camera object.
		camera = mainCamera.AddComponent<Camera2D>();

		mainCamera.transform->setParent(player.transform);
		mainCamera.transform->setLocalPosition(Vector2(0, 4));

		camera->setSize(8);
#if DEBUG_GIZMOS
		camera->backgroundColor = Color(23, 23, 23, 255);
#else
		camera->backgroundColor = Color(173, 208, 249, 255);
#endif

		// Setup player object.
		playerRenderer = player.AddComponent<SpriteRenderer>();
		playerColldier = player.AddComponent<BoxCollider2D>();

		player.transform->setPosition(Vector2(0, 2));
		playerRenderer->setSprite(playerSprite);

		playerColldier->offset = Vector2(0.0f, 0.5f);
		playerColldier->scale = Vector2(0.8f, 1.0f);
		playerColldier->RecalculateVertices();

		// Setup ground object.
		groundRenderer = ground.AddComponent<SpriteRenderer>();
		groundColldier = ground.AddComponent<BoxCollider2D>();

		groundRenderer->setSprite(groundSprite);

		groundColldier->offset = Vector2(0.0f, -0.5f);
		groundColldier->scale = Vector2(17, 1);
		groundColldier->RecalculateVertices();

		// Setup ground2 object.
		groundRenderer2 = ground2.AddComponent<SpriteRenderer>();
		groundColldier2 = ground2.AddComponent<BoxCollider2D>();

		ground2.transform->setPosition(Vector2(10, 3));

		groundRenderer2->setSprite(groundSprite);

		groundColldier2->offset = Vector2(0.0f, -0.5f);
		groundColldier2->scale = Vector2(17, 1);
		groundColldier2->RecalculateVertices();

		// Setup groundCheck object.
		groundCheckCollider = groundCheck.AddComponent<BoxCollider2D>();

		groundCheck.transform->setParent(player.transform);
		groundCheck.transform->setLocalPosition(Vector2::zero);

		groundCheckCollider->scale = Vector2(0.8f, 0.15f);
		groundCheckCollider->RecalculateVertices();

		// Audio setup
		AudioManager::PlayLooped(backgroundMusic);
	}

	inline void OnUserUpdate() override {
		CameraControls(*camera);

		// Player controls
		horizontalDirection = 0;
		if (Input::GetKey(KeyCode::A) || Input::GetKey(KeyCode::LeftArrow)) {
			horizontalDirection += -1;
		}
		if (Input::GetKey(KeyCode::D) || Input::GetKey(KeyCode::RightArrow)) {
			horizontalDirection += 1;
		}

		jumped = false;
		if (Input::GetKeyDown(KeyCode::W) || Input::GetKey(KeyCode::UpArrow)) {
			if (IsGrounded()) jumped = true;
		}

		// Update velocity
		float targetSpeed = horizontalDirection * maxMoveSpeed;
		velocity.x = Mathf::MoveTowards(velocity.x, targetSpeed, moveSpeedAccel * Time::GetDeltaTime());
		if (jumped) {
			velocity.y = jumpPower;
			AudioManager::PlayOneShot(jumpClip, 0.7f);
		}

		if (horizontalDirection < 0) {
			playerRenderer->flipX = false;
		}
		else if (horizontalDirection > 0) {
			playerRenderer->flipX = true;
		}

		playerColldier->transform->Translate(velocity * Time::GetDeltaTime());
		playerColldier->RecalculateVertices();
		groundCheckCollider->RecalculateVertices();

		// Collision resolve
		auto result = playerColldier->Distance(*groundColldier);
		if (result.isOverlapped) {
			playerColldier->transform->Translate(result.distance * result.normal);
		}

		result = playerColldier->Distance(*groundColldier2);
		if (result.isOverlapped) {
			playerColldier->transform->Translate(result.distance * result.normal);
		}

		if (!IsGrounded()) {
			UpdateGravity();
			wasInAir = true;
		}
		else if (wasInAir) {
			wasInAir = false;
			velocity.y = -1.5f;
		}
	}

	inline void OnDrawGizmos() override {
#if DEBUG_GIZMOS
		Gizmos::DrawWorldGrid(*camera);
#endif

#if DEBUG_GIZMOS
		Gizmos::setColor(Color::green * 0.75f);
		Gizmos::DrawPolygonWorld(*camera, playerColldier->vertices());
		Gizmos::DrawPolygonWorld(*camera, groundColldier->vertices());
		Gizmos::DrawPolygonWorld(*camera, groundColldier2->vertices());
		Gizmos::DrawPolygonWorld(*camera, groundCheckCollider->vertices());
#endif

		text.Display(m_renderer, Vector2(20, Screen::height() - 20), Color::white);
	}

	inline void OnUserExit() override {
		updateFPS.join();
	}

private:
	inline void UpdateGravity() {
		velocity.y += Physics2D::gravity().y * Time::GetDeltaTime();
	}

	inline bool IsGrounded() {
		bool result = groundCheckCollider->OverlapCollider(*groundColldier);
		result |= groundCheckCollider->OverlapCollider(*groundColldier2);
		return result;
	}

	inline void UpdateFPSText() {
		using namespace std::literals::chrono_literals;

		while (isRunning()) {
			text.text = std::to_string(Time::GetFrameRate());
			std::this_thread::sleep_for(0.1s);
		}
	}

	inline void CreatePlatform() {

	}

	inline void CameraControls(Camera2D& _camera) {
		// Screen controls
		if (Input::GetKeyDown(KeyCode::F11)) {
			if (Screen::fullScreen()) {
				Screen::SetFullScreenMode(FullScreenMode::Windowed);
			}
			else {
				Screen::SetFullScreenMode(FullScreenMode::FullScreen);
			}
		}

		// Camera controls
		if (Input::GetKeyDown(KeyCode::P)) {
			_camera.transform->setLocalPosition(Vector2::zero);
			_camera.transform->setLocalRotation(0);
		}

		if (Input::GetKeyDown(KeyCode::O)) {
			_camera.transform->setParent(nullptr);
		}

		if (Input::GetMouseButtonDown(MouseButton::Left)) {
			lastMousePos = Input::GetMousePosition();
			panning = true;
		}
		else if (Input::GetMouseButtonUp(MouseButton::Left)) {
			panning = false;
		}

		if (Input::GetMouseButtonDown(MouseButton::Right)) {
			lastMousePos = Input::GetMousePosition();
			rotating = true;
		}
		else if (Input::GetMouseButtonUp(MouseButton::Right)) {
			rotating = false;
		}

		// Update camera properties
		if (panning) {
			moveDirection = _camera.transform->InverseTransformDirection(lastMousePos - Input::GetMousePosition()) / _camera.pixelsPerUnit();
			lastMousePos = Input::GetMousePosition();
		}
		if (rotating) {
			targetAngle = Vector2::SignedAngle(Input::GetMousePosition().normalized(), lastMousePos.normalized());
			lastMousePos = Input::GetMousePosition();
		}

		_camera.transform->Translate(moveDirection);
		_camera.transform->Rotate(targetAngle * 2);
		_camera.setSize(camera->size() - Input::mouseScroll().y * Time::GetDeltaTime() * zoomSpeed);
		_camera.RecalculateViewMatrix();
	}
};
#pragma warning(default : 4244)
#endif

#if false
class PolygonDecompositionTest : public LMKEngine {
private:
	std::shared_ptr<Scene> scene;

	Camera2D* camera;

	PolygonCollider2D* collider;

public:
	inline void OnUserStart() override {
		scene = std::make_shared<Scene>();

		auto goCam = scene->CreateGameObject("Main Camera");
		camera = goCam->AddComponent<Camera2D>();
		camera->backgroundColor = Color::grey;

		auto go = scene->CreateGameObject("Polygon");
		collider = go->AddComponent<PolygonCollider2D>();
	}

	inline void OnUserUpdate() override {

	}

	inline void OnUserFixedUpdate() override {
		
	}

	inline void OnDrawGizmos() override {

	}
};
#endif

#if true
#include "Examples/flatctrl.h"
#include "Examples/mousectrl.h"

#include <format>

class PhysicsResponseTest : public LMKEngine {
private:
	std::shared_ptr<Scene> scene;

	Camera2D* camera;

	PolygonCollider2D* colliderA;
	Rigidbody2D* bodyA;

	BoxCollider2D* colliderB;
	Rigidbody2D* bodyB;

	std::shared_ptr<Physics2D::ContactManifold> manifold;

	bool applyingImpulse;
	Vector2 startPosition;

public:
	inline void OnUserStart() override {
		scene = std::make_shared<Scene>();

		auto goCam = scene->CreateGameObject("Main Camera");
		camera = goCam->AddComponent<Camera2D>();
		camera->backgroundColor = Color(25, 25, 25, 1);

		auto goA = scene->CreateGameObject("Polygon A");
		goA->transform->SetPosition(Vector2(0, 2));
		goA->AddComponent<FlatControlBehaviour>();
		colliderA = goA->AddComponent<PolygonCollider2D>();
		colliderA->AddPath(CreatePolygon(7, 0.8f));
		bodyA = goA->AddComponent<Rigidbody2D>();
		bodyA->RegisterCollider(colliderA);
		bodyA->gravityScale = 0;
		bodyA->angularDamping = 0.2f;
		bodyA->linearDamping = 0.2f;

		auto goB = scene->CreateGameObject("Polygon B");
		colliderB = goB->AddComponent<BoxCollider2D>();
		bodyB = goB->AddComponent<Rigidbody2D>();
		bodyB->RegisterCollider(colliderB);
		bodyB->bodyType = RigidbodyType2D::Static;
	}

	inline void OnUserUpdate() override {
		manifold = Physics2D::CollisionDetection_GJK(*colliderA, *colliderB, 1, 2);

		if (Input::GetMouseButtonDown(MouseButton::Left)) {
			if (applyingImpulse) {
				applyingImpulse = false;

				bodyA->ApplyImpulse(camera->ScreenToWorldPoint(Input::GetMousePosition()) - startPosition, startPosition);
			}
			else {
				applyingImpulse = true;
				startPosition = camera->ScreenToWorldPoint(Input::GetMousePosition());
			}
		}

		if (Input::GetKeyDown(KeyCode::Space)) {
			bodyA->linearVelocity = Vector2::zero;
		}
	}

	inline void OnDrawGizmos() override {
		Gizmos::DrawWorldGrid(*camera);

		if (applyingImpulse) {
			Gizmos::SetColor(Color::white);
			Gizmos::DrawLineWorld(*camera, startPosition, camera->ScreenToWorldPoint(Input::GetMousePosition()));
		}

#if true
		// Bounds
		colliderA->RecalculateBounds();
		colliderB->RecalculateBounds();
		Color color = Color::green;
		color.a = 50;
		Gizmos::SetColor(color);
		Gizmos::DrawRectWorld(*camera, colliderA->GetBounds()->GetCenter(), colliderA->GetBounds()->GetExtents());
		Gizmos::DrawRectWorld(*camera, colliderB->GetBounds()->GetCenter(), colliderB->GetBounds()->GetExtents());
#endif

		// Collider
		auto verts = colliderA->GetShapes()->GetShapeVertices(0);
		colliderA->transform->TransformPoints(verts);
		Gizmos::SetColor(Color::yellow);
		Gizmos::DrawPolygonWorld(*camera, verts);

		verts = colliderB->GetShapes()->GetShapeVertices(0);
		colliderB->transform->TransformPoints(verts);
		Gizmos::SetColor(Color::white);
		Gizmos::DrawPolygonWorld(*camera, verts);

		// Collision data
		if (manifold) {
			for (size_t i = 0; i < manifold->pointCount; i++) {
				const auto& contact = (*manifold)[i];

				Gizmos::SetColor(Color::red);
				Gizmos::DrawRectWorld(*camera, contact->point, Vector2(0.08f, 0.08f));
			}

			std::vector<Physics2D::Polygon> polysA, polysB;
			Physics2D::PopulatePolygonsFromCollider(*colliderA, polysA);
			Physics2D::PopulatePolygonsFromCollider(*colliderB, polysB);

			Gizmos::SetColor(Color(200, 200, 200, 255));
			Gizmos::DrawLineWorld(*camera, colliderA->transform->GetPosition(), colliderA->transform->GetPosition() + manifold->normal);

			// Edge selection
			Vector2 normal = manifold->normal.GetNormalized();

			Physics2D::Segment edgeA = Physics2D::GetBestEdge(polysA[0], normal);
			Physics2D::Segment edgeB = Physics2D::GetBestEdge(polysB[0], -normal);

			float dotA = std::abs(Vector2::Dot(edgeA.GetForward(), normal));
			float dotB = std::abs(Vector2::Dot(edgeB.GetForward(), normal));
			Physics2D::Segment *refEdge, *incEdge;
			if (dotA <= dotB)
			{
				refEdge = &edgeA;
				incEdge = &edgeB;
			}
			else
			{
				refEdge = &edgeB;
				incEdge = &edgeA;
			}

			Gizmos::SetColor(Color::green);
			Gizmos::DrawLineWorld(*camera, refEdge->v1.position, refEdge->v2.position);
			Gizmos::DrawRectWorld(*camera, refEdge->v1.position, Vector2(0.05f, 0.05f));
			Gizmos::SetColor(Color::magenta);
			Gizmos::DrawRectWorld(*camera, refEdge->v2.position, Vector2(0.05f, 0.05f));

			Vector2 sideNormal1 = refEdge->GetForward();
			Vector2 sideNormal2 = refEdge->GetBackward();
			float offset1 = Vector2::Dot(refEdge->v2.position, sideNormal1);
			float offset2 = Vector2::Dot(refEdge->v1.position, sideNormal2);
			Physics2D::ClipSegmentToLine(*incEdge, sideNormal1, offset1);
			Physics2D::ClipSegmentToLine(*incEdge, sideNormal2, offset2);

			Gizmos::SetColor(Color::red);
			Gizmos::DrawLineWorld(*camera, incEdge->v1.position, incEdge->v2.position);
		}
	}

private:
	inline std::vector<Vector2> CreatePolygon(size_t _corners, float _radius) const {
		float rotationIter = -360.0f / _corners;
		std::vector<Vector2> vertices = { Vector2(0, _radius) };
		for (size_t i = 1; i < _corners; i++) {
			vertices.push_back(Vector2(
				-1 * _radius * std::sinf(LMK_DtoR(rotationIter * i)),
				_radius * std::cosf(LMK_DtoR(rotationIter * i)))
			);
		}
		return vertices;
	}
};

class EqualityConstraintsTest: public LMKEngine {

	std::shared_ptr<Scene> scene;

	Camera2D* camera;

	Vector2 startPosition;
	Rigidbody2D* lastBody;

	std::vector<Rigidbody2D*> bodies;
	std::vector<EqualityConstraint*> constraints;

public:
	inline void OnUserStart() override {
		scene = std::make_shared<Scene>();

		auto goCam = scene->CreateGameObject("Main Camera");
		camera = goCam->AddComponent<Camera2D>();
		camera->backgroundColor = Color(25, 25, 25, 1);

		auto go = scene->CreateGameObject("Base Collider");
		go->transform->SetPosition(Vector2(0, 4));
		auto collider = go->AddComponent<BoxCollider2D>();
		collider->isTrigger = true;
		auto body = go->AddComponent<Rigidbody2D>();
		body->RegisterCollider(collider);
		body->bodyType = RigidbodyType2D::Static;
		body->gravityScale = 0;
		body->angularDamping = 0.5f;
		body->linearDamping = 0.5f;

		lastBody = body;
		bodies.push_back(body);
		AddColliderAttachedTo(lastBody, lastBody->GetPosition() -
			(lastBody->transform->UpDirection() + lastBody->transform->RightDirection()) * 0.5f);
	}

	inline void OnUserUpdate() override {
		if (Input::GetMouseButtonDown(MouseButton::Left)) {
			startPosition = camera->ScreenToWorldPoint(Input::GetMousePosition());
		}
		if (Input::GetMouseButtonUp(MouseButton::Left)) {
			Vector2 impulse = (camera->ScreenToWorldPoint(Input::GetMousePosition()) - startPosition) * 5.f;
			for (auto& body : bodies)
				body->ApplyImpulse(impulse, startPosition);
		}

		if (Input::GetKeyDown(KeyCode::Space)) {
			AddColliderAttachedTo(lastBody, lastBody->GetPosition() - 
				(lastBody->transform->UpDirection() + lastBody->transform->RightDirection()) * 0.5f);
		}
	}

	inline void OnDrawGizmos() override {
		Gizmos::DrawWorldGrid(*camera, 0.5f);

		Gizmos::SetColor(Color::white);
		for (const auto& c : bodies) {
			auto verts = c->GetCollider()->GetShapes()->GetShapeVertices(0);
			c->transform->TransformPoints(verts);
			Gizmos::DrawPolygonWorld(*camera, verts);
		}

		for (const auto& c : constraints) {
			Gizmos::SetColor(Color::yellow);
			Gizmos::DrawRectWorld(*camera, c->GetWorldA(), Vector2(0.05f, 0.05f));
			Gizmos::SetColor(Color::cyan);
			Gizmos::DrawRectWorld(*camera, c->GetWorldB(), Vector2(0.05f, 0.05f));
		}
	}

private:
	inline void AddColliderAttachedTo(Rigidbody2D*& _parentBody, Vector2 _worldPoint) {
		auto go = scene->CreateGameObject("Chain Collider");
		go->transform->SetPosition(_worldPoint - (go->transform->UpDirection() + go->transform->RightDirection()) * 0.5f);
		auto collider = go->AddComponent<BoxCollider2D>();
		collider->isTrigger = false;
		auto body = go->AddComponent<Rigidbody2D>();
		body->RegisterCollider(collider);
		body->mass = Random::Range(10.f, 20.f);
		body->inertia = body->mass;
		body->gravityScale = 1.f;
		body->angularDamping = 0.5f;
		body->linearDamping = 0.5f;

		EqualityConstraint* constraint = new EqualityConstraint(_parentBody, body, _worldPoint);
		Physics2D::GetScene()->Register(constraint);

		bodies.push_back(body);
		constraints.push_back(constraint);

		_parentBody = body;
	}
};

class ContactConstraintsTest : public LMKEngine {

	std::shared_ptr<Scene> scene;

	Camera2D* camera;

	Rigidbody2D* lastBody;
	Vector2 startPosition;

	std::vector<Rigidbody2D*> bodies;

public:
	inline void OnUserStart() override {
		scene = std::make_shared<Scene>();

		auto goCam = scene->CreateGameObject("Main Camera");
		camera = goCam->AddComponent<Camera2D>();
		camera->backgroundColor = Color(25, 25, 25, 1);

		auto go = scene->CreateGameObject("Base Collider");
		go->transform->SetPosition(Vector2(0, -3));
		auto collider = go->AddComponent<BoxCollider2D>();
		collider->scale = Vector2(10.f, 2.f);
		auto body = go->AddComponent<Rigidbody2D>();
		body->RegisterCollider(collider);
		body->bodyType = RigidbodyType2D::Static;
		body->gravityScale = 0;
		body->angularDamping = 0.5f;
		body->linearDamping = 0.5f;
		auto flat = go->AddComponent<FlatControlBehaviour>();
		flat->targetBody = body;

		lastBody = body;
		bodies.push_back(body);
	}

	inline void OnUserUpdate() override {
		if (Input::GetMouseButtonDown(MouseButton::Left)) {
			startPosition = camera->ScreenToWorldPoint(Input::GetMousePosition());
		}
		if (Input::GetMouseButtonUp(MouseButton::Left)) {
			Vector2 impulse = (camera->ScreenToWorldPoint(Input::GetMousePosition()) - startPosition);
			lastBody->ApplyImpulse(impulse * lastBody->mass, startPosition);
		}

		if (Input::GetMouseButtonDown(MouseButton::Right)) {
			AddBody(GetMousePosWorld(), RigidbodyType2D::Dynamic);
		}

		if (Input::GetMouseButtonDown(MouseButton::Middle)) {
			AddBody(GetMousePosWorld(), RigidbodyType2D::Static);
		}
	}

	inline void OnDrawGizmos() override {
		Gizmos::DrawWorldGrid(*camera, 0.5f);

		// colliders
		Color boundsColor = Color::green;
		boundsColor.a = 40;
		for (const auto& b : bodies) {
			auto bounds = b->GetCollider()->GetBounds();
			Gizmos::SetColor(boundsColor);
			Gizmos::DrawRectWorld(*camera, bounds->GetCenter(), bounds->GetExtents());

			auto verts = b->GetCollider()->GetWorldVertices(0);
			Gizmos::SetColor(b->bodyType == RigidbodyType2D::Static ? Color::yellow : Color::white);
			Gizmos::DrawPolygonWorld(*camera, verts);
		}

		// constrains
		Gizmos::SetColor(Color::cyan);
		for (const auto& c : Physics2D::GetScene()->GetContacts()) {
			if (!c) continue;

			Gizmos::DrawRectWorld(*camera, c->GetWorldContactPointB(), Vector2(0.05f, 0.05f));
		}
	}

private:
	inline void AddBody(Vector2 _worldPoint, RigidbodyType2D _bodyType) {
		static int count = 0;

		auto go = scene->CreateGameObject("Collider " + std::to_string(count));
		go->transform->SetPosition(_worldPoint);
		auto collider = go->AddComponent<BoxCollider2D>();
		collider->scale = Vector2(Random::Range(1, 3), Random::Range(1, 3));
		auto body = go->AddComponent<Rigidbody2D>();
		body->RegisterCollider(collider);
		body->bodyType = _bodyType;
		body->gravityScale = 1.f;
		body->angularDamping = 0.5f;
		body->linearDamping = 0.5f;

		bodies.push_back(body);
		lastBody = body;

		count++;
	}

	inline Vector2 GetMousePosWorld() {
		return camera->ScreenToWorldPoint(Input::GetMousePosition());
	}
};
#endif

#endif // !_TEST_ENGINES_H_