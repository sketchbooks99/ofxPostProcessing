#pragma once

#include "RenderPass.h"
#include "ofShader.h"

namespace itg
{
	class ReflectY : public RenderPass
	{
	public:
		typedef shared_ptr<ReflectY> Ptr;
		
		ReflectY(const ofVec2f& aspect, bool arb);

		void render(ofFbo& readFbo, ofFbo& writeFbo);

	private:
		ofShader shader;
	};
}