#pragma once

#include "Player.h"
#include "Ground.h"
#include "LFont.h"
#include "Ghost.h"
#include "GhostTypes.h"
#include "Gizmo.h"
#include "Settings.h"

const int START_MENU_FLOATING_TIME = 3000;


enum class LevelEvent
{
	REINITIALIZE		= 0x00000000,
	QUIT				= 0x00000001,
	RESTART				= 0x00000002,
	LEVEL_RESTART		= 0x00000004,

	PAUSE				= 0x00000010,
	CONTINUE			= 0x00000011,

	ENTER				= 0x00000100,
	ESCAPE				= 0x00000101,

	DEBUG				= 0x7FFFFFFF,

	PLAYER_1_UP			= 0x00001101,
	PLAYER_1_LEFT		= 0x00001102,
	PLAYER_1_DOWN		= 0x00001104,
	PLAYER_1_RIGHT		= 0x00001108,

	PLAYER_2_UP			= 0x00001201,
	PLAYER_2_LEFT		= 0x00001202,
	PLAYER_2_DOWN		= 0x00001204,
	PLAYER_2_RIGHT		= 0x00001208,
};

enum class LevelStage
{
	NONE				= 0x00000000,
	START_MENU			= 0x00000001,
	TUTORIAL			= 0x00000002,
	GAME_LOOP			= 0x00000000,
	TRANSITION			= 0x00000003,
	RESTART				= 0x00000004,
	COMPLETE			= 0x00000005,
	GAME_OVER			= 0x00000006,
};

class LevelManager
{
public:
	struct LevelInfo
	{
		double playerVelocity;
		double ghostChasingVelocity;
		double ghostFleeingVelocity;
		double ghostRenbornVelocity;

		int ghostFleeingTime;
		int ghostScatterTime;

		int bashFulPreparingTime;
		int pokeyPreparingTime;
	};

	LevelManager();
	~LevelManager();

	bool Initialize(SDL_Renderer* mainRenderer, Gizmo* mGizmo = NULL);
	void InitializeLevelInfo();
	void ReInitialize();
	bool HandleEvent(LevelEvent lEvent);

	bool UpdateGameState();
	void SetStage(LevelStage stage);
	void SetLevel(int level, bool restart = false);

	bool Render();
	bool RenderStartMenu();
	bool RenderTutorial();
	bool RenderGame();
	bool RenderComplete();

	// Render score gizmo
	void AddScoreToRenderList(int score, int x, int y);
	void UpdateScoreRenderList();
	void RenderScoreList();

	// Render total score at top
	bool RenderScoreAt(int centerX, int centerY);
	// Render extra life
	bool RenderExtraLife(int leftX, int centerY);

private:
	SDL_Renderer* mRenderer;
	Ground* mGround;

	LFont* titleFont;
	LFont* redTitleFont;
	LFont* numberFont;

	LTexture* logoTexture;
	LTexture* pacManTexture;

	Player* mPlayer;
	std::vector<Ghost*> ghostList;

	Gizmo* mGizmo;

	LevelStage levelStage;
	Uint32 lastStageTime;
	
	// in which level (1-12)
	int nowLevel;
	std::vector<LevelInfo> levelInfo;

	// 0->ReadyState 1->InGame 2->CompleteState 3->FailureState 4->GameOverState
	int inLevelState;
	Uint32 lastInLevelStateTime;

	int playModeSelected;

	int playerExtraLife;

	int score;
	int comboScore;

	// last power pellet eaten time
	Uint32 comboCountTime;
	
	// How long does a gizmo time appears
	Uint32 scoreGizmoTime;

	std::vector<int> scoreRenderList;
	std::vector<std::pair<int, int> > scorePositionList;
	std::vector<Uint32> getScoreTimeList;

	bool completeRestartFlag;
};