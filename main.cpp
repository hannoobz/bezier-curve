#include "iostream"
#include <ostream>
#include <string>
#include <vector>
#include "functional"
#include "raylib.h"

typedef struct Point {
    float x;
    float y;
} Point ;

using Curve = std::function<Point(float)>;

Curve interpolate(Curve a, Curve b) {
    return [a, b](float t) {
        Point p1 = a(t);
        Point p2 = b(t);

        return Point{
            .x = p1.x + t * (p2.x - p1.x),
            .y = p1.y + t * (p2.y - p1.y),
        };
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
                DrawLineV({.x=p0.x,.y=p0.y}, {.x=p1.x,.y=p1.y}, PINK);
            }
        }
    }
}

void renderPoint(const std::vector<std::vector<Curve>>& curve, float t) {
    for (size_t depth = 0; depth < curve.size(); ++depth) {
        for (size_t i = 0; i < curve[depth].size(); ++i) {
            Point p0 = curve[depth][i](t);
                DrawCircleV({.x=p0.x,.y=p0.y},7.5f ,MAGENTA);
        }
    }
}

void print(std::string inputed){
    std::cout << inputed << std::endl;
}

void insertPoint(std::vector<std::vector<Curve>>& vec, const Point& p) {
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
}

void movePoint(std::vector<std::vector<Curve>>& vec, int index, const Point& p){
    int depth = vec.size();
    if(index>=depth) return;
    vec[0][index] = constant(p);
    for(int i=0;i<depth;i++){
        for(int j=index-i-1;j<index+1; j++){
            if(j>=0 && j<depth-i-1){
                vec[i+1][j] = interpolate(vec[i][j], vec[i][j+1]);
            }
        }
    }
}

// void testIteratePoint(int depth,int index){
//     for(int i=0;i<depth;i++){
//         for(int j=index-i-1;j<index+1; j++){
//             if(j>=0 && j<depth-i-1){
//                 std::cout << j << " " ;
//             }
//         }
//         std::cout << std::endl;
//     }
// }



int main(){

    std::vector<std::vector<Curve>> curve;
    Vector2 mousepos;
    int divider = 10;

    InitWindow(800, 800,"Bezier Curve");
    SetTargetFPS(60);

    while(!WindowShouldClose()){
        if(IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)){
            mousepos = GetMousePosition();
            Point p = {
                .x =mousepos.x,
                .y = mousepos.y
            };
            insertPoint(curve, p);
        }

        if(IsKeyDown(KEY_UP)){
            divider += 1;
        }
        if(IsKeyDown(KEY_DOWN)){
            if(divider>1)divider -= 1;
        }

        BeginDrawing();
        ClearBackground(WHITE);
        renderPoint(curve, 0);
        for (int i=0;i<divider;i++){
           float tval = (float)i/divider;
           renderTSegment(curve, tval, 1.0f/divider);
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
