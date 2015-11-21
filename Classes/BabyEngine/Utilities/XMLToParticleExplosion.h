#ifndef __XML_TO_PARTICLE_EXPLOSION__
#define __XML_TO_PARTICLE_EXPLOSION__

//Forward declaration.
namespace cocos2d
{
	class ParticleExplosion;
}
namespace tinyxml2
{
	class XMLElement;
}

namespace utilities
{
	cocos2d::ParticleExplosion * XMLToParticleExplosion(const tinyxml2::XMLElement * xmlElement);
}

#endif // !__XML_TO_PARTICLE_EXPLOSION__
