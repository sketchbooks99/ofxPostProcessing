#pragma once

#include "RenderPass.h"
#include "ofShader.h"

namespace itg
{
	class Invert : public RenderPass
	{
	public:
		typedef shared_ptr<Invert> Ptr;

		Invert(const ofVec2f& aspect, bool arb);

		void render(ofFbo& readFbo, ofFbo& writeFbo);

	private:
		ofShader shader;
	};
}
