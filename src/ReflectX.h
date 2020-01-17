#pragma once

#include "RenderPass.h"
#include "ofShader.h"

namespace itg
{
	class ReflectX : public RenderPass
	{
	public:
		typedef shared_ptr<ReflectX> Ptr;
		
		ReflectX(const ofVec2f& aspect, bool arb);

		void render(ofFbo& readFbo, ofFbo& writeFbo);

	private:
		ofShader shader;
	};
}