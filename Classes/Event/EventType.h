#ifndef __EVENT_TYPE__
#define __EVENT_TYPE__

enum class EventType
{
	InvalidEventType,

	RequestDestoryActor,

	PlayerGotScore,

	PlayerExplodedStars,

	LevelNoMoreMove,

	NewGameStarted,
	LevelStarted,
	StartLevelLabelDisappeared,
	LevelSummaryStarted,
	LevelSummaryFinished,
	RemainingStarsExploded,

	GameOver,

	CurrentScoreUpdated,
	HighScoreUpdated,
	TargetScoreUpdated,
};

#endif // !__EVENT_TYPE__
