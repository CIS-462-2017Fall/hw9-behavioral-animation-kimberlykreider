#include "aBehaviors.h"

#include <math.h>
#include "GL/glew.h"
#include "GL/glut.h"

// Base Behavior
///////////////////////////////////////////////////////////////////////////////
Behavior::Behavior()
{
}

Behavior::Behavior( char* name) 
{
	m_name = name;
	m_pTarget = NULL;
}

Behavior::Behavior( Behavior& orig) 
{
	m_name = orig.m_name;
	m_pTarget = NULL;
}

string& Behavior::GetName() 
{
    return m_name;
}

// Behaviors derived from Behavior
//----------------------------------------------------------------------------//
// Seek behavior
///////////////////////////////////////////////////////////////////////////////
// For the given the actor, return a desired velocity in world coordinates
// Seek returns a maximum velocity towards the target
// m_pTarget contains target world position
// actor.getPosition() returns Agent's world position


Seek::Seek( AJoint* target) 
{
	m_name = "seek";
	m_pTarget = target;

}

Seek::Seek( Seek& orig) 
{
	m_name = "seek";
	m_pTarget = orig.m_pTarget;
}


Seek::~Seek()
{
}

vec3 Seek::calcDesiredVel( BehaviorController* actor)
{
	vec3 Vdesired = vec3(0.0, 0.0, 0.0);
	vec3 targetPos = m_pTarget->getLocalTranslation();
	vec3 actorPos = actor->getPosition();

	// TODO: add your code here to compute Vdesired
	//seek returns a maximum velocity towards the target
	vec3 pathToTarget = targetPos - actorPos; //this is the direction of the velocity
	pathToTarget.Normalize();
	Vdesired = pathToTarget * BehaviorController::gMaxSpeed;

	return Vdesired;
}


// Flee behavior
///////////////////////////////////////////////////////////////////////////////
// For the given the actor, return a desired velocity in world coordinates
// Flee calculates a a maximum velocity away from the target
// m_pTarget contains target world position
// actor.getPosition() returns Agent's world position

Flee::Flee( AJoint* target) 
{
	m_name = "flee";
	m_pTarget = target;
}

Flee::Flee( Flee& orig) 
{
	m_name = "flee";
	m_pTarget = orig.m_pTarget;
}

Flee::~Flee()
{
}

vec3 Flee::calcDesiredVel( BehaviorController* actor)
{
	vec3 Vdesired = vec3(0.0, 0.0, 0.0);
	vec3 targetPos = m_pTarget->getLocalTranslation();
	vec3 actorPos = actor->getPosition();
	// TODO: add your code here to compute Vdesired
	// flee returns maximum velocity away from target
	vec3 pathFromTarget = actorPos - targetPos; //this is the direction of the velocity
	pathFromTarget.Normalize();
	Vdesired = pathFromTarget * BehaviorController::gMaxSpeed;

	return Vdesired;

}

// Arrival behavior
///////////////////////////////////////////////////////////////////////////////
// Given the actor, return a desired velocity in world coordinates
// Arrival returns a desired velocity vector whose speed is proportional to
// the actors distance from the target
// m_pTarget contains target world position
// actor.getPosition() returns Agent's world position
//  Arrival strength is in BehavioralController::KArrival


Arrival::Arrival( AJoint* target) 
{
	m_name = "arrival";
	m_pTarget = target;
}

Arrival::Arrival( Arrival& orig) 
{
	m_name = "arrival";
	m_pTarget = orig.m_pTarget;
}

Arrival::~Arrival()
{
}

vec3 Arrival::calcDesiredVel( BehaviorController* actor)
{
	vec3 Vdesired = vec3(0.0, 0.0, 0.0);
	vec3 targetPos = m_pTarget->getLocalTranslation();
	vec3 actorPos = actor->getPosition();

	// TODO: add your code here to compute Vdesired
	// Arrival returns a desired velocity vector whose speed is proportional to
	// the actors distance from the target
	vec3 pathToTarget = targetPos - actorPos;
	double distance = pathToTarget[0] * pathToTarget[0] + pathToTarget[1] * pathToTarget[1] + pathToTarget[2] * pathToTarget[2];
	distance = sqrt(distance);
	pathToTarget.Normalize();
	Vdesired = BehaviorController::KArrival * pathToTarget / distance;

	return Vdesired;
}


// Departure behavior
///////////////////////////////////////////////////////////////////////////////
// Given the actor, return a desired velocity in world coordinates
// Arrival returns a desired velocity vector whose speed is proportional to
// 1/(actor distance) from the target
// m_pTarget contains target world position
// actor.getPosition() returns Agent's world position
//  Departure strength is in BehavioralController::KDeparture

Departure::Departure(AJoint* target) 
{
	m_name = "departure";
	m_pTarget = target;
}

Departure::Departure( Departure& orig) 
{
	m_name = "departure";
	m_pTarget = orig.m_pTarget;
}

Departure::~Departure()
{
}

vec3 Departure::calcDesiredVel( BehaviorController* actor)
{
	vec3 Vdesired = vec3(0.0, 0.0, 0.0);
	vec3 targetPos = m_pTarget->getLocalTranslation();
	vec3 actorPos = actor->getPosition();

	// TODO: add your code here to compute Vdesired
	// Departure returns a desired velocity vector whose speed is proportional to
	// 1/(actor distance) from the target

	vec3 pathToTarget = targetPos - actorPos;
	double distance = pathToTarget[0] * pathToTarget[0] + pathToTarget[1] * pathToTarget[1] + pathToTarget[2] * pathToTarget[2];
	distance = sqrt(distance);
	pathToTarget.Normalize();
	Vdesired = BehaviorController::KDeparture * pathToTarget * distance;


	return Vdesired;
}


// Avoid behavior
///////////////////////////////////////////////////////////////////////////////
//  For the given the actor, return a desired velocity in world coordinates
//  If an actor is near an obstacle, avoid adds a normal response velocity to the 
//  the desired velocity vector computed using arrival
//  Agent bounding sphere radius is in BehavioralController::radius
//  Avoidance parameters are  BehavioralController::TAvoid and BehavioralController::KAvoid

Avoid::Avoid(AJoint* target, vector<Obstacle>* obstacles) 
{
	m_name = "avoid";
	m_pTarget = target;
	mObstacles = obstacles;
}

Avoid::Avoid( Avoid& orig) 
{
	m_name = "avoid";
	m_pTarget = orig.m_pTarget;
	mObstacles = orig.mObstacles;
}

Avoid::~Avoid()
{
}

vec3 Avoid::calcDesiredVel( BehaviorController* actor)
{

	vec3 Vdesired = vec3(0.0, 0.0, 0.0);
	m_actorPos = actor->getPosition();
	m_actorVel = actor->getVelocity();
	vec3 targetPos = m_pTarget->getLocalTranslation();

	//TODO: add your code here
	vec3 Varrival(0, 0, 0);
	// Step 1. compute initial value for Vdesired = Varrival so agent moves toward target
	vec3 pathToTarget = targetPos - m_actorPos;
	double distance = pathToTarget[0] * pathToTarget[0] + pathToTarget[1] * pathToTarget[1] + pathToTarget[2] * pathToTarget[2];
	distance = sqrt(distance);
	pathToTarget.Normalize();
	Varrival = BehaviorController::KArrival * pathToTarget / distance;

	vec3 Vavoid(0, 0, 0);
	//TODO: add your code here to compute Vavoid 
	//adds a normal response velocity: 90 degrees away from Varrival, rotate about y axis 90 degrees
	// - z y x
	Vavoid[0] = -Varrival[2];
	Vavoid[2] = Varrival[0];
	Vavoid.Normalize();

	// Step 2. compute Lb
	//TODO: add your code here
	double lb = BehaviorController::TAvoid * m_actorVel.Length();


	// Step 3. find closest obstacle 
	//TODO: add your code here
	double minDistance = INT_MAX;
	Obstacle *closestObstacle;
	for (int i = 0; i < mObstacles->size(); i++) {
		Obstacle obstacle = mObstacles->at(i);
		vec3 obstaclePos = obstacle.m_Center.getGlobalTranslation(); //position of obstacle in world coordinates
		vec3 distance = obstaclePos - m_actorPos;
		double dist = distance.Length();
		if (dist < minDistance) {
			minDistance = dist;
			closestObstacle = &obstacle;
		}
	}


	// Step 4. determine whether agent will collide with closest obstacle (only consider obstacles in front of agent)
	//TODO: add your code here
	//dworld = p obstacle - p
	vec3 dworld = closestObstacle->m_Center.getGlobalTranslation() - m_actorPos;
	//convert this to body coordinates
	mat3 rotationB0 = actor->getGuide().getLocalRotation(); //this is the rotation of b
	rotationB0 = rotationB0.Inverse();
	vec3 dbody = rotationB0 * dworld;
	double dx = abs(dbody[0]);
	if (dx > lb + actor->gAgentRadius + closestObstacle->m_Radius) {
		//make sure it's also the right direction
		vec3 actorDirection = closestObstacle->m_Center.getGlobalTranslation() - m_actorPos;
		if (!((actorDirection[0] > 0 && m_actorVel[0] > 0) || (actorDirection[0] < 0 && m_actorVel[1] < 0))) {
			return Varrival;
		}
	}

	// Step 5.  if potential collision detected, compute Vavoid and set Vdesired = Varrival + Vavoid
	//TODO: add your code here

	Vavoid = BehaviorController::KAvoid * ((actor->gAgentRadius + closestObstacle->m_Radius - abs(dbody[2])) / (actor->gAgentRadius + closestObstacle->m_Radius));


	return Vdesired;
	
}

void Avoid::display( BehaviorController* actor)
{
	//  Draw Debug info
	vec3 angle = actor->getOrientation();
	vec3 vel = actor->getVelocity();
	vec3 dir = vec3(cos(angle[1]), 0, sin(angle[1]));
	vec3 probe = dir * (vel.Length()/BehaviorController::gMaxSpeed)*BehaviorController::TAvoid;
	
	glBegin(GL_LINES);
	glColor3f(0, 0, 1);
	glVertex3f(m_actorPos[0], m_actorPos[1], m_actorPos[2]);
	glVertex3f(m_obstaclePos[0], m_obstaclePos[1], m_obstaclePos[2]);
	glColor3f(0, 1, 1);
	glVertex3f(m_actorPos[0], m_actorPos[1], m_actorPos[2]);
	glVertex3f(m_actorPos[0] + probe[0], m_actorPos[1] + probe[1], m_actorPos[2] + probe[2]);
	glEnd();
}


// Wander Behavior
///////////////////////////////////////////////////////////////////////////////
// For the given the actor, return a desired velocity in world coordinates
// Wander returns a desired velocity vector whose direction changes at randomly from frame to frame
// Wander strength is in BehavioralController::KWander

Wander::Wander() 
{
	m_name = "wander";
	m_Wander = vec3(1.0, 0.0, 0.0);
}

Wander::Wander( Wander& orig) 
{
	m_name = "wander";
	m_Wander = orig.m_Wander;
}

Wander::~Wander()
{
}

vec3 Wander::calcDesiredVel( BehaviorController* actor)
{
	vec3 Vdesired = vec3(0.0, 0.0, 0.0);
	vec3 actorPos = actor->getPosition();

	// compute Vdesired = Vwander

	// Step. 1 find a random direction
	//TODO: add your code here
	int x = rand();
	if (x % 2 == 0) {
		x = -x;
	}
	int z = rand();
	if (z % 2 == 0) {
		z = -z;
	}

	vec3 direction = { (double) x, 0, (double) z };
	direction.Normalize();


	// Step2. scale it with a noise factor
	//TODO: add your code here
	
	direction = BehaviorController::KNoise * direction;


	// Step3. change the current Vwander  to point to a random direction
	//TODO: add your code here

	m_Wander = m_Wander + direction;
	m_Wander.Normalize();

	// Step4. scale the new wander velocity vector and add it to the nominal velocity
	//TODO: add your code here

	Vdesired = BehaviorController::KWander * m_Wander;
	Vdesired[0] += 1;
	return Vdesired;
}


// Alignment behavior
///////////////////////////////////////////////////////////////////////////////
// For the given the actor, return a desired velocity vector in world coordinates
// Alignment returns the average velocity of all active agents in the neighborhood
// agents[i] gives the pointer to the ith agent in the environment
// Alignment parameters are in BehavioralController::RNeighborhood and BehavioralController::KAlign


Alignment::Alignment(AJoint* target, vector<AActor>* agents) 
{
	m_name = "alignment";
	m_pAgentList = agents;
	m_pTarget = target;
}



Alignment::Alignment( Alignment& orig) 
{
	m_name = orig.m_name;
	m_pAgentList = orig.m_pAgentList;
	m_pTarget = orig.m_pTarget;

}

Alignment::~Alignment()
{
}

vec3 Alignment::calcDesiredVel( BehaviorController* actor)
{
	vec3 Vdesired = vec3(0.0, 0.0, 0.0);
	vec3 targetPos = m_pTarget->getLocalTranslation();
	vec3 actorPos = actor->getPosition();
	vector<AActor>& agentList = *m_pAgentList;
	

	// compute Vdesired 
	
	// Step 1. compute value of Vdesired for fist agent (i.e. m_AgentList[0]) using an arrival behavior so it moves towards the target
	 
	BehaviorController* leader = agentList[0].getBehaviorController(); // first agent is the leader
	//TODO: add your code here
	//add all the neighbors velocities
	if (leader == actor) {
		vec3 pathToTarget = targetPos - actorPos;
		double distance = pathToTarget[0] * pathToTarget[0] + pathToTarget[1] * pathToTarget[1] + pathToTarget[2] * pathToTarget[2];
		distance = sqrt(distance);
		pathToTarget.Normalize();
		Vdesired = pathToTarget / distance;
		return Vdesired;
	}

	// Step 2. if not first agent compute Valign as usual
	//TODO: add your code here
	vec3 combinedVelocity;
	int numAgents = 0;
	for (int i = 0; i < agentList.size(); i++) {
		AActor agent = agentList[i];
		vec3 agentVel = agent.getBehaviorController()->getVelocity();
		combinedVelocity += agentVel;
		numAgents++;
	}
	
	Vdesired = combinedVelocity / numAgents;
	
	return Vdesired;
}

// Separation behavior
///////////////////////////////////////////////////////////////////////////////
// For the given te actor, return a desired velocity vector in world coordinates
// Separation tries to maintain a constant distance between all agents
// within the neighborhood
// agents[i] gives the pointer to the ith agent in the environment
// Separation settings are in BehavioralController::RNeighborhood and BehavioralController::KSeperate

 

Separation::Separation( AJoint* target,  vector<AActor>* agents) 
{
	m_name = "separation";
	m_AgentList = agents;
	m_pTarget = target;
}

Separation::~Separation()
{
}

Separation::Separation( Separation& orig) 
{
	m_name = "separation";
	m_AgentList = orig.m_AgentList;
	m_pTarget = orig.m_pTarget;
}

vec3 Separation::calcDesiredVel( BehaviorController* actor)
{
	vec3 Vdesired = vec3(0.0, 0.0, 0.0);
	vec3 targetPos = m_pTarget->getLocalTranslation();
	vec3 actorPos = actor->getPosition();
	vector<AActor>& agentList = *m_AgentList;
	
	// compute Vdesired = Vseparate
	// TODO: add your code here to compute Vdesired 
	
	//weighted average of departure-like repulsive velocities in neighborhood
	//V separate = K separate * sum (wi * (di / length di 2))
	//w is based on radius?
	for (int i = 0; i < agentList.size(); i++) {
		AActor agent = agentList[i];
		vec3 agentPos = agent.getBehaviorController()->getPosition();
		vec3 distance = agentPos - actorPos;
		double radius = distance.Length();
		double neighborhood = BehaviorController::RNeighborhood;
		if (radius < neighborhood) {
			// its in the neighborhood
			Vdesired += (distance / (radius * radius));
		}
	}

	Vdesired *= BehaviorController::KSeparation;

	if (Vdesired.Length() < 5.0)
		Vdesired = 0.0;
	
	return Vdesired;
}


// Cohesion behavior
///////////////////////////////////////////////////////////////////////////////
// For the given actor, return a desired velocity vector in world coordinates
// Cohesion moves actors towards the center of the group of agents in the neighborhood
//  agents[i] gives the pointer to the ith agent in the environment
//  Cohesion parameters are in BehavioralController::RNeighborhood and BehavioralController::KCohesion


Cohesion::Cohesion( vector<AActor>* agents) 
{
	m_name = "cohesion";
	m_AgentList = agents;
}

Cohesion::Cohesion( Cohesion& orig) 
{
	m_name = "cohesion";
	m_AgentList = orig.m_AgentList;
}

Cohesion::~Cohesion()
{
}

vec3 Cohesion::calcDesiredVel( BehaviorController* actor)
{
	vec3 Vdesired = vec3(0.0, 0.0, 0.0);
	vec3 actorPos = actor->getPosition();
	vector<AActor>& agentList = *m_AgentList;
	
	// compute Vdesired = Vcohesion
	// TODO: add your code here 
	//find center of mass in neighborhood
	int numNeighbors = 0;
	vec3 combinedPos;
	for (int i = 0; i < agentList.size(); i++) {
		AActor agent = agentList[i];
		vec3 agentPos = agent.getBehaviorController()->getPosition();
		vec3 distance = agentPos - actorPos;
		double radius = distance.Length();
		double neighborhood = BehaviorController::RNeighborhood;
		if (radius < neighborhood) {
			// its in the neighborhood
			numNeighbors++;
			combinedPos += agentPos;
		}
	}
	combinedPos = combinedPos / numNeighbors;
	Vdesired = BehaviorController::KCohesion * (combinedPos - actorPos);
	return Vdesired;
}

// Flocking behavior
///////////////////////////////////////////////////////////////////////////////
// For the given actor, return a desired velocity vector  in world coordinates
// Flocking combines separation, cohesion, and alignment behaviors
//  Utilize the Separation, Cohesion and Alignment behaviors to determine the desired velocity vector


Flocking::Flocking( AJoint* target,  vector<AActor>* agents) 
{
	m_name = "flocking";
	m_AgentList = agents;
	m_pTarget = target;
}

Flocking::Flocking( Flocking& orig) 
{
	m_name = "flocking";
	m_AgentList = orig.m_AgentList;
	m_pTarget = orig.m_pTarget;
}

Flocking::~Flocking()
{
}

vec3 Flocking::calcDesiredVel( BehaviorController* actor)
{
	vec3 Vdesired = vec3(0.0, 0.0, 0.0);
	vec3 actorPos = actor->getPosition();
	vector<AActor>& agentList = *m_AgentList;
	vec3 targetPos = m_pTarget->getLocalTranslation();

	// compute Vdesired = Vflocking
	// TODO: add your code here 
	//v flock = c separate * v separate + c alignment * v alignment + c cohesion * v cohesion
	//lets do equal parts of all
	double c = 1 / 3;
	
	//separate
	vec3 vseparate;
	for (int i = 0; i < agentList.size(); i++) {
		AActor agent = agentList[i];
		vec3 agentPos = agent.getBehaviorController()->getPosition();
		vec3 distance = agentPos - actorPos;
		double radius = distance.Length();
		double neighborhood = BehaviorController::RNeighborhood;
		if (radius < neighborhood) {
			// its in the neighborhood
			vseparate += (distance / (radius * radius));
		}
	}
	vseparate = BehaviorController::KSeparation;

	//align
	vec3 valign;
	BehaviorController* leader = agentList[0].getBehaviorController();
	if (leader == actor) {
		vec3 pathToTarget = targetPos - actorPos;
		double distance = pathToTarget[0] * pathToTarget[0] + pathToTarget[1] * pathToTarget[1] + pathToTarget[2] * pathToTarget[2];
		distance = sqrt(distance);
		pathToTarget.Normalize();
		valign = pathToTarget / distance;
	}

	// Step 2. if not first agent compute Valign as usual
	//TODO: add your code here
	else {
		vec3 combinedVelocity;
		int numAgents = 0;
		for (int i = 0; i < agentList.size(); i++) {
			AActor agent = agentList[i];
			vec3 agentVel = agent.getBehaviorController()->getVelocity();
			combinedVelocity += agentVel;
			numAgents++;
		}

		valign = combinedVelocity / numAgents;
	}

	//cohesion
	vec3 vcohesion;
	int numNeighbors = 0;
	vec3 combinedPos;
	for (int i = 0; i < agentList.size(); i++) {
		AActor agent = agentList[i];
		vec3 agentPos = agent.getBehaviorController()->getPosition();
		vec3 distance = agentPos - actorPos;
		double radius = distance.Length();
		double neighborhood = BehaviorController::RNeighborhood;
		if (radius < neighborhood) {
			// its in the neighborhood
			numNeighbors++;
			combinedPos += agentPos;
		}
	}
	combinedPos = combinedPos / numNeighbors;
	vcohesion = BehaviorController::KCohesion * (combinedPos - actorPos);

	Vdesired = c * vseparate + c * valign + c * vcohesion;

	return Vdesired;
}

//	Leader behavior
///////////////////////////////////////////////////////////////////////////////
// For the given actor, return a desired velocity vector in world coordinates
// If the agent is the leader, move towards the target; otherwise, 
// follow the leader at a set distance behind the leader without getting to close together
//  Utilize Separation and Arrival behaviors to determine the desired velocity vector
//  You need to find the leader, who is always agents[0]

Leader::Leader( AJoint* target, vector<AActor>* agents) 
{
	m_name = "leader";
	m_AgentList = agents;
	m_pTarget = target;
}

Leader::Leader( Leader& orig) 
{
	m_name = "leader";
	m_AgentList = orig.m_AgentList;
	m_pTarget = orig.m_pTarget;
}

Leader::~Leader()
{
}

vec3 Leader::calcDesiredVel( BehaviorController* actor)
{
	
	vec3 Vdesired = vec3(0.0, 0.0, 0.0);
	vec3 actorPos = actor->getPosition();
	vector<AActor>& agentList = *m_AgentList;
	vec3 targetPos = m_pTarget->getLocalTranslation();

	// TODO: compute Vdesired  = Vleader
	// followers should stay directly behind leader at a distance of -200 along the local z-axis

	float CSeparation = 4.0;  float CArrival = 2.0;

	BehaviorController* leader = agentList[0].getBehaviorController(); // first agent is the leader
	mat3 Rmat = leader->getGuide().getLocalRotation();  // is rotattion matrix of lead agent

	vec3 varrive;
	vec3 pathToTarget = targetPos - actorPos;
	double distance = pathToTarget[0] * pathToTarget[0] + pathToTarget[1] * pathToTarget[1] + pathToTarget[2] * pathToTarget[2];
	distance = sqrt(distance);
	pathToTarget.Normalize();
	varrive = BehaviorController::KArrival * pathToTarget / distance;

	if (leader == actor) {
		return varrive;
	}

	// Step 2. if not first agent compute Valign as usual
	//TODO: add your code here
	vec3 combinedVelocity;
	int numAgents = 0;
	for (int i = 0; i < agentList.size(); i++) {
		AActor agent = agentList[i];
		vec3 agentVel = agent.getBehaviorController()->getVelocity();
		combinedVelocity += agentVel;
		numAgents++;
	}

	vec3 valign = combinedVelocity / numAgents;


	//separate
	vec3 vseparate;
	for (int i = 0; i < agentList.size(); i++) {
		AActor agent = agentList[i];
		vec3 agentPos = agent.getBehaviorController()->getPosition();
		vec3 distance = agentPos - actorPos;
		double radius = distance.Length();
		double neighborhood = BehaviorController::RNeighborhood;
		if (radius < neighborhood) {
			// its in the neighborhood
			vseparate += (distance / (radius * radius));
		}
	}
	vseparate = BehaviorController::KSeparation;

	Vdesired = CSeparation * vseparate + CArrival * varrive;

	return Vdesired;
}

///////////////////////////////////////////////////////////////////////////////

