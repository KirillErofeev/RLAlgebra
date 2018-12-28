#include <algorithm>
#include <iostream>

#include <cmath>

#include "model/Ball.h"

#include "Strategy.h"

#include "consts.h"

#include "algebra.h"

using namespace model;

Algebra::Algebra(
        const Robot& me, const Rules& rules, const Game& game, Action& action)
    : me(me), rules(rules), game(game), action(action){}

const Robot& Algebra::mate(){
    for(const auto& r : game.robots){
        if(r.is_teammate && (r.id != me.id)){
            return r;
        }
    }
}

template<>
Vec location(const Prediction& p){
    return p.position;
}

template<>
Vec velocity(const Prediction& p){
    return p.velocity;
}

Prediction& Algebra::move(Prediction& p, const Ball& b, double dt){
    p.velocity = velocity(p).clamp(MAX_ENTITY_SPEED);   
    p.position += p.velocity*dt; 
    p.position.y -= GRAVITY*dt*dt/2;
    p.velocity.y -= GRAVITY*dt;

    return p;
}

//Prediction& Algebra::collideArena(Prediction& p, const Ball& b){
//    CI ci = CIToArena(b);
//
//    double penetration = b.radius - ci.distance;
//    if (penetration > 0){
//        p.position = location(b) + (ci.normal * penetration); 
//
//        double v = velocity(b) * ci.normal;
//        if (v < 0)
//            p.velocity = velocity(b) - ci.normal * ((1+rules.BALL_ARENA_E) * v);
//    }
//    
//    return p;
//}

Prediction& Algebra::collideArena(Prediction& p, const Ball& b){
    CI ci = CIToArena(p);

    double penetration = b.radius - ci.distance;
    if (penetration > 0){
        p.position = location(p) + (ci.normal * penetration); 

        double v = velocity(p) * ci.normal;
        std::cout << "rules.BALL_ARENA_E" << v << std::endl;
        std::cout << "rules.BALL_ARENA_E" << velocity(p) << std::endl;
        if (v < 0)
            p.velocity = velocity(p) - ci.normal * ((1+rules.BALL_ARENA_E) * v);
    }
    
    return p;
}


//Is i'm closer to the ball than my mate
bool Algebra::isICloserToBall(){
    return distanceToBall(me) <= distanceToBall(mate());
}

void Algebra::setVelocity(Vec v){
    action.target_velocity_x = v.x;
    action.target_velocity_y = v.y;
    action.target_velocity_z = v.z;
}

void Algebra::goTo(Vec d){
    setVelocity((location(d) - location(me)).maximize());
}

Vec Algebra::toBallVector(){
    return location(game.ball) - location(me);
}

Vec Algebra::toBallGroundVector(){
    Vec v = location(game.ball) - location(me);
    v.y = 0;
    return v;
}

void Algebra::goToBall(){
    setVelocity(toBallGroundVector().maximize());
}

void Algebra::jump(double speed){
    action.jump_speed = speed;
}

void Algebra::goDefCenter(){
    goTo(Vec(0, 0, -0.5*rules.arena.depth));
}

//void Algebra::predictBall(std::vector<Prediction>& ballPredictions, int ticks){
//    Vec pos = location(game.ball);
//    Vec vel = velocity(game.ball);
//    
//    int ct = game.current_tick / TICK_DT + 1;
//    int ticksToCt = ct*TICK_DT - game.current_tick;
//    
//    //predictions[ct].position = 
//}

//Prediction predict(const Ball& ball, int ticks){
//    Prediction p;
//    
//    //p.position = 
//}

Vec Algebra::predictCurVelByVel(const Vec& curVelocity, const Vec& velocity, int ticks){
    //Vec n = (curVelocity - velocity).normalize();
    //velocity + (curVelocity - velocity) * () * ROBOT_ACCELERATION * ticks;
    //return velocity + (curVelocity - velocity) * ROBOT_ACCELERATION * ticks;
}

Vec Algebra::predictPosByVel(const Vec& position, const Vec& velocity, int ticks){
    
    return (velocity*(ticks/TICKS_PER_SECOND)) + position;
}

Vec Algebra::chooseVel(Vec curVel, Vec vel, int ticks){
    if(ticks < ROBOT_MAX_GROUND_SPEED/ROBOT_ACCELERATION*TICKS_PER_SECOND*2)
        return (curVel - vel)/2;
    return vel;

}

void Algebra::predictBall(std::vector<Prediction>& predictions,
                          double dt,//in TICK_DT 
                          double time)
{
    const Ball& b = game.ball;
    int ct = game.current_tick / TICK_DT + 1;
    int ticksToCt = ct*TICK_DT - game.current_tick;

    Prediction p;
    p.position = location(b);
    p.velocity = velocity(b);
    move(p, b, dt*TICK_DT/rules.TICKS_PER_SECOND);
    std::cout << "velocity" << p.velocity << std::endl;
    collideArena(p, b);

    predictions[ct] = p;

    //for(int i = 1; i*dt*TICKS_PER_SECOND<=time; ++i){
    //    predictions[ct].velocity = predictCurVelByVel(predictions[ct+(i-1)*dt].velocity, velocity, ticksToCt);
    //    predictions[ct + i*dt].position = 
    //        predictPosByVel(
    //                predictions[ct+(i-1)*dt].position,
    //                chooseVel(predictions[ct+(i-1)*dt].velocity, 
    //                          predictions[ct+i*dt].velocity, dt*TICK_DT),
    //                dt*TICK_DT);
    //}
    
    
    
}
