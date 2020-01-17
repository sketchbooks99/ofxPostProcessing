#pragma once

#include "RenderPass.h"
#include "ofShader.h"

namespace itg
{
	class Rotate : public RenderPass
	{
	public:
		typedef shared_ptr<Rotate> Ptr;

		Rotate(const ofVec2f& aspect, bool arb);

		void render(ofFbo& readFbo, ofFbo& writeFbo);

	private:
		ofShader shader;
	};

}
