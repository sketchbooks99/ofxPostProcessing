#pragma once

#include "RenderPass.h"
#include "ofShader.h"

namespace itg
{
	class Beyoon : public RenderPass
	{
	public:
		typedef shared_ptr<Beyoon> Ptr;

		Beyoon(const ofVec2f& aspect, bool arb);

		void render(ofFbo& readFbo, ofFbo& writeFbo);

        void setOffset(ofVec2f _offset) { offset = _offset; }
	private:
		ofShader shader;
        ofVec2f offset;
	};

}