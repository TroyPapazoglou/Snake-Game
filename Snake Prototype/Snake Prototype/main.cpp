#include <raylib.h>
#include <iostream>
#include <deque>
#include <raymath.h>
#include <string>

//testing testing 123
// colours for background and snake
Color lightPurple = { 213, 184, 255, 255 };
Color purple = { 159, 90, 253, 255 };

// declaring the size of the grid and border
int gridSize = 30;
int gridCount = 25;
int border = 60;
Sound eatSound;
Sound collidedSound;

// keep track of the time after the last update of the snake occurred
double lastUpdateTime = 0;

// return true if a given element is in a given deque else return false
bool ElementInDeque(Vector2 element, std::deque<Vector2> deque)
{
    for (int i = 0; i < deque.size(); i++)
    {
        if (Vector2Equals(deque[i], element))
        {
            return true;
        }
    }
    return false;
}

// check if an interval has passed to return a boolean value
bool eventTriggered(double interval)
{
    // using GetTime function from raylib to get the current time in seconds since the program began running
    double currentTime = GetTime();

    // compare the difference between the current time and last update time to the interval
    if (currentTime - lastUpdateTime >= interval)
    {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}


class Food
{
public:
    // creating an x and y position for the food
    Vector2 position;
    Texture2D texture;

    Food(std::deque<Vector2> snakeBody)
    {
        // Loads the image and texture then unloads the image to free memory
        Image image = LoadImage("Images/apple.png");
        texture = LoadTextureFromImage(image);
        UnloadImage(image);               
        position = GenerateRandomPos(snakeBody);
    }

    ~Food()
    {
        UnloadTexture(texture);
    }

    // drawing the texture to create the food
    void Draw()
    {
        DrawTexture(texture, position.x * gridSize, position.y * gridSize, WHITE);
    }

    Vector2 GenerateRandomCell()
    {
        float x = GetRandomValue(1, gridCount - 1);
        float y = GetRandomValue(2, gridCount - 1);
        return Vector2{ x, y };
    }

    Vector2 GenerateRandomPos(std::deque<Vector2> snakeBody)
    {
        Vector2 position = GenerateRandomCell();

        // Ensure the generated position is not on the border or in the snake body
        while (ElementInDeque(position, snakeBody))
        {
            position = GenerateRandomCell();
        }

        return position;
    }
};


class Snake
{
public:
    // creating the body of the snake, using deque because it can take stuff in and out of the start and end of the snake.
    std::deque<Vector2> body = { Vector2{ 6, 9 }, Vector2{ 5, 9 }, Vector2{ 4, 9 } };
    Vector2 direction = { 1, 0 };
    bool addSegment = false;

    // responsible for drawing the snake
    void Draw()
    {
        for (int i = 0; i < body.size(); i++)
        {
            float x = body[i].x;
            float y = body[i].y;

            Rectangle segment = Rectangle{ x * gridSize, y * gridSize, (float)gridSize, (float)gridSize };
            DrawRectangleRounded(segment, 0.5, 6, purple);
        }
    }

    // responsible for removing a cell from the end and adding it to the front to make it seem as if the snake is moving
    void Update()
    {
        body.push_front(Vector2Add(body[0], direction));
        if (addSegment == true)
        {
            addSegment = false;
        }
        else
        {
            body.pop_back();
        }
    }

    // resets the snake to a 3 segments and then sets its position
    void Reset()
    {
        body = { Vector2{ 6, 9 }, Vector2{ 5, 9 }, Vector2{ 4, 9 } };
        direction = { 1, 0 };
    }
};


class Game  
{
public:
    Snake snake = Snake();
    Food food = Food(snake.body);
    bool running = true;
    int score = 0;

    void Draw()
    {
        // Draw border and title
        DrawRectangle(0, 0, GetScreenWidth(), border, purple);
        DrawText("Snake Game", 20, 10, 40, WHITE);

        // Draw score
        DrawText(("Score: " + std::to_string(score)).c_str(), GetScreenWidth() - 200, 23, 20, WHITE);

        food.Draw();
        snake.Draw();

    }

    void Update()
    {
        if (running)
        {
            snake.Update();
            CheckCollisionWithFood();
            CheckCollisionWithEdges();
            CheckCollisionWithTail();
        }
    }

   

    

    // checks if the head and food are in the same x and y if so then generates food in a new position that isn't in the snakes current x and y and adds an extra segment to the body
    void CheckCollisionWithFood()
    {
        if (Vector2Equals(snake.body[0], food.position))
        {
            std::cout << "Nom Nom Nom" << std::endl;
            food.position = food.GenerateRandomPos(snake.body);
            snake.addSegment = true;
            score++;       
            
            PlaySound(eatSound);

        }
    }

    ~Game()
    {
        // Unload the collision sound
        UnloadSound(eatSound);
    }

    // checks if the snake's head collides with the x and y of the grid then ends the game
    void CheckCollisionWithEdges()
    {
        if (snake.body[0].x > gridCount - 1 || snake.body[0].x < 0 ||
            snake.body[0].y >= gridCount + 2|| snake.body[0].y <= 1) // Exclude top border cells
        {
            GameOver();
            PlaySound(collidedSound);
        }
    }

    // resets the snake to a set starting position, resets the food to a new position that isn't the snake's, ends the game
    void GameOver()
    {
        std::cout << "Game Over Man!" << std::endl;
        snake.Reset();
        food.position = food.GenerateRandomPos(snake.body);
        running = false;       
        
    }

    // checks if the first segment collides with any of the other segments after it, if a collision was detected end the game
    void CheckCollisionWithTail()
    {
        std::deque<Vector2> headlessBody = snake.body;
        headlessBody.pop_front();
        if (ElementInDeque(snake.body[0], headlessBody))
        {
            GameOver();
            PlaySound(collidedSound);
        }
    }
};




int main()
{
    std::cout << "Starting Snake..." << std::endl;
    // making the window and declaring its size
    InitWindow(gridSize * gridCount, gridSize * gridCount + border, "Snake Prototype");
    // setting the games frames per second
    SetTargetFPS(60);

    InitAudioDevice();
    collidedSound = LoadSound("Sounds/oof.wav");
    eatSound = LoadSound("Sounds/nom.wav");

    Game game = Game();

    // runs until the user closes the game
    while (!WindowShouldClose())
    {
        // creates blank canvas
        BeginDrawing();

        // if the function returns true it means the interval has passed, update the snake position every 0.1 seconds
        if (eventTriggered(0.1))
        {
            game.Update();
        }



        // if key is pressed check if snake is going in opposite direction if not then move snack in desired direction, if game isn't already running and a key (not including A or LEFT) is pressed runs the game, Reset the score after the player dies and presses a key
        bool directionChanged = false;

        if (!game.running)
        {
            
            DrawText("Game Over", GetScreenWidth() / 2 - MeasureText("Game Over", 60) / 2, GetScreenHeight() / 2 - 30, 60, RED);
            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
            {
                game.snake.direction = { 0, -1 };
                game.running = true;
                game.score = 0;
            }
            else if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
            {
                game.snake.direction = { 0, 1 };
                game.running = true;
                game.score = 0;
            }
            else if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A))
            {
                game.snake.direction = { -1, 0 };
                game.running = false;
                game.score = 0;
            }
            else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D))
            {
                game.snake.direction = { 1, 0 };
                game.running = true;
                game.score = 0;
            }
            
        }
        else
        {
            
            if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1 && !directionChanged)
            {
                game.snake.direction = { 0, -1 };
                directionChanged = true;
            }
            else if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1 && !directionChanged)
            {
                game.snake.direction = { 0, 1 };
                directionChanged = true;
            }
            else if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1 && !directionChanged)
            {
                game.snake.direction = { -1, 0 };
                directionChanged = true;
            }
            else if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1 && !directionChanged)
            {
                game.snake.direction = { 1, 0 };
                directionChanged = true;
            }
            else if (IsKeyPressed(KEY_W) && game.snake.direction.y != 1 && !directionChanged)
            {
                game.snake.direction = { 0, -1 };
                directionChanged = true;
            }
            else if (IsKeyPressed(KEY_S) && game.snake.direction.y != -1 && game.snake.direction.x != 0 && !directionChanged)
            {
                game.snake.direction = { 0, 1 };
                directionChanged = true;
            }
            else if (IsKeyPressed(KEY_A) && game.snake.direction.x != 1 && game.snake.direction.y != 0 && !directionChanged)
            {
                game.snake.direction = { -1, 0 };
                directionChanged = true;
            }
            else if (IsKeyPressed(KEY_D) && game.snake.direction.x != -1 && game.snake.direction.y != 0 && !directionChanged)
            {
                game.snake.direction = { 1, 0 };
                directionChanged = true;
            }
        }       

        // Draws the canvas
        ClearBackground(lightPurple);

        // Draw game elements
        game.Draw();

        // Ends canvas drawing
        EndDrawing();
    }
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
