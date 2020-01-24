#include "Pixelsort.h"
#include "ofMain.h"

namespace itg {
	Pixelsort::Pixelsort(const ofVec2f& aspect, bool arb) : RenderPass(aspect, arb, "Pixelsort") {
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
			uniform vec2 resolution;
            uniform float time;

            bool VERT = false;
		    float THR = ( sin( time * 5.0 ) * 0.5 + 0.5 );
			bool SHADOW = false;
			bool REVERSE = true;

			in vec4 vColor;
			in vec2 vTexCoord;

			uniform float thresholdLow = .7;
			uniform float thresholdHigh = .9;

			out vec4 fragColor;

			float gray(vec3 c) {
				return dot(c, vec3(0.299, 0.587, 0.114));
			}

			vec3 toRgb(float i) {
				return vec3(
					mod(i, 256.0),
					mod(floor(i / 256.0), 256.0),
					floor(i / 65536.0)
				) / 255.0;
			}

			bool thr(float v) {
				return SHADOW ? (THR < v) : (v < THR);
			}

			vec4 draw(vec2 uv) {
				vec2 texOffset = vec2(1.0) / resolution;
				vec2 dirVec = VERT ? vec2(0.0, 1.0) : vec2(1.0, 0.0);
				float wid = 1.0;
				//float pos = VERT ? floor(uv.y * resolution.y) : floor(uv.x * resolution.x);
				float pos = uv.x;

				float val = gray(texture(tex, uv).xyz);

				if (!thr(val)) {
					float post = pos;
					float rank = 0.0;
					float head = 0.0;
					float tail = 0.0;

					for (float i = 0.0; i < wid; i += texOffset.x) {
						post -= texOffset.x;
						if (post < 0.0) { head = post + texOffset.x; break; }
						vec2 p = dirVec * (post + 0.5) / wid + dirVec.yx * uv;
						float v = gray(texture(tex, p).xyz);
						if (thr(v)) { head = post + texOffset.x; break; }
						if (v <= val) { rank += texOffset.x; }
					}

					post = pos;
					for (float i = 0.0; i < wid; i+=texOffset.x) {
						post += texOffset.x;
						if (wid == post) { tail = post - texOffset.x; break; }
						vec2 p = dirVec * (post + 0.5) / wid + dirVec.yx * uv;
						float v = gray(texture(tex, p).xyz);
						if (thr(v)) { tail = post - texOffset.x; break; }
						if (v < val) { rank += texOffset.x; }
					}

					pos = REVERSE ? (tail - rank) : (head + rank);
				}

				return vec4(toRgb(pos), 1.0);
			}

			void main() {
				fragColor = draw(vTexCoord);
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

	void Pixelsort::render(ofFbo& readFbo, ofFbo& writeFbo) {
		writeFbo.begin();
		shader.begin();
		shader.setUniformTexture("tex", readFbo.getTexture(), 0);
		shader.setUniform2f("resolution", readFbo.getWidth(), readFbo.getHeight());
		quad.draw(OF_MESH_FILL);
		shader.end();
		writeFbo.end();
	}
}
