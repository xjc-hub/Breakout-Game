#include <iostream>
#include \"raylib.h\"

int main() {
    // 测试CheckCollisionCircleRec函数
    Vector2 circlePos = {100, 100};
    float radius = 10;
    
    // 测试1：完全包含
    Rectangle rect1 = {95, 95, 10, 10};
    bool test1 = CheckCollisionCircleRec(circlePos, radius, rect1);
    std::cout << \"测试1（完全包含）: \" << (test1 ? \"通过\" : \"失败\") << std::endl;
    
    // 测试2：不碰撞
    Rectangle rect2 = {200, 200, 10, 10};
    bool test2 = !CheckCollisionCircleRec(circlePos, radius, rect2);
    std::cout << \"测试2（不碰撞）: \" << (test2 ? \"通过\" : \"失败\") << std::endl;
    
    // 测试3：边缘碰撞
    Rectangle rect3 = {105, 100, 10, 10};
    bool test3 = CheckCollisionCircleRec(circlePos, radius, rect3);
    std::cout << \"测试3（边缘碰撞）: \" << (test3 ? \"通过\" : \"失败\") << std::endl;
    
    return 0;
}
