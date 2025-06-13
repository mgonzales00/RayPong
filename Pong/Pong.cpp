#include <raylib.h>
#include <iostream>

// Globals
const int screenWidth = 1920;
const int screenHeight = 1080;

int playerScore = 0;
int opponentScore = 0;

class Ball
{

public:
	float x;
	float y;
	double radius;
	int speedX, speedY;

	void Draw()
	{
		DrawCircle(static_cast<int>(x), static_cast<int>(y), radius, WHITE);
	}

	void Update()
	{
		x += speedX;
		y += speedY;
		
		if (y + radius >= screenHeight || y - radius <= 0)
		{
			// Reverse direction of ball when bottom of the ball (ball y + radius) hits bottom of screen
			speedY *= -1;
		}

		// Opponent wins
		if (x + radius >= screenWidth) 
		{
			opponentScore++;
			Reset();
		}

		// We win
		if (x - radius <= 0)
		{
			playerScore++;
			Reset();
		}

	}

	void Reset()
	{
		x = screenWidth / 2;
		y = screenHeight / 2;

		int speedChoices[2] = { -1, 1 };
		speedX *= speedChoices[GetRandomValue(0, 1)];
		speedY *= speedChoices[GetRandomValue(0, 1)];
	}
};

class Paddle
{
protected:
	void LimitMovement()
	{
		// Check if paddle reaches the top edge of the screen
		if (y <= 0)
		{
			y = 0;
		}

		// Check if paddle reaches the bottom edge of the screen
		if (y + height >= screenHeight)
		{
			y = screenHeight - height;
		}
	}

public:
	float x, y;
	float width, height;
	int speed;
	float spaceFromEdge;

	void Draw()
	{
		DrawRectangle(x, y, width, height, WHITE);
	}

	void Update()
	{
		if (IsKeyDown(KEY_UP))
		{
			// Decrease the y coordinate to move up
			y = y - speed;
		}

		if (IsKeyDown(KEY_DOWN))
		{
			y = y + speed;
		}
		LimitMovement();
	}
};

// CPU we battle against
class OpponentPaddle : public Paddle
{
public:
	// Override the update function so the CPU can move independently
	void Update(int ballY)
	{
		// Check if the y position of the ball is above the y position of the paddle
		// If it is, we move the paddle up, if not we move it down
		int paddleCenter = y + (height / 2);

		if (paddleCenter > ballY)
		{
			y -= speed;
		}

		if (paddleCenter <= ballY)
		{
			y += speed;
		}

		LimitMovement();
	}
};

class Pong
{
private:
	void Setup()
	{
		SetConfigFlags(FLAG_MSAA_4X_HINT);
		InitWindow(screenWidth, screenHeight, "Ray Pong");
		SetTargetFPS(60);
	}

public:
	void Init()
	{
		Ball ball;
		ball.x = screenWidth / 2;
		ball.y = screenHeight / 2;
		ball.radius = 15;
		ball.speedX = 7;
		ball.speedY = 7;

		Paddle player;
		player.height = 120;
		player.width = 25;
		player.spaceFromEdge = 30;
		player.x = player.spaceFromEdge;
		player.y = (screenHeight / 2) - (player.height / 2);
		player.speed = 6;

		OpponentPaddle op;
		op.height = 120;
		op.width = 25;
		op.spaceFromEdge = 30;
		op.x = screenWidth - op.width - op.spaceFromEdge;
		op.y = (screenHeight / 2) - (op.height / 2);
		op.speed = 6;

		Setup();

		while (WindowShouldClose() == false)
		{
			BeginDrawing();

			// Updates
			ball.Update();
			player.Update();
			op.Update(ball.y);

			// Collision detection for player
			if (CheckCollisionCircleRec(Vector2{ ball.x, ball.y }, ball.radius, Rectangle{ player.x, player.y, player.width, player.height }))
			{
				// Force ball to go other way instead of blindly flipping direction back and forth
				ball.speedX = abs(ball.speedX); // Force the ball right
				ball.x = player.x + player.width + ball.radius; // Push ball to the right of the paddle. Formula is left edge of paddle + width of paddle + radius of ball
			}
			// Collision detection for opponent
			if (CheckCollisionCircleRec(Vector2{ ball.x, ball.y }, ball.radius, Rectangle{ op.x, op.y, op.width, op.height }))
			{
				ball.speedX = -abs(ball.speedX); // Force the ball left
				ball.x = op.x - ball.radius; // Snap ball to left of opponent panel
			}

			// Drawing
			ClearBackground(BLACK);
			DrawLine(screenWidth / 2, 0, screenWidth / 2, screenHeight, WHITE);
			ball.Draw();
			player.Draw();
			op.Draw();

			// Draw score
			int fontSize = 80;

			const char* opponentScoreText = TextFormat("%i", opponentScore);
			const char* playerScoreText = TextFormat("%i", playerScore);

			int opponentTextWidth = MeasureText(opponentScoreText, 80);
			int playerTextWidth = MeasureText(playerScoreText, 80);

			int opponentTextX = (screenWidth / 4) - (opponentTextWidth / 2);
			int playerTextX = (3 * (screenWidth / 4) - (playerTextWidth / 2));

			DrawText(opponentScoreText, opponentTextX, 20, fontSize, WHITE);
			DrawText(playerScoreText, playerTextX, 20, fontSize, WHITE);

			EndDrawing();
		}

		CloseWindow();
	}
};

int main()
{
	Pong pong;

	pong.Init();

	return 0;
}
