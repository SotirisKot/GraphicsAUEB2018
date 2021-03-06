#pragma once

#include "GeometryNode.h"
#include "GeometricMesh.h"
#include "BoundingSphere.h"
#include "ParticleSystem.h"

class Pirate{

public:
	Pirate();
	~Pirate();

	void setX(float x);
	void setY(float y);
	void setTargetX(float x);
	void setTargetY(float y);
	void setRoadIdx(int idx);

	void setStartHealth(int hp);
	void setHealthPoints(int hp);

	void setSpeed(float speed);

	void setAnimStart(int start);

	void setSize(float size);

	void setType(int type);

	void setLevel(int level);

	void setDir(int dir);

	void setBody(GeometryNode* body);
	void setBodyTM(glm::mat4 tm);
	void setBodyTNM(glm::mat4 tnm);

	void setSword(GeometryNode* sword);
	void setSwordTM(glm::mat4 tm);
	void setSwordTNM(glm::mat4 tnm);

	void setLeftFoot(GeometryNode* leftfoot);
	void setLeftFootTM(glm::mat4 tm);
	void setLeftFootTNM(glm::mat4 tnm);

	void setRightFoot(GeometryNode* rightfoot);
	void setRightFootTM(glm::mat4 tm);
	void setRightFootTNM(glm::mat4 tnm);

	void setHealthGreen(GeometryNode* healthgreen);
	void setHealthGreenTM(glm::mat4 tm);
	void setHealthGreenTNM(glm::mat4 tnm);

	void setHealthRed(GeometryNode* healthred);
	void setHealthRedTM(glm::mat4 tm);
	void setHealthRedTNM(glm::mat4 tnm);

	float getX();
	float getY();

	float getTargetX();
	float getTargetY();

	int getRoadIdx();

	int getStartHealth();
	int getHealthPoints();

	bool isDead();
	void setDead(bool state);

	bool isGoo();
	void setGoo(bool state);

	bool isDummy();
	void setDummy(bool state);

	int getDummyRefs();
	void setDummyRefs(int i);

	void setDeadCycle(int i);
	int getDeadCycle();

	float getSpeed();

	int getAnimStart();

	float getSize();

	int getType();

	int getLevel();

	int getDir();

	GeometryNode* getBody();
	glm::mat4 getBodyTM();
	glm::mat4 getBodyTNM();

	GeometryNode* getSword();
	glm::mat4 getSwordTM();
	glm::mat4 getSwordTNM();

	GeometryNode* getLeftFoot();
	glm::mat4 getLeftFootTM();
	glm::mat4 getLeftFootTNM();

	GeometryNode* getRightFoot();
	glm::mat4 getRightFootTM();
	glm::mat4 getRightFootTNM();

	GeometryNode* getHealthGreen();
	glm::mat4 getHealthGreenTM();
	glm::mat4 getHealthGreenTNM();

	GeometryNode* getHealthRed();
	glm::mat4 getHealthRedTM();
	glm::mat4 getHealthRedTNM();

	BoundingShpere* getBoundingSphere();

	ParticleEmitter* getParticleEmmiter();

private:

	GeometryNode*									m_geometric_objectBody;
	glm::mat4										m_geometric_objectBody_transformation_matrix;
	glm::mat4										m_geometric_objectBody_transformation_normal_matrix;
	
	GeometryNode*									m_geometric_objectSword;
	glm::mat4										m_geometric_objectSword_transformation_matrix;
	glm::mat4										m_geometric_objectSword_transformation_normal_matrix;

	GeometryNode*									m_geometric_objectLeftFoot;
	glm::mat4										m_geometric_objectLeftFoot_transformation_matrix;
	glm::mat4										m_geometric_objectLeftFoot_transformation_normal_matrix;

	GeometryNode*									m_geometric_objectRightFoot;
	glm::mat4										m_geometric_objectRightFoot_transformation_matrix;
	glm::mat4										m_geometric_objectRightFoot_transformation_normal_matrix;

	GeometryNode*									m_geometric_objectHealthGreen;
	glm::mat4										m_geometric_objectHealthGreen_transformation_matrix;
	glm::mat4										m_geometric_objectHealthGreen_transformation_normal_matrix;

	GeometryNode*									m_geometric_objectHealthRed;
	glm::mat4										m_geometric_objectHealthRed_transformation_matrix;
	glm::mat4										m_geometric_objectHealthRed_transformation_normal_matrix;

	GeometricMesh*									obj_mesh;

	float x_pirate;
	float y_pirate;

	float target_x;
	float target_y;

	int roadIdx;

	int start_health;
	int health_points;
	bool dead;
	bool goo;
	bool dummy;
	int dummy_refs;
	int dead_cycle;

	float speed;

	int animStart;

	float size;

	// Type 0 -> fast
	// Type 1 -> normal
	// Type 2 -> heavy
	// Type 3 -> boss
	int type;

	int level;

	// Dir 0 -> Up
	// Dir 1 -> Down
	// Dir 2 -> Left
	// Dir 3 -> Right
	int dir;

	BoundingShpere*									bound_shpere;

	ParticleEmitter*								m_particle_emitter;

};