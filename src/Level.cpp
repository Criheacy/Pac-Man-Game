#include "Level.h"

LevelManager::LevelManager()
{
	mRenderer = NULL;
	mGround = NULL;
	titleFont = NULL;
	redTitleFont = NULL;
	numberFont = NULL;
	mPlayer = NULL;
	mGizmo = NULL;
	logoTexture = NULL;
	pacManTexture = NULL;
	ghostList.clear();

	levelStage = LevelStage::START_MENU;
	lastStageTime = 0;
	lastInLevelStateTime = 0;

	nowLevel = 0;
	inLevelState = 1;
	playModeSelected = 0;

	scoreGizmoTime = 1000;

	score = 0;
	comboScore = 100;
	comboCountTime = 0;

	playerExtraLife = 2;
	completeRestartFlag = false;
}

LevelManager::~LevelManager()
{
	mRenderer = NULL;
	mGround = NULL;
	titleFont = NULL;
	numberFont = NULL;
	mPlayer = NULL;

	ghostList.clear();

	mGizmo = NULL;
}

bool LevelManager::Initialize(SDL_Renderer* mRenderer, Gizmo* mGizmo)
{
	/* -------------------- Renderer -------------------- */
	if (this->mRenderer != NULL && this->mRenderer != mRenderer)
	{
		SDL_DestroyRenderer(this->mRenderer);
		this->mRenderer = NULL;
	}
	this->mRenderer = mRenderer;


	/* -------------------- Ground -------------------- */
	if (mGround != NULL)
	{
		delete mGround;
		mGround = NULL;
	}
	mGround = new Ground();
	mGround->LoadTexture(this->mRenderer);
	mGround->LoadMap(MAP_PATH, MAP_WIDTH, MAP_HEIGHT);

	/* -------------------------------------------------- */
	int groundOriginX = SCREEN_WIDTH / 2 - int(((MAP_WIDTH - 1.0) / 2) * (WALL_WIDTH * GROUND_SCALE));
	int groundOriginY = SCREEN_HEIGHT / 2 - int(((MAP_HEIGHT - 1.0) / 2) * (WALL_HEIGHT * GROUND_SCALE));

	/* -------------------- Player -------------------- */
	if (mPlayer != NULL)
	{
		delete mPlayer;
		mPlayer = NULL;
	}
	mPlayer = new Player();
	mPlayer->LoadTexture(this->mRenderer);
	mPlayer->SetOriginPoint(groundOriginX, groundOriginY);
	mPlayer->StartAliveAnimation();
	mPlayer->StartMove(mGround, 10, 20, 0x03);

	/* -------------------- Font -------------------- */
	if (titleFont != NULL)
	{
		delete titleFont;
		titleFont = NULL;
	}
	if (redTitleFont != NULL)
	{
		delete redTitleFont;
		redTitleFont = NULL;
	}
	if (numberFont != NULL)
	{
		delete numberFont;
		numberFont = NULL;
	}

	titleFont = new LFont();
	titleFont->LoadTexture(this->mRenderer, "res/image/font.png");
	titleFont->SetOrigin(groundOriginX, groundOriginY);
	titleFont->SetUnitSize(int(WALL_WIDTH * GROUND_SCALE), int(WALL_HEIGHT * GROUND_SCALE));
	titleFont->SetTileSize(FONT_WIDTH, FONT_HEIGHT);

	redTitleFont = new LFont();
	redTitleFont->LoadTexture(this->mRenderer, "res/image/font.png", 0xD7, 0x00, 0x00);
	redTitleFont->SetOrigin(groundOriginX, groundOriginY);
	redTitleFont->SetUnitSize(int(WALL_WIDTH * GROUND_SCALE), int(WALL_HEIGHT * GROUND_SCALE));
	redTitleFont->SetTileSize(FONT_WIDTH, FONT_HEIGHT);

	numberFont = new LFont();
	numberFont->LoadTexture(this->mRenderer, "res/image/font_number.png");
	numberFont->SetOrigin(groundOriginX, groundOriginY);
	numberFont->SetUnitSize(int(WALL_WIDTH * GROUND_SCALE), int(WALL_HEIGHT * GROUND_SCALE));
	numberFont->SetTileSize(NUMBER_FONT_WIDTH, NUMBER_FONT_HEIGHT);

	/* -------------------- Logo Texture -------------------- */
	if (logoTexture != NULL)
	{
		delete logoTexture;
		logoTexture = NULL;
	}
	logoTexture = new LTexture();
	logoTexture->LoadFromFile(mRenderer, "res/image/logo.png");

	/* -------------------- Logo Texture -------------------- */
	if (pacManTexture != NULL)
	{
		delete pacManTexture;
		pacManTexture = NULL;
	}
	pacManTexture = new LTexture();
	pacManTexture->LoadFromFile(mRenderer, "res/image/pacman.png");
	pacManTexture->SetTileSize(16, 16);


	/* -------------------- Ghosts -------------------- */
	// Delete and free previous ghosts in list
	for (auto ghost = ghostList.begin(); ghost != ghostList.end(); ghost++)
	{
		if (*ghost != NULL)
		{
			delete *ghost;
			*ghost = NULL;
		}
	}
	ghostList.clear();

	Shadow* shadow = new Shadow();
	shadow->LoadTexture(this->mRenderer, SHADOW_COLOR[0], SHADOW_COLOR[1], SHADOW_COLOR[2]);
	ghostList.push_back(shadow);

	Speedy* speedy = new Speedy();
	speedy->LoadTexture(this->mRenderer, SPEEDY_COLOR[0], SPEEDY_COLOR[1], SPEEDY_COLOR[2]);
	ghostList.push_back(speedy);

	BashFul* bashful = new BashFul();
	bashful->LoadTexture(this->mRenderer, BASHFUL_COLOR[0], BASHFUL_COLOR[1], BASHFUL_COLOR[2]);
	bashful->SetShadowGhost(shadow);
	ghostList.push_back(bashful);

	Pokey* pokey = new Pokey();
	pokey->LoadTexture(this->mRenderer, POKEY_COLOR[0], POKEY_COLOR[1], POKEY_COLOR[2]);
	ghostList.push_back(pokey);

	for (auto ghost = ghostList.begin(); ghost != ghostList.end(); ghost++)
	{
		if (*ghost != NULL)
		{
			(*ghost)->LoadPlayer(mPlayer);
			(*ghost)->SetOriginPoint(groundOriginX, groundOriginY);
			(*ghost)->StartAliveAnimation();
		}
	}

	shadow->StartMove(mGround, 0x00);
	speedy->StartMove(mGround, 0x02);
	bashful->StartMove(mGround, 0x02);
	pokey->StartMove(mGround, 0x00);

	// Connect player to ghost list
	mPlayer->SetGhosts(ghostList);

	if (mGizmo != NULL)
	{
		if (this->mGizmo != NULL && this->mGizmo != mGizmo)
		{
			delete this->mGizmo;
			this->mGizmo = NULL;
		}
		this->mGizmo = mGizmo;
		this->mGizmo->SetOrigin(groundOriginX, groundOriginY);
		this->mGizmo->SetUnitSize(int(WALL_WIDTH * GROUND_SCALE), int(WALL_HEIGHT * GROUND_SCALE));
	}

	InitializeLevelInfo();

	SetStage(LevelStage::START_MENU);

	return true;
}

void LevelManager::InitializeLevelInfo()
{
	levelInfo.clear();
	levelInfo.resize(12);

				//  (more info to see defination of "LevelInfo")
				//  player chase flee reborn fT    sT   b.pT  p.pT
	levelInfo[0]  = { 6.1,  3.9,  2.0, 10.0, 9500, 8000, 20000, 20000 };
	levelInfo[1]  = { 6.5,  4.3,  2.1, 11.0, 8500, 8000, 7000,  20000 };
	levelInfo[2]  = { 6.9,  4.7,  2.2, 12.0, 7500, 8000, 1500,  10000 };
	levelInfo[3]  = { 7.2,  5.0,  2.3, 13.0, 6000, 8000, 0,     2000  };
	levelInfo[4]  = { 7.5,  5.3,  2.4, 13.5, 4500, 8000, 0,     500   };
	levelInfo[5]  = { 7.8,  5.6,  2.5, 14.0, 3500, 8000, 0,     0     };
	levelInfo[6]  = { 8.2,  6.0,  2.6, 14.5, 2500, 8000, 0,     0     };
	levelInfo[7]  = { 8.4,  6.4,  2.7, 15.0, 1000, 0,    0,     0     };
	levelInfo[8]  = { 8.6,  6.8,  2.8, 15.5, 500,  0,    0,     0     };
	levelInfo[9]  = { 8.8,  7.2,  2.9, 15.5, 100,  0,    0,     0     };
	levelInfo[10] = { 8.9,  7.5,  3.0, 16.0, 0,    0,    0,     0     };
	levelInfo[11] = { 9.0,  7.8,  3.0, 16.0, 0,    0,    0,     0     };
}

void LevelManager::ReInitialize()
{
	playModeSelected = 0;
	playerExtraLife = 2;
	score = 0;
}

bool LevelManager::HandleEvent(LevelEvent lEvent)
{
	if (levelStage == LevelStage::START_MENU)
	{
		switch (lEvent)
		{
		case LevelEvent::REINITIALIZE:
			break;
		case LevelEvent::QUIT:
			break;
		case LevelEvent::RESTART:
			lastStageTime = SDL_GetTicks();
			break;
		case LevelEvent::PLAYER_1_UP:
			if (playModeSelected != 1) playModeSelected = 1;
			else playModeSelected = 2;
			break;
		case LevelEvent::PLAYER_1_DOWN:
			if (playModeSelected != 1) playModeSelected = 1;
			else playModeSelected = 2;
			break;
		case LevelEvent::PLAYER_2_UP:
			if (playModeSelected != 1) playModeSelected = 1;
			else playModeSelected = 2;
			break;
		case LevelEvent::PLAYER_2_DOWN:
			if (playModeSelected != 1) playModeSelected = 1;
			else playModeSelected = 2;
			break;
		case LevelEvent::ENTER:
			if (playModeSelected == 1) SetStage(LevelStage::TUTORIAL);
		default:
			break;
		}
	}
	else if (levelStage == LevelStage::TUTORIAL)
	{
		switch (lEvent)
		{
		case LevelEvent::REINITIALIZE:
			break;
		case LevelEvent::QUIT:
			break;
		case LevelEvent::RESTART:
			lastStageTime = SDL_GetTicks();
			break;
		case LevelEvent::ENTER:
			if (playModeSelected == 1)
			{
				SetStage(LevelStage::GAME_LOOP);
				nowLevel = 1;
				SetLevel(nowLevel);
			}
		default:
			break;
		}
	}
	else if (levelStage == LevelStage::GAME_LOOP)
	{
		switch (lEvent)
		{
		case LevelEvent::REINITIALIZE:
			break;
		case LevelEvent::QUIT:
			break;
		case LevelEvent::RESTART:
			break;
		case LevelEvent::LEVEL_RESTART:
			break;
		case LevelEvent::PAUSE:
			break;
		case LevelEvent::CONTINUE:
			break;
		case LevelEvent::DEBUG:
			break;
		case LevelEvent::PLAYER_1_UP:
			mPlayer->HandleControl(0);
			break;
		case LevelEvent::PLAYER_1_LEFT:
			mPlayer->HandleControl(1);
			break;
		case LevelEvent::PLAYER_1_DOWN:
			mPlayer->HandleControl(2);
			break;
		case LevelEvent::PLAYER_1_RIGHT:
			mPlayer->HandleControl(3);
			break;
		case LevelEvent::PLAYER_2_UP:
			break;
		case LevelEvent::PLAYER_2_LEFT:
			break;
		case LevelEvent::PLAYER_2_DOWN:
			break;
		case LevelEvent::PLAYER_2_RIGHT:
			break;
		default:
			break;
		}
	}
	else if (levelStage == LevelStage::COMPLETE)
	{
		switch (lEvent)
		{
		case LevelEvent::REINITIALIZE:
			break;
		case LevelEvent::QUIT:
			break;
		case LevelEvent::RESTART:
			lastStageTime = SDL_GetTicks();
			break;
		case LevelEvent::ENTER:
			completeRestartFlag = true;
		default:
			break;
		}
	}
	return true;
}

bool LevelManager::UpdateGameState()
{
	Uint32 nowTicks = SDL_GetTicks();

	if (levelStage == LevelStage::GAME_LOOP)
	{
		if (inLevelState == 0)
		{
			if (nowTicks - lastStageTime >= 2500)
			{
				ghostList[0]->StartMove(mGround, 0x00);
				ghostList[1]->StartMove(mGround, 0x02);
				ghostList[2]->StartMove(mGround, 0x02);
				ghostList[3]->StartMove(mGround, 0x00);

				mPlayer->StartMove(mGround, 10, 20, 0x03);

				inLevelState = 1;
			}
		}
		else if (inLevelState == 1)
		{
			int _preScore = score;

			// Calculate score

			// Food Score
			score += mPlayer->GetFoodEatenScore();

			int specialFoodScore = mPlayer->GetSpecialFoodEatenScore();
			if (specialFoodScore != 0)
			{
				score += specialFoodScore;
				AddScoreToRenderList(specialFoodScore, int(mPlayer->GetPosition().first),
					int(mPlayer->GetPosition().second));
			}

			if (mPlayer->GetPowerPelletEaten())
			{
				comboCountTime = nowTicks;
			}

			if (int(nowTicks - comboCountTime) >= levelInfo[nowLevel - 1].ghostFleeingTime
				&& comboScore > 100)
			{
				comboScore = 100;
			}

			// Combo Score
			for (auto ghost = ghostList.begin(); ghost != ghostList.end(); ghost++)
			{
				int result = (*ghost)->GetGhostEaten();
				if (result != 0)
				{
					comboScore *= 2;
					if (comboScore >= 6400) comboScore = 6400;

					score += comboScore;
					AddScoreToRenderList(comboScore, int((*ghost)->GetPosition().first),
						int((*ghost)->GetPosition().second));
				}
			}

			// Bonus extra life whenever over 10000 points
			if (score / 10000 - _preScore / 10000 != 0)
			{
				playerExtraLife++;
				if (playerExtraLife > 5)
				{
					playerExtraLife = 5;
				}
			}

			// Level accomplished check
			if (mGround->WinStateCheck())
			{
				// Level Complete
				inLevelState = 2;
				mPlayer->Freeze();
				for (auto ghost = ghostList.begin(); ghost != ghostList.end(); ghost++)
				{
					(*ghost)->Freeze();
				}
				lastInLevelStateTime = SDL_GetTicks();
			}

			if (mPlayer->GetAliveState() != 0)
			{
				inLevelState = 3;
				mPlayer->Freeze();
				for (auto ghost = ghostList.begin(); ghost != ghostList.end(); ghost++)
				{
					(*ghost)->Freeze();
				}
				lastInLevelStateTime = SDL_GetTicks();
			}
		}
		else if (inLevelState == 2)
		{
			if (nowTicks - lastInLevelStateTime >= 2000)
			{
				SetStage(LevelStage::TRANSITION);
			}
		}
		else if (inLevelState == 3)
		{
			if (nowTicks - lastInLevelStateTime >= 2000)
			{
				if (playerExtraLife > 0)
				{
					playerExtraLife--;
					mPlayer->ClearNextTowardsStack();
					mGround->ClearSpecialFood();
					SetStage(LevelStage::RESTART);
				}
				else
				{
					inLevelState = 4;
					lastInLevelStateTime = SDL_GetTicks();
				}
			}
		}
		else if (inLevelState == 4)
		{
			if (nowTicks - lastInLevelStateTime >= 5000)
			{
				ReInitialize();
				SetStage(LevelStage::START_MENU);
			}
		}
	}
	else if (levelStage == LevelStage::TRANSITION)
	{
		if (nowTicks - lastStageTime >= 800)
		{
			SetStage(LevelStage::GAME_LOOP);
			nowLevel++;
			SetLevel(nowLevel);
		}
	}
	else if (levelStage == LevelStage::RESTART)
	{
		if (nowTicks - lastStageTime >= 800)
		{
			SetStage(LevelStage::GAME_LOOP);
			SetLevel(nowLevel, true);
		}
	}
	else if (levelStage == LevelStage::COMPLETE)
	{
		if (completeRestartFlag)
		{
			completeRestartFlag = false;
			ReInitialize();
			SetStage(LevelStage::START_MENU);
		}
	}
	return true;
}

void LevelManager::SetStage(LevelStage stage)
{
	lastStageTime = SDL_GetTicks();
	levelStage = stage;

	switch (levelStage)
	{
	case LevelStage::START_MENU:
		break;
	case LevelStage::TUTORIAL:
		break;
	case LevelStage::GAME_LOOP:
		//nowLevel++;
		//SetLevel(nowLevel);
		inLevelState = 0;
		break;
	case LevelStage::TRANSITION:
		break;
	case LevelStage::COMPLETE:
		break;
	default:
		break;
	}
}

void LevelManager::SetLevel(int level, bool restart)
{
	if (level > 12)
	{
		SetStage(LevelStage::COMPLETE);
		return;
	}

	Uint32 nowTicks = SDL_GetTicks();
	inLevelState = 0;
	comboScore = 100;
	UpdateGameState();

	if (!restart)
	{
		mGround->AllowcateFood();
		mGround->SetSpecialFoodType
			(int(level / 12.0 * 8));
	}

	mPlayer->StartAliveAnimation();
	mPlayer->SetVelocity(levelInfo[level-1].playerVelocity);

	mPlayer->StartMove(mGround, 10, 20, 0x03);
	mPlayer->Freeze();


	for (auto ghost = ghostList.begin(); ghost != ghostList.end(); ghost++)
	{
		int ghostPreparingTime = 0;
		switch (ghost - ghostList.begin())
		{
		case 0: ghostPreparingTime = -1; break;
		case 1: ghostPreparingTime = 0; break;
		case 2: ghostPreparingTime = levelInfo[level-1].bashFulPreparingTime; break;
		case 3: ghostPreparingTime = levelInfo[level-1].pokeyPreparingTime; break;
		default:
			break;
		}

		(*ghost)->InitializeInfo(levelInfo[level-1].ghostChasingVelocity,
			levelInfo[level-1].ghostFleeingVelocity,
			levelInfo[level-1].ghostRenbornVelocity,
			levelInfo[level-1].ghostFleeingTime,
			levelInfo[level-1].ghostScatterTime,
			ghostPreparingTime);
		(*ghost)->SetState(0);
	}

	ghostList[0]->StartMove(mGround, 0x00);
	ghostList[1]->StartMove(mGround, 0x02);
	ghostList[2]->StartMove(mGround, 0x02);
	ghostList[3]->StartMove(mGround, 0x00);

	for (auto ghost = ghostList.begin(); ghost != ghostList.end(); ghost++)
	{
		(*ghost)->Freeze();
	}
}

bool LevelManager::Render()
{
	UpdateGameState();

	SDL_SetRenderDrawColor(mRenderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderClear(mRenderer);

	switch (levelStage)
	{
	case LevelStage::START_MENU:
		RenderStartMenu();
		break;
	case LevelStage::TUTORIAL:
		RenderTutorial();
		break;
	case LevelStage::GAME_LOOP:
		RenderGame();
		break;
//	case LevelStage::TRANSITION:
//		break;
	case LevelStage::COMPLETE:
		RenderComplete();
	default:
		RenderScoreAt(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 270);
		break;
	}
	SDL_RenderPresent(mRenderer);
	return true;
}

bool LevelManager::RenderStartMenu()
{
	int centerX = SCREEN_WIDTH / 2;
	int centerY = SCREEN_HEIGHT / 2;
	int deltaY = int(SCREEN_HEIGHT * (1 - (double(SDL_GetTicks() - lastStageTime) / START_MENU_FLOATING_TIME)));
	if (deltaY < 0)
	{
		deltaY = 0;
	}

	// Lay out

	RenderScoreAt(centerX, centerY - 270 + deltaY);

	logoTexture->Render(centerX, centerY - 130 + deltaY, NULL, GROUND_SCALE * 1.5);

	if (playModeSelected == 1)
	{
		titleFont->RenderString("> 1 PLAYER ", centerX - 10, centerY - 10 + deltaY, GROUND_SCALE);
	}
	else
	{
		titleFont->RenderString("  1 PLAYER ", centerX - 10, centerY - 10 + deltaY, GROUND_SCALE);
	}

	if (playModeSelected == 2)
	{
		titleFont->RenderString("> 2 PLAYERS", centerX - 10, centerY + 30 + deltaY, GROUND_SCALE);
	}
	else
	{
		titleFont->RenderString("  2 PLAYERS", centerX - 10, centerY + 30 + deltaY, GROUND_SCALE);
	}

	titleFont->RenderString("TM & @ 1980 1993 NAMCO LTD.", centerX, centerY + 110 + deltaY, GROUND_SCALE);
	titleFont->RenderString("NAMCO HOMETEK, INC.", centerX, centerY + 135 + deltaY, GROUND_SCALE);
	titleFont->RenderString("LICENSED BY NINTENDO", centerX, centerY + 160 + deltaY, GROUND_SCALE);
	redTitleFont->RenderString("REWRITE BY Criheacy @ SDU. WITH RESPECT", centerX, centerY + 195 + deltaY, GROUND_SCALE);

	if (playModeSelected == 0 && (SDL_GetTicks() / 500) % 2 == 0)
	{
		titleFont->RenderString("[1 COIN TO START]", centerX, centerY + 245 + deltaY, GROUND_SCALE);
	}

	return true;
}

bool LevelManager::RenderTutorial()
{
	int centerX = SCREEN_WIDTH / 2;
	int centerY = SCREEN_HEIGHT / 2;

	Uint32 nowTicks = SDL_GetTicks();

	RenderScoreAt(centerX, centerY - 270);

	if (nowTicks - lastStageTime < 500) return false;

	titleFont->RenderString("CHARACTER /  NICKNAME", centerX + 15, centerY - 180, GROUND_SCALE);

	if (nowTicks - lastStageTime < 1250) return false;

	int groundOriginX = SCREEN_WIDTH / 2 - int(((MAP_WIDTH - 1.0) / 2) * (WALL_WIDTH * GROUND_SCALE));
	int groundOriginY = SCREEN_HEIGHT / 2 - int(((MAP_HEIGHT - 1.0) / 2) * (WALL_HEIGHT * GROUND_SCALE));

	LFont* colorFont;
	colorFont = new LFont();
	colorFont->LoadTexture(this->mRenderer, "res/image/font.png", SHADOW_COLOR[0], SHADOW_COLOR[1], SHADOW_COLOR[2]);
	colorFont->SetOrigin(groundOriginX, groundOriginY);
	colorFont->SetUnitSize(int(WALL_WIDTH * GROUND_SCALE), int(WALL_HEIGHT * GROUND_SCALE));
	colorFont->SetTileSize(FONT_WIDTH, FONT_HEIGHT);

	ghostList[0]->Render(centerX - 160, centerY - 125, GROUND_SCALE);
	colorFont->RenderString("-SHADOW    \"BLINKY\"", centerX + 30, centerY - 125, GROUND_SCALE);

	delete colorFont;
	colorFont = NULL;

	if (nowTicks - lastStageTime < 2000) return false;

	colorFont = new LFont();
	colorFont->LoadTexture(this->mRenderer, "res/image/font.png", SPEEDY_COLOR[0], SPEEDY_COLOR[1], SPEEDY_COLOR[2]);
	colorFont->SetOrigin(groundOriginX, groundOriginY);
	colorFont->SetUnitSize(int(WALL_WIDTH * GROUND_SCALE), int(WALL_HEIGHT * GROUND_SCALE));
	colorFont->SetTileSize(FONT_WIDTH, FONT_HEIGHT);

	ghostList[1]->Render(centerX - 160, centerY - 70, GROUND_SCALE);
	colorFont->RenderString("-SPEEDY    \"PINKY\" ", centerX + 30, centerY - 70, GROUND_SCALE);

	delete colorFont;
	colorFont = NULL;

	if (nowTicks - lastStageTime < 2750) return false;

	colorFont = new LFont();
	colorFont->LoadTexture(this->mRenderer, "res/image/font.png", BASHFUL_COLOR[0], BASHFUL_COLOR[1], BASHFUL_COLOR[2]);
	colorFont->SetOrigin(groundOriginX, groundOriginY);
	colorFont->SetUnitSize(int(WALL_WIDTH * GROUND_SCALE), int(WALL_HEIGHT * GROUND_SCALE));
	colorFont->SetTileSize(FONT_WIDTH, FONT_HEIGHT);

	ghostList[2]->Render(centerX - 160, centerY - 15, GROUND_SCALE);
	colorFont->RenderString("-BASHFUL   \"INKY\"  ", centerX + 30, centerY - 15, GROUND_SCALE);

	delete colorFont;
	colorFont = NULL;

	if (nowTicks - lastStageTime < 3500) return false;

	colorFont = new LFont();
	colorFont->LoadTexture(this->mRenderer, "res/image/font.png", POKEY_COLOR[0], POKEY_COLOR[1], POKEY_COLOR[2]);
	colorFont->SetOrigin(groundOriginX, groundOriginY);
	colorFont->SetUnitSize(int(WALL_WIDTH * GROUND_SCALE), int(WALL_HEIGHT * GROUND_SCALE));
	colorFont->SetTileSize(FONT_WIDTH, FONT_HEIGHT);

	ghostList[3]->Render(centerX - 160, centerY + 40, GROUND_SCALE);
	colorFont->RenderString("-POKEY     \"CLYDE\" ", centerX + 30, centerY + 40, GROUND_SCALE);

	delete colorFont;
	colorFont = NULL;

	if (nowTicks - lastStageTime < 4250) return false;

	redTitleFont->RenderString("NAMCO @", centerX, centerY + 250, GROUND_SCALE * 1.5);

	return true;
}

bool LevelManager::RenderGame()
{
	int centerX = SCREEN_WIDTH / 2;
	int centerY = SCREEN_HEIGHT / 2;

	Uint32 nowTicks = SDL_GetTicks();

	RenderScoreAt(centerX, centerY - 270);

	titleFont->RenderString("LEVEL " + std::to_string(nowLevel), centerX, centerY - 220, GROUND_SCALE);
	
	mGround->Render(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, GROUND_SCALE);
	mPlayer->RenderAtPosition(GROUND_SCALE);
	
	for (auto ghost = ghostList.begin(); ghost != ghostList.end(); ghost++)
	{
		(*ghost)->RenderAtPosition(GROUND_SCALE);
	}

	if (DRAW_GHOST_GIZMO)
	{
		for (auto ghost = ghostList.begin(); ghost != ghostList.end(); ghost++)
		{
			(*ghost)->DrawGizmos();
		}
	}
	
	// Render score gizmos
	RenderScoreList();

	// Render extra life
	RenderExtraLife(centerX - 200, centerY + 250);

	// Render START / READY signs
	if (nowTicks - lastStageTime <= 2000)
	{
		LFont* colorFont;
		
		int groundOriginX = SCREEN_WIDTH / 2 - int(((MAP_WIDTH - 1.0) / 2) * (WALL_WIDTH * GROUND_SCALE));
		int groundOriginY = SCREEN_HEIGHT / 2 - int(((MAP_HEIGHT - 1.0) / 2) * (WALL_HEIGHT * GROUND_SCALE));

		colorFont = new LFont();
		colorFont->LoadTexture(this->mRenderer, "res/image/font.png", 0xDE, 0x95, 0x00);
		colorFont->SetOrigin(groundOriginX, groundOriginY);
		colorFont->SetUnitSize(int(WALL_WIDTH * GROUND_SCALE), int(WALL_HEIGHT * GROUND_SCALE));
		colorFont->SetTileSize(FONT_WIDTH, FONT_HEIGHT);
		colorFont->RenderStringAt("READY", 11, 15, GROUND_SCALE);

		delete colorFont;
		colorFont = NULL;
	}
	else if (nowTicks - lastStageTime <= 3000)
	{
		LFont* colorFont;

		int groundOriginX = SCREEN_WIDTH / 2 - int(((MAP_WIDTH - 1.0) / 2) * (WALL_WIDTH * GROUND_SCALE));
		int groundOriginY = SCREEN_HEIGHT / 2 - int(((MAP_HEIGHT - 1.0) / 2) * (WALL_HEIGHT * GROUND_SCALE));

		colorFont = new LFont();
		colorFont->LoadTexture(this->mRenderer, "res/image/font.png", 0xDE, 0x00, 0x00);
		colorFont->SetOrigin(groundOriginX, groundOriginY);
		colorFont->SetUnitSize(int(WALL_WIDTH * GROUND_SCALE), int(WALL_HEIGHT * GROUND_SCALE));
		colorFont->SetTileSize(FONT_WIDTH, FONT_HEIGHT);
		colorFont->RenderStringAt("START", 11, 15, GROUND_SCALE);

		delete colorFont;
		colorFont = NULL;
	}

	if (inLevelState == 4)
	{
		redTitleFont->RenderStringAt("GAME OVER", 11, 13, GROUND_SCALE * 1.5);
	}

	return true;
}

bool LevelManager::RenderComplete()
{
	int nowTicks = SDL_GetTicks();

	int colorTicks[] = { (nowTicks - 2000) % 6000,
		(nowTicks + 0) % 6000,
		(nowTicks + 2000) % 6000 };

	Uint8 fontColor[3] = { 0x00, 0x00, 0x00 };

	for (int i = 0; i < 3; i++)
	{
		if (colorTicks[i] <= 1000)
		{
			fontColor[i] = Uint8((colorTicks[i] / 1000.0) * 0xFF);
		}
		else if (colorTicks[i] <= 3000)
		{
			fontColor[i] = 0xFF;
		}
		else if (colorTicks[i] <= 4000)
		{
			fontColor[i] = Uint8(((colorTicks[i] - 3000.0) / 1000) * 0xFF);
		}
	}

	LFont* colorFont;

	int groundOriginX = SCREEN_WIDTH / 2 - int(((MAP_WIDTH - 1.0) / 2) * (WALL_WIDTH * GROUND_SCALE));
	int groundOriginY = SCREEN_HEIGHT / 2 - int(((MAP_HEIGHT - 1.0) / 2) * (WALL_HEIGHT * GROUND_SCALE));

	colorFont = new LFont();
	colorFont->LoadTexture(this->mRenderer, "res/image/font.png", fontColor[0], fontColor[1], fontColor[2]);
	colorFont->SetOrigin(groundOriginX, groundOriginY);
	colorFont->SetUnitSize(int(WALL_WIDTH * GROUND_SCALE), int(WALL_HEIGHT * GROUND_SCALE));
	colorFont->SetTileSize(FONT_WIDTH, FONT_HEIGHT);
	colorFont->RenderStringAt("ALL LEVEL", 11, 11, GROUND_SCALE * 1.5);
	colorFont->RenderStringAt("COMPLETE", 11, 14, GROUND_SCALE * 1.5);

	delete colorFont;
	colorFont = NULL;

	return true;
}


void LevelManager::AddScoreToRenderList(int score, int x, int y)
{
	UpdateScoreRenderList();
	scoreRenderList.push_back(score);
	scorePositionList.push_back(std::make_pair(x, y));
	getScoreTimeList.push_back(SDL_GetTicks());
}

void LevelManager::UpdateScoreRenderList()
{
	Uint32 nowTicks = SDL_GetTicks();
	while (true)
	{
		if (getScoreTimeList.empty())
		{
			break;
		}
		if (nowTicks - *(getScoreTimeList.begin()) >= scoreGizmoTime)
		{
			scoreRenderList.erase(scoreRenderList.begin());
			scorePositionList.erase(scorePositionList.begin());
			getScoreTimeList.erase(getScoreTimeList.begin());
		}
		else break;
	}
}

void LevelManager::RenderScoreList()
{
	UpdateScoreRenderList();
	int length = scoreRenderList.size();
	for (int i = 0; i < length; i++)
	{
		int x = scorePositionList[i].first + 1;
		int y = scorePositionList[i].second - 1;

		int score = scoreRenderList[i];

		std::string scoreStr = "";
		int _score = score;
		while (true)
		{
			scoreStr = char(_score % 10 + '0') + scoreStr;
			_score /= 10;
			if (_score == 0)
				break;
		}

		numberFont->RenderStringAt(scoreStr, x, y, GROUND_SCALE);
	}
}

bool LevelManager::RenderScoreAt(int centerX, int centerY)
{
	redTitleFont->RenderString("1UP", centerX - 200, centerY, GROUND_SCALE);
	redTitleFont->RenderString("HI-SCORE", centerX, centerY, GROUND_SCALE);
	redTitleFont->RenderString("2UP", centerX + 200, centerY, GROUND_SCALE);

	std::string scoreStr = "       00";
	int _score = this->score;
	for (int i = 8; i >= 0; i--)
	{
		scoreStr[i] = _score % 10 + '0';
		_score /= 10;
		if (_score == 0)
			break;
	}

	titleFont->RenderString(scoreStr, centerX - 200, centerY + 20, GROUND_SCALE);
	titleFont->RenderString("10000", centerX, centerY + 20, GROUND_SCALE);
	titleFont->RenderString("00", centerX + 250, centerY + 20, GROUND_SCALE);

	/*
	if (PLAYER_IMMUNE)
	{
		redTitleFont->RenderString("DEBUG-MODE ACTIVE", centerX, centerY + 495, GROUND_SCALE);
	}
	*/
	return true;
}

bool LevelManager::RenderExtraLife(int leftX, int centerY)
{
	int centerX = leftX;
	for (int i = 0; i < playerExtraLife; i++)
	{
		pacManTexture->RenderTile(centerX, centerY, 0x02, GROUND_SCALE, 180);
		centerX += int(pacManTexture->GetWidth() * GROUND_SCALE / 4) + 5;
	}
	return true;
}