#include "PlayerController.h"

using namespace std;

PlayerController::PlayerController()
{
	m_camera = 0;
	m_input = 0;
	m_timer = 0;
	m_movement = 0;
}

PlayerController::PlayerController(const PlayerController& other)
{

}

PlayerController::~PlayerController()
{

}

bool PlayerController::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	bool result;

	LightMovement = true;

	// Create the camera object.
	m_camera = new Camera;
	if (!m_camera)
	{
		return false;
	}

	//m_camera->SetPosition(0.0f, 0.0f, -10.0f);

	m_input = new Input;
	if (!m_input)
	{
		return false;
	}

	// Initialize the input object.
	result = m_input->Initialize(hinstance, hwnd, screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the player input object.", L"Error", MB_OK);
		return false;
	}

	keyReleased = true;
	keyWaiting = false;

	// Create the timer object.
	m_timer = new Timer;
	if (!m_timer)
	{
		return false;
	}

	// Initialize the timer object.
	result = m_timer->Initialize();
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the timer object.", L"Error", MB_OK);
		return false;
	}

	// Create movement object.
	m_movement = new Movement;
	if (!m_movement)
	{
		return false;
	}

	// Set the initial position and rotation of the viewer.
	m_movement->SetPosition(0.0f, 0.0f, -10.0f);
	m_movement->SetRotation(0.0f, 0.0f, 0.0f);

	return true;
}

// Called every frame.
bool PlayerController::Frame()
{
	bool result;

	// Update the system stats.
	m_timer->Frame();

	// Read the user input.
	result = m_input->Frame();
	if (!result)
	{
		return false;
	}

	// Do the frame input processing.
	result = HandleMovement(m_timer->GetTime());
	if (!result)
	{
		return false;
	}

	// Render.
	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool PlayerController::HandleMovement(float frameTime)
{
	bool keyDown;
	XMFLOAT3 rot, pos;
	int mouseChangeX, mouseChangeY;
	float mouseSensitivity = 0.1f;// Used to affect mouseMovement speed easily.

	// Gets the mouseChange between frames for the X axis.
	mouseChangeX = m_input->GetMouseXChange();

	// Gets the mouseChange between frames for the Y axis.
	mouseChangeY = m_input->GetMouseYChange();

	// Set the frame time for calculating the updated position.
	m_movement->SetFrameTime(frameTime);

	// Handle the input.
	keyDown = m_input->IsKeyDown(DIK_W);
	if (keyDown) m_movement->MoveForward(true);

	keyDown = m_input->IsKeyDown(DIK_S);
	if (keyDown) m_movement->MoveBackward(true);

	keyDown = m_input->IsKeyDown(DIK_A);
	if (keyDown) m_movement->MoveLeft(true);

	keyDown = m_input->IsKeyDown(DIK_D);
	if (keyDown) m_movement->MoveRight(true);

	keyDown = m_input->IsKeyDown(DIK_SPACE);
	if (keyDown) m_movement->MoveUpward(true);

	keyDown = m_input->IsKeyDown(DIK_LCONTROL);
	if (keyDown) m_movement->MoveDownward(true);

	// Run the poisition method moveMouse and pass in variables from the input class.
	m_movement->MoveMouse(mouseChangeX, mouseChangeY, mouseSensitivity);

	// Get the new view point position/rotation.
	m_movement->GetPosition(pos);
	m_movement->GetRotation(rot);

	// Reset the cursor to the centre of the screen as we now have mouse movement.
	m_input->ResetMousePosition();

	// Set the position of the camera.
	m_camera->SetPosition(pos.x, pos.y, pos.z);
	m_camera->SetRotation(rot.x, rot.y, rot.z);

	// Handle interaction events (On key release).
	HandleInteraction(frameTime);

	return true;
}

bool PlayerController::HandleInteraction(float frameTime)
{
	bool keyDown;
	float movementSpeed = 1.0;// Used to affect movement speed easily.

	keyDown = m_input->IsKeyDown(DIK_LSHIFT);
	if (keyDown && keyReleased)
	{
		keyReleased = false;
		keyWaiting = true;

		m_movement->SetMoveSpeed(false, movementSpeed);
	}
	else if (!keyDown && keyWaiting)
	{
		keyReleased = true;
		keyWaiting = false;

		m_movement->SetMoveSpeed(false, 0.0f);
	}

	keyDown = m_input->IsKeyDown(DIK_M);
	if (keyDown && keyReleased)
	{
		keyReleased = false;
		keyWaiting = true;
	}
	else if (!keyDown && keyWaiting)
	{
		keyReleased = true;
		keyWaiting = false;

		LightMovement = !LightMovement;
	}


	return true;
}

bool PlayerController::Render()
{
	// Render the camera.
	m_camera->Render();

	return true;
}

// Return the view matrix of the camera inside the player controller object.
void PlayerController::GetCameraViewMatrix(XMMATRIX& matrix)
{
	m_camera->GetViewMatrix(matrix);
	return;
}

// Return the players position.
void PlayerController::GetPlayerPosition(XMFLOAT3& pos)
{
	m_movement->GetPosition(pos);

	return;
}

void PlayerController::Shutdown()
{
	// Release the camera object.
	if (m_camera)
	{
		delete m_camera;
		m_camera = 0;
	}

	// Release the input object.
	if (m_input)
	{
		m_input->Shutdown();
		delete m_input;
		m_input = 0;
	}

	// Release the timer object.
	if (m_timer)
	{
		delete m_timer;
		m_timer = 0;
	}

	// Release the movement object.
	if (m_movement)
	{
		delete m_movement;
		m_movement = 0;
	}
}

