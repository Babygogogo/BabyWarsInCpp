#include "cocos2d.h"
#include "../cocos2d/external/tinyxml2/tinyxml2.h"

#include "XMLToParticleExplosion.h"

cocos2d::ParticleExplosion * utilities::XMLToParticleExplosion(const tinyxml2::XMLElement * xmlElement)
{
	auto particle = cocos2d::ParticleExplosion::create();
	if (auto properties = xmlElement->FirstChildElement("Properties")) {
		if (auto texture = properties->Attribute("Texture"))
			particle->setTexture(cocos2d::Director::getInstance()->getTextureCache()->addImage(texture));
		if (auto totalParticles = properties->IntAttribute("TotalParticles"))
			particle->setTotalParticles(totalParticles);
		if (auto startSize = properties->FloatAttribute("StartSize"))
			particle->setStartSize(startSize);
		if (auto life = properties->FloatAttribute("Life"))
			particle->setLife(life);
		if (auto speed = properties->FloatAttribute("Speed"))
			particle->setSpeed(speed);
		if (auto speedVariance = properties->FloatAttribute("SpeedVariance"))
			particle->setSpeedVar(speedVariance);

		if (auto startColorVariance = properties->FirstChildElement("StartColorVariance")) {
			auto r = startColorVariance->FloatAttribute("R") / 255.0f;
			auto g = startColorVariance->FloatAttribute("G") / 255.0f;
			auto b = startColorVariance->FloatAttribute("B") / 255.0f;
			auto a = startColorVariance->FloatAttribute("A") / 255.0f;
			particle->setStartColorVar({ r, g, b, a });
		}
		if (auto endColorVariance = properties->FirstChildElement("EndColorVariance")) {
			auto r = endColorVariance->FloatAttribute("R") / 255.0f;
			auto g = endColorVariance->FloatAttribute("G") / 255.0f;
			auto b = endColorVariance->FloatAttribute("B") / 255.0f;
			auto a = endColorVariance->FloatAttribute("A") / 255.0f;
			particle->setEndColorVar({ r, g, b, a });
		}
		if (auto gravity = properties->FirstChildElement("Gravity")) {
			auto x = gravity->FloatAttribute("X");
			auto y = gravity->FloatAttribute("Y");
			particle->setGravity({ x, y });
		}
	}

	return particle;
}
