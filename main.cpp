#include "iostream"
#include "ostream"
#include "string"
#include "vector"
#include "functional"
#include "raylib.h"
#include "string"
#include "chrono"
#include <memory>
#include <cmath>
#include <utility>


typedef struct Point {
    float x;
    float y;
} Point ;

using Curve = std::function<Point(float)>;


Curve interpolate(Curve a, Curve b) {
    auto A = std::make_shared<Curve>(std::move(a));
    auto B = std::make_shared<Curve>(std::move(b));

    auto cache = std::make_shared<std::pair<float, Point>>(NAN, Point{});

    return [A, B, cache](float t) {
        if (t == cache->first) return cache->second;

        Point p1 = (*A)(t);
        Point p2 = (*B)(t);
        cache->second = Point{
            .x = p1.x + t * (p2.x - p1.x),
            .y = p1.y + t * (p2.y - p1.y),
        };
        cache->first = t;
        return cache->second;
    };
}

Curve constant(Point p) {
    return [p](float) {
        return p;
    };
}

// AI GENERATED
void printer(const std::vector<std::vector<Curve>>& curve, float t) {
    for (size_t depth = 0; depth < curve.size(); ++depth) {
        std::cout << "Depth " << depth << ":\n";

        for (size_t i = 0; i < curve[depth].size(); ++i) {
            Point p = curve[depth][i](t);

            std::cout << "  Curve " << i << ": ("
                      << p.x << ", " << p.y << ")\n";
        }
    }
}
// end of AI generated code

void renderTSegment(const std::vector<std::vector<Curve>>& curve, float t, float increment) {
    for (size_t depth = 0; depth < curve.size(); ++depth) {
        for (size_t i = 0; i < curve[depth].size(); ++i) {
            Point p0 = curve[depth][i](t);
            Point p1 = curve[depth][i](t+increment);
            if(depth==curve.size()-1){
                DrawLineEx(
                    { .x = p0.x, .y = p0.y },
                    { .x = p1.x, .y = p1.y },
                    1.5f,
                    Color{244, 219, 214, 255}
                );
            }
            else{
                DrawCircleV({.x=p0.x,.y=p0.y}, 0.75f ,Color{54, 58, 79,255});
            }
        }
    }
}

void renderPoint(const std::vector<std::vector<Curve>>& curve, float t) {
    for (size_t depth = 0; depth < curve.size(); ++depth) {
        for (size_t i = 0; i < curve[depth].size(); ++i) {
            Point p0 = curve[depth][i](t);
            std::string text = std::to_string(i) + " (" + std::to_string(static_cast<int>(p0.x)) + "," + std::to_string(static_cast<int>(p0.y)) + ")\n";
                DrawCircleV({.x=p0.x,.y=p0.y},7.5f ,Color{237, 135, 150,255});
                DrawText(text.data(), p0.x+7, p0.y+7, 24, Color{202, 211, 245,255});
        }
    }
}

void print(std::string inputed){
    std::cout << inputed << std::endl;
}

void insertPoint(std::vector<std::vector<Curve>>& vec, const Point& p) {
    auto start = std::chrono::high_resolution_clock::now();
    int depth = vec.size();
    Curve current = constant(p);
    int length = 0;
    for(int i=0;i<depth;i++){
            length = vec[i].size()-1;
            vec[i].push_back(current);
            Curve nextlevel = interpolate(vec[i][length], current);
            current = nextlevel;
    }
    vec.push_back({current});
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration<double, std::milli>(end - start);
    std::cout << "Execution time: " << elapsed.count() << " ms\n";
}

void movePoint(std::vector<std::vector<Curve>>& vec, int index, const Point& p){
    auto start = std::chrono::high_resolution_clock::now();
    int depth = vec.size();
    if(index >= depth) return;
    vec[0][index] = constant(p);
    for(int i = 0; i < depth; i++){
        for(int j = index - i - 1; j < index + 1; j++){
            if(j >= 0 && j < depth - i - 1){
                vec[i + 1][j] = interpolate(vec[i][j], vec[i][j + 1]);
            }
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration<double, std::milli>(end - start);
    std::cout << "Execution time: " << elapsed.count() << " ms\n";
}


void deleteLast(std::vector<std::vector<Curve>>& vec){
    int depth = vec.size();
    for(int i=0;i<depth;i++){
        vec[i].pop_back();
        if(i==depth-1){
            vec.pop_back();
        }
    }
}


int main(){
    std::vector<std::vector<Curve>> curve;
    bool dirty = false;
    Vector2 mousepos;
    int divider = 66;
    int clickedIndex =-1;
    int screenWidth = 1280;
    int screenHeight = 720;
    bool firstLoad = true;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Bézier Curve");
    SetTargetFPS(60);

    while(!WindowShouldClose()){
        if(IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)){
            mousepos = GetMousePosition();
            Point p = {
                .x =mousepos.x,
                .y = mousepos.y
            };
            insertPoint(curve, p);
            dirty = true;
        }

        if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
            mousepos =  GetMousePosition();
            float xpos;
            float ypos;
            if(!curve.empty() && clickedIndex==-1){
                for(int i=0;i<curve[0].size();i++){
                    xpos = curve[0][i](0).x;
                    ypos = curve[0][i](0).y;
                    bool isInside = CheckCollisionPointCircle(mousepos, {.x=xpos,.y=ypos}, 10.0f);
                    if(isInside){
                        clickedIndex = i;
                        break;
                    }
                }
            }
            if(clickedIndex!=-1){
                movePoint(curve, clickedIndex, {.x=mousepos.x,.y=mousepos.y});
            }
            dirty = true;
        }
        if(IsMouseButtonUp(MOUSE_LEFT_BUTTON)){
            clickedIndex=-1;
        }

        if(IsKeyPressed(KEY_UP)){
            divider++;
            dirty = true;
        }
        if(IsKeyPressed(KEY_DOWN)){
            divider--;
            dirty = true;
        }
        if(IsKeyPressed(KEY_C)){
            curve = {};
            dirty = true;
        }
        if(IsKeyPressed(KEY_U)){
            deleteLast(curve);
            dirty = true;
        }

        BeginDrawing();
        if(dirty || firstLoad){
            dirty = false;
            firstLoad = false;
            ClearBackground({24, 25, 38});
            for (int x = 0; x <= GetScreenWidth(); x += 10)
            {
                DrawLine(x, 0, x, GetScreenHeight(), Color{30, 32, 48,255});
            }

            for (int y = 0; y <= GetScreenHeight(); y += 10)
            {
                DrawLine(0, y, GetScreenWidth(), y,  Color{30, 32, 48,255});
            }
            renderPoint(curve, 0);
            for (int i=0;i<divider;i++){
               float tval = (float)i/divider;
               renderTSegment(curve, tval, 1.0f/divider);
            }
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
