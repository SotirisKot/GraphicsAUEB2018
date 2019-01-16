#pragma once

#include "Tower.h"
#include "Pirate.h"
#include "CannonBall.h"
#include "GeometricMesh.h"
#include "TreasureChest.h"

class GameState {
public:

	GameState();
	~GameState();

	void setGold(unsigned int gold);
	unsigned int getGold();

	void setScore(unsigned int score);
	unsigned int getScore();

	void setPirateWave(unsigned int wave);
	unsigned int getPirateWave();

	void setPirateSubWave(unsigned int subwave);
	unsigned int getPirateSubWave();

	void setPirateRate(unsigned int rate);
	unsigned int getPirateRate();

	void setAvailableTowers(std::vector<Tower*> availableTowers);
	std::vector<Tower*> getAvailableTowers();

	void setCreatedTowers(std::vector<Tower*> createdTowers);
	std::vector<Tower*> getCreatedTowers();

	void setPirates(std::vector<Pirate*> pirates);
	std::vector<Pirate*> getPirates();

	void setCannonBalls(std::vector<CannonBall*> cannonballs);
	std::vector<CannonBall*> getCannonBalls();

	void setTreasureChest(TreasureChest* chest);
	TreasureChest* getTreasureChest();

	std::tuple<int, int>* getRoadTiles();

	void setGameOver(bool state);
	bool getGameOver();

	// Tower functions
	void										addTower(float x, float y);
	void										addTower();

	void										removeTower(float x, float y);
	void										removeTower();

	void										rearrangeTower(float x, float y);
	void										rearrangeTower();

	void										upgradeTower(float x, float y);
	void										upgradeTower();

	// Grid functions
	void										getRealPos(float& x, float& y);
	void										getGridPos(float& x, float& y);

	// Tile Functions
	void										setTilePosition(int x, int y);
	void										setTileX(int x);
	void										setTileY(int y);
	int											getTileX();
	int											getTileY();

	void										updateInRoad();
	bool										getInRoad();
	bool										isInRoad(int x, int y);

	// Meshes

	void										setPirateBodyMesh(GeometricMesh* mesh);
	void										setPirateSwordMesh(GeometricMesh* mesh);
	void										setPirateLFootMesh(GeometricMesh* mesh);
	void										setPirateRFootMesh(GeometricMesh* mesh);
	void										assignMeshtoPirates();

	void										setPirateFastBodyMesh(GeometricMesh* mesh);
	void										setPirateFastSwordMesh(GeometricMesh* mesh);
	void										setPirateFastLFootMesh(GeometricMesh* mesh);
	void										setPirateFastRFootMesh(GeometricMesh* mesh);

	void										setPirateHeavyBodyMesh(GeometricMesh* mesh);
	void										setPirateHeavySwordMesh(GeometricMesh* mesh);
	void										setPirateHeavyLFootMesh(GeometricMesh* mesh);
	void										setPirateHeavyRFootMesh(GeometricMesh* mesh);

	void										setTowerMesh(GeometricMesh* mesh);
	void										setTowerLevelTwoMesh(GeometricMesh* mesh);
	void										setTowerLevelThreeMesh(GeometricMesh* mesh);
	void										assignMeshToTowers();

	void										setCannonballMesh(GeometricMesh* mesh);

	void										setTreasureChestMesh(GeometricMesh* mesh);
	void										assignTreasureChest();

	// Adding objects

	void										createTower();
	void										createPirate(int pType, int pLevel);
	void										shootCannonBall(Tower* tower, Pirate* pirate);

	// Update paths

	void										updatePirateTargets();

	// 

	void										deleteHitCannonBall(CannonBall* cannonball);
	void										deleteHitCannonBalls();

	void										towersFire();

	void										deletePirate(Pirate* pirate);

	void										checkPiratesAtChest();

	void										deleteToRemoveLists();

private:

	bool										gameOver;

	// Tile positions
	int											tileX, tileY;

	// Game variables
	unsigned int								score;
	unsigned int								gold;
	unsigned int								pirateWave;
	unsigned int								pirateSubWave;
	unsigned int								pirateRate;

	// Towers
	std::vector<Tower*>							availableTowers;
	std::vector<Tower*>							createdTowers;

	// Pirates
	std::vector<Pirate*>						pirates;

	// Cannonballs
	std::vector<CannonBall*>					cannonballs;

	// to be deleted objects
	std::vector<Pirate*>						toRemPirates;
	std::vector<CannonBall*>					toRemCannonBalls;

	// Treasure Chest
	TreasureChest*								treasureChest;

	// Road Tile List
	std::tuple<int, int>						road_tiles[29];

	bool										inRoad;

	// Meshes
	GeometricMesh*								pirateBodyMesh;
	GeometricMesh*								pirateSwordMesh;
	GeometricMesh*								pirateLFootMesh;
	GeometricMesh*								pirateRFootMesh;

	GeometricMesh*								pirateFastBodyMesh;
	GeometricMesh*								pirateFastSwordMesh;
	GeometricMesh*								pirateFastLFootMesh;
	GeometricMesh*								pirateFastRFootMesh;

	GeometricMesh*								pirateHeavyBodyMesh;
	GeometricMesh*								pirateHeavySwordMesh;
	GeometricMesh*								pirateHeavyLFootMesh;
	GeometricMesh*								pirateHeavyRFootMesh;

	GeometricMesh*								towerMesh;
	GeometricMesh*								towerLevelTwoMesh;
	GeometricMesh*								towerLevelThreeMesh;

	GeometricMesh*								cannonballMesh;

	GeometricMesh*								treasureChestMesh;

};