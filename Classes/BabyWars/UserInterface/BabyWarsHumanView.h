#ifndef __BABY_WARS_HUMAN_VIEW__
#define __BABY_WARS_HUMAN_VIEW__

#include "../../BabyEngine/UserInterface/BaseHumanView.h"

class BabyWarsHumanView : public BaseHumanView
{
public:
	BabyWarsHumanView() = default;
	~BabyWarsHumanView() = default;

private:
	virtual void vLoadResource() override;
};

#endif // !__BABY_WARS_HUMAN_VIEW__
