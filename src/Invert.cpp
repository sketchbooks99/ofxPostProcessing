#include "Invert.h"
#include "ofMain.h"

namespace itg
{
	Invert::Invert(const ofVec2f& aspect, bool arb) : RenderPass(aspect, arb, "Invert")
	{
		string vertShaderSrc = STRINGIFY(
			in vec2 texcoord;
		in vec4 position;
		out vec2 vTexCoord;
		void main() {
			gl_Position = position;
			vTexCoord = texcoord;
		}
		);

		string fragShaderSrc = STRINGIFY(
			uniform sampler2D tex;

		in vec2 vTexCoord;
		out vec4 fragColor;

		void main()
		{
			fragColor = vec4(vec3(1.0) - texture(tex, vTexCoord).rgb, 1.0);
		}
		);

        ostringstream oss;
        oss << "#version 150" << endl;
        oss << vertShaderSrc << endl;
        shader.setupShaderFromSource(GL_VERTEX_SHADER, oss.str());
        
        oss.str("");
        oss << "#version 150" << endl;
        oss << fragShaderSrc << endl;
        shader.setupShaderFromSource(GL_FRAGMENT_SHADER, oss.str());
        
		shader.bindDefaults();
		shader.linkProgram();
	}

	void Invert::render(ofFbo & readFbo, ofFbo & writeFbo)
	{
		writeFbo.begin();
		shader.begin();
		shader.setUniformTexture("tex", readFbo.getTexture(), 0);

		//texturedQuad(0, 0, writeFbo.getWidth(), writeFbo.getHeight());
		quad.draw(OF_MESH_FILL);

		shader.end();
		writeFbo.end();
	}
}
