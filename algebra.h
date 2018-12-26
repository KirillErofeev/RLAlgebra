#ifndef ALGEBRA_H
#define ALGEBRA_H

#include <iostream>
#include <cmath>

#include "Strategy.h"

using namespace model;

template<class T>
struct Vec3{
    T x;
    T y;
    T z;

    Vec3() : x(0), y(0), z(0){}
    Vec3(T x, T y, T z) : x(x), y(y), z(z){}

    template<class T0>
    Vec3(const T0& v){
        x = v.x;
        y = v.y;
        z = v.z;
    }

    template<class T0>
    void operator=(const T0& v){
        x = v.x;
        y = v.y;
        z = v.z;
    }

    template<class T0>
    Vec3 operator-(const T0& v) const{
        return Vec3(x-v.x, y-v.y, z-v.z);
    }

    template<class T0>
    Vec3& operator-=(const T0& v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    template<class T0>
    Vec3 operator+(const T0& v) const{
        return Vec3(x+v.x, y+v.y, z+v.z);
    }

    template<class T0>
    double operator*(T0& v) const{
        return x*v.x + y*v.y + z*v.z;
    }

    Vec3 operator/(double v) const{
        return Vec3(x/v, y/v, z/v);
    }

    Vec3 operator*(double v) const{
        return Vec3(x*v, y*v, z*v);
    }

    template<class L>
    double distanceTo(L s) const{
        return std::sqrt((x-s.x)*(x-s.x) +
                    (y-s.y)*(y-s.y) +
                    (z-s.z)*(z-s.z));
    }

    double norm() const{
        return std::sqrt(x*x + y*y + z*z);
    }

    Vec3 maximize() const{
        return Vec3(x * 1000,
                    y * 1000,
                    z * 1000);
    }

    Vec3 normalize() const{
        return *this / this->norm();
    }
};

typedef Vec3<double> Vec;

template<class T>
std::ostream& operator<<(std::ostream& out, Vec3<T> v){
    return out << "(" << v.x << "," << v.y << "," << v.z << ")";
}

template<class T>
Vec location(const T& t){
    return Vec(t.x,
               t.y,
               t.z);
}

template<class T>
Vec velocity(const T& t){
    return Vec(t.velocity_x,
               t.velocity_y,
               t.velocity_z);
}

template<class T>
Vec touch_normal(const T& t){
    return Vec(t.touch_normal_x,
               t.touch_normal_y,
               t.touch_normal_z);
}

bool isICloserToBall(const Robot& robot, const Game& game);

struct Prediction{
    Vec position;   
    Vec velocity;   
    double radius;
};

template<class T, class D>
struct CollideInformation{
    D distance;
    Vec3<T> normal;

    CollideInformation(D d, Vec3<T> n) : distance(d), normal(n){};

    bool operator>(CollideInformation<T,D> c) const{
        return distance > c.distance;
    }

    bool operator<(CollideInformation<T,D> c) const{
        return distance < c.distance;
    }

    bool operator<=(CollideInformation<T,D> c) const{
        return distance <= c.distance;
    }

    bool operator>=(CollideInformation<T,D> c) const{
        return distance >= c.distance;
    }
};

typedef CollideInformation<double, double> CI;
struct Algebra{
    const Robot&  me;
    const Rules&  rules;
    const Game&   game;
          Action& action;

    Algebra(const Robot& me, const Rules& rules, const Game& game, Action& action);

    bool isICloserToBall();

    const Robot& mate();

    void goTo(Vec);
    void goToBall();
    void goDefCenter();
    void setVelocity(Vec v);
    void jump(double speed = 1000);

    template<class S>
    double distanceToBall(const S& s){
        return location(game.ball).distanceTo(s);
    }

    Vec toBallVector();
    Vec toBallGroundVector();

    Vec chooseVel(Vec curVel, Vec vel, int ticks);
    Vec predictCurVelByVel(const Vec& curVelocity, const Vec& velocity, int ticks);
    Vec predictPosByVel(const Vec& position, const Vec& velocity, int ticks);
    void predict(
            std::vector<Prediction>& predictions, 
            double dt, double time,
            const Vec& velocity
            );
    void predictBall(
            std::vector<Prediction>& ballPredictions, 
            double dt, double time,
            const Vec& velocity
            );


    template <class T>
    CI CIToPlane(const T& p, const Vec& planeP, const Vec& normalP){
        return CI((p-planeP)*normalP, normalP);
    }
    
    template <class T>
    CI CIToArenaQ(const T& p){
        CI c = CIToPlane(p, Vec(), Vec(0,1,0));
        c = std::min(c, CIToPlane(p, Vec(0,rules.arena.height,0) , Vec(0,-1,0)));
        c = std::min(c, CIToPlane(p, Vec(rules.arena.width/2,0,0), Vec(-1,0,0)));
        Vec t = p; t.z=0;
        t -= Vec(rules.arena.goal_width/2 - rules.arena.goal_top_radius, 
                 rules.arena.goal_height  - rules.arena.goal_top_radius, 0);
        if (p.x >= rules.arena.goal_width/2 + rules.arena.goal_side_radius ||
            p.y >= rules.arena.goal_height  + rules.arena.goal_side_radius ||
            (t.x > 0 && t.y > 0 && t.norm() >= 
             rules.arena.goal_top_radius + rules.arena.goal_side_radius))
            c = std::min(c, CIToPlane(p, Vec(0,0,rules.arena.depth/2), Vec(0,0,-1)));
    
        return c;
    
    }
     
    template <class T>
    CI CIToArena(const T& p){
        bool isXNegative = p.x < 0;    
        bool isZNegative = p.z < 0;    
    
        Vec p0 = p;
        p0.x = std::fabs(p.x);
        p0.z = std::fabs(p.z);
    
        CI c = CIToArenaQ(p0);
    
        if (isXNegative)
            c.normal.x = -c.normal.x;
        if (isZNegative)
            c.normal.z = -c.normal.z;
    
        return c;
    }
    
Prediction collideArena(const Ball& b);
};



#endif
