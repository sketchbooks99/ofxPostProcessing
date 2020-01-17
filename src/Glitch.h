#pragma once

#include "RenderPass.h"
#include "ofShader.h"

namespace itg
{
	class Glitch : public RenderPass
	{
	public:
		typedef shared_ptr<Glitch> Ptr;

		Glitch(const ofVec2f& aspect, bool arb);

		void render(ofFbo& readFbo, ofFbo& writeFbo);

	private:
		ofShader shader;
	};

}