#include "Beyoon.h"
#include "ofMain.h"

namespace itg
{
	Beyoon::Beyoon(const ofVec2f& aspect, bool arb) : RenderPass(aspect, arb, "Beyoon")
	{
        offset = ofVec2f(0.5, 0.5);
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
		uniform vec2 offset;

            in vec2 vTexCoord;
            out vec4 fragColor;

            void main()
            {
                if((vTexCoord.x <= offset.x && vTexCoord.y <= offset.y) || (vTexCoord.x >= 1.0 + offset.x && vTexCoord.y >= 1.0 + offset.y)) {
                    fragColor = texture(tex, vec2(offset));
                }
                else if(vTexCoord.x <= offset.x || vTexCoord.x >= (1.0 + offset.x)) {
                    fragColor = texture(tex, vec2(offset.x, vTexCoord.y));
                }
                else if(vTexCoord.y <= offset.y || vTexCoord.y >= (1.0 + offset.y)) {
                    fragColor = texture(tex, vec2(vTexCoord.x, offset.y));
                } else {
                    fragColor = texture(tex, vTexCoord);
                }
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

	void Beyoon::render(ofFbo & readFbo, ofFbo & writeFbo)
	{
		writeFbo.begin();
		shader.begin();
		shader.setUniformTexture("tex", readFbo.getTexture(), 0);
		shader.setUniform2f("offset", offset);

		//texturedQuad(0, 0, writeFbo.getWidth(), writeFbo.getHeight());
		quad.draw(OF_MESH_FILL);

		shader.end();
		writeFbo.end();
	}
}
