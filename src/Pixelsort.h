#pragma once

#include "RenderPass.h"
#include "ofShader.h"

namespace itg
{
	class Pixelsort : public RenderPass {
	public:
		typedef shared_ptr<Pixelsort> Ptr;

		Pixelsort(const ofVec2f& aspect, bool arb);

		void render(ofFbo& readFbo, ofFbo& writeFbo);

	private:
		ofShader shader;
	};
}