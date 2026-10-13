#include "iostream"
#include <ostream>
#include <string>
#include <vector>
#include "functional"
// #include "raylib.h""

typedef struct Point {
    double x;
    double y;
} Point ;

using Curve = std::function<Point(double)>;

Curve interpolate(Curve a, Curve b) {
    return [a, b](double t) {
        Point p1 = a(t);
        Point p2 = b(t);

        return Point{
            .x = p1.x + t * (p2.x - p1.x),
            .y = p1.y + t * (p2.y - p1.y),
        };
    };
}

Curve constant(Point p) {
    return [p](double) {
        return p;
    };
}

// AI GENERATED
void printer(const std::vector<std::vector<Curve>>& curve, double t) {
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
    Point p1 = {
        .x = 0, .y=0,
    };
    Point p2 = {
        .x = 0, .y=8,
    };

    Point p3 = {
        .x = 5, .y=1,
    };
    Point p4 = {
        .x = 9, .y=9,
    };
    Point p5 = {
        .x = 5, .y=2.1,
    };
    Point p6 = {
        .x = 8, .y=0,
    };

    std::vector<std::vector<Curve>> curve;
    insertPoint(curve,p1);
    insertPoint(curve,p2);
    insertPoint(curve,p3);
    insertPoint(curve,p4);
    movePoint(curve, 2, p5);
    printer(curve,0.5);
    movePoint(curve, 2, p3);
    printer(curve,0.5);
    movePoint(curve, 0, p6);
    printer(curve,1);
    return 0;
}
