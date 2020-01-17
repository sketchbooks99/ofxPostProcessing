#pragma once

#include "RenderPass.h"
#include "ofShader.h"

namespace itg
{
	class Split : public RenderPass
	{
	public:
		typedef shared_ptr<Split> Ptr;

		Split(const ofVec2f& aspect, bool arb);

		void render(ofFbo& readFbo, ofFbo& writeFbo);

	private:
		ofShader shader;
	};
}
