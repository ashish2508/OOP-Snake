#include <iostream>
#include <raylib.h>
#include <deque>
#include <raymath.h>
using namespace std;
Color BackGroundColor={112, 119, 181, 255};
Color TextColor={250, 250, 190, 230};
Color SnakeC={63, 51, 81, 255};

const char* Game_Name = "SNAKE's Maze";
const int CellSize=30;
const int CellCount=25;
int offset=75;

double lastupdatetime=0.0;

bool eventTriggered(double interval)
{
    double currenttime=GetTime();
    if( currenttime - lastupdatetime >= interval)
    {
        lastupdatetime= currenttime; return true;
    }
    return false;
}

bool ElementInDeque(Vector2 element,deque<Vector2>deque)
{
    for (unsigned int i=0; i<deque.size(); i++)
    {
        if(Vector2Equals(deque[i],element)){
            return true;
        }
    }
    return false;
}

class Snake
{
    public :
    deque<Vector2> body={Vector2{6,9},Vector2{5,9},Vector2{4,9}};

    Vector2 direction={1,0};

    bool addSegment=false;

    void Draw()
    {
        for(unsigned int i=0 ; i<body.size();i++){
            float x=body[i].x;
            float y=body[i].y;
            Rectangle segment = Rectangle {offset+x*CellSize, offset+y*CellSize,CellSize,CellSize};
            DrawRectangleRounded(segment,0.8f,6,SnakeC);
        }
    }
    void Update()
    {
        body.push_front(Vector2Add(body[0],direction));
        if(addSegment==true)
        {
            addSegment=false;
        }
        else
        {
        body.pop_back();
        }
    }

    void Reset()
    {
        body={Vector2{6,9},Vector2{5,9},Vector2{4,9}};
        direction={1,0};
    }

};


class Food{

    public:
    Vector2 position;
    Texture2D texture;



    Food(deque<Vector2>snakeBody)
    {
        Image image =LoadImage("Graphics/Food.png");

        if (image.data == nullptr)

        {

            puts("Failed to load food image!");

            exit(1);

        }
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
        position=GenerateRandomPos(snakeBody);
    }

    ~Food()
    {
        UnloadTexture(texture);
    }

    void Draw(){
        DrawTexture(texture,offset+position.x*CellSize,offset+position.y*CellSize,WHITE);
    }

Vector2 GenerateRandomCell(){
        float x=GetRandomValue(0,CellCount-1);
        float y=GetRandomValue(0,CellCount-1);
        return Vector2{x,y};
}
    Vector2 GenerateRandomPos(deque<Vector2>snakeBody)
    {
        Vector2 position=GenerateRandomCell();
        while(ElementInDeque(position,snakeBody))
        {
            position=GenerateRandomCell();
        }
        return position;
    }
};

class Game
{
    public:
Snake snake =Snake();
Food food=Food(snake.body);
int score=0;
bool running = true;
Sound eatSound;
Sound wallSound;

Game()
{
    InitAudioDevice();
    eatSound=LoadSound("Sounds/eat.mp3");
    wallSound=LoadSound("Sounds/wall.mp3");
}

~Game(){
    UnloadSound(eatSound);
    UnloadSound(wallSound);
    CloseAudioDevice();
}

void Draw()
{
    food.Draw();
    snake.Draw();
}

void Update()
{
    if(running)
    {
    snake.Update();
    CheckCollisionWithFood();
    CheckcollisionWithEdges();
    CheckcollisionWithTail();
    }
}
void CheckCollisionWithFood()
{
    if(Vector2Equals(snake.body[0],food.position))
    {
        food.position = food.GenerateRandomPos(snake.body);
        snake.addSegment=true;
        score++;
        PlaySound(eatSound);

    }
}
void CheckcollisionWithEdges()
{
if((snake.body[0].x==CellCount || snake.body[0].x==-1) || (snake.body[0].y==CellCount || snake.body[0].y==-1) )
{
    GameOver();
}
}

void GameOver()
{
    snake.Reset();
    food.position = food.GenerateRandomPos(snake.body);
    running = false;
    score = 0 ;
    PlaySound(wallSound);
}
void CheckcollisionWithTail()
{
    deque<Vector2>headlessbody=snake.body;
    headlessbody.pop_front();
    if(ElementInDeque(snake.body[0],headlessbody))
    {
        GameOver();

    }
}
};

int main()
{

    puts("Starting the Game.... ");

    InitWindow(2*offset+CellSize*CellCount,2*offset+CellSize*CellCount,Game_Name);

    SetTargetFPS(60);

    Game game=Game();



//Game LOOP Start

    while(WindowShouldClose()==false)
    {

    BeginDrawing();

    if(eventTriggered(0.13)){
        game.Update();
    }
    // W = towards North
    if((IsKeyPressed(KEY_UP)&&game.snake.direction.y!=1) || (IsKeyPressed(KEY_W)&&game.snake.direction.y!=1)){
        game.snake.direction = {0,-1};

    }
    // S = towards South
    if((IsKeyPressed(KEY_DOWN)&&game.snake.direction.y!=-1) || (IsKeyPressed(KEY_S)&&game.snake.direction.y!=-1)){
        game.snake.direction = {0,1};

    }
    // A = towards West
    if((IsKeyPressed(KEY_LEFT)&&game.snake.direction.x!=1) || (IsKeyPressed(KEY_A)&&game.snake.direction.x!=1)){
        game.snake.direction = {-1,0};

    }
    // D = towards East
    if((IsKeyPressed(KEY_RIGHT)&&game.snake.direction.x!=-1) || (IsKeyPressed(KEY_D)&&game.snake.direction.x!=-1)){
        game.snake.direction = {1,0};

    }
    // if(IsKeyPressed(KEY_Z)){
    //     CloseWindow();
    // }
    if (IsKeyPressed(KEY_SPACE)) {
        game.running = !game.running;
    }


    ClearBackground(BackGroundColor);

     DrawRectangleLinesEx(Rectangle{(float)offset - 5, (float)offset - 5, (float)CellSize * CellCount + 10, (float)CellSize * CellCount + 10}, 5, TextColor);
    DrawText("Snake's Maze",offset-5,20,40,TextColor);
    DrawText(TextFormat(" SCORE : %i",game.score),offset-5,offset+CellSize*CellCount+10,40,TextColor);
    game.Draw();

        EndDrawing();
    }
//Game LOOP End

    CloseWindow();
    return 0;
}
