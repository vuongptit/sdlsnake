#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>

using namespace std;

const int SCREEN_WIDTH = 640;//kích thước
const int SCREEN_HEIGHT = 480;
const int GRID_SIZE = 20;
const SDL_Color HEAD_COLOR = { 0, 255, 0, 255 };// r g b a

struct Point {//cấu trúc điểm theo grid
    int x;
    int y;
};

enum class Direction { UP, DOWN, LEFT, RIGHT }; //lớp liệt kê

void CheckBorderCrossing(Point& head) {//hàm kiểm tra vượt biên
    if (head.x < 0) {
        head.x = (SCREEN_WIDTH / GRID_SIZE) - 1;
    }
    else if (head.x >= (SCREEN_WIDTH / GRID_SIZE)) {
        head.x = 0;
    }

    if (head.y < 2) {
        head.y = (SCREEN_HEIGHT / GRID_SIZE) - 1;
    }
    else if (head.y >= (SCREEN_HEIGHT / GRID_SIZE)) {
        head.y = 2;
    }
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cerr << "SDL initialization failed: " << SDL_GetError() << endl;
        return 1;
    }

    if (TTF_Init() < 0) {
        cerr << "SDL_ttf initialization failed: " << TTF_GetError() << endl;
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        cerr << "Window creation failed: " << SDL_GetError() << endl;
        TTF_Quit();
        SDL_Quit();
        return 2;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        cerr << "Renderer creation failed: " << SDL_GetError() << endl;
        TTF_Quit();
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 3;
    }

    TTF_Font* font = TTF_OpenFont("your_font.ttf", 30);
    if (!font) {
        cerr << "Font could not be loaded! TTF_Error: " << TTF_GetError() << endl;
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 4;
    }

    srand(static_cast<unsigned>(time(nullptr)));// thay đổi giá trị ngẫu nhiên

    vector<Point> snake;//vector chứa các phần của rắn
    Direction snakeDirection = Direction::RIGHT;
    Point food;//khai báo mồi
    food.x = rand() % (SCREEN_WIDTH / GRID_SIZE);//hàm lấy ngẫu nhiên vị trị mồi
    food.y = rand() % (SCREEN_HEIGHT / GRID_SIZE-4) + 2;

    for (int i = 0; i < 4; ++i) {
        Point p;
        p.x = (SCREEN_WIDTH / GRID_SIZE) / 2 - i;//sinh rắn
        p.y = SCREEN_HEIGHT / GRID_SIZE / 2;
        snake.push_back(p);
    }
    //khởi tạo các check point
    int score = 0;
    bool quit = false;
    bool gameOver = false;
    bool gameStarted = false;
    //vòng lặp chính
    while (!quit) {
        SDL_Event event;
        //sử lý các sự kiện SDL
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            else if (event.type == SDL_KEYDOWN) {
                if (!gameOver) {
                    if (!gameStarted) {
                        if (event.key.keysym.sym == SDLK_RETURN) {
                            gameStarted = true;
                        }
                    }
                    // Xử lý các phím mũi tên và ESC khi trò chơi đã bắt đầu
                    else {
                        switch (event.key.keysym.sym) {
                        case SDLK_UP:
                            if (snakeDirection != Direction::DOWN) snakeDirection = Direction::UP;
                            break;
                        case SDLK_DOWN:
                            if (snakeDirection != Direction::UP) snakeDirection = Direction::DOWN;
                            break;
                        case SDLK_LEFT:
                            if (snakeDirection != Direction::RIGHT) snakeDirection = Direction::LEFT;
                            break;
                        case SDLK_RIGHT:
                            if (snakeDirection != Direction::LEFT) snakeDirection = Direction::RIGHT;
                            break;
                        case SDLK_ESCAPE:
                            quit = true;
                            break;
                        }
                    }
                }
                else {
                    if (event.key.keysym.sym == SDLK_RETURN) {
                        snake.clear();//bắt đầu lại trò chơi khi nhấn enter 
                        for (int i = 0; i < 4; ++i) { //tạo lại vị trí ban đầu rắn 
                            Point p;
                            p.x = (SCREEN_WIDTH / GRID_SIZE) / 2 - i;
                            p.y = SCREEN_HEIGHT / GRID_SIZE / 2;
                            snake.push_back(p);
                        }
                        // thiết lập hướng ban đầu, tạo lại food  và reset điểm về 0
                        snakeDirection = Direction::RIGHT;
                        score = 0;
                        gameOver = false;
                        gameStarted = false;
                        food.x = rand() % (SCREEN_WIDTH / GRID_SIZE);
                        food.y = rand() % (SCREEN_HEIGHT / GRID_SIZE-4) + 2;
                    }
                }
            }
        }
        // sử lý logic trò chơi 
        if (gameStarted && !gameOver) {
            // logic di chuyển 
            Point newHead = snake[0];
            switch (snakeDirection) {
            case Direction::UP:
                newHead.y -= 1;
                break;
            case Direction::DOWN:
                newHead.y += 1;
                break;
            case Direction::LEFT:
                newHead.x -= 1;
                break;
            case Direction::RIGHT:
                newHead.x += 1;
                break;
            }

            CheckBorderCrossing(newHead);

            for (size_t i = 1; i < snake.size(); ++i) {
                if (newHead.x == snake[i].x && newHead.y == snake[i].y) {
                    cerr << "Snake collision!" << endl;
                    gameOver = true;
                    break;
                }
            }

            if (!gameOver) {
                snake.insert(snake.begin(), newHead);

                if (newHead.x == food.x && newHead.y == food.y) {//nếu rắn ăn food thì sinh food mới
                    food.x = rand() % (SCREEN_WIDTH / GRID_SIZE);
                    food.y = rand() % (SCREEN_HEIGHT / GRID_SIZE-4) + 2;
                    score += 10;
                }
                else {//
                    snake.pop_back();
                }

                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);//Thiết lập màu nền cho cửa sổ trò chơi thành màu đen (RGB: 0, 0, 0) với độ trong suốt là 255 
                SDL_RenderClear(renderer);//xóa mọi nội dung đã được vẽ trước đó trên renderer
                //Vẽ một hình chữ nhật (foodRect) tại vị trí của thức ăn trên màn hình. Màu của hình chữ nhật này là đỏ (RGB: 255, 0, 0)
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                SDL_Rect foodRect = { food.x * GRID_SIZE, food.y * GRID_SIZE, GRID_SIZE, GRID_SIZE };
                SDL_RenderFillRect(renderer, &foodRect);

                //Vẽ các phần của con rắn
                for (size_t i = 0; i < snake.size(); ++i) {
                    SDL_Rect snakeRect = { snake[i].x * GRID_SIZE, snake[i].y * GRID_SIZE, GRID_SIZE, GRID_SIZE };

                    if (i == 0) {
                        SDL_SetRenderDrawColor(renderer, HEAD_COLOR.r, HEAD_COLOR.g, HEAD_COLOR.b, HEAD_COLOR.a);
                    }
                    else {
                        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                    }

                    SDL_RenderFillRect(renderer, &snakeRect);
                }

                /*Vẽ điểm số của người chơi trên màn hình sử dụng màu xanh lục(RGB: 0, 255, 0).
                Điểm số được chuyển đổi thành chuỗi và sau đó vẽ lên màn hình tại vị trí (10, 10).*/
                SDL_Color textColor = { 0, 255, 0, 255 };
                string scoreText = "Score:  " + to_string(score);
                SDL_Surface* textSurface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
                SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                SDL_Rect textRect = { 10, 10, textSurface->w, textSurface->h };
                SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
                SDL_FreeSurface(textSurface);
                SDL_DestroyTexture(textTexture);

                SDL_RenderPresent(renderer);
                SDL_Delay(100);
            }
        }
        else {
            
            // thực hiện các thao tác sau khi kết thúc trò chơi 
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            if (gameOver) {
                // Nếu trò chơi kết thúc:

                // Thiết lập màu chữ và tạo chuỗi thông báo kết thúc trò chơi kèm theo điểm số
                SDL_Color textColor = { 255, 0, 0, 255 };
                string gameOverText = "Game Over! Score: " + to_string(score);

                // Tạo một bề mặt (textSurface) từ chuỗi kết quả với màu chữ được thiết lập
                SDL_Surface* textSurface = TTF_RenderText_Solid(font, gameOverText.c_str(), textColor);

                // Tạo một texture (textTexture) từ bề mặt văn bản để vẽ lên renderer
                SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

                // Tạo một hình chữ nhật (textRect) để đặt vị trí và kích thước của văn bản trên màn hình
                SDL_Rect textRect = { SCREEN_WIDTH / 4, SCREEN_HEIGHT / 2, textSurface->w, textSurface->h };

                // Vẽ văn bản lên renderer tại vị trí và kích thước được xác định bởi textRect
                SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);

                // Giải phóng bộ nhớ của bề mặt và texture để tránh rò rỉ bộ nhớ
                SDL_FreeSurface(textSurface);
                SDL_DestroyTexture(textTexture);

                // Hiển thị thông báo lựa chọn tiếp tục hoặc kết thúc trò chơi
                SDL_Color exitTextColor = { 0, 255, 0, 255 };
                string exitText = "Press  Enter  to Play Again or  ESC  to Exit";

                // Tạo bề mặt và texture cho thông báo lựa chọn tiếp tục hoặc kết thúc
                SDL_Surface* exitTextSurface = TTF_RenderText_Solid(font, exitText.c_str(), exitTextColor);
                SDL_Texture* exitTextTexture = SDL_CreateTextureFromSurface(renderer, exitTextSurface);

                // Tạo hình chữ nhật (exitTextRect) để đặt vị trí và kích thước của thông báo trên màn hình
                SDL_Rect exitTextRect = { SCREEN_WIDTH / 8, (SCREEN_HEIGHT / 2) + 100, exitTextSurface->w, exitTextSurface->h };

                // Vẽ thông báo lựa chọn trên renderer
                SDL_RenderCopy(renderer, exitTextTexture, nullptr, &exitTextRect);

                // Giải phóng bộ nhớ của bề mặt và texture
                SDL_FreeSurface(exitTextSurface);
                SDL_DestroyTexture(exitTextTexture);

                // Hiển thị tất cả các thay đổi trên màn hình
                SDL_RenderPresent(renderer);
            }
            else {
                // Nếu trò chơi chưa bắt đầu hoặc đang chạy:

                // Thiết lập màu chữ và tạo chuỗi thông báo bắt đầu trò chơi
                SDL_Color startTextColor = { 0, 255, 0, 255 };
                string startText = "Press Enter to Start";

                // Tạo bề mặt và texture cho thông báo bắt đầu trò chơi
                SDL_Surface* startTextSurface = TTF_RenderText_Solid(font, startText.c_str(), startTextColor);
                SDL_Texture* startTextTexture = SDL_CreateTextureFromSurface(renderer, startTextSurface);

                // Tạo hình chữ nhật (startTextRect) để đặt vị trí và kích thước của thông báo trên màn hình
                SDL_Rect startTextRect = { SCREEN_WIDTH / 3, (SCREEN_HEIGHT / 2) - 25, startTextSurface->w, startTextSurface->h };

                // Vẽ thông báo bắt đầu trò chơi lên renderer
                SDL_RenderCopy(renderer, startTextTexture, nullptr, &startTextRect);

                // Giải phóng bộ nhớ của bề mặt và texture
                SDL_FreeSurface(startTextSurface);
                SDL_DestroyTexture(startTextTexture);

                // Hiển thị tất cả các thay đổi trên màn hình
                SDL_RenderPresent(renderer);
            }
            //sử lý sự kiện khi bắt đầu lại
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    quit = true;
                }
                else if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_RETURN) {
                        if (gameOver) {
                            snake.clear();
                            for (int i = 0; i < 4; ++i) {
                                Point p;
                                p.x = (SCREEN_WIDTH / GRID_SIZE) / 2 - i;
                                p.y = SCREEN_HEIGHT / GRID_SIZE / 2;
                                snake.push_back(p);
                            }
                            snakeDirection = Direction::RIGHT;
                            score = 0;
                            gameOver = false;
                            gameStarted = false;
                            food.x = rand() % (SCREEN_WIDTH / GRID_SIZE);
                            food.y = rand() % (SCREEN_HEIGHT / GRID_SIZE -4) + 2;
                        }
                        else {
                            gameStarted = true;
                        }
                    }
                    else if (event.key.keysym.sym == SDLK_ESCAPE) {
                        if (!gameStarted) {
                            quit = true;
                        }
                    }
                }
            }
        }
    }
    // giải phóng bộ nhớ và tắt SDl khi thoát vòng lặp chính
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
