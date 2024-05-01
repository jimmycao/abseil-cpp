#include <bits/stdc++.h>

#include <gtest/gtest.h>

#include "log.h"
#include "factory.h"

namespace mlf {

class Shape {
public:
    Shape() = default;
    virtual ~Shape() = default;

    float get_area() {
        return _area;
    }

protected:
    float _area;
};

class Rectangle : public Shape {
public:
    Rectangle() = default;

    void init(float length, float width) {
        _length = length; 
        _width = width;
        _area = _length * _width;
    }

private:
    float _length;
    float _width;
};

class Square : public Shape {
public:
    Square() = default; 
    
    void init(float edge) {
        _edge = edge;
        _area = _edge * _edge;
    }
        
private:
    float _edge;
};

class Circle : public Shape {
public:
    Circle() = default;

    void init(float radius) {
        _radius = radius;
        _area = 3.14 * _radius * _radius;
    }
        
private:
    float _radius;
};

TEST(FactoryTest, add_get_all_names) {
    Factory<Shape>::instance().add<Rectangle>("rectange_1");
    Factory<Shape>::instance().add<Square>("square_1");
    Factory<Shape>::instance().add<Circle>("circle_1");

    std::vector<std::string> all_names = Factory<Shape>::instance().get_all_names();
    std::sort(all_names.begin(), all_names.end());

    std::vector<std::string> shapes {"rectange_1", "square_1", "circle_1"};
    std::sort(shapes.begin(), shapes.end());

    EXPECT_EQ(shapes, all_names);
}

TEST(FactoryTest, exist) {
    Factory<Shape>::instance().add<Rectangle>("rectange_2");
    EXPECT_TRUE(Factory<Shape>::instance().exist("rectange_2"));
}

TEST(FactoryTest, produce) {
    Factory<Shape>::instance().add<Circle>("circle_2");
    std::shared_ptr<Circle> c = Factory<Shape>::instance().produce<Circle>("circle_2");
    float r = 10;
    c->init(r);
    EXPECT_FLOAT_EQ(3.14 * r * r, c->get_area());
}

} //namespace mlf

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    mlf::init_glog_conf();
    int rt = RUN_ALL_TESTS();
    return rt;
}
