#include "Boid.h"
#include <iostream>
#include <cstdlib>
#include <cmath>
/*
Add functions needed to create a working flock.
*/

float generateRandom(float x, float y){
  return x + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(y-x)));
}

Boid::Boid(
  int x,
  int y,
  int xbound,
  int ybound,
  int mboundaryPadding,
  float mmaxSpeed,
  float mmaxForce,
  float mflockSepWeight,
  float mflockAliWeight,
  float mflockCohWeight,
  float mflockSepRadius,
  float mflockAliRadius,
  float mflockCohRadius
){
    acc.setval(0, 0);

    orient = (float)randomRange(0, 360);
    vel.setval(cos(orient), sin(orient));
    loc.setval(x,y);

    r = 2.0;
    endCorner.setval(xbound,ybound);
    reachedDestination = false;
    hitObstacle = false;

    boundaryPadding = mboundaryPadding;
    maxSpeed = mmaxSpeed;
    maxForce = mmaxForce;
    flockSepWeight = mflockSepWeight;
    flockAliWeight = mflockAliWeight;
    flockCohWeight = mflockCohWeight;
    flockSepRadius = mflockSepRadius;
    flockAliRadius = mflockAliRadius;
    flockCohRadius = mflockCohRadius;
}

// Method to update location
bool Boid::update(vector<Boid> &boids, Vec2f destination) {

	flock(boids, destination);

  vel += acc;
  vel.limit(maxSpeed);
  loc += vel;
  acc.setval(0,0);  // Resetval accelertion to 0 each cycle
  orient = (float)atan2(vel.y,vel.x) * 180/PI;
  boundCheck(boundaryPadding);

  if (dist(loc, destination) < 5.0f)
    reachedDestination = true;

  return reachedDestination;
}

void Boid::boundCheck(int padding){

  if(loc.x>endCorner.x-padding)
  {
      loc.x=endCorner.x-padding;
      vel.x=-vel.x;

  }

  else if(loc.x<0+padding)
   {
       loc.x=0+padding;
       vel.x=-vel.x;
   }

  if(loc.y>endCorner.y-padding)
  {
      loc.y=endCorner.y-padding;
      vel.y=-vel.y;
  }

  else if(loc.y<0+padding)
  {
      loc.y=0+padding;
      vel.y=-vel.y;
  }
}

void Boid::applyForce(Vec2f force){
    acc += force;
}

Vec2f Boid::seek(Vec2f target){
  Vec2f desired = target - loc;

  desired.normalize();
  desired *= maxSpeed;

  Vec2f steer = desired - vel;
  steer.limit(maxForce);
  return steer;
}

Vec2f Boid::separate(vector<Boid> &boids){
  Vec2f steer(0.0, 0.0);
  int neighbourSeparationCount = 0;

  for(int i = 0; i < boids.size(); i++){
    float distance = dist(loc, boids[i].loc);

    if(distance > 0.0f && distance < flockSepRadius){
      Vec2f difference = loc - boids[i].loc;
      difference.normalize();
      steer += difference;
      neighbourSeparationCount++;
    }
  }

  if (neighbourSeparationCount > 0)
    steer /= neighbourSeparationCount;

  if (steer.length() > 0){
    steer.normalize();
    steer *= maxSpeed;
    steer -= vel;
    steer.limit(maxForce);
  }

  return steer;
}

Vec2f Boid::align(vector<Boid> &boids){
  Vec2f sum(0.0f, 0.0f);
  int neighbourAlignmentCount = 0;

  for(int i = 0; i < boids.size(); i++){
    float distance = dist(loc, boids[i].loc);

    if(distance > 0.0f && distance < flockAliRadius){
      sum += boids[i].vel;
      neighbourAlignmentCount++;
    }
  }

  if(neighbourAlignmentCount > 0){
    sum /= neighbourAlignmentCount;
    sum.normalize();
    sum *= maxSpeed;

    Vec2f steer = sum - vel;
    steer.limit(maxForce);
    return steer;
  }
  else
    return Vec2f(0.0, 0.0);
}

Vec2f Boid::cohesion(vector<Boid> &boids){
  Vec2f sum(0.0f, 0.0f);
  int neighbourCohesionCount = 0;

  for(int i = 0; i < boids.size(); i++){
    float distance = dist(loc, boids[i].loc);

    if(distance > 0.0f && distance < flockCohRadius){
      sum += boids[i].loc;
      neighbourCohesionCount++;
    }
  }

  if (neighbourCohesionCount > 0){
    sum /= neighbourCohesionCount;
    return seek(sum);
  }
  else
    return Vec2f(0.0, 0.0);
}

//Flock your boids here
void Boid::flock(vector<Boid> &boids, Vec2f destination){
  Vec2f separationVector = separate(boids);
  Vec2f alignmentVector = align(boids);
  Vec2f cohesionVector = cohesion(boids);

  separationVector *= flockSepWeight;
  alignmentVector *= flockAliWeight;
  cohesionVector *= flockCohWeight;

  applyForce(separationVector);
  applyForce(alignmentVector);
  applyForce(cohesionVector);
  applyForce((destination - loc) * 0.00001);
}

bool Boid::isHit(int x, int y, int radius) {

    int range = 1;//calculation error range
    int dist = radius + range;
    if(pow((x-loc.x),2)+pow((y-loc.y),2) < (dist * dist) ){
        return true;
    }
    return false;
}


float Boid::dist(Vec2f v1,Vec2f v2)
{
    return v1.distance(v2);
}

float Boid::clamp(float val, float minval, float maxval)
{
    if(val<minval)
        return minval;
    else if (val>maxval)
        return maxval;
    else
        return val;
}
