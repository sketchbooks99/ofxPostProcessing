/*
 *  GodRaysPass.cpp
 *
 *  Copyright (c) 2013, satcy, http://satcy.net
 *  All rights reserved. 
 *  
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met: 
 *  
 *  * Redistributions of source code must retain the above copyright notice, 
 *    this list of conditions and the following disclaimer. 
 *  * Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 *  * Neither the name of Neil Mendoza nor the names of its contributors may be used 
 *    to endorse or promote products derived from this software without 
 *    specific prior written permission. 
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 *  POSSIBILITY OF SUCH DAMAGE. 
 *
 */
#include "GodRaysPass.h"
#include "ofMain.h"

namespace itg
{
    GodRaysPass::GodRaysPass(const ofVec2f& aspect, bool arb, const ofVec3f & lightPositionOnScreen, float lightDirDOTviewDir) :
        lightPositionOnScreen(lightPositionOnScreen), lightDirDOTviewDir(lightDirDOTviewDir), RenderPass(aspect, arb, "godrays")
    {
        
        string vertShaderSrc = STRINGIFY(
            #version 410 core\n
            uniform mat4 modelViewProjectionMatrix;
            in vec2 texcoord;
            in vec4 color;
            in vec4 position;

            out vec2 vTexCoord;
            
            void main(void)
            {
                vTexCoord = texcoord;
                // gl_FrontColor = color;
    
				gl_Position = position;
            }
        );
        
        string fragShaderSrc = STRINGIFY(
            #version 410 core\n
            uniform sampler2D rtex;
            uniform sampler2D otex;

            const int NUM_SAMPLES = 50;

            uniform vec2 lightPositionOnScreen;
            uniform float lightDirDOTviewDir;

            in vec2 vTexCoord;
            out vec4 fragColor;

            void main(void)
            {
                vec4 origColor = texture(otex, vTexCoord);
                vec4 raysColor = texture(rtex, vTexCoord);

                if (lightDirDOTviewDir>0.0){
                    float exposure	= 0.1/float(NUM_SAMPLES);
                    float decay		= 1.0 ;
                    float density	= 0.5;
                    float weight	= 6.0;
                    float illuminationDecay = 1.0;

                    vec2 deltaTextCoord = vec2( vTexCoord - lightPositionOnScreen);
                    vec2 textCoo = vTexCoord;
                    deltaTextCoord *= 1.0 / float(NUM_SAMPLES) * density;



                    for(int i=0; i < NUM_SAMPLES ; i++)
                    {
                        textCoo -= deltaTextCoord;
                        vec4 tsample = texture(rtex, textCoo );
                        tsample *= illuminationDecay * weight;
                        raysColor += tsample;
                        illuminationDecay *= decay;
                    }
                    raysColor *= exposure * lightDirDOTviewDir;
                    float p = 0.3 *raysColor.g + 0.59*raysColor.r + 0.11*raysColor.b;
                    fragColor = origColor + p;
                } else {
                    fragColor = origColor;
                }
            }
        );
        shader.setupShaderFromSource(GL_VERTEX_SHADER, vertShaderSrc);
        shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragShaderSrc);
		shader.bindDefaults();
        shader.linkProgram();
        
    }
    
    void GodRaysPass::render(ofFbo& readFbo, ofFbo& writeFbo, ofTexture& depthTex)
    {
        writeFbo.begin();
        
        shader.begin();
        shader.setUniformTexture("otex", readFbo.getTexture(), 0);
        shader.setUniformTexture("rtex", readFbo.getTexture(), 1);
        shader.setUniform2f("lightPositionOnScreen", lightPositionOnScreen.x, lightPositionOnScreen.y);
        shader.setUniform1f("lightDirDOTviewDir", lightDirDOTviewDir);
        
        
        //texturedQuad(0, 0, writeFbo.getWidth(), writeFbo.getHeight());
		quad.draw(OF_MESH_FILL);
        
        shader.end();
        writeFbo.end();
    }
}