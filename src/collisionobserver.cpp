#include "collisionobserver.h"
#include "rect.h"
#include "ball.h"
#include "subject.h"

#include <cmath>

CollisionObserver::CollisionObserver() {}

void CollisionObserver::onNotify(Subject& entity, Event event)
{
    if (event == Event::SUBJECT_MOVED)
        checkCollisions(dynamic_cast<Ball*>(&entity));
}

double CollisionObserver::distanceSquared(double x1, double y1, double x2, double y2)
{
    double deltaX = x2 - x1;
    double deltaY = y2 - y1;

    return deltaX*deltaX + deltaY*deltaY;
}

// Check if 2 balls collided
bool CollisionObserver::collided(Ball* b, Ball* b2)
{
    double totalRadiusSquared = b->posData.radius + b2->posData.radius;
    totalRadiusSquared = totalRadiusSquared * totalRadiusSquared;

    return distanceSquared(b->posData.pos_x, b->posData.pos_y, b2->posData.pos_x, b2->posData.pos_y) < totalRadiusSquared;
}

// Check is Ball* b collided with Rect* r
bool CollisionObserver::collided(Ball* b, Rect* r)
{
    //Closest point on collision box
    auto closer = find_closer(b, r);

    return distanceSquared(closer.first, closer.second, b->posData.pos_x, b->posData.pos_y) < b->posData.radius * b->posData.radius;
}

// Check collision of this Ball* b with every subject watched
void CollisionObserver::checkCollisions(Ball* b)
{
    if (b == nullptr) return;

    for (Subject* sub: observed_)
    {
        Ball* b2 = dynamic_cast<Ball*>(sub);
        if (b2 != nullptr)
        {
            if (b2 != b)
            {
                if (collided(b, b2))
                {
                    // std::cout << "2 balls collided!" << std::endl;
                    fix(b, b2);

                    if (b2->id == 999) // Side of the pocket
                        b->notify(Event::SUBJECT_WALL_COLLIDED);
                    else // Other ball
                        b->notify(Event::SUBJECT_BALL_COLLIDED);

                    // Set first_hit
                    if (first_hit == -1 && b2->id != 999)
                        first_hit = b2->id;
                }
            }
        }
        else
        {
            Rect* r = dynamic_cast<Rect*>(sub);
            if (r != nullptr)
            {
                if (collided(b, r))
                {
                    // std::cout << "Collided with wall!!" << std::endl;
                    fix(b, r);
                    b->notify(Event::SUBJECT_WALL_COLLIDED);
                }
            }
        }
    }
}

// Find closest point on collision box
std::pair<int, int> CollisionObserver::find_closer(Ball* b, Rect* rec)
{
    int cX, cY;

    //Find closest x offset
    if(b->posData.pos_x < rec->rect.x)
        cX = rec->rect.x;
    else if(b->posData.pos_x > rec->rect.x + rec->rect.w )
        cX = rec->rect.x + rec->rect.w;
    else
        cX = static_cast<int>(b->posData.pos_x);

    //Find closest y offset
    if(b->posData.pos_y < rec->rect.y)
        cY = rec->rect.y;
    else if( b->posData.pos_y > rec->rect.y + rec->rect.h )
        cY = rec->rect.y + rec->rect.h;
    else
        cY = static_cast<int>(b->posData.pos_y);

    return {cX, cY};
}

// Fix movement of the ball when collided with wall
void CollisionObserver::fix(Ball* b, Rect* rec)
{
    auto closer = find_closer(b, rec);
    int cX = closer.first, cY = closer.second;

    if (b->movData.speed_y > 0)
    {
        if (cY == rec->rect.y) // collided from above
        {
            b->movData.speed_y *= -1;
            b->movData.speed_y *= 0.8;
            b->posData.pos_y = cY - b->posData.radius;
        }
        else // collided from side
        {
            b->movData.speed_x *= -1;
            b->movData.speed_x *= 0.8;
            if (b->movData.speed_x > 0) // from left side
                b->posData.pos_x = cX + b->posData.radius;
            else // from right side
                b->posData.pos_x = cX - b->posData.radius;
        }
    }
    else
    {
        if (cY == rec->rect.y + rec->rect.h) //collided from bellow
        {
            b->movData.speed_y *= -1;
            b->movData.speed_y *= 0.8;
            b->posData.pos_y = cY + b->posData.radius;
        }
        else // collided from side
        {
            b->movData.speed_x *= -1;
            b->movData.speed_x *= 0.8;

            if (b->movData.speed_x > 0) // from left side
                b->posData.pos_x = cX + b->posData.radius;
            else // from right side
                b->posData.pos_x = cX - b->posData.radius;
        }
    }
}

// Anonymous namespace
namespace  {

//******************************************************************************
//   This program is a 'remote' 2D-collision detector for two balls on linear
//   trajectories and returns, if applicable, the location of the collision for
//   both balls as well as the new velocity vectors (assuming a partially elastic
//   collision as defined by the restitution coefficient).
//   The equations on which the code is based have been derived at
//   http://www.plasmaphysics.org.uk/collision2d.htm
//
//   In  'f' (free) mode no positions but only the initial velocities
//   and an impact angle are required.
//   All variables apart from 'mode' and 'error' are of Double Precision
//   Floating Point type.
//
//   The Parameters are:
//
//    mode  (char) (if='f' alpha must be supplied; otherwise arbitrary)
//    alpha (impact angle) only required in mode='f';
//                     should be between -PI/2 and PI/2 (0 = head-on collision))
//    R    (restitution coefficient)  between 0 and 1 (1=perfectly elastic collision)
//    m1   (mass of ball 1)
//    m2   (mass of ball 2)
//    r1   (radius of ball 1)        not needed for 'f' mode
//    r2   (radius of ball 2)                "
//  & x1   (x-coordinate of ball 1)          "
//  & y1   (y-coordinate of ball 1)          "
//  & x2   (x-coordinate of ball 2)          "
//  & y2   (y-coordinate of ball 2)          "
//  & vx1  (velocity x-component of ball 1)
//  & vy1  (velocity y-component of ball 1)
//  & vx2  (velocity x-component of ball 2)
//  & vy2  (velocity y-component of ball 2)
//  & error (int)  (0: no error
//                  1: balls do not collide
//                  2: initial positions impossible (balls overlap))
//
//   Note that the parameters with an ampersand (&) are passed by reference,
//   i.e. the corresponding arguments in the calling program will be updated;
//   however, the coordinates and velocities will only be updated if 'error'=0.
//
//   All variables should have the same data types in the calling program
//   and all should be initialized before calling the function even if
//   not required in the particular mode.
//
//   This program is free to use for everybody. However, you use it at your own
//   risk and I do not accept any liability resulting from incorrect behaviour.
//   I have tested the program for numerous cases and I could not see anything
//   wrong with it but I can not guarantee that it is bug-free under any
//   circumstances.
//
//   I would appreciate if you could report any problems to me
//   (for contact details see  http://www.plasmaphysics.org.uk/feedback.htm ).
//
//   Thomas Smid, January  2004
//                December 2005 (corrected faulty collision detection;
//                               a few minor changes to improve speed;
//                               added simplified code without collision detection)
//                December 2009 (generalization to partially inelastic collisions)
//*********************************************************************************

// https://www.plasmaphysics.org.uk/programs/coll2d_cpp.htm
void collision2Ds(double m1, double m2, double R,
                  double x1, double y1, double x2, double y2,
                  double& vx1, double& vy1, double& vx2, double& vy2)
{
    double  m21,dvx2,a,x21,y21,vx21,vy21,fy21,sign,vx_cm,vy_cm;

    m21=m2/m1;
    x21=x2-x1;
    y21=y2-y1;
    vx21=vx2-vx1;
    vy21=vy2-vy1;

    vx_cm = (m1*vx1+m2*vx2)/(m1+m2) ;
    vy_cm = (m1*vy1+m2*vy2)/(m1+m2) ;


    //     *** return old velocities if balls are not approaching ***
    if ( (vx21*x21 + vy21*y21) >= 0) return;


    //     *** I have inserted the following statements to avoid a zero divide;
    //         (for single precision calculations,
    //          1.0E-12 should be replaced by a larger value). **************

    fy21=1.0E-12*std::fabs(y21);
    if ( std::fabs(x21)<fy21 ) {
        if (x21<0) { sign=-1; }
        else { sign=1;}
        x21=fy21*sign;
    }

    //     ***  update velocities ***
    a=y21/x21;
    dvx2= -2*(vx21 +a*vy21)/((1+a*a)*(1+m21)) ;
    vx2=vx2+dvx2;
    vy2=vy2+a*dvx2;
    vx1=vx1-m21*dvx2;
    vy1=vy1-a*m21*dvx2;

    //     ***  velocity correction for inelastic collisions ***
    vx1=(vx1-vx_cm)*R + vx_cm;
    vy1=(vy1-vy_cm)*R + vy_cm;
    vx2=(vx2-vx_cm)*R + vx_cm;
    vy2=(vy2-vy_cm)*R + vy_cm;

    return;
}

} // End of Anonymous namespace

void CollisionObserver::fix(Ball* b, Ball* b2)
{
    collision2Ds(b->mass, b2->mass, 0.92, b->posData.pos_x, b->posData.pos_y,
                 b2->posData.pos_x, b2->posData.pos_y, b->movData.speed_x, b->movData.speed_y,
                 b2->movData.speed_x, b2->movData.speed_y);
}


